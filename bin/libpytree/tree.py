import typecategories

import wx
import wx.py.introspect as introspect
import wx.py.dispatcher as dispatcher

import types
import cgi

class PyTreeFavourites:
    """ This class is only used to provide some quick navigation links in PyTree"""
    pass

class FillingTree(wx.TreeCtrl):
    """FillingTree based on TreeCtrl."""

    name = 'Filling Tree'

    def __init__(self, parent, id=-1, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.TR_DEFAULT_STYLE,
                 rootObject=None, rootLabel=None, rootIsNamespace=False,
                 static=False):
        """Create FillingTree instance."""

        self.installFavourites(rootObject)

        wx.TreeCtrl.__init__(self, parent, id, pos, size, style)
        self.rootIsNamespace = rootIsNamespace
        import __main__
        if rootObject is None:
            rootObject = __main__.__dict__
            self.rootIsNamespace = True
        if rootObject is __main__.__dict__ and rootLabel is None:
            rootLabel = 'locals()'
        if not rootLabel:
            rootLabel = 'Ingredients'
        rootData = wx.TreeItemData(rootObject)
        self.item = self.root = self.AddRoot(rootLabel, -1, -1, rootData)
        self.SetItemHasChildren(self.root, self.objHasChildren(rootObject))
        self.Bind(wx.EVT_TREE_ITEM_EXPANDING, self.OnItemExpanding, id=self.GetId())
        self.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.OnItemCollapsed, id=self.GetId())
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged, id=self.GetId())
        self.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.OnItemActivated, id=self.GetId())
        if not static:
            dispatcher.connect(receiver=self.push, signal='Interpreter.push')

        self.showPrivateMembers = False
        self.showModules = False
        self.showClasses = False
        self.showTypes = False
        self.showMethods = False

    def installFavourites(self, rootObject):

        rootObject.FAVOURITES = PyTreeFavourites()
        import openwns.simulator
        rootObject.FAVOURITES.simulator = openwns.simulator.getSimulator()
        try:
            rootObject.FAVOURITES.measurementSources = openwns.simulator.getSimulator().environment.probeBusRegistry.measurementSources
        except:
            pass

        try:
            rootObject.FAVOURITES.nodes = openwns.simulator.getSimulator().simulationModel.nodes
        except:
            pass

    def toggleShowPrivate(self, event):
        self.showPrivateMembers = event.Checked()

    def toggleShowModules(self, event):
        self.showModules = event.Checked()

    def toggleShowClasses(self, event):
        self.showClasses = event.Checked()

    def toggleShowTypes(self, event):
        self.showTypes = event.Checked()

    def toggleShowMethods(self, event):
        self.showMethods = event.Checked()
        
    def push(self, command, more):
        """Receiver for Interpreter.push signal."""
        self.display()

    def OnItemExpanding(self, event):
        """Add children to the item."""
        busy = wx.BusyCursor()
        item = event.GetItem()
        if self.IsExpanded(item):
            return

        self.addChildren(item)
#        self.SelectItem(item)

    def OnItemCollapsed(self, event):
        """Remove all children from the item."""
        busy = wx.BusyCursor()
        item = event.GetItem()
