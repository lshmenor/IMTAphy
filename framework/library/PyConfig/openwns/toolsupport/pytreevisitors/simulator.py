import openwns.toolsupport
import openwns.simulator

class OpenWNSVisitor(openwns.toolsupport.IPyTreeVisitor):

    def renderShortText(self, o):
        return o.__class__.__name__

    def renderLongText(self, o, pathToObject):
        result = openwns.toolsupport.renderLongFromObject(o.environment, pathToObject+".environment", "Simulation Environment")
        result += openwns.toolsupport.renderLongFromObject(o.simulationModel, pathToObject+".simulationModel", "Simulation Model")
        return result

openwns.toolsupport.PyTreeVisitorFactory.registerVisitor(openwns.simulator.OpenWNS, OpenWNSVisitor())

