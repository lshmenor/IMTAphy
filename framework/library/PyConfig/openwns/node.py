###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2007
# Chair of Communication Networks (ComNets)
# Kopernikusstr. 16, D-52074 Aachen, Germany
# phone: ++49-241-80-27910,
# fax: ++49-241-80-22242
# email: info@openwns.org
# www: http://www.openwns.org
# _____________________________________________________________________________
#
# openWNS is free software; you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License version 2 as published by the
# Free Software Foundation;
#
# openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

from logger import Logger
import openwns.probebus
import scenarios.interfaces

class NodeSimulationModel(object):
    __slots__ = ['nameInFactory', 'logger', 'nodes']

    def __init__(self):
        self.nameInFactory = 'wns.Node.NodeSimulationModel'
        self.logger = Logger("WNS", "NodeSimulationModel", True)
        self.nodes = []

    def getNodesByProperty(self, propertyName, propertyValue):
        """
        Get a property name and value. 
        Return a list of nodes those property of the given name is the same
        as the given value
        
        @type propertyName: str
        @param propertyName: The property name

        @type propertyValue: any type
        @param propertyValue: The property value

        @rtype:  [openwns.node.Node]
        @return: A list of nodes
        """
        r = [n for n in self.nodes if n.getProperty(propertyName) == propertyValue]
        return r

class Node(object):
    """ A Node is a list of Components
    """

    components = None
    """ List of components
    """

    nodeID = None
    """ The numerical ID for this node, aka nodeID
    """

    name = None
    """ The name of this Node (something like AP1 ...)
    """

    logger = None
    """ instance of an logger for this Node
    """

    contextProviders = None
    """ A list of context provider

    The node itself adds a ConstantContextProvider with its
    nodeID. The context is named 'wns.node.Node.id'.
    """

    id = 1

    def __init__(self, name):
        """ The name of the node has to be provided and must be unique
        """
        super(Node, self).__init__()

        self.nodeID = Node.id
        Node.id += 1

        self.contextProviders = []
        self.contextProviders.append(
            openwns.probebus.ConstantContextProvider("wns.node.Node.id", self.nodeID))
        self.contextProviders.append(
            openwns.probebus.ConstantContextProvider("openwns.node.Node.id", self.nodeID))

        self.name = name
        self.components = []
        self.logger = Logger("WNS", self.name, True)
        self.property = dict()

    def addComponent(self, component):
        """ add a component
        """
        assert component.name not in [ i.name for i in self.components ], component.name + " already known."
        self.components.append(component)

    def addConstantContextProvider(self, key, value):
        assert(type(key) == type(""))
        assert(type(value) == type(42))
        self.contextProviders.append(openwns.probebus.ConstantContextProvider(key, value))

    def getProperty(self, propertyName):
        if propertyName not in self.property:
            return
        
        return self.property[propertyName]
    
    def setProperty(self, propertyName, propertyValue):
        self.property[propertyName] = propertyValue


class NoRadio(scenarios.interfaces.INode):
    
    def setPosition(self, position):
        assert False, "This node is not a mobile radio node"

    def getPosition(self):
        assert False, "This node is not a mobile radio node"
        return None
    
    def setAntenna(self, antenna):
        assert False, "This node is not a mobile radio node"

    def getAntenna(self):
        assert False, "This node is not a mobile radio node"
        return None

    def setChannelModel(self, model):
        assert False, "This node is not a mobile radio node"


class Component(object):
    """ A particular part of a Node (like Data Link Layer). Each
        Module can contribute one or more Components to a Node
    """

    nameInComponentFactory = None
    """ The Node will ask the ComponentFactory with name for a creator
    """

    name = None
    """ The name of this Component
    """

    logger = None

    node = None

    def __init__(self, node, name):
        super(Component, self).__init__()
        self.name = name
        self.logger = Logger("WNS", self.name + str(node.nodeID), True, node.logger)
        self.node = node
        self.node.addComponent(self)

class FQSN:
    nodeName = None
    serviceName = None

    def __init__(self, node, serviceName):
        self.nodeName = node.name
        self.serviceName = serviceName

class NodeDropIn(Node):
    def __init__(self, name="The Node"):
        super(NodeDropIn, self).__init__(name)

class ComponentDropIn(Component):
    nameInComponentFactory = "None"

    def __init__(self, node, name):
        super(ComponentDropIn, self).__init__(node, name)

class TCPDropIn(Component):
    nameInComponentFactory = "wns.node.component.tests.TCP"
    ipInstance = None

    def __init__(self, node):
        super(TCPDropIn, self).__init__(node, "TCP")
        self.ipInstance = "IP"

class IPDropIn(Component):
    nameInComponentFactory = "wns.node.component.tests.IP"

    def __init__(self, node):
        super(IPDropIn, self).__init__(node, "IP")
