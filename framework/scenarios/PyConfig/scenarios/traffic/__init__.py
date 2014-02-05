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
import constanze.traffic
import scenarios.binding

class CBR:

    def __init__(self, offset, trafficRate, packetSize, loggerRetriever = lambda node: node.logger):
        self.offset = offset
        self.trafficRate = trafficRate
        self.packetSize = packetSize
        self.loggerRetriever = loggerRetriever

    def create(self, node):
        return constanze.traffic.CBR0(offset = self.offset,
                                      throughput = self.trafficRate,
                                      packetSize = self.packetSize,
                                      parentLogger = self.loggerRetriever(node))

class VoIP:

    def __init__(self, offset, loggerRetriever = lambda node: node.logger):
        self.offset = offset
        self.loggerRetriever = loggerRetriever

    def create(self, node):
        return constanze.traffic.VoIP(offset = self.offset,
                                      parentLogger = self.loggerRetriever(node))

class ApplCBR:

    def __init__(self, packetSize, bitRate, settlingTime, 
                 minStartDelay, maxStartDelay, 
                 loggerRetriever = lambda node: node.logger):
        self.packetSize = packetSize
        self.bitRate = bitRate
        self.loggerRetriever = loggerRetriever
        self.settlingTime = settlingTime
        self.minStartDelay = minStartDelay
        self.maxStartDelay = maxStartDelay

    def create(self, node):
        import applications.clientSessions
        return applications.clientSessions.CBR(packetSize = self.packetSize,
                                               bitRate = self.bitRate,
                                               settlingTime = self.settlingTime, 
                                               minStartDelay = self.minStartDelay,
                                               maxStartDelay = self.maxStartDelay,
                                               parentLogger = self.loggerRetriever(node))

class ApplEmail:

    def __init__(self, meanOfNumberOfEmails, sigmaOfNumberOfEmails,
                 medianOfLargeEmailSize, sigmaOfLargeEmailSize,
                 valueOfLargeEmailSize, medianOfSmallEmailSize,
                 sigmaOfSmallEmailSize, valueOfSmallEmailSize,
                 shapeOfEmailWritingTime, scaleOfEmailWritingTime,
                 shapeOfEmailReadingTime, scaleOfEmailReadingTime,
                 settlingTime, minStartDelay, maxStartDelay, 
                 loggerRetriever = lambda node: node.logger):
        self.meanOfNumberOfEmails = meanOfNumberOfEmails
        self.sigmaOfNumberOfEmails = sigmaOfNumberOfEmails
        self.medianOfLargeEmailSize = medianOfLargeEmailSize
        self.sigmaOfLargeEmailSize = sigmaOfLargeEmailSize
        self.valueOfLargeEmailSize = valueOfLargeEmailSize
        self.medianOfSmallEmailSize = medianOfSmallEmailSize
        self.sigmaOfSmallEmailSize = sigmaOfSmallEmailSize
        self.valueOfSmallEmailSize = valueOfSmallEmailSize
        self.shapeOfEmailWritingTime = shapeOfEmailWritingTime
        self.scaleOfEmailWritingTime = scaleOfEmailWritingTime
        self.shapeOfEmailReadingTime = shapeOfEmailReadingTime
        self.scaleOfEmailReadingTime = scaleOfEmailReadingTime
        self.settlingTime = settlingTime
        self.minStartDelay = minStartDelay
        self.maxStartDelay = maxStartDelay
        self.loggerRetriever = loggerRetriever

    def create(self, node):
        import applications.clientSessions
        return applications.clientSessions.Email(meanOfNumberOfEmails = self.meanOfNumberOfEmails,
                                                 sigmaOfNumberOfEmails = self.sigmaOfNumberOfEmails,
                                                 medianOfLargeEmailSize = self.medianOfLargeEmailSize,
                                                 sigmaOfLargeEmailSize = self.sigmaOfLargeEmailSize,
                                                 valueOfLargeEmailSize = self.valueOfLargeEmailSize,
                                                 medianOfSmallEmailSize = self.medianOfSmallEmailSize,
                                                 sigmaOfSmallEmailSize = self.sigmaOfSmallEmailSize,
                                                 valueOfSmallEmailSize = self.valueOfSmallEmailSize,
                                                 shapeOfEmailWritingTime = self.shapeOfEmailWritingTime,
                                                 scaleOfEmailWritingTime = self.scaleOfEmailWritingTime,
                                                 shapeOfEmailReadingTime = self.shapeOfEmailReadingTime,
                                                 scaleOfEmailReadingTime = self.scaleOfEmailReadingTime,
                                                 settlingTime = self.settlingTime,
                                                 minStartDelay = self.minStartDelay,
                                                 maxStartDelay = self.maxStartDelay, 
                                                 parentLogger = self.loggerRetriever(node))

