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
import imtaphy.ScenarioSupport
import imtaphy.Antenna
import imtaphy.Logger
import imtaphy.Receiver
import imtaphy.covarianceEstimation
import imtaphy.channelEstimation

import imtaphy.Feedback

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



simTime = 0.11          # total simulation duration in seconds; choose simTime slightly larger than setting + N*windowSize
windowSize = 0.0750      # window size during which to measure, e.g., throughput
settlingTime = 0.0250    # time at the beginning during which no measurements are taken; windowing starts after settling time

# makes the UEs (see end of file) probe time/frequency samples of the channel gain
# and installs suitable probes
# visualize channels with, e.g. /testConfigs/plotChannel.py output/channelGain_UE3_scmLinkId0_antennaPair00_max.m 
dumpChannel = False

# for plotting a scenario view (e.g. SINR / geometry over area)
# enables suitable probes and places mobiles on a uniform grid to sample whole area
plotting = False
# define the resolution for the grid in x and y direction
class probeConfig:
    xBins = 25
    yBins = 25

# dumps a trace file of all received uplink and downlink transmissions to the output
# directory; can be viewed with IMTAphyViewer. Disabled by default. See bottom of
# config for further options (e.g. restricting to certain cells for speed/file size reasons)
phyTracing = False


# When running standalone, comment the "from openw..." import
# When running a campaign, uncomment the import statement and comment the 2 lines
# class params: 
#     pass
# For a campaign, comment the params definitions that are set in the campaign config.

#from openwns.wrowser.simdb.SimConfig import params
class params:
    pass

params.fdd = "DL" # "DL", "DUPLEX"
params.scenario = "UMa" # "InH", "UMa", "UMi", "RMa", "SMa"
params.scmLinks = "all" #"serving" "all" or "no"
params.seed = 42
params.fullBuffer = True

if not params.fullBuffer:
    params.offeredDLtrafficBps = 1E7 #
    params.offeredULtrafficBps = 1E7 #
    packetSize = 500 # bytes

params.receiver = "MRC" #"NoFilter" # "MMSE" # "MRC"
params.numBSAntennas = 2
params.numMSAntennas = 2
params.numMSperBS = 10
params.msSpeed = 0 # speed in km/h, negative values (msSpeed < 0)  means scenario-specific default speed
params.numULPRBs = 50
params.numDLPRBs = 50
params.feedbackDelay = 6
params.cqiUpdateFrequency = 5
params.dlScheduler = "ProportionalFair" #"ProportionalFair" # "ZF""ProportionalFair" #"PU2RC" # ProportionalFair "RoundRobin"
params.pfAlpha = 0.001 # ProportionalFair scheduling fairness tuner with 0 => greedy, 1 => fair
params.laThreshold = 0 #positive value in dB => more conservative link-adaptation
params.precodingMode = "ClosedLoopCodebookBased" #"SingleAntenna" #"NoPrecoding", "ClosedLoopCodebookBased"
params.fixedPMIs = False # true: assign fixed PMIs to each PRB, see below
params.outdoorOnlyUMiLoS = True # assign UMi LoS probabiltiy on outdoor part of distance only. 3GPP pathgain+geometry assumes False, otherwise True is used
params.powerControl = "calibration" # "calibration" or "3GPPdefault"
params.thresholdUL = 0 # uplink LA offset in dB
params.adaptiveUplinkLA = True
params.bsAntennaConfiguration = "C" #"BASESTATIONITU" # "BASESTATIONITU", or "A", "B", "C", "D", "E" for the corresponding 3GPP configs from 36.814
params.channelEstimation = "perfect" # "thermalNoiseBased", "IandNCovarianceBased" with further parameters see below 
params.covarianceEstimation = "perfect"# "Wishart32.829" #  "Wishart32.829" "None" "equalDiagonal", "perfect", "gaussianError" and "distinguish" (with further parameters)
params.maxRank = 4 # affectes MMSE only: 0 means determine from min(numRx,numTx) antennas; MRC is rank 1 by default

