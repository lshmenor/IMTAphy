import openwns.toolsupport
import openwns.node

class NodeSimulationModelVisitor(openwns.toolsupport.IPyTreeVisitor):

    def renderShortText(self, o):
        return o.__class__.__module__ + "." + o.__class__.__name__

    def renderLongText(self, o, pathToObject):
        result = ""
        for ii in xrange(len(o.nodes)):
            result += openwns.toolsupport.renderLongFromObject(o.nodes[ii], pathToObject + ".nodes[%d]" % ii, "Node %s" % o.nodes[ii].name)
        return result

openwns.toolsupport.PyTreeVisitorFactory.registerVisitor(openwns.node.NodeSimulationModel, NodeSimulationModelVisitor())