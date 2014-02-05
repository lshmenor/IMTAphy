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

import openwns.node
import openwns.logger

import ltea.dll.component

import imtaphy.Logger
import imtaphy.Station
import imtaphy.Receiver

class eNB(openwns.node.Node):
    
    def __init__(self, pos, antenna, dlScheduler, ulScheduler, eNBreceiver, windowSize, settlingTime, config, processingDelay= 0.002999, fullBuffer = False):
        openwns.node.Node.__init__(self, "eNB")

        self.logger = openwns.logger.Logger("LTEA", "eNB%d" % self.nodeID, True, level=2) # 3: verbose, 2: normal, 1: quiet (only startup/shutdown)

        self.setProperty("Type", "eNB")

        self.name += str(self.nodeID)

        self.dll = ltea.dll.component.eNBLayer2(node = self, name = "eNB", dlScheduler = dlScheduler, ulScheduler = ulScheduler, 
        windowSize = windowSize, settlingTime = settlingTime, processingDelay = processingDelay, parentLogger = self.logger, fullBuffer = fullBuffer)
        
        stationPhyLogger = imtaphy.Logger.Logger(self.name + ".stationPhy")
        self.stationPhy = imtaphy.Station.StationPhy(self, self.name + ".PHY",
                                                     pos,
                                                     stationPhyLogger,
                                                     phyDataTransmissionName = "tx",
                                                     phyDataReceptionName = "rx",
                                                     stationType = "BS", #TODO: is this necessary?
                                                     antenna = antenna,
                                                     receiver = eNBreceiver)
        
        # TODO: make these loggers nicer
        antennaLogger = imtaphy.Logger.Logger(self.name + ".antenna")
        self.stationPhy.antenna.logger = antennaLogger


        #self.dll.setPhyDataTransmission(aMode.modeName, self.phys[aMode.modeName].dataTransmission)
        #self.dll.setPhyNotification(aMode.modeName, self.phys[aMode.modeName].notification)

    def setPosition(self, position):
        self.stationPhy.position.setCoords(position)      

    def getPosition(self):
        return self.stationPhy.position.getCoords()

