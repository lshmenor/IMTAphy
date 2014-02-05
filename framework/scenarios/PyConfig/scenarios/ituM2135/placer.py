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
import scenarios.placer
import openwns.geometry.position

class IndoorHotspotBSPlacer(scenarios.placer.LinearPlacer):

    def __init__(self):
        super(IndoorHotspotBSPlacer, self).__init__(numberOfNodes = 2, positionsList = [-30.0, 30.0], rotate = 0.0)
        self.setCenter(openwns.geometry.position.Position(1000.0, 1000.0, 0.0))

class IndoorHotspotUEPlacer(scenarios.placer.RectangularAreaPlacer):

    def __init__(self, numberOfNodes, minDistance):
        super(IndoorHotspotUEPlacer, self).__init__(numberOfNodes, w = 120.0, h = 50.0, minDistance = minDistance, perBS = False)
        
class UrbanMicroBSPlacer(scenarios.placer.HexagonalPlacer):

    def __init__(self, numberOfCircles):
        super(UrbanMicroBSPlacer,self).__init__(numberOfCircles, interSiteDistance = 200.0)
        self.setCenter(openwns.geometry.position.Position(1000.0, 1000.0, 0.0))

class UrbanMicroUEPlacer(scenarios.placer.UniformDistributedPlacerInHexagonalGrid):

    def __init__(self, numberOfNodes, numberOfCircles, minDistance):
        super(UrbanMicroUEPlacer,self).__init__(numberOfNodes, numberOfCircles, 
                                                interSiteDistance = 200.0, minDistance=minDistance)

class UrbanMacroBSPlacer(scenarios.placer.HexagonalPlacer):

    def __init__(self, numberOfCircles):
        super(UrbanMacroBSPlacer,self).__init__(numberOfCircles, interSiteDistance = 500.0)
        self.setCenter(openwns.geometry.position.Position(1000.0, 1000.0, 0.0))

class UrbanMacroUEPlacer(scenarios.placer.UniformDistributedPlacerInHexagonalGrid):

    def __init__(self, numberOfNodes, numberOfCircles, minDistance):
        super(UrbanMacroUEPlacer,self).__init__(numberOfNodes, numberOfCircles, 
                                                interSiteDistance = 500.0, minDistance=minDistance)


class RuralMacroBSPlacer(scenarios.placer.HexagonalPlacer):

    def __init__(self, numberOfCircles):
        super(RuralMacroBSPlacer,self).__init__(numberOfCircles, interSiteDistance = 1732.0)
        self.setCenter(openwns.geometry.position.Position(5000.0, 5000.0, 0.0))


class RuralMacroUEPlacer(scenarios.placer.UniformDistributedPlacerInHexagonalGrid):

    def __init__(self, numberOfNodes, numberOfCircles, minDistance):
        super(RuralMacroUEPlacer,self).__init__(numberOfNodes, numberOfCircles, 
                                                interSiteDistance = 1732.0, minDistance=minDistance)


class SuburbanMacroBSPlacer(scenarios.placer.HexagonalPlacer):

    def __init__(self, numberOfCircles):
        super(SuburbanMacroBSPlacer,self).__init__(numberOfCircles, interSiteDistance = 1299.0)
        self.setCenter(openwns.geometry.position.Position(5000.0, 5000.0, 0.0))


class SuburbanMacroUEPlacer(scenarios.placer.UniformDistributedPlacerInHexagonalGrid):

    def __init__(self, numberOfNodes, numberOfCircles, minDistance):
        super(SuburbanMacroUEPlacer,self).__init__(numberOfNodes, numberOfCircles, 
                                                   interSiteDistance = 1299.0, minDistance=minDistance)