params.pmis = 5 # 1,2, 3, 4, 5, or 15

numberOfCircles = 1 # tier of cell sites surrounding center site (0: 1 site, 1: 7 sites, 2: 19 sites)

random.seed(params.seed) # this fixes the seed for Python within this config.py

# simulator setup stuff
WNS = openwns.Simulator(simulationModel = openwns.node.NodeSimulationModel())
openwns.setSimulator(WNS)
WNS.maxSimTime = simTime
WNS.rng.seed = params.seed # this fixes the seed for the C++ simulator#
WNS.masterLogger.backtrace.enabled = False
WNS.masterLogger.enabled = True #False 
WNS.outputStrategy = openwns.simulator.OutputStrategy.DELETE
WNS.statusWriteInterval = 30 # in seconds
WNS.probesWriteInterval = 3600 # in seconds

######## scenario params ########
wrapAround = True    # allows evaluating all cells because it virtually surrounds all cells by all others

msHeight = 1.5 # meters
scenarioConfig = imtaphy.ScenarioSupport.Scenario(params.scenario, numberOfCircles, msHeight)

if plotting:
    scenarioConfig.extendBoundingBoxToMultiplesOf(probeConfig.xBins, probeConfig.yBins)

if params.msSpeed < 0:
    msSpeedKmh = scenarioConfig.msSpeedKmh
else:
    msSpeedKmh = params.msSpeed

if wrapAround and not (params.scenario == 'InH'):
    wrapAroundShiftVectors = imtaphy.LinkManagement.computeShiftVectors(scenarioConfig.getInterSiteDistance(), numberOfCircles)
else:
    wrapAroundShiftVectors = []

# "scenario" is the variable the wrowser looks for to display the scenario
scenario = scenarioConfig.getBoundingBox()

if params.receiver == "MMSE":
    filter = imtaphy.Receiver.MMSEFilter(maxRank = params.maxRank)
#    covarianceEstimation = imtaphy.covarianceEstimation.Diagonal()
elif params.receiver == "MMSE-IRC":
    filter = imtaphy.Receiver.MMSEFilter(maxRank = params.maxRank)
#    covarianceEstimation = imtaphy.covarianceEstimation.Perfect()
elif params.receiver == "MRC":
    filter = imtaphy.Receiver.MRCFilter()
    # actually, the MRC doees not care about the covariance
#    covarianceEstimation = imtaphy.covarianceEstimation.Diagonal()
else:
    raise Exception("Bad receiver filter option")


#covarianceEstimation = imtaphy.covarianceEstimation.GaussianError(relativeError_dB = 0.0)
#channelEstimation = imtaphy.channelEstimation.ThermalNoiseBasedGaussianError(errorPowerRelativeToNoise_dB = 3)

if params.channelEstimation == "perfect":
    channelEstimation = None
elif params.channelEstimation ==  "thermalNoiseBased":
    channelEstimation = imtaphy.channelEstimation.ThermalNoiseBasedGaussianError(errorPowerRelativeToNoise_dB = 3)
elif params.channelEstimation == "IandNCovarianceBased":
    channelEstimation = imtaphy.channelEstimation.IandNCovarianceBasedGaussianError(gainOverIandN_dB = 10, coloredEstimationError = False)  
else:
    raise Exception("Bad channel estimation option")

if params.covarianceEstimation == "diagonal":
    covarianceEstimation = imtaphy.covarianceEstimation.Diagonal()
elif params.covarianceEstimation == "equalDiagonal":
    covarianceEstimation = imtaphy.covarianceEstimation.EqualDiagonal()
elif params.covarianceEstimation == "perfect":
    covarianceEstimation = imtaphy.covarianceEstimation.Perfect()
