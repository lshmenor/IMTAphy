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

#todo: remove unnecessary includes

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
import imtaphy.Probes
import imtaphy.Spectrum
import imtaphy.ScenarioSupport
import imtaphy.Receiver
import random
import openwns.probebus
from openwns import dB, dBm, fromdB, fromdBm
import math

# This config is used to dump CIR and CTF data for all links in the scenario over a certain number of TTIs
# The dumping is performed by a pseudo station called dumpingNode, see bottom of file.
# The output is written in Matlab script format to a file called CIRCTFdump.m and can be read into matlab as eval('CIRCTFdump')
# The dumping node could be added to any config.py configuration (e.g., also during a normal simulation)
# Here dummy base stations "LTESenders" and dummy mobiles "LTEScanners" are configured
# The scanner/sender simulation runs in multiple rounds (numberOfRounds) and each round lasts until all 
# mobiles have received a transmission

numMS = 50 # total number of mobiles in scenario
numberOfRounds = 2

class params:
	scenario = "UMa" # "InH", "UMa", "UMi", "RMa", "SMa"
	seed = 1979

random.seed(params.seed) # this fixes the seed for Python within this config.py

# max number of TTIs needed
# we need to have as many TTIs as the cell with the most associated mobiles has mobiles
simTime = 1E42 # the simulation will terminate automatically when using the LTESender/Scanner



######## scenario params ########
wrapAround = True    # allows evaluating all cells because it virtually surrounds all cells by all others
numberOfCircles = 0  # tier of cell sites surrounding center site (0: 1 site, 1: 7 sites, 2: 19 sites)


# define number of antenna elements for hexagonal scenarios - InH will be 1x1
numTxAntennas = 1 # number of antennas (S) at the base station
numRxAntennas = 1 # number of antennas (U) at the mobile station


msSpeed = 3 
msHeight = 1.5 # meters
scenarioConfig = imtaphy.ScenarioSupport.Scenario(params.scenario, numberOfCircles, msHeight)

# choose the receiver to be used in the Scanner:
receiver = imtaphy.Receiver.MRC(logger = imtaphy.Logger.Logger("MRCReceiver"))

######## simulator setup stuff ########
WNS = openwns.Simulator(simulationModel = openwns.node.NodeSimulationModel())
openwns.setSimulator(WNS)
WNS.rng.seed = params.seed # this fixes the seed for the C++ simulator
WNS.maxSimTime = simTime
WNS.masterLogger.backtrace.enabled = False
WNS.masterLogger.enabled = True
WNS.outputStrategy = openwns.simulator.OutputStrategy.DELETE
WNS.statusWriteInterval = 30 # in seconds
WNS.probesWriteInterval = 300 # in seconds

if params.scenario == 'InH':
    probeConfig = imtaphy.Probes.ProbeConfig(params.scenario, scenarioConfig.xMax, scenarioConfig.yMax, numberOfRounds)
else:
    probeConfig = imtaphy.Probes.ProbeConfig(params.scenario, 1, 1, numberOfRounds)

if wrapAround and not (params.scenario == 'InH'):
    wrapAroundShiftVectors = imtaphy.LinkManagement.computeShiftVectors(scenarioConfig.getInterSiteDistance(), numberOfCircles)
else:
    wrapAroundShiftVectors = []


######## IMTAphy modules ########

pathloss = imtaphy.Pathloss.M2135Pathloss()
scm = imtaphy.SCM.M2135SinglePrecision(logger = imtaphy.Logger.Logger("SCM.M2135"))

spectrum = imtaphy.Spectrum.Spectrum(centerFrequencyUplinkHz = scenarioConfig.centerFreqHz,   
                                     centerFrequencyDownlinkHz = scenarioConfig.centerFreqHz, 
                                     numberOfULPRBs = 50, 
                                     numberOfDLPRBs = 50, 
                                     prbBandwidthHz = 180000)

scmLinkCriterion = "all"  
classifier = imtaphy.LinkManagement.ITUClassifier(params.scenario)
linkManager = imtaphy.LinkManagement.LinkManager(classifier = classifier,
                                                scmLinkCriterion = scmLinkCriterion,
                                                handoverMargin = "1 dB",
                                                shiftVectors = wrapAroundShiftVectors)

channelConfig = imtaphy.Channel.Channel(pathlossModel = pathloss,
                                        spatialChannelModel = scm,
                                        linkManager = linkManager,
                                        spectrum = spectrum)
openwns.simulator.OpenWNS.modules.imtaphy.channelConfig = channelConfig


######## create and place MS and BS ########

bsPositions = scenarioConfig.bsPlacer.getPositions()
i = 1;
for azimuth in scenarioConfig.getAzimuths():
    for pos in bsPositions:
        print "bsID=", i + 1, " pos=", pos
        WNS.simulationModel.nodes.append(imtaphy.Scanner.createSender(pos.x, pos.y, i, azimuth,
                                      scenarioConfig, probeConfig, numTxAntennas, txPowerdBm = 0))
        i = i + 1

if params.scenario == 'InH':
    msPositions = imtaphy.ScenarioSupport.placeMobilesUniformlyRandomlyInRectangle(numMS, bsPositions, scenarioConfig)
else:
    sites = bsPositions
    msPositions = imtaphy.ScenarioSupport.placeMobilesUniformlyRandomlyInCells(numMS, sites, scenarioConfig)

print "Number of sites:", len(bsPositions)
print "Number of mobiles:", len(msPositions)


msId = 0
for msPos in msPositions:
    print "msID=", msId + 1, " pos=", msPos
    scanner = imtaphy.Scanner.createScanner(msPos, msId, scenarioConfig, probeConfig, numRxAntennas, msSpeed, receiver)
    WNS.simulationModel.nodes.append(scanner)
    msId += 1

# add channel dumping mobile:
dumpingNode = imtaphy.Scanner.ScannerStation("dumpingStation")
dumper = imtaphy.Scanner.ChannelDumper(dumpingNode, "channelDumper", "phyTx", "phyRx")                  
dumperPhy = imtaphy.Station.StationPhy(dumpingNode, "dumperPhy", openwns.geometry.position.Position(0, 0, msHeight), imtaphy.Logger.Logger("dumperPhy") ,"phyTx", "phyRx", antenna = imtaphy.Antenna.Omnidirectional(antennaGain = "0 dB", azimuth = 0, numElements = numRxAntennas , elementSpacingMeters = 0.5*0.15, logger = imtaphy.Logger.Logger("dumperAntenna")))
WNS.simulationModel.nodes.append(dumpingNode)