#        self.CollapseAndReset(item)
#        self.DeleteChildren(item)
#        self.SelectItem(item)

    def OnSelChanged(self, event):
        """Display information about the item."""
        busy = wx.BusyCursor()
        self.item = event.GetItem()
        self.display()

    def OnItemActivated(self, event):
        """Launch a DirFrame."""
        item = event.GetItem()
        text = self.getFullName(item)
        obj = self.GetPyData(item)
        frame = FillingFrame(parent=self, size=(600, 100), rootObject=obj,
                             rootLabel=text, rootIsNamespace=False, title='PyTree - The openWNS Configuration Browser [%s]' %text)
        frame.Show()

    def objHasChildren(self, obj):
        """Return true if object has children."""
        if self.objGetChildren(obj):
            return True
        else:
            return False

    def objGetChildren(self, obj):
        """Return dictionary with attributes or contents of object."""
        busy = wx.BusyCursor()
        otype = type(obj)
        if otype is types.DictType \
        or str(otype)[17:23] == 'BTrees' and hasattr(obj, 'keys'):
            return obj
        d = {}
        if otype is types.ListType or otype is types.TupleType:
            for n in range(len(obj)):
                key = '[' + str(n) + ']'
                d[key] = obj[n]
        if otype not in typecategories.COMMONTYPES:
            for key in introspect.getAttributeNames(obj):
                # Believe it or not, some attributes can disappear,
                # such as the exc_traceback attribute of the sys
                # module. So this is nested in a try block.
                try:
                    d[key] = getattr(obj, key)
                except:
                    pass
        return d

    def addChildren(self, item):
        self.DeleteChildren(item)
        obj = self.GetPyData(item)
        children = self.objGetChildren(obj)
        if not children:
            return
        keys = children.keys()
        keys.sort(lambda x, y: cmp(str(x).lower(), str(y).lower()))

        # Sort some important things to the top
        pushFront = []

        if item == self.root:
            for key in keys:
                try:
                    if key == "__dict__":
                        continue
                    str(children[key]).upper().index('FAVOURITES')

                    pushFront.append(keys.index(key))
                except:
                    pass

            for key in pushFront:
            
                k = keys.pop(key)
                keys.insert(0,k)
        
        for key in keys:
            itemtext = str(key)

            if not self.showPrivateMembers and itemtext.startswith("__"):
                continue

            if not self.showModules and type(children[key]) is types.ModuleType:
                continue

            if not self.showTypes and type(children[key]) is types.TypeType:
                continue

            if not self.showClasses and type(children[key]) is types.ClassType:
                continue

            if not self.showMethods and type(children[key]) is types.MethodType:
                continue

            # Show list objects with number and classname of child
            if type (obj) is types.ListType:
                import openwns.toolsupport
                s = openwns.toolsupport.renderLinkFromObject(children[key])

                if s=="":
                    s = str(children[key])

                itemtext += " " + s
                    

            if type(children[key]) is types.MethodType:
                itemtext += "()"

            # Show string dictionary items with single quotes, except
            # for the first level of items, if they represent a
            # namespace.
            if type(obj) is types.DictType \
            and type(key) is types.StringType \
            and (item != self.root \
                 or (item == self.root and not self.rootIsNamespace)):
                itemtext = repr(key)

            child = children[key]
            data = wx.TreeItemData(child)
            branch = self.AppendItem(parent=item, text=itemtext, data=data)
            self.SetItemHasChildren(branch, self.objHasChildren(child))

    def getClassTreeString(self, mrolist, indent = ""):
        if len(mrolist) == 0:
            return ""

        myName = mrolist[-1].__module__ + "." + mrolist[-1].__name__ + "<br>"

        # __builtin__ should not be displayed
        myName = myName.replace("__builtin__.", "")

        return indent + myName + self.getClassTreeString(mrolist[0:-1], indent=indent.replace("+---", "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;") +"+---")

    def getNavigationString(self, item):
        import re
        p=re.compile('\([\w\.]*\)')
        result=p.sub("", self.getFullName(item))
        result = result.replace(" ","")
        return result

    def navigateTo(self, navigationString):
        self.CollapseAll()
        path = navigationString.split(".")[2:]
        self.Expand(self.root)

        currentNode = self.root

        for subpath in path:
            if "[" in subpath:
                base = subpath.split("[")[0]
                index = subpath.split("[")[1].rstrip("]")

                subItem = self.GetFirstChild(currentNode)[0]
                while subItem.IsOk():
                    if self.GetItemText(subItem) == base:
                        if isinstance(self.GetPyData(subItem), dict):
                            try:
                                index = "'"+self.GetPyData(subItem).keys()[int(index)]+"'"
                            except:
                                pass

                        currentNode = subItem
                        self.Expand(currentNode)
                        indexItem = self.GetFirstChild(currentNode)[0]
                        while indexItem.IsOk():
                            obj = self.GetPyData(indexItem)

                            if self.GetItemText(indexItem).startswith("[%s]" % index) or self.GetItemText(indexItem)==index:
                                currentNode = indexItem
                                self.Expand(currentNode)
                                break
                            indexItem = self.GetNextSibling(indexItem)
                        break
                    subItem = self.GetNextSibling(subItem)
            else:
                subItem = self.GetFirstChild(currentNode)[0]
                while subItem.IsOk():
                    if self.GetItemText(subItem) == subpath:
                        currentNode = subItem
                        self.Expand(currentNode)
                        break
                    subItem = self.GetNextSibling(subItem)
        self.SelectItem(currentNode)

    def breadCrumbs(self, item):
        full = self.getNavigationString(item).split(".")[1:]
        full[0] = "Root"
        parent = ""
        result = ""
        delimit = "&nbsp;&raquo;&nbsp;"
        for ii in xrange(len(full)):
            if ii == len(full)-1:
                delimit = ""
            result += "<a href=\"_pytree_object_id#%s\">%s</a>%s" % (cgi.escape(parent + "." + full[ii]), cgi.escape(full[ii]), delimit)
            parent += "." + full[ii]

        return result

    def display(self):
        item = self.item
        if not item:
            return
        if self.IsExpanded(item):
            self.addChildren(item)
        self.setText('')
        obj = self.GetPyData(item)
        if wx.Platform == '__WXMSW__':
            if obj is None: # Windows bug fix.
                return
        self.SetItemHasChildren(item, self.objHasChildren(obj))
        otype = type(obj)

        #DisplayFactory.render(obj)

        #text = ''
        text = "<a name=\"top\"><center><h1>%s.%s</h1></center><br>"% (obj.__class__.__module__,obj.__class__.__name__)
        text += "<center>" + self.breadCrumbs(item) + "</center>"

        text += "<hr><a href=\"#top\">[Top]</a>&nbsp;&nbsp;<a href=\"#visitor\">[Visitor]</a>&nbsp;&nbsp;<a href=\"#inheritance\">[Inheritance Graph]</a><hr>"
        if item == self.root:
            text += "<p><font size=\"+2\"><b>"
            text += "Navigate on the left. Start by expanding the config file "
            text += "and then extending the FAVOURITES subtree. There you can "
            text += "find the most important configuration entries.</b></font></p>"

        import openwns.toolsupport
        text += "<a name=\"visitor\">"
        v = openwns.toolsupport.PyTreeVisitorFactory.getVisitor(obj)
        s = openwns.toolsupport.renderLongFromObject(obj, self.getNavigationString(item))
        if s == "":
            s = "<p>No PyTreeVistor found for this object. You can implement one of your own. See openwns.toolsupport.pytreevisitors.simulator for an example.<br></p>"
        text += s

        text += "<center><h2>Inheritance Graph</h2></center><hr><a name=\"inheritance\"/>"
        text += self.getClassTreeString(type(obj).mro(), "   ")

        text += "<center><h2>Low level information</h2></center><hr>"

        try:
            value = str(obj)
        except:
            value = ''
        if otype is types.StringType or otype is types.UnicodeType:
            value = repr(obj)
        text += '<p>Value: ' + value + "</p>"
        if otype not in typecategories.SIMPLETYPES:
            try:
                text += '<br>Docstring:<br><br><pre>"""' + \
                        inspect.getdoc(obj).strip() + '"""</pre>'
            except:
                pass
        if otype is types.InstanceType:
            try:
                text += '<br><br><pre>Class Definition:<br><br>' + \
                        inspect.getsource(obj.__class__) + "</pre>"
            except:
                pass

        text += "<hr><a href=\"#top\">[Top]</a>&nbsp;&nbsp;<a href=\"#visitor\">[Visitor]</a>&nbsp;&nbsp;<a href=\"#inheritance\">[Inheritance Graph]</a><hr>"
        self.setText(text)

    def getFullName(self, item, partial=''):
        """Return a syntactically proper name for item."""
        name = self.GetItemText(item)
        parent = None
        obj = None
        if item != self.root:
            parent = self.GetItemParent(item)
            obj = self.GetPyData(parent)
        # Apply dictionary syntax to dictionary items, except the root
        # and first level children of a namepace.
        if (type(obj) is types.DictType \
            or str(type(obj))[17:23] == 'BTrees' \
            and hasattr(obj, 'keys')) \
        and ((item != self.root and parent != self.root) \
            or (parent == self.root and not self.rootIsNamespace)):
            name = '[' + name + ']'
        # Apply dot syntax to multipart names.
        if partial:
            if partial[0] == '[':
                name += partial
            else:
                name += '.' + partial
        # Repeat for everything but the root item
        # and first level children of a namespace.
        if (item != self.root and parent != self.root) \
        or (parent == self.root and not self.rootIsNamespace):
            name = self.getFullName(parent, partial=name)
        return name

    def setText(self, text):
        """Display information about the current selection."""

        # This method will likely be replaced by the enclosing app to
        # do something more interesting, like write to a text control.
        print text

    def setStatusText(self, text):
        """Display status information."""

        # This method will likely be replaced by the enclosing app to
        # do something more interesting, like write to a status bar.
        print text