elif params.covarianceEstimation == "gaussianError":
    # 0 means error as big as I+N cov itself, negative values mean smaller error
    covarianceEstimation = imtaphy.covarianceEstimation.GaussianError(relativeError_dB = 0)
elif params.covarianceEstimation == "Wishart32.829":
    covarianceEstimation = imtaphy.covarianceEstimation.WishartModel36829(numberOfSamples = 16)
elif params.covarianceEstimation == "distinguish":
    covarianceEstimation = imtaphy.covarianceEstimation.IntraAndInterCellDistinguisher(interCellEstimation = imtaphy.covarianceEstimation.WishartModel36829(numberOfSamples = 16),
                                                                                       intraCellEstimation = None)
else:
    raise Exception("Bad covariance estimation option")    
  
ueReceiver = imtaphy.Receiver.LinearReceiver(imtaphy.Logger.Logger(params.receiver), filter = filter, noiseFigure = "7 dB", 
                                             channelEstimation = channelEstimation, covarianceEstimation = covarianceEstimation)
eNBreceiver = imtaphy.Receiver.LinearReceiver(imtaphy.Logger.Logger(params.receiver), filter = filter, noiseFigure = "5 dB", 
                                              channelEstimation = channelEstimation, covarianceEstimation = covarianceEstimation)

    
feederLoss = 0 # for wideband calibration, set to 2 dB 
pathloss = imtaphy.Pathloss.M2135Pathloss(feederLoss = feederLoss)
classifier = imtaphy.LinkManagement.ITUClassifier(params.scenario, onlyOutdoorDistanceUMi = params.outdoorOnlyUMiLoS)

if params.scmLinks == "no":
    scm = imtaphy.SCM.No()
    linkManager = imtaphy.LinkManagement.LinkManager(classifier = classifier,
                                                     scmLinkCriterion = "none",
                                                     handoverMargin = "1 dB",
                                                     shiftVectors = wrapAroundShiftVectors,
                                                     useSCMforRSRP = False)
else:
    scm = imtaphy.SCM.M2135SinglePrecision(logger = imtaphy.Logger.Logger("SCM.M2135"), computeEffectiveAntennaGains = False)
    linkManager = imtaphy.LinkManagement.LinkManager(classifier = classifier,
                                                     scmLinkCriterion = params.scmLinks,
                                                     handoverMargin = "1 dB",
                                                     shiftVectors = wrapAroundShiftVectors,
                                                     useSCMforRSRP = False)
                                                     
# in case only DL or UL are used, make sure the other direction does not eat too  many simulator resources
if (params.fdd == "DL"):
    params.numULPRBs = 0
    params.offeredULtrafficBps = 1E-10 # setting it to 0 does not work, triggers division by 0
if (params.fdd == "UL"):
    params.numDLPRBs = 0
    params.offeredDLtrafficBps = 1E-10 # setting it to 0 does not work, triggers division by 0

spectrum = imtaphy.Spectrum.Spectrum(centerFrequencyUplinkHz = scenarioConfig.centerFreqHz,   # TODO: different frequencies for UL/DL?
                                     centerFrequencyDownlinkHz = scenarioConfig.centerFreqHz, 
                                     numberOfULPRBs = params.numULPRBs, 
                                     numberOfDLPRBs = params.numDLPRBs, 
                                     prbBandwidthHz = 180000)

channelConfig = imtaphy.Channel.Channel(pathlossModel = pathloss,
                                        spatialChannelModel = scm,
                                        linkManager = linkManager,
                                        spectrum = spectrum)


