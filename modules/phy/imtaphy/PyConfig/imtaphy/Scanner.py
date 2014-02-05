################################################################################
# This file is part of IMTAphy
# _____________________________________________________________________________
#
# Copyright (C) 2010
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

import openwns.node
import openwns.geometry.position
import imtaphy.Logger
import imtaphy.Antenna
import imtaphy.Station
import math


class BaseStation(openwns.node.Node):
    def __init__(self, name):
        super(BaseStation, self).__init__(name)

class ScannerStation(openwns.node.Node):
    def __init__(self, name):
        super(ScannerStation, self).__init__(name)



class LteSender(openwns.node.Component):
    nameInComponentFactory = "imtaphy.LteSender"
    phyDataTransmission = None
    txPowerPerPRBdBm = None
    node = None
    phyDataReception = None
    bsId = None
    numberOfRounds = None

    def __init__(self, node, name, bsId, numberOfRounds, phyDataTransmission, phyDataReception, txPowerPerPRBdBm):
        super(LteSender, self).__init__(node, name)
        self.node = node
        self.phyDataTransmission = phyDataTransmission
        self.txPowerPerPRBdBm = txPowerPerPRBdBm
        self.phyDataReception = phyDataReception
        self.bsId = bsId
        self.numberOfRounds = numberOfRounds

class LteScanner(openwns.node.Component):
    node = None
    nameInComponentFactory = "imtaphy.LteScanner"
    phyDataTransmission = None
    phyDataReception = None

    wblProbeName  = None 
    sinrProbeName = None
    pathlossProbeName = None
    shadowingProbeName = None
    fastFadingProbeName = None
    avgSINRProbeName = None

    def __init__(self, node, name,  phyDataTransmission, phyDataReception,
                wblProbeName = "wbl", sinrProbeName = "sinr", pathlossProbeName = "pathloss", shadowingProbeName = "shadowing", avgSINRProbeName = "avgSINR", medianSINRProbeName = "medianSINR", fastFadingProbeName = "fastFading" ):
        super(LteScanner, self).__init__(node, name)
        self.node = node
        self.phyDataTransmission = phyDataTransmission
        self.phyDataReception = phyDataReception
        self.wblProbeName = wblProbeName
        self.sinrProbeName = sinrProbeName
        self.pathlossProbeName = pathlossProbeName
        self.shadowingProbeName = shadowingProbeName
        self.avgSINRProbeName = avgSINRProbeName
        self.medianSINRProbeName = medianSINRProbeName
        self.fastFadingProbeName = fastFadingProbeName

class ChannelDumper(LteScanner):
    nameInComponentFactory = "imtaphy.ChannelDumper"
    def __init__(self, node, name,  phyDataTransmission, phyDataReception):
        super(ChannelDumper, self).__init__(node, name,  phyDataTransmission, phyDataReception)
        

def createSender(x, y, bsId, azimuthDegree, scenarioConfig, probeConfig, numTxAntennas, txPowerdBm):
    azimuth = float(azimuthDegree) / 180.0 * math.pi
    name = "BS" + str(bsId)
    node = BaseStation(name)
    phyDataTxName = "phyTx"
    phyDataRxName = "phyRx"

    sender = LteSender(node, name + ".Sender", bsId,
               probeConfig.numberOfRounds,
               phyDataTransmission = phyDataTxName,
               phyDataReception = phyDataRxName,
               txPowerPerPRBdBm = txPowerdBm)

    bsPosition = openwns.geometry.position.Position(x = x,y = y, z = scenarioConfig.bsHeight)

    # provide the mobility component that allows wrowser to plot the station
    try:
        import rise.Mobility
        mobility = rise.Mobility.No(bsPosition)
        mobilityComponent = rise.Mobility.Component(node, name + ".Mobility", mobility)
    except:
        pass

    stationPhyLogger = imtaphy.Logger.Logger(name+".stationPhy")

    if scenarioConfig.scenarioName == "InH":
        bsAntenna = imtaphy.Antenna.Omnidirectional(numElements = 1,
                           logger = stationPhyLogger)
    else:
            bsAntenna = imtaphy.Antenna.ITU(azimuth = azimuth, downtilt = scenarioConfig.downtilt,
                        antennaGain = "17 dB",  numElements = numTxAntennas,
                        logger = stationPhyLogger)

    # Create stationPhy componenent and add it to the node
    stationPhy = imtaphy.Station.BaseStation(node, name + ".PHY",
                         bsPosition,
                         stationPhyLogger,
                         phyDataTransmissionName = phyDataTxName,
                         phyDataReceptionName = phyDataRxName,
                         antenna = bsAntenna)
    return node



def createScanner(position, msId, scenarioConfig, probeConfig, numRxAntennas, speed, receiver = None):
    name = "Scanner" + str(msId)
    node = ScannerStation(name)

    phyDataTxName = "phyTx"
    phyDataRxName = "phyRx"
    scanner = LteScanner(node, name + ".Scanner",
                 phyDataTransmission = phyDataTxName,
                 phyDataReception = phyDataRxName,
                 wblProbeName = probeConfig.widebandLossProbeName,
                 sinrProbeName = probeConfig.sinrProbeName,
                 pathlossProbeName = probeConfig.pathlossProbeName,
                 shadowingProbeName = probeConfig.shadowingProbeName,
                 avgSINRProbeName = probeConfig.avgSINRProbeName,
                 medianSINRProbeName = probeConfig.medianSINRProbeName,
                 fastFadingProbeName = probeConfig.fastFadingProbeName)

    # Create stationPhy componenent and add it to the node

    # this is the station phy component
    stationPhyLogger = imtaphy.Logger.Logger(name+".stationPhy")
    stationPhy = imtaphy.Station.ScannerStation(node, name + ".PHY",
                            position,
                            stationPhyLogger,
                            phyDataTransmissionName = phyDataTxName,
                            phyDataReceptionName = phyDataRxName,
                            speed = speed,
                            antenna = imtaphy.Antenna.Omnidirectional(numElements = numRxAntennas,
                                                  logger = stationPhyLogger),
                            receiver = receiver
                            )

    # uncomment this to provide the mobility component that allow wrowser to plot the station
    try:
        import rise.Mobility
        mobility = rise.Mobility.No(position)
        mobilityComponent = rise.Mobility.Component(node, name + ".Mobility", mobility)
    except:
        pass
    return node