class ApplFTP:

    def __init__(self, meanOfReadingTime, meanOfAmountOfData,
                 sigmaOfAmountOfData, settlingTime, 
                 minStartDelay, maxStartDelay,
                 loggerRetriever = lambda node: node.logger):
        self.meanOfReadingTime = meanOfReadingTime
        self.meanOfAmountOfData = meanOfAmountOfData
        self.sigmaOfAmountOfData = sigmaOfAmountOfData
        self.settlingTime = settlingTime
        self.minStartDelay = minStartDelay
        self.maxStartDelay = maxStartDelay
        self.loggerRetriever = loggerRetriever

    def create(self, node):
        import applications.clientSessions
        return applications.clientSessions.FTP(meanOfReadingTime = self.meanOfReadingTime,
                                               meanOfAmountOfData = self.meanOfAmountOfData,
                                               sigmaOfAmountOfData = self.sigmaOfAmountOfData,
                                               settlingTime = self.settlingTime, 
                                               minStartDelay = self.minStartDelay,
                                               maxStartDelay = self.maxStartDelay,
                                               parentLogger = self.loggerRetriever(node))

class ApplVideo:

    def __init__(self, settlingTime, minStartDelay,
                 maxStartDelay, loggerRetriever = lambda node: node.logger):
        self.settlingTime = settlingTime
        self.minStartDelay = minStartDelay
        self.maxStartDelay = maxStartDelay
        self.loggerRetriever = loggerRetriever

    def create(self, node):
        import applications.clientSessions
        return applications.clientSessions.Video(parentLogger = self.loggerRetriever(node))

class ApplVoIP:

    def __init__(self, codecType, comfortNoiseChoice,
                 settlingTime, minStartDelay, maxStartDelay,
                 loggerRetriever = lambda node: node.logger):
        self.codecType = codecType
        self.comfortNoiseChoice = comfortNoiseChoice
        self.settlingTime = settlingTime
        self.minStartDelay = minStartDelay
        self.maxStartDelay = maxStartDelay
        self.loggerRetriever = loggerRetriever

    def create(self, node):
        import applications.clientSessions
        return applications.clientSessions.VoIP(codecType = self.codecType,
                                                comfortNoiseChoice = self.comfortNoiseChoice,
                                                settlingTime = self.settlingTime, 
                                                minStartDelay = self.minStartDelay,
                                                maxStartDelay = self.maxStartDelay, 
                                                parentLogger = self.loggerRetriever(node))

class ApplVideoTelephony:

    def __init__(self, voiceCodecType, comfortNoiseChoice,
                 videoCodecType, formatType, qualityChoice,
                 settlingTime, minStartDelay, maxStartDelay,
                 loggerRetriever = lambda node: node.logger):
        self.voiceCodecType = voiceCodecType
        self.comfortNoiseChoice = comfortNoiseChoice
        self.videoCodecType = videoCodecType
        self.formatType = formatType
        self.qualityChoice = qualityChoice
        self.settlingTime = settlingTime
        self.minStartDelay = minStartDelay
        self.maxStartDelay = maxStartDelay
        self.loggerRetriever = loggerRetriever

    def create(self, node):
        import applications.clientSessions
        return applications.clientSessions.VideoTelephony(voiceCodecType = self.voiceCodecType,
                                                          comfortNoiseChoice = self.comfortNoiseChoice,
                                                          videoCodecType = self.videoCodecType,
                                                          formatType = self.formatType,
                                                          qualityChoice = self.qualityChoice,
                                                          settlingTime = self.settlingTime, 
                                                          minStartDelay = self.minStartDelay,
                                                          maxStartDelay = self.maxStartDelay,
                                                          parentLogger = self.loggerRetriever(node))

class ApplVideoTrace:

    def __init__(self, genreChoice, codecChoice, formatChoice, rateControlChoice,
                 qualityChoice, settlingTime, minStartDelay, maxStartDelay,
                 loggerRetriever = lambda node: node.logger):
        self.genreChoice = genreChoice
        self.codecChoice = codecChoice
        self.formatChoice = formatChoice
        self.rateControlChoice = rateControlChoice
        self.qualityChoice = qualityChoice
        self.settlingTime = settlingTime
        self.minStartDelay = minStartDelay
        self.maxStartDelay = maxStartDelay
        self.loggerRetriever = loggerRetriever

    def create(self, node):
        import applications.clientSessions
        return applications.clientSessions.VideoTrace(genreChoice = self.genreChoice,
                                                      codecChoice = self.codecChoice,
                                                      formatChoice = self.formatChoice,
                                                      rateControlChoice = self.rateControlChoice,
                                                      qualityChoice = self.qualityChoice,
                                                      settlingTime = self.settlingTime, 
                                                      minStartDelay = self.minStartDelay,
                                                      maxStartDelay = self.maxStartDelay,
                                                      parentLogger = self.loggerRetriever(node))