if (params.fdd=="DL") or (params.fdd=="DUPLEX"):
    if params.dlScheduler == "PU2RC" or params.dlScheduler == "ZF":
        if params.pmis == 1:
            pmis = [0]
        elif params.pmis == 2:
            pmis = [0, 1]
        elif params.pmis == 3:
            pmis = [0, 1, 4]
        elif params.pmis == 4:
            pmis = [0, 1, 4, 5]
        elif params.pmis == 5:
            pmis = [0, 1, 4, 5, 12]
        else:
            pmis = range(16)
        openwns.simulator.OpenWNS.modules.imtaphy.downlinkFeedbackManager = imtaphy.Feedback.PU2RCFeedbackManager(enabled = True,
                                                                                                                  pmis = pmis,
                                                                                                                    precodingMode = params.precodingMode,
                                                                                                                    numPrbsPerSubband = 1,
                                                                                                                    cqiUpdateFrequency = params.cqiUpdateFrequency, #5,
                                                                                                                    rankUpdateFrequency = 10,
                                                                                                                    feedbackTotalDelay = params.feedbackDelay) #6
        from openwns.evaluation import *
        node = openwns.evaluation.createSourceNode(WNS, "groupSize")
        settling = node.appendChildren(SettlingTimeGuard(settlingTime=settlingTime))
        settling.appendChildren(PDF(name = "Group Size",
                            description = "Group Size",
                            minXValue = 1,
                            maxXValue = 4,
                            resolution = 3))
        if params.dlScheduler == "PU2RC":                        
            node = openwns.evaluation.createSourceNode(WNS, "imperfectTransmissionRatio")
            settling = node.appendChildren(SettlingTimeGuard(settlingTime=settlingTime))
            settling.appendChildren(PDF(name = "Ratio of imperfect transmission resources",
                                description = "Ratio of imperfect transmission resources",
                                minXValue = 0,
                                maxXValue = 1,
                                resolution = 200))
            node = openwns.evaluation.createSourceNode(WNS, "imperfectRetransmissionRatio")
            settling = node.appendChildren(SettlingTimeGuard(settlingTime=settlingTime))
            settling.appendChildren(PDF(name = "Ratio of imperfect retransmission resources",
                                description = "Ratio of imperfect retransmission resources",
                                minXValue = 0,
                                maxXValue = 1,
                                resolution = 200))
            node = openwns.evaluation.createSourceNode(WNS, "initialFillLevel")
            settling = node.appendChildren(SettlingTimeGuard(settlingTime=settlingTime))
            settling.appendChildren(PDF(name = "Percentage of resources allocated after initial scheduling",
                                description = "Percentage of resources allocated after initial scheduling",
                                minXValue = 0,
                                maxXValue = 1,
                                resolution = 200))
    else:
        if params.fixedPMIs:
            openwns.simulator.OpenWNS.modules.imtaphy.downlinkFeedbackManager = imtaphy.Feedback.FixedPMIPRBFeedbackManager(enabled = True,
                                                                                                                            pmis = range(8),
                                                                                                                            randomize = False,
                                                                                                                            fixedRank = 1,
                                                                                                                            numPrbsPerSubband = 2,
                                                                                                                            cqiUpdateFrequency = params.cqiUpdateFrequency, #5,
                                                                                                                            rankUpdateFrequency = 10,
                                                                                                                            feedbackTotalDelay = params.feedbackDelay) #6
        else:   

            openwns.simulator.OpenWNS.modules.imtaphy.downlinkFeedbackManager = imtaphy.Feedback.LTERel8DownlinkFeedbackManager(enabled = True,
                                                                                                                                precodingMode = params.precodingMode,
                                                                                                                                numPrbsPerSubband = 2,
                                                                                                                                cqiUpdateFrequency = params.cqiUpdateFrequency, #5,
                                                                                                                                rankUpdateFrequency = 10,
                                                                                                                                feedbackTotalDelay = params.feedbackDelay) #6
if (params.fdd=="UL") or (params.fdd=="DUPLEX"):
    openwns.simulator.OpenWNS.modules.imtaphy.uplinkStatusManager = imtaphy.Feedback.LTERel10UplinkChannelStatusManager(enabled = True,
                                                                                                                        precodingMode = "NoPrecoding", # or "NoPrecoding" #TODO: make it SingleAntenna
                                                                                                                        srsUpdateFrequency = 5,
                                                                                                                        statusDelay = 3) # together with 4 TTIs scheduling delay gives 7 TTIs total delay (cf. 36.814)
                                                  

