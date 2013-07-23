################################################################################
# This file is part of IMTAphy
# _____________________________________________________________________________
#
# Copyright (C) 2011
# Institute of Communication Networks (LKN)
# Department of Electrical Engineering and Information Technology (EE & IT)
# Technische Universitaet Muenchen
# Arcisstr. 21
# 80333 Muenchen - Germany
# http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
#
# _____________________________________________________________________________
#
#   IMTAphy is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   IMTAphy is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
#
#################################################################################

import openwns
import openwns.node
import openwns.geometry.position
import imtaphy.Station
import imtaphy.Logger
import imtaphy.Channel
import imtaphy.Pathloss
import imtaphy.Scanner
import imtaphy.LinkManagement
import imtaphy.SCM
import rise.Mobility

import openwns.probebus
from openwns import dB, dBm, fromdB, fromdBm
from openwns.evaluation import *
import math
import scenarios.ituM2135

import random

simTime = 0.1

# simulator setup stuff
WNS = openwns.Simulator(simulationModel = openwns.node.NodeSimulationModel())
openwns.setSimulator(WNS)
WNS.maxSimTime = simTime
WNS.masterLogger.backtrace.enabled = False
WNS.masterLogger.enabled = True
WNS.outputStrategy = openwns.simulator.OutputStrategy.DELETE
WNS.statusWriteInterval = 30 # in seconds
WNS.probesWriteInterval = 300 # in seconds

class params:
    seed = 1987 # seed for the random propagation condition assignment
openwns.simulator.OpenWNS.modules.imtaphy.params = params

# the channel configuration
scenario = openwns.geometry.position.BoundingBox(xmin = 0.0, xmax = 2000.0,
                                                 ymin = 0.0, ymax = 2500.0,
                                                 zmin = 0.0, zmax = 100.0)



pathloss = imtaphy.Pathloss.M2135Pathloss()
classifier = imtaphy.LinkManagement.UMa() 
m2135 = imtaphy.SCM.M2135SinglePrecision(logger = imtaphy.Logger.Logger("SCM.M2135"))
#m2135 = imtaphy.SCM.M2135(logger = imtaphy.Logger.Logger("SCM.M2135"))
scmLinkCriterion = "all"
linkManager = imtaphy.LinkManagement.LinkManager(classifier = classifier,
                        scmLinkCriterion = scmLinkCriterion,
                        handoverMargin = "1 dB")

spectrum = imtaphy.Spectrum.Spectrum(numberOfDLPRBs = 50, numberOfULPRBs = 50)


channelConfig = imtaphy.Channel.Channel( pathlossModel = pathloss, spatialChannelModel = m2135, linkManager = linkManager, spectrum = spectrum)
openwns.simulator.OpenWNS.modules.imtaphy.channelConfig = channelConfig


def createBS(x, y, id, azimuth = 0):
    azimuth = float(azimuth) / 180.0 * math.pi

    # see 3GPP TR 36.814 for downtilt examples
    # 12 degrees for UMa / UMi
    downtilt = 12.0 / 180.0 * math.pi

    #name = "Sender" + str(x) +str(y)
    name = "BS" + str(id)
    node = openwns.node.Node(name)

    # Create stationPhy componenent and add it to the node

    # this is the station phy component
    stationPhyLogger = imtaphy.Logger.Logger(name+".stationPhy")
    stationPhy = imtaphy.Station.StationPhy(node, name + ".PHY",
                           openwns.geometry.position.Position(x = x, y = y, z = 25.0),
                           stationPhyLogger,
                                           phyDataTransmissionName = "tx",
                                           phyDataReceptionName = "rx",
                                           stationType = "BS",
                                           antenna = imtaphy.Antenna.ITU(azimuth = azimuth, downtilt = downtilt,
                                        antennaGain = "17 dB",  numElements = 4,
                                        logger = stationPhyLogger))

    return node


def createMS(x, y, id):
    name = "MS"+str(id)
    node = openwns.node.Node(name)

    # Create stationPhy componenent and add it to the node

    # this is the station phy component
    stationPhyLogger = imtaphy.Logger.Logger(name+".stationPhy")
    stationPhy = imtaphy.Station.StationPhy(node, name + ".PHY",
                                           openwns.geometry.position.Position(x = x, y = y, z = 1.5),
                                           stationPhyLogger,
                                           phyDataTransmissionName = "tx",
                                           phyDataReceptionName = "rx",
                                           stationType = "MS",
                                           speed = 3.0,
                                           antenna = imtaphy.Antenna.Omnidirectional(numElements = 4,
                                            logger = stationPhyLogger))
    return node

bsPlacer = scenarios.ituM2135.UrbanMacroBSPlacer(numberOfCircles=2)
bsPlacer.setCenter(openwns.geometry.position.Position(x = 1000.0, y = 1200.0, z = 0.0))
bsPositions = bsPlacer.getPositions()
i = 1;

for azimuth in [-120.0, 0.0, 120.0] :
    for pos in bsPositions :
        WNS.simulationModel.nodes.append(createBS(pos.x, pos.y, i , azimuth))
        i = i + 1

#for pos in bsPositions :
    #WNS.simulationModel.nodes.append(createBS(pos.x, pos.y, i , 0.0))
    #i = i + 1


numMSperBS = 10
X = range(0,2000); random.shuffle(X)
Y = range(0,2500); random.shuffle(Y)

for j in range(1,(i-1)*numMSperBS+1):
    WNS.simulationModel.nodes.append(createMS(X[j], Y[j], j))