class ApplWWW:

    def __init__(self, meanOfReadingTime, meanOfParsingTime, shapeOfEmbeddedObjects,
                 scaleOfEmbeddedObjects, settlingTime, minStartDelay, maxStartDelay,
                 loggerRetriever = lambda node: node.logger):
        self.meanOfReadingTime = meanOfReadingTime
        self.meanOfParsingTime = meanOfParsingTime
        self.shapeOfEmbeddedObjects = shapeOfEmbeddedObjects
        self.scaleOfEmbeddedObjects = scaleOfEmbeddedObjects
        self.settlingTime = settlingTime
        self.minStartDelay = minStartDelay
        self.maxStartDelay = maxStartDelay
        self.loggerRetriever = loggerRetriever

    def create(self, node):
        import applications.clientSessions
        return applications.clientSessions.WWW(meanOfReadingTime = self.meanOfReadingTime,
                                               meanOfParsingTime = self.meanOfParsingTime,
                                               shapeOfEmbeddedObjects = self.shapeOfEmbeddedObjects,
                                               scaleOfEmbeddedObjects = self.scaleOfEmbeddedObjects,
                                               settlingTime = self.settlingTime, 
                                               minStartDelay = self.minStartDelay,
                                               maxStartDelay = self.maxStartDelay,
                                               parentLogger = self.loggerRetriever(node))

class ApplWiMAXVideo:

    def __init__(self, settlingTime, minStartDelay, maxStartDelay,
                 loggerRetriever = lambda node: node.logger):
        self.settlingTime = settlingTime
        self.minStartDelay = minStartDelay
        self.maxStartDelay = maxStartDelay
        self.loggerRetriever = loggerRetriever

    def create(self, node):
        import applications.clientSessions
        return applications.clientSessions.WiMAXVideo(settlingTime = self.settlingTime, 
                                                      minStartDelay = self.minStartDelay,
                                                      maxStartDelay = self.maxStartDelay,
                                                      parentLogger = self.loggerRetriever(node))

class ApplWiMAXVideoTelephony:

    def __init__(self,  codecType, comfortNoiseChoice,
                 framesPerSecond, scaleOfIFrame, shapeOfIFrame, shiftOfIFrameSize,
                 meanOfBFrameSize, sigmaOfBFrameSize, meanOfPFrameSize, sigmaOfPFrameSize,
                 settlingTime, minStartDelay, maxStartDelay, loggerRetriever = lambda node: node.logger):
        self.codecType = codecType
        self.comfortNoiseChoice = comfortNoiseChoice
        self.framesPerSecond = framesPerSecond
        self.scaleOfIFrame = scaleOfIFrame
        self.shapeOfIFrame = shapeOfIFrame
        self.shiftOfIFrameSize = shiftOfIFrameSize
        self.meanOfBFrameSize = meanOfBFrameSize
        self.sigmaOfBFrameSize = sigmaOfBFrameSize
        self.meanOfPFrameSize = meanOfPFrameSize
        self.sigmaOfPFrameSize = sigmaOfPFrameSize
        self.settlingTime = settlingTime
        self.minStartDelay = minStartDelay
        self.maxStartDelay = maxStartDelay
        self.loggerRetriever = loggerRetriever

    def create(self, node):
        import applications.clientSessions
        return applications.clientSessions.WiMAXVideoTelephony(codecType = self.codecType,
                                                               comfortNoiseChoice = self.comfortNoiseChoice,
                                                               framesPerSecond = self.framesPerSecond,
                                                               scaleOfIFrame = self.scaleOfIFrame,
                                                               shapeOfIFrame = self.shapeOfIFrame,
                                                               shiftOfIFrameSize = self.shiftOfIFrameSize,
                                                               meanOfBFrameSize = self.meanOfBFrameSize,
                                                               sigmaOfBFrameSize = self.sigmaOfBFrameSize,
                                                               meanOfPFrameSize = self.meanOfPFrameSize,
                                                               sigmaOfPFrameSize = self.sigmaOfPFrameSize,
                                                               settlingTime = self.settlingTime, 
                                                               minStartDelay = self.minStartDelay,
                                                               maxStartDelay = self.maxStartDelay,
                                                               parentLogger = self.loggerRetriever(node))


def addTraffic(bindingCreator,loadCreator, direction="UL"):
    ueNodes = openwns.simulator.getSimulator().simulationModel.getNodesByProperty("Type", "UE")
 
    for ue in ueNodes:
        if direction=="UL":
            ue.addTraffic(bindingCreator.create(ue), loadCreator.create(ue))