# Channel and (currently also) the feedback manager are singletons so we put their configs into the IMTAphy module itself
openwns.simulator.OpenWNS.modules.imtaphy.channelConfig = channelConfig

bsPositions = scenarioConfig.bsPlacer.getPositions()

# The queues should not be too big to avoid using to much memory for storing outgoing packets
# 75376 is the biggest TB size for a single layer with 110 PRBs  (20 MHz spectrum) so this should be enough:
maxQueueSize = 75376 * min([params.numMSAntennas, params.numMSAntennas]) * params.numDLPRBs / 100

azimuths = scenarioConfig.getAzimuths()

for pos in bsPositions:
    for azimuth in  azimuths:
        # see 3GPP TR 36.814 for downtilt examples
        # 12 degrees for UMa / UMi
        downtilt = scenarioConfig.downtilt
        pos.z = scenarioConfig.bsHeight
        
        if params.scenario == "InH":
            antenna =  imtaphy.Antenna.Omnidirectional(type = params.bsAntennaConfiguration,
                                        antennaGain = "0 dB", azimuth = math.radians(azimuth), numElements = params.numBSAntennas,
                                        wavelengthMeters = scenarioConfig.wavelengthMeters)
        else:
            antenna =  imtaphy.Antenna.IMTAdvancedAntenna(type = params.bsAntennaConfiguration, azimuth = math.radians(azimuth), 
                                        downtilt = downtilt, antennaGain = "17 dB",  numElements = params.numBSAntennas,
                                        wavelengthMeters = scenarioConfig.wavelengthMeters)
                

        if params.dlScheduler != "RoundRobin":
            # This is an outer-loop link-adaptation module that dynamically adapts an individual threshold per user based on HARQ ACK/NACK feedback
            # In the default config it aims at a first attempt BLER of 10%. For very slow speeds, smaller BLER targets and for higher speeds higher BLER targets might perform better
            # Adaptive LA usually performs better than static for most schedulers (e.g., PF) but for TD-RR (e.g., in IMT-A calibration) at higher speeds, static is better
            linkAdaptation = ltea.dll.linkAdaptation.downlink.BLERadaptive(threshold_dB = params.laThreshold, offsetDelta = 0.03, updateInterval = 5, targetBLER = 0.1, 
                                                                        rel8Ports = 2, rel10Ports = 0)                                                                       
        else: # no dynamic outer-loop link-adaptation, just add a static threshold
            linkAdaptation = ltea.dll.linkAdaptation.downlink.SINRThreshold(threshold_dB = params.laThreshold, 
                                                                        rel8Ports = 2, rel10Ports = 0)
            

        if params.dlScheduler == "ProportionalFair":
            dlScheduler = ltea.dll.schedulers.downlink.ProportionalFair(linkAdaptation, txPowerdBmPerPRB = scenarioConfig.bsPerPRBTxPowerdBm, throughputSmoothing = params.pfAlpha, queueSize = maxQueueSize, syncHARQ = True)   
        elif params.dlScheduler == "PU2RC":
            dlScheduler = ltea.dll.schedulers.downlink.PU2RCScheduler(linkAdaptation, txPowerdBmPerPRB = scenarioConfig.bsPerPRBTxPowerdBm, throughputSmoothing = params.pfAlpha, estimateOther="PERFECT", queueSize = maxQueueSize, syncHARQ = False)   
        elif params.dlScheduler == "ZF":
            dlScheduler = ltea.dll.schedulers.downlink.ZFScheduler(linkAdaptation, txPowerdBmPerPRB = scenarioConfig.bsPerPRBTxPowerdBm, throughputSmoothing = params.pfAlpha,  queueSize = maxQueueSize, syncHARQ = False)   
        else: 
            # prbsPerUser<=0 means allocating all PRBs to the user, otherwise only the indicated number per TTI 
            dlScheduler = ltea.dll.schedulers.downlink.RoundRobin(linkAdaptation, txPowerdBmPerPRB = scenarioConfig.bsPerPRBTxPowerdBm, queueSize = maxQueueSize, prbsPerUser = 0, syncHARQ = False)

        #dlScheduler = ltea.dll.schedulers.downlink.MultiUserScheduler(linkAdaptation, txPowerdBmPerPRB = scenarioConfig.bsPerPRBTxPowerdBm, throughputSmoothing = params.pfAlpha, queueSize = maxQueueSize, syncHARQ = True)    
            
        if params.adaptiveUplinkLA:
            linkAdaptationUL = ltea.dll.linkAdaptation.uplink.Adaptive(fastCrossingWeight = 0.01, 
                                                                       longTimeWeight = 0.005, 
                                                                       crossingThreshold = 45, 
                                                                       threshold_dB = params.thresholdUL)
        else:
            linkAdaptationUL = ltea.dll.linkAdaptation.uplink.SINRThreshold(threshold_dB = params.thresholdUL)        

        if params.powerControl == "calibration":
            alpha = 1.0
            P0dBmPerPRB = -106
        if params.powerControl == "3GPPdefault": # see 3GPP Self-evaluation methodology and results / assumptions by Tetsushi Abe, slide 27
            alpha = 0.8
            if params.scenario == "InH":
                P0dBmPerPRB = -80.0
            if params.scenario == "UMi":
                P0dBmPerPRB = -85.0
            if params.scenario == "UMa":
                P0dBmPerPRB = -83.0
            if params.scenario == "RMa":
                P0dBmPerPRB = -84.0
        ulScheduler = ltea.dll.schedulers.uplink.RoundRobin(linkAdaptation = linkAdaptationUL,  alpha = alpha, P0dBmPerPRB = P0dBmPerPRB, 
                                                            threegppCalibration = True, Ks = 0, prachPeriod = 99999999999, #no MCS-based PowerControl
                                                            pathlossEstimationMethod = "WBL") 
        WNS.simulationModel.nodes.append(ltea.nodes.eNB(pos, antenna, dlScheduler, ulScheduler, eNBreceiver, windowSize, settlingTime, None, fullBuffer = params.fullBuffer))


