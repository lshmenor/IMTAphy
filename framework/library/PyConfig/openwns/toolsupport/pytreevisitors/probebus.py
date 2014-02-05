import openwns.toolsupport
import openwns.probebus

class MeasurementSourceVisitor(openwns.toolsupport.IPyTreeVisitor):

    def renderShortText(self, o):
        return o.__class__.__module__ + "." + o.__class__.__name__

    def getTree(self, o, indent=""):
        result = ""
        for observer in o.observers:
            
            v = openwns.toolsupport.PyTreeVisitorFactory.getVisitor(observer)
            s = v.renderShortText(observer)

            if s == "":
                s = str(self.renderShortText(observer))

            result += indent + s + "<br>"
            result += self.getTree(observer, indent.replace("+---", "&nbsp;&nbsp;&nbsp;&nbsp;") +"+---")
        return result

    def renderLongText(self, o, pathToObject):
        result = "<br><font size=\"+2\">Evaluation Tree</font><br><br>"
        result += self.getTree(o)
        return result

class ContextFilterProbeBusVisitor(openwns.toolsupport.IPyTreeVisitor):

    def renderShortText(self, o):
        return "if %s in %s" % (o.idName, str(o.idValues))

    def renderLongText(self, o, pathToObject):
        result =  "ContextFilterProbeBus accepts measurements if<br>"
        result += "Context Entry : %s<br>" % o.idName
        result += "is equal to : %s<br>" % o.idValues
        return result

class StatEvalProbeBusVisitor(openwns.toolsupport.IPyTreeVisitor):

    def renderShortText(self, o):
        v = openwns.toolsupport.PyTreeVisitorFactory.getVisitor(o.statEval)
        if len(v.customRenderers) > 0:
            s = v.renderShortText(o.statEval)
        else:
            s = o.statEval.__class__.__module__ + "." + o.statEval.__class__.__name__ + " ("+ (o.outputFilename) + ")"
        return s

    def renderLongText(self, o, pathToObject):
        v = openwns.toolsupport.PyTreeVisitorFactory.getVisitor(o.statEval)
        s = v.renderLongText(o.statEval)
        return s

class SettlingTimeGuardProbeBusVisitor(openwns.toolsupport.IPyTreeVisitor):

    def renderShortText(self, o):
        return "Start logging after %f seconds" % o.settlingTime

    def renderLongText(self, o, pathToObject):
        return ""

openwns.toolsupport.PyTreeVisitorFactory.registerVisitor(openwns.probebus.MeasurementSource, MeasurementSourceVisitor())
openwns.toolsupport.PyTreeVisitorFactory.registerVisitor(openwns.probebus.ContextFilterProbeBus, ContextFilterProbeBusVisitor())
openwns.toolsupport.PyTreeVisitorFactory.registerVisitor(openwns.probebus.StatEvalProbeBus, StatEvalProbeBusVisitor())
openwns.toolsupport.PyTreeVisitorFactory.registerVisitor(openwns.probebus.SettlingTimeGuardProbeBus, SettlingTimeGuardProbeBusVisitor())