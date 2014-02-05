"""Filling is the gui tree control through which a user can navigate
the local namespace or any object."""

__author__ = "Original author Patrick K. O'Brien <pobrien@orbtech.com>, Modifications by Daniel Bueltmann <me@daniel-bueltmann.de>"
__cvsid__ = "$Id: filling.py 37633 2006-02-18 21:40:57Z RD $"
__revision__ = "$Revision: 37633 $"[11:-2]

import wx
import wx.py.dispatcher as dispatcher
import wx.html
import inspect
import keyword
import os
import sys
import imp
from wx.py.version import VERSION
import tree
import printer

class FillingText(wx.html.HtmlWindow):
    """FillingText based on StyledTextCtrl."""

    name = 'Filling Text'
    revision = __revision__

    def __init__(self, parent, id=-1, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.CLIP_CHILDREN,
                 static=False, tree=None):
        """Create FillingText instance."""
        wx.html.HtmlWindow.__init__(self, parent, id, pos, size, style)
        if not static:
            dispatcher.connect(receiver=self.push, signal='Interpreter.push')

        self.tree = tree
        self.currentHTML = ""

    def push(self, command, more):
        """Receiver for Interpreter.push signal."""
        self.Refresh()

    def OnLinkClicked(self, linkinfo):
        href = linkinfo.GetHref()
        if href.startswith("_pytree_object_id#"):
            theId = href.split("#")[1]
            self.tree.navigateTo(theId)
        else:
            wx.html.HtmlWindow.OnLinkClicked(self,linkinfo)

    def SetText(self, string):
        self.SetPage("<html><body>"+ string + "</body></html>")
        self.currentHTML = "<html><body>"+ string + "</body></html>"

    def getHTML(self):
        return self.currentHTML

class Filling(wx.SplitterWindow):
    """Filling based on wxSplitterWindow."""

    name = 'Filling'
    revision = __revision__

    def __init__(self, parent, id=-1, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.SP_3D|wx.SP_LIVE_UPDATE,
                 name='Filling Window', rootObject=None,
                 rootLabel=None, rootIsNamespace=False, static=False):
        """Create a Filling instance."""
        wx.SplitterWindow.__init__(self, parent, id, pos, size, style, name)

        self.tree = tree.FillingTree(parent=self, rootObject=rootObject,
                                     rootLabel=rootLabel,
                                     rootIsNamespace=rootIsNamespace,
                                     static=static)
        self.text = FillingText(parent=self, static=static, tree=self.tree)
        
        wx.FutureCall(1, self.SplitVertically, self.tree, self.text, 200)
        
        self.SetMinimumPaneSize(1)

        # Override the filling so that descriptions go to FillingText.
        self.tree.setText = self.text.SetText

        # Display the root item.
        self.tree.SelectItem(self.tree.root)
        self.tree.display()

        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGED, self.OnChanged)

    def OnChanged(self, event):
        #this is important: do not evaluate this event=> otherwise, splitterwindow behaves strange
        #event.Skip()
        pass


    def LoadSettings(self, config):
        pos = config.ReadInt('Sash/FillingPos', 200)
        wx.FutureCall(250, self.SetSashPosition, pos)
        zoom = config.ReadInt('View/Zoom/Filling', -99)
        if zoom != -99:
            self.text.SetZoom(zoom)

    def SaveSettings(self, config):
        config.WriteInt('Sash/FillingPos', self.GetSashPosition())
        config.WriteInt('View/Zoom/Filling', self.text.GetZoom())