if params.scenario == "InH":
    msPositions = imtaphy.ScenarioSupport.placeMobilesUniformlyRandomlyInRectangle(params.numMSperBS * len(bsPositions),
                                                                                   bsPositions,
                                                                                   scenarioConfig)
else:
    if plotting:
        msPositions = imtaphy.ScenarioSupport.placeMobilesEquallyInCells(bsPositions,
                                                                         scenarioConfig,
                                                                         probeConfig)
    else:
        msPositions = imtaphy.ScenarioSupport.placeMobilesUniformlyRandomlyInCells(params.numMSperBS * len(bsPositions) * len(azimuths),
                                                                                   bsPositions,
                                                                                   scenarioConfig)

UEs = []
for pos in msPositions:
    pos.z = msHeight
    directionOfTravel = random.uniform(-math.pi, math.pi)
    arrayBroadsideAzimuth = directionOfTravel + math.pi / 2
    if arrayBroadsideAzimuth > math.pi:
        arrayBroadsideAzimuth -= 2 * math.pi
    antenna =  imtaphy.Antenna.Omnidirectional(type = "MobileStationITU", antennaGain = "0 dB",
                                        azimuth = arrayBroadsideAzimuth, numElements = params.numMSAntennas,
                                        wavelengthMeters = scenarioConfig.wavelengthMeters)
    ulScheduler = None
    ulScheduler =  ltea.dll.schedulers.uplink.UE(totalTxPowerdBm = scenarioConfig.msTotalTxPowerdBm)
    ue = ltea.nodes.UE(pos, msSpeedKmh, directionOfTravel, antenna, ulScheduler, ueReceiver, windowSize, settlingTime, None, fullBuffer = params.fullBuffer)
    UEs.append(ue)
    WNS.simulationModel.nodes.append(ue)

