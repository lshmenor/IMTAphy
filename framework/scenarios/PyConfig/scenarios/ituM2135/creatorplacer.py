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

import scenarios.builders
import scenarios.ituM2135
import scenarios.channelmodel

class CreatorPlacerBuilderIndoorHotspot(scenarios.builders.CreatorPlacerBuilder):

    def __init__(self, bsCreator,  utCreator, numberOfNodes = 10, transceiverTypes = scenarios.channelmodel.defaultPairs):
      super(CreatorPlacerBuilderIndoorHotspot, self).__init__(bsCreator,  
                                                              scenarios.ituM2135.IndoorHotspotBSPlacer(), 
                                                              scenarios.ituM2135.IndoorHotspotAntennaCreator(),
                                                              utCreator,
                                                              scenarios.ituM2135.IndoorHotspotUEPlacer(numberOfNodes, minDistance=3), 
                                                              scenarios.ituM2135.IndoorHotspotChannelModelCreator(transceiverTypes))


class CreatorPlacerBuilderUrbanMicro(scenarios.builders.CreatorPlacerBuilder):

    def __init__(self, bsCreator, utCreator,  sectorization, 
                    numberOfCircles = 2, numberOfNodes = 30, transceiverTypes = scenarios.channelmodel.defaultPairs):
      super(CreatorPlacerBuilderUrbanMicro, self).__init__(bsCreator,  
                                                           scenarios.ituM2135.UrbanMicroBSPlacer(numberOfCircles),
                                                           scenarios.ituM2135.UrbanMicroAntennaCreator(sectorization),
                                                           utCreator, 
                                                           scenarios.ituM2135.UrbanMicroUEPlacer(numberOfNodes,
                                                                                                 numberOfCircles,
                                                                                                 minDistance=10),
                                                           scenarios.ituM2135.UrbanMicroChannelModelCreator(transceiverTypes))




class CreatorPlacerBuilderUrbanMacro(scenarios.builders.CreatorPlacerBuilder):

    def __init__(self, bsCreator, utCreator, sectorization, 
                    numberOfCircles = 2, numberOfNodes = 30, transceiverTypes = scenarios.channelmodel.defaultPairs):
      super(CreatorPlacerBuilderUrbanMacro, self).__init__(bsCreator,  
                                                           scenarios.ituM2135.UrbanMacroBSPlacer(numberOfCircles), 
                                                           scenarios.ituM2135.UrbanMacroAntennaCreator(sectorization),
                                                           utCreator,
                                                           scenarios.ituM2135.UrbanMacroUEPlacer(numberOfNodes,
                                                                                                 numberOfCircles,
                                                                                                 minDistance=25),
                                                           scenarios.ituM2135.UrbanMacroChannelModelCreator(transceiverTypes))

class CreatorPlacerBuilderRuralMacro(scenarios.builders.CreatorPlacerBuilder):

    def __init__(self, bsCreator, utCreator,  sectorization, 
                    numberOfCircles = 2, numberOfNodes = 30, transceiverTypes = scenarios.channelmodel.defaultPairs):
      super(CreatorPlacerBuilderRuralMacro, self).__init__(bsCreator,
                                                           scenarios.ituM2135.RuralMacroBSPlacer(numberOfCircles),
                                                           scenarios.ituM2135.RuralMacroAntennaCreator(sectorization),
                                                           utCreator,
                                                           scenarios.ituM2135.RuralMacroUEPlacer(numberOfNodes,
                                                                                                 numberOfCircles,
                                                                                                 minDistance=35),
                                                           scenarios.ituM2135.RuralMacroChannelModelCreator(transceiverTypes))

class CreatorPlacerBuilderSuburbanMacro(scenarios.builders.CreatorPlacerBuilder):

    def __init__(self, bsCreator, utCreator,  sectorization, 
                    numberOfCircles = 2, numberOfNodes = 30, transceiverTypes = scenarios.channelmodel.defaultPairs):
      super(CreatorPlacerBuilderSuburbanMacro, self).__init__(bsCreator,  
                                                              scenarios.ituM2135.SuburbanMacroBSPlacer(numberOfCircles),
                                                              scenarios.ituM2135.SuburbanMacroAntennaCreator(sectorization),
                                                              utCreator,
                                                              scenarios.ituM2135.SuburbanMacroUEPlacer(numberOfNodes,
                                                                                                       numberOfCircles,
                                                                                                       minDistance=35),
                                                              scenarios.ituM2135.SuburbanMacroChannelModelCreator(transceiverTypes))

