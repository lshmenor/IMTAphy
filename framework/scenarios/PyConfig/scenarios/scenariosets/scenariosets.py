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

import scenarios.interfaces

import openwns.simulator
import openwns.node

import scenarios.builders
import scenarios.placer
import scenarios.antenna
import scenarios.traffic
import scenarios.binding


class CreateScenarioSets(object):
    def __init__(self, bscreator, uecreator, qosClass, trafficOffset=0.05, trafficDirection="UL"):

        self.qosClass = qosClass
        self.trafficDirection = trafficDirection
        self.bscreator = bscreator
        self.uecreator = uecreator
        self.trafficOffset = trafficOffset



class CreateIndoorHotspotSets(CreateScenarioSets):
    def __init__(self, bscreator, uecreator, qosClass, numOfNodesPerCell=10, trafficOffset=0.05, trafficDirection="UL"):
        super(CreateIndoorHotspotSets,self).__init__(bscreator,uecreator,qosClass,trafficOffset,trafficDirection)
        self.numOfNodesPerCell = numOfNodesPerCell
        self.scenario = scenarios.builders.CreatorPlacerBuilderIndoorHotspot(self.bscreator, self.uecreator, self.numOfNodesPerCell )


        self.MyBinding = scenarios.binding.binding(qosClass=self.qosClass)
        scenarios.traffic.addTraffic(self.MyBinding,
                                     scenarios.traffic.VoIP(offset=self.trafficOffset),
                                     self.trafficDirection)
    def getScenario(self):
        return self.scenario



class CreateUrbanMicroSets(CreateScenarioSets):


    def __init__(self, bscreator, uecreator, qosClass, numOfCircles=2, numOfNodesPerCell=10, trafficOffset=0.05, trafficDirection="UL"):
        super(CreateUrbanMicroSets,self).__init__(bscreator,uecreator,qosClass,trafficOffset,trafficDirection)
        self.numOfCircles = numOfCircles
        self.numOfNodesPerCell = numOfNodesPerCell
        self.scenario = scenarios.builders.CreatorPlacerBuilderUrbanMicro(self.bscreator, self.uecreator, self.numOfCircles, self.numOfNodesPerCell )
   
        self.MyBinding = scenarios.binding.binding(qosClass=self.qosClass)
        scenarios.traffic.addTraffic(self.MyBinding,
                                     scenarios.traffic.VoIP(offset=self.trafficOffset),
                                     self.trafficDirection)

    def getScenario(self):
        return self.scenario


class CreateUrbanMacroSets(CreateScenarioSets):


    def __init__(self, bscreator, uecreator, qosClass, numOfCircles=2, numOfNodesPerCell=10, trafficOffset=0.05, trafficDirection="UL"):
        super(CreateUrbanMacroSets,self).__init__(bscreator,uecreator,qosClass, trafficOffset,trafficDirection)

        self.numOfCircles = numOfCircles
        self.numOfNodesPerCell = numOfNodesPerCell
        self.scenario = scenarios.builders.CreatorPlacerBuilderUrbanMacro(self.bscreator, self.uecreator, self.numOfCircles, self.numOfNodesPerCell )

        self.MyBinding = scenarios.binding.binding(qosClass=self.qosClass)
        scenarios.traffic.addTraffic(self.MyBinding,
                                     scenarios.traffic.VoIP(offset=self.trafficOffset),
                                     self.trafficDirection)

    def getScenario(self):
        return self.scenario


class CreateRuralMacroSets(CreateScenarioSets):


    def __init__(self, bscreator, uecreator, qosClass,  numOfCircles=2, numOfNodesPerCell=10, trafficOffset=0.05, trafficDirection="UL"):
        super(CreateuralMacroSets,self).__init__(bscreator,uecreator,qosClass, trafficOffset,trafficDirection)

        self.numOfCircles = numOfCircles
        self.numOfNodesPerCell = numOfNodesPerCell
        self.scenario = scenarios.builders.CreatorPlacerBuilderRuralMacro(self.bscreator, self.uecreator, self.numOfCircles, self.numOfNodesPerCell )

        self.MyBinding = scenarios.binding.binding(qosClass=self.qosClass)
        scenarios.traffic.addTraffic(self.MyBinding,
                                     scenarios.traffic.VoIP(offset=self.trafficOffset),
                                     self.trafficDirection)

    def getScenario(self):
        return self.scenario


class CreateSuburbanMacroSets(CreateScenarioSets):


    def __init__(self, bscreator, uecreator, qosClass,  numOfCircles=2, numOfNodesPerCell=10, trafficOffset=0.05, trafficDirection="UL"):
        super(CreateSuburbanMacroSets,self).__init__(bscreator,uecreator,qosClass, trafficOffset,trafficDirection)

        self.numOfCircles = numOfCircles
        self.numOfNodesPerCell = numOfNodesPerCell
        self.scenario = scenarios.builders.CreatorPlacerBuilderSuburbanMacro(self.bscreator, self.uecreator, self.numOfCircles, self.numOfNodesPerCell )

        self.MyBinding = scenarios.binding.binding(qosClass=self.qosClass)
        scenarios.traffic.addTraffic(self.MyBinding,
                                     scenarios.traffic.VoIP(offset=self.trafficOffset),
                                     self.trafficDirection)

    def getScenario(self):
        return self.scenario