if not params.fullBuffer:
    ltea.helper.createEPCandTraffic(simulator = WNS,
                                offeredDLtrafficBps = params.offeredDLtrafficBps,
                                offeredULtrafficBps = params.offeredULtrafficBps,
                                packetSize = packetSize,
                                probeWindow = windowSize, 
                                settlingTime = settlingTime, 
                                useTCP = False,
                                enableLogger = False)

# see 3GPP TS 36.104 Section 5.6 Channel bandwidth
if params.numDLPRBs == 6:
    bandwidthDLHz = 1.4e6
elif params.numDLPRBs == 15:
    bandwidthDLHz = 3e6
elif params.numDLPRBs == 25:
    bandwidthDLHz = 5e6
elif params.numDLPRBs == 50:
    bandwidthDLHz = 1e7
elif params.numDLPRBs == 75:
    bandwidthDLHz = 1.5e7
elif params.numDLPRBs == 100:
    bandwidthDLHz = 2e7
else:
    bandwidthDLHz = 1 # won't make sense but...

if params.numULPRBs == 6:
    bandwidthULHz = 1.4e6
elif params.numULPRBs == 15:
    bandwidthULHz = 3e6
elif params.numULPRBs == 25:
    bandwidthULHz = 5e6
elif params.numULPRBs == 50:
    bandwidthULHz = 1e7
elif params.numULPRBs == 75:
    bandwidthULHz = 1.5e7
elif params.numULPRBs == 100:
    bandwidthULHz = 2e7
else:
    bandwidthULHz = 1 # won't make sense but...


if plotting:
    ltea.evaluation.default.installProbes(WNS, settlingTime = settlingTime, numBSs = len(bsPositions)*len(azimuths), users = params.numMSperBS, bandwidthDL = bandwidthDLHz, bandwidthUL = bandwidthULHz, restrictToBSIds= None, scenarioConfig = scenarioConfig, probeConfig = probeConfig)
else:
    ltea.evaluation.default.installProbes(WNS, settlingTime = settlingTime, numBSs = len(bsPositions)*len(azimuths), users = params.numMSperBS, bandwidthDL = bandwidthDLHz, bandwidthUL = bandwidthULHz, restrictToBSIds= None)

if dumpChannel:
    dumpUEsNodeIDs = []
    for i in range(0, min(9, len(UEs))):
        ue = UEs[i]
        ue.dll.enableChannelGainProbing()
        dumpUEsNodeIDs.append(ue.nodeID)

    ltea.evaluation.default.installChannelPlottingProbes(WNS,
                                                         params.numMSAntennas * params.numBSAntennas,
                                                         params.numDLPRBs,
                                                         500, # TTIs
                                                         dumpUEsNodeIDs # list of UE node ids to dump the channel for
                                                        )

if phyTracing:                                                        
    import openwns.evaluation                                                        
    node = openwns.evaluation.createSourceNode(WNS, "phyRxTracing")
    json = openwns.evaluation.JSONTrace(key="__json__", description="PhyInterfaceRx Tracing Test")

    centralSite = node.appendChildren(Accept(by = 'BSID', ifIn = [1, 2, 3], suffix="")) # only trace inner site (otherwise trace big/slow to open)
    
    ues = centralSite.appendChildren(Accept(by = 'NodeType', ifIn = [1], suffix="DL"))
    ues.appendChildren(json)
            
    eNBs = centralSite.appendChildren(Accept(by = 'NodeType', ifIn = [2], suffix="UL"))
    eNBs.appendChildren(json)
                                                        