class FillingFrame(wx.Frame):
    """Frame containing the namespace tree component."""

    name = 'Filling Frame'
    revision = __revision__

    def __init__(self, parent=None, id=-1, title='PyTree - The openWNS Configuration Browser',
                 pos=(0,0), size=(600, 400),
                 style=wx.DEFAULT_FRAME_STYLE, rootObject=None,
                 rootLabel=None, filename="Unknown Filename", rootIsNamespace=False, static=False):
        """Create FillingFrame instance."""

        wx.Frame.__init__(self, parent, id, title, pos, wx.DisplaySize(), style)
        intro = 'PyTree - The openWNS Configuration Browser'
        self.CreateStatusBar()
        self.SetStatusText(intro)
        #import wx.py.images
        #self.SetIcon(wx.py.images.getPyIcon())
        self.filling = Filling(parent=self, rootObject=rootObject,
                               rootLabel=rootLabel,
                               rootIsNamespace=rootIsNamespace,
                               static=static)
        # Override so that status messages go to the status bar.
        self.filling.tree.setStatusText = self.SetStatusText
        self.printer = printer.Printer()
        self.filename = filename

        menubar = wx.MenuBar()
        file = wx.Menu()
        view = wx.Menu()

        # File
        file.Append(101, '&Open', 'Open a new document')
        file.Append(102, '&Save Page', 'Save the current page')
        file.Append(103, '&Print Page', 'Print the current page')
        file.AppendSeparator()

        quit = wx.MenuItem(file, 104, '&Quit\tCtrl+Q', 'Quit the Application')
        #quit.SetBitmap(wx.Image('stock_exit-16.png',wx.BITMAP_TYPE_PNG).ConvertToBitmap())
        file.AppendItem(quit)

        # View
        view.Append(201, 'Show &Private Members\tCtrl+P', "Show Private Members", wx.ITEM_CHECK)
        view.Append(202, 'Show &Modules\tCtrl+M', "Show Python Modules", wx.ITEM_CHECK)
        view.Append(203, 'Show C&lasses\tCtrl+L', "Show Python Classes", wx.ITEM_CHECK)
        view.Append(204, 'Show &Types\tCtrl+T', "Show Python Types", wx.ITEM_CHECK)
        view.Append(205, 'Show Meth&ods\tCtrl+O', "Show Python Methods", wx.ITEM_CHECK)

        menubar.Append(file, '&File')
        menubar.Append(view, '&View')
        self.SetMenuBar(menubar)


        wx.EVT_MENU(self, 101, self.OnOpenFile )
        wx.EVT_MENU(self, 102, self.OnSaveFile )
        wx.EVT_MENU(self, 103, self.OnPrint )
        wx.EVT_MENU(self, 104, self.OnQuit )
        wx.EVT_MENU(self, 201, self.filling.tree.toggleShowPrivate )
        wx.EVT_MENU(self, 202, self.filling.tree.toggleShowModules )
        wx.EVT_MENU(self, 203, self.filling.tree.toggleShowClasses )
        wx.EVT_MENU(self, 204, self.filling.tree.toggleShowTypes )
        wx.EVT_MENU(self, 205, self.filling.tree.toggleShowMethods )

    def OnOpenFile(self, event):
       dlg = wx.FileDialog(self, "Choose a file", os.getcwd(), "", "*.py", wx.OPEN)
       if dlg.ShowModal() == wx.ID_OK:
                path = dlg.GetPath()
                mypath = os.path.basename(path)
                self.SetStatusText("You selected: %s" % mypath)
       dlg.Destroy()

    def OnQuit(self, event):
        self.Close()

    def OnSaveFile(self, event):
        dlg = wx.FileDialog(self, "Save File", os.getcwd(), "", "*.html", wx.SAVE)
        if dlg.ShowModal() == wx.ID_OK:
                path = dlg.GetPath()
                mypath = os.path.basename(path)
                self.SetStatusText("You selected: %s" % path)
                f = open(path, "w")
                f.write(self.filling.text.getHTML())
                f.close()

        dlg.Destroy()

    def OnPrint(self, event):
        self.printer.Print(self.filling.text.getHTML(), self.filename)

class App(wx.App):
    """PyFilling standalone application."""

    def __init__(self, **kwargs):
        self.kwargs = kwargs
        wx.App.__init__(self)


    def OnInit(self):
        wx.InitAllImageHandlers()
        self.fillingFrame = FillingFrame(**self.kwargs)
        self.fillingFrame.Show(True)
        self.SetTopWindow(self.fillingFrame)
        return True
