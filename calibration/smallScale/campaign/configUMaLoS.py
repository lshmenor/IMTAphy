# -*- coding: utf-8 -*-
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

import openwns
import openwns.node
import openwns.geometry.position
import imtaphy.Station
import imtaphy.Logger
import imtaphy.Channel
import imtaphy.Pathloss
import imtaphy.Scanner
import imtaphy.Spectrum
import imtaphy.LinkManagement
import imtaphy.ScenarioSupport
import imtaphy.SCM
import imtaphy.Probes

import openwns.probebus
from openwns import dB, dBm, fromdB, fromdBm
from openwns.evaluation import *
import math

import random

import ltea.dll.schedulers.downlink
import ltea.dll.schedulers.uplink
import ltea.dll.linkAdaptation.downlink
import ltea.dll.linkAdaptation.uplink
import ltea.evaluation.default
import ltea.helper

simTime = 0.001
class params:
	scenario = "UMa"
	propagation = "LoS"
	seed = 1987
#from openwns.wrowser.simdb.SimConfig import params

avgNumMSperBS = 5

######## scenario params ########
wrapAround = True    # allows evaluating all cells because it virtually surrounds all cells by all others
numberOfCircles = 2  # tier of cell sites surrounding center site (0: 1 site, 1: 7 sites, 2: 19 sites)

# define number of antenna elements for hexagonal scenarios - InH will be 1x1
numTxAntennas = 1 # number of antennas (S) at the base station
numRxAntennas = 1 # number of antennas (U) at the mobile station

# set both to 1 unless you want to average instantaneous SINR values over
# multiple PRBs and multiple TTIs (rounds) 
numPRBs = 1

msSpeed = 3 # 3 km/h
msHeight = 1.5 # meters
scenarioConfig = imtaphy.ScenarioSupport.Scenario(params.scenario, numberOfCircles, msHeight)


# simulator setup stuff
WNS = openwns.Simulator(simulationModel = openwns.node.NodeSimulationModel())
openwns.setSimulator(WNS)
WNS.rng.seed = params.seed # this fixes the seed for the C++ simulator
WNS.maxSimTime = simTime
WNS.masterLogger.backtrace.enabled = False
WNS.masterLogger.enabled = True
WNS.outputStrategy = openwns.simulator.OutputStrategy.DELETE
WNS.statusWriteInterval = 1 # in seconds
WNS.probesWriteInterval = 300 # in seconds

#scenario = scenarioConfig.getBoundingBox()

if wrapAround and not (params.scenario == 'InH'):
    wrapAroundShiftVectors = imtaphy.LinkManagement.computeShiftVectors(scenarioConfig.getInterSiteDistance(), numberOfCircles)
else:
    wrapAroundShiftVectors = []

calibrationOutputFileName = "../calibrationData_" + params.scenario + "_" + params.propagation + ".it"


pathloss = imtaphy.Pathloss.M2135Pathloss()
scmLinkCriterion = "all"  
scm = imtaphy.SCM.M2135SinglePrecision(logger = imtaphy.Logger.Logger("SCM.M2135"),
			calibrationOutputFileName = calibrationOutputFileName, dumpCalibrationData = True)
classifier = imtaphy.LinkManagement.StaticClassifier(params.scenario, params.propagation, params.propagation, "outdoor") #user location "outdoor" is not relevant for small scale calibration
linkManager = imtaphy.LinkManagement.LinkManager(classifier = classifier,
						 scmLinkCriterion = scmLinkCriterion,
						 handoverMargin = "1 dB",
						 shiftVectors = wrapAroundShiftVectors)
spectrum = imtaphy.Spectrum.Spectrum(centerFrequencyUplinkHz = scenarioConfig.centerFreqHz,   # TODO: different frequencies for UL/DL?
                                     centerFrequencyDownlinkHz = scenarioConfig.centerFreqHz, )
channelConfig = imtaphy.Channel.Channel(pathlossModel = pathloss,
                                        spatialChannelModel = scm,
                                        linkManager = linkManager,
                                        spectrum = spectrum)
openwns.simulator.OpenWNS.modules.imtaphy.channelConfig = channelConfig


######## create and place MS and BS ########

def createBS(x, y, id, azimuth = 0):
    azimuth = float(azimuth) / 180.0 * math.pi

    #name = "Sender" + str(x) +str(y)
    name = "BS" + str(id)
    node = openwns.node.Node(name)



    # this is the station phy component
    stationPhyLogger = imtaphy.Logger.Logger(name+".stationPhy")
    if params.scenario == "InH":
        antenna = imtaphy.Antenna.Omnidirectional(numElements = 1, logger = stationPhyLogger)
    else:
        antenna = imtaphy.Antenna.ITU(azimuth = azimuth, downtilt = scenarioConfig.downtilt,
                                      antennaGain = "17 dB",  numElements = 1,
                                      logger = stationPhyLogger)
    stationPhy = imtaphy.Station.StationPhy(node, name + ".PHY",
                                            openwns.geometry.position.Position(x = x, y = y, z = scenarioConfig.bsHeight),
                                            stationPhyLogger,
                                            phyDataTransmissionName = "tx",
                                            phyDataReceptionName = "rx",
                                            stationType = "BS",
                                            antenna = antenna)
    return node


def createMS(x, y, id):
	name = "MS"+str(id)
	node = openwns.node.Node(name)

	# this is the station phy component
	stationPhyLogger = imtaphy.Logger.Logger(name+".stationPhy")
	stationPhy = imtaphy.Station.StationPhy(node, name + ".PHY", 
                                            openwns.geometry.position.Position(x = x, y = y, z = msHeight),
                                            stationPhyLogger, 
                                            phyDataTransmissionName = "tx", 
                                            phyDataReceptionName = "rx", 
                                            stationType = "MS", 
                                            speed = 3.0, 
                                            antenna = imtaphy.Antenna.Omnidirectional(numElements = 1, logger = stationPhyLogger))
	return node


bsPositions = scenarioConfig.bsPlacer.getPositions()
if params.scenario == 'InH':
    msPositions = imtaphy.ScenarioSupport.placeMobilesUniformlyRandomlyInRectangle(2 * avgNumMSperBS * 20, bsPositions, scenarioConfig)
else:
    msPositions = imtaphy.ScenarioSupport.placeMobilesUniformlyRandomlyInCells(len(bsPositions) * 3 * avgNumMSperBS, 
                                                                               bsPositions, scenarioConfig)

print "Number of sites:", len(bsPositions)
print "Number of mobiles:", len(msPositions)


bsId = 0;
for azimuth in scenarioConfig.getAzimuths():
    for pos in bsPositions:
        print "bsID=", bsId + 1, " pos=", pos
        WNS.simulationModel.nodes.append(createBS(pos.x, pos.y, bsId+1 , azimuth))
        bsId = bsId + 1


msId = 0
for msPos in msPositions:
    print "msID=", msId + 1, " pos=", msPos
    WNS.simulationModel.nodes.append(createMS(msPos.x, msPos.y, msId))
    msId += 1


