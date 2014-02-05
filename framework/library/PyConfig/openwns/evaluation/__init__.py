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

import openwns.simulator
import openwns.probebus
from openwns.evaluation.wrappers import *
from openwns.evaluation.generators import *
from openwns.evaluation.tree import *
from openwns.evaluation.formatters import *

class Error(Exception):
    """ Base class for exceptions in this module."""
    def __str__(self):
        return "\n\n" + self.message  
  
class SourceNotRegistered(Error):
  
    def __init__(self, sourceName, keys):
        self.message = "Source node " 
        self.message += sourceName 
        self.message += " was not yet created. Use createSourceNode()\n"
        self.message += "The following source nodes were created by now:\n"
        for s in keys():
            self.message += str(s) + "\n"

class SourceRegisteredAndHasChildren(Error):
    
    def __init__(self, sourceName):
        self.message = "Source "
        self.message += sourceName 
        self.message += " was already created and has children. "
        self.message += "Either remove the source node first by using removeMeasurementSource() "
        self.message += "or use getMeasurementSourceNode()"    

class TreeNodeProbeBusRegistry(object):

    def __init__(self, probeBusRegistry):
        self.probeBusRegistry = probeBusRegistry
        self.treeNodes = {}

    def getMeasurementSource(self, probeBusId):
        # Just forward to openwns.probebus.ProbeBusRegistry
        return self.probeBusRegistry.getMeasurementSource(probeBusId)

    def removeMeasurementSource(self, probeBusId):
        # Just forward to openwns.probebus.ProbeBusRegistry
        return self.probeBusRegistry.removeMeasurementSource(probeBusId)

    def getMeasurementSources(self):
        # Just forward to openwns.probebus.ProbeBusRegistry
        return self.probeBusRegistry.getMeasurementSources()

    def removeMeasurementSourceNode(self, measurementSource):
        self.probeBusRegistry.removeMeasurementSource(measurementSource)
        self.treeNodes.pop(measurementSource)

    def createMeasurementSourceNode(self, measurementSource):
        if not self.treeNodes.has_key(measurementSource):
            pb = self.probeBusRegistry.getMeasurementSource(measurementSource)

            wrapper = openwns.evaluation.ProbeBusWrapper(pb, measurementSource)

            node = TreeNode(wrapper)

            self.treeNodes[measurementSource] = node

        sourceNode = self.treeNodes[measurementSource]
        if sourceNode.hasChildren():
            raise SourceRegisteredAndHasChildren(measurementSource)
        
        return sourceNode

    def getMeasurementSourceNode(self, measurementSource):
        if not self.treeNodes.has_key(measurementSource):
            raise SourceNotRegistered(measurementSource, self.treeNodes.keys)

        return self.treeNodes[measurementSource]

    def getLogger(self):
        return self.probeBusRegistry.logger

    measurementSources = property(getMeasurementSources)

    logger = property(getLogger)

def createSourceNode(sim, measurementSource):
    pbr = sim.environment.probeBusRegistry

    """ If we use TreeNodes we must first replace the standard ProbeBusRegistry
    with an extended version that knows about TreeNodes
    """
    if not isinstance(pbr, openwns.evaluation.TreeNodeProbeBusRegistry):
        sim.environment.probeBusRegistry = openwns.evaluation.TreeNodeProbeBusRegistry(pbr)
        pbr = sim.environment.probeBusRegistry

    return pbr.createMeasurementSourceNode(measurementSource)

def getSourceNode(sim, measurementSource):
    pbr = sim.environment.probeBusRegistry

    """ If we use TreeNodes we must first replace the standard ProbeBusRegistry
    with an extended version that knows about TreeNodes
    """
    if not isinstance(pbr, openwns.evaluation.TreeNodeProbeBusRegistry):
        sim.environment.probeBusRegistry = openwns.evaluation.TreeNodeProbeBusRegistry(pbr)
        pbr = sim.environment.probeBusRegistry

    return pbr.getMeasurementSourceNode(measurementSource)

def printSortingTree(treenode, formatter=str, indent=0):

    pad = " " * indent
    content = treenode.getContent()
    if content is not None:
        if isinstance(content.getWrappee(), openwns.probebus.ContextFilterProbeBus):
            print pad + "|--" + contextFilterFormatter(content)
        else:
            print pad + "|--" + formatter(content.getWrappee())
    else:
        print pad + "|-- None"

    for child in treenode.children():
        printSortingTree(child, formatter, indent + 2)




    
