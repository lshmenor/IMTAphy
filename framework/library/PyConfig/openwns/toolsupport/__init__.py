import openwns.interface
import cgi

class PyTreeVisitorFactory:

    visitors = {}

    @staticmethod
    def registerVisitor(cls, visitor):
        PyTreeVisitorFactory.visitors[cls] = visitor

    @staticmethod
    def getVisitor(obj):
        import openwns.toolsupport.pytreevisitors
        for cls in type(obj).mro():
            try:
                v = PyTreeVisitorFactory.visitors[cls]
                wrap = DefaultPyTreeVisitor()
                wrap.appendCustomRenderer(v)
                return wrap
            except KeyError:
                pass
        return DefaultPyTreeVisitor()

def renderLinkFromObject(o):
    v = PyTreeVisitorFactory.getVisitor(o)
    return v.renderLinkText(o)

def renderShortFromObject(o):
    v = PyTreeVisitorFactory.getVisitor(o)
    return v.renderShortText(o)

def renderLongFromObject(o, pathToObject="", alternateTitle="Default View"):
    v = PyTreeVisitorFactory.getVisitor(o)
    anchor = "<a name=\"%s\"></a>" % str(pathToObject)
    return anchor + v.renderLongText(o, pathToObject, alternateTitle)

def createTempfileName():
    import tempfile
    return tempfile.mktemp()


class IPyTreeVisitor(openwns.interface.Interface):

    def renderLinkText(self, objectToRender):
        return renderShortText(objectToRender)

    def renderShortText(self, objectToRender):
        pass

    def renderLongText(self, objectToRender):
        pass

class EmptyPyTreeVisitor(IPyTreeVisitor):

    def renderShortText(self, objectToRender):
        return ""

    def renderLongText(self, objectToRender):
        return ""

class DefaultPyTreeVisitor(IPyTreeVisitor):

    def __init__(self):
        self.customRenderers = []

    def appendCustomRenderer(self, renderer):
        self.customRenderers.append(renderer)

    def renderLinkText(self, objectToRender):
        return "(" + objectToRender.__class__.__module__ + "." + objectToRender.__class__.__name__ + ")"

    def renderShortText(self, objectToRender):
        if len(self.customRenderers) > 0:
            return self.customRenderers[0].renderShortText(objectToRender)

        return "(" + objectToRender.__class__.__module__ + "." + objectToRender.__class__.__name__ + ")"

    def renderLongText(self, objectToRender, pathToObject="", alternateTitle="Default View"):

        result = ""

        import types
        import wx.py.introspect as introspect

        EXCLUDE = ['MethodType', 'UnboundMethodType', 'BuiltinFunctionType', 'BuiltinMethodType']
        EXCLUDETYPES = [getattr(types, t) for t in EXCLUDE]
        Simple = ['IntType', 'FloatType', 'StringType']
        SIMPLETYPES = [getattr(types, t) for t in Simple]

        SIMPLETYPES = [getattr(types, t) for t in dir(types) \
                       if not t.startswith('_') and t not in EXCLUDE]

        del t

        if type(objectToRender) in SIMPLETYPES:
            return ""
        result += "<center><h2>%s</h2></center>" % alternateTitle
        result += "<center>%s</center><hr>" % pathToObject

        result += "<table border=\"1\">"
        for key in introspect.getAttributeNames(objectToRender):
            try:
                if key.startswith("_"):
                    continue
                sub = getattr(objectToRender, key)
                if not type(sub) in SIMPLETYPES and not type(sub) in EXCLUDETYPES:
                    result += "<tr><td>" + key + "</td><td><a href =\"_pytree_object_id#%s.%s\">%s</a></td></tr>" % (pathToObject, key, cgi.escape(str(sub)))
                    continue

                if isinstance(sub, list):
                    result +="<tr><td>" + key +"</td><td>"
                    for ii in xrange(len(sub)):
                        nav = pathToObject + "." + key + "[%d]" % ii
                        result += "<a href=\"_pytree_object_id#%s\">[%d] %s</a><br>" % (nav, ii, cgi.escape(str(sub[ii])))
                    result += "</td></tr>"
                    continue

                if isinstance(sub, dict):
                    result +="<tr><td>" + key +"</td><td>"
                    for ii in xrange(len(sub.keys())):
                        nav = pathToObject + "." + key + "[%d]" % ii
                        result += "<a href=\"_pytree_object_id#%s\">[%s] %s</a><br>" % (nav, cgi.escape(str(sub.keys()[ii])), cgi.escape(str(sub.values()[ii])))
                    result += "</td></tr>"
                    continue

                if not type(sub) in EXCLUDETYPES and not key.startswith("_"):
                    result += "<tr><td>" + key + "</td><td>" + str(type(sub)) + cgi.escape(str(sub)) + "</td></tr>"
            except:
                pass
        result += "</table>"

        for renderer in self.customRenderers:
            result += renderer.renderLongText(objectToRender, pathToObject)

        return result
