###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2009
# Chair of Communication Networks (ComNets)
# Kopernikusstr. 5, D-52074 Aachen, Germany
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

import openwns.module
import openwns.pyconfig
import openwns.node
import openwns.Buffer
import openwns.ARQ
import openwns.CRC
import openwns.Probe
import openwns.FUN
import openwns.logger
import openwns.SAR
import openwns.Tools
import openwns.Multiplexer
import openwns.Group
import openwns.FlowSeparator

import glue.Glue
import glue.Trigger
import glue.Routing
import glue.BERMeasurementReporting
import glue.KeyBuilder

class ShortCutComponent(glue.Glue.Component):
    """Minimalistic configuration for testing

    This configuration contains (in addtion to lowerConvergence and
    upperConvergence) a dropping buffer with configurable size and CRC
    in order to throw away broken packets. Best used for testing of
    higher layers"""

    def __init__(self, node, name, phyDataTransmission, phyNotification, bufferSize = 20):
        super(ShortCutComponent, self).__init__(node, name, phyDataTransmission, phyNotification)
        # create Buffer and CRC
        # These two have intentionally no probe configuration
        unicastBuffer = openwns.FUN.Node("unicastBuffer", openwns.Buffer.Dropping(size = bufferSize))
        broadcastBuffer = openwns.FUN.Node("broadcastBuffer", openwns.Buffer.Dropping(size = bufferSize))
        crc = openwns.FUN.Node("crc", openwns.CRC.CRC("lowerConvergence", lossRatioProbeName='glue.crcLoss', parentLogger=self.logger))
        
        self.lowerConvergence = openwns.FUN.Node(
            "lowerConvergence",
            glue.Glue.Lower2Copper(unicastRouting = self.unicastUpperConvergence.commandName,
                         broadcastRouting = self.broadcastUpperConvergence.commandName,
                         blocking = True,
                         parentLogger = self.logger,
                         enabled = self.loggerEnabled))
        
        # add Buffer, LC and CRC to fun
        self.fun.add(unicastBuffer)
        self.fun.add(broadcastBuffer)
        self.fun.add(crc)
        self.fun.add(self.lowerConvergence)

        # connect unicast path
        self.unicastUpperConvergence.connect(self.bottleNeckDetective)
        self.bottleNeckDetective.connect(unicastBuffer)
        unicastBuffer.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(self.lowerConvergence)

class ShortCut(glue.Glue.Component):
    def __init__(self, _node, _name, _phyDataTransmission, _phyNotification, _bufferSize = 20, _sarFragmentSize = 160):
        super(ShortCut, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        unicastBuffer = openwns.FUN.Node("unicastBuffer", openwns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'glue.unicastBufferLoss', sizeProbeName = 'glue.unicastBufferSize'))
        broadcastBuffer = openwns.FUN.Node("broadcastBuffer", openwns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'glue.broadcastBufferLoss', sizeProbeName = 'glue.broadcastBufferSize'))
        topWindowProbe = openwns.FUN.Node("topWindowProbe", openwns.Probe.Window("glue.topWindowProbe", "glue.unicastTop", windowSize=.25))
        topDelayProbe = openwns.FUN.Node("delayProbe", openwns.Probe.Packet("glue.topDelayProbe", "glue.unicastTop"))
        crc = openwns.FUN.Node("crc", openwns.CRC.CRC("lowerConvergence", lossRatioProbeName='glue.crcLoss'))
        bottomWindowProbe = openwns.FUN.Node("bottomWindowProbe", openwns.Probe.Window("glue.bottomWindowProbe", "glue.bottom", windowSize=.25))
        bottomDelayProbe = openwns.FUN.Node("bottomDelayProbe", openwns.Probe.Packet("glue.bottomDelayProbe", "glue.bottom"))
        
        self.lowerConvergence = openwns.FUN.Node(
            "lowerConvergence",
            glue.Glue.Lower2Copper(unicastRouting = self.unicastUpperConvergence.commandName,
                         broadcastRouting = self.broadcastUpperConvergence.commandName,
                         blocking = True,
                         parentLogger = self.logger,
                         enabled = self.loggerEnabled))
        
        # add
        self.fun.add(unicastBuffer)
        self.fun.add(broadcastBuffer)
        self.fun.add(topWindowProbe)
        self.fun.add(topDelayProbe)
        self.fun.add(crc)
        self.fun.add(bottomWindowProbe)
        self.fun.add(bottomDelayProbe)
        self.fun.add(self.lowerConvergence)

        # connect unicast path
        self.unicastUpperConvergence.connect(unicastBuffer)
        unicastBuffer.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(topWindowProbe)
        topWindowProbe.connect(topDelayProbe)
        topDelayProbe.connect(crc)
        crc.connect(bottomWindowProbe)
        bottomWindowProbe.connect(bottomDelayProbe)
        bottomDelayProbe.connect(self.lowerConvergence)

class OFDMAShortCut(glue.Glue.Component):
    def __init__(self, 
                _node, 
                _name, 
                _phyDataTransmission, 
                _phyNotification, 
                dataRate, 
                _bufferSize = 20, 
                _sarFragmentSize = 160):
        super(OFDMAShortCut, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        unicastBuffer = openwns.FUN.Node(
            "unicastBuffer", openwns.Buffer.Dropping(
                size = _bufferSize, 
                lossRatioProbeName = 'glue.unicastBufferLoss', 
                sizeProbeName = 'glue.unicastBufferSize'))
                
        broadcastBuffer = openwns.FUN.Node(
            "broadcastBuffer", openwns.Buffer.Dropping(
                size = _bufferSize, 
                lossRatioProbeName = 'glue.broadcastBufferLoss', 
                sizeProbeName = 'glue.broadcastBufferSize'))
                
        topWindowProbe = openwns.FUN.Node(
            "topWindowProbe", openwns.Probe.Window(
                "glue.topWindowProbe", 
                "glue.unicastTop", 
                windowSize=.25))
                
        topDelayProbe = openwns.FUN.Node(
            "delayProbe", 
            openwns.Probe.Packet(
                "glue.topDelayProbe", 
                "glue.unicastTop"))
                
        crc = openwns.FUN.Node(
            "crc", openwns.CRC.CRC(
                "lowerConvergence", 
                lossRatioProbeName='glue.crcLoss'))
                
        perProbe = openwns.Probe.ErrorRate(
            name = "errorRate",
            prefix = "glue.packet",
            errorRateProvider = "lowerConvergence",
            commandName = "packetErrorRate")
                
        bottomWindowProbe = openwns.FUN.Node(
            "bottomWindowProbe", openwns.Probe.Window(
                "glue.bottomWindowProbe", 
                "glue.bottom", 
                windowSize=.25))
                
        bottomDelayProbe = openwns.FUN.Node(
            "bottomDelayProbe", openwns.Probe.Packet(
                "glue.bottomDelayProbe", 
                "glue.bottom"))
        
        self.lowerConvergence = openwns.FUN.Node(
            "lowerConvergence", glue.Glue.Lower2OFDMAPhy(
                modulation = "BPSK",
                symbolDuration = 1.0 / dataRate,
                numSubcarriers = 1,
                unicastRouting = self.unicastUpperConvergence.commandName,
                broadcastRouting = self.broadcastUpperConvergence.commandName))

        
        # add
        self.fun.add(unicastBuffer)
        self.fun.add(broadcastBuffer)
        self.fun.add(topWindowProbe)
        self.fun.add(topDelayProbe)
        self.fun.add(crc)
        self.fun.add(perProbe)
        self.fun.add(bottomWindowProbe)
        self.fun.add(bottomDelayProbe)
        self.fun.add(self.lowerConvergence)

        # connect unicast path
        self.unicastUpperConvergence.connect(unicastBuffer)
        unicastBuffer.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(topWindowProbe)
        topWindowProbe.connect(topDelayProbe)
        topDelayProbe.connect(crc)
        crc.connect(perProbe)
        perProbe.connect(bottomWindowProbe)
        bottomWindowProbe.connect(bottomDelayProbe)
        bottomDelayProbe.connect(self.lowerConvergence)

class AcknowledgedModeShortCutComponent(glue.Glue.Component):
    """Minimalistic configuration for testing

    This configuration contains (in addtion to lowerConvergence and
    upperConvergence) a dropping buffer with configurable size, a Stop-and-Wait ARQ
    and CRC in order to throw away broken packets."""

    def __init__(self, node, name, phyDataTransmission, phyNotification, bufferSize = 20, resendTimeout = 0.1):
        super(AcknowledgedModeShortCutComponent, self).__init__(node, name, phyDataTransmission, phyNotification)
        # probes
        perProbe = openwns.Probe.ErrorRate(
            name = "errorRate",
            prefix = "glue.packet",
            errorRateProvider = "lowerConvergence",
            commandName = "packetErrorRate")

        # create Buffer, ARQ and CRC
        unicastBuffer = openwns.FUN.Node("unicastBuffer", openwns.Buffer.Dropping(
            size = bufferSize,
            lossRatioProbeName = 'glue.unicastBufferLoss',
            sizeProbeName = 'glue.unicastBufferSize'))

        broadcastBuffer = openwns.FUN.Node("broadcastBuffer", openwns.Buffer.Dropping(
            size = bufferSize,
            lossRatioProbeName = 'glue.broadcastBufferLoss',
            sizeProbeName = 'glue.broadcastBufferSize'))

        arq = openwns.FUN.Node("arq", openwns.ARQ.StopAndWait(resendTimeout=resendTimeout))
        crc = openwns.FUN.Node("crc", openwns.CRC.CRC("lowerConvergence", lossRatioProbeName='glue.crcLoss'))

        self.lowerConvergence = openwns.FUN.Node(
            "lowerConvergence",
            glue.Glue.Lower2Copper(unicastRouting = self.unicastUpperConvergence.commandName,
                         broadcastRouting = self.broadcastUpperConvergence.commandName,
                         blocking = True,
                         parentLogger = self.logger,
                         enabled = self.loggerEnabled))

        # add probes
        self.fun.add(perProbe)
        # add Buffer, ARQ and CRC to fun
        self.fun.add(unicastBuffer)
        self.fun.add(broadcastBuffer)
        self.fun.add(arq)
        self.fun.add(crc)
        self.fun.add(self.lowerConvergence)

        # connect unicast path
        self.unicastUpperConvergence.connect(unicastBuffer)
        unicastBuffer.connect(arq)
        arq.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(perProbe)
        perProbe.connect(self.lowerConvergence)

class RichConnectShortCutComponent(glue.Glue.Component):
    """Minimalistic configuration for testing

    This configuration contains (in addtion to lowerConvergence and
    upperConvergence) a dropping buffer with configurable size, a Stop-and-Wait ARQ
    and CRC in order to throw away broken packets."""

    def __init__(self, node, name, phyDataTransmission, phyNotification, bufferSize = 20, resendTimeout = 0.1):
        super(RichConnectShortCutComponent, self).__init__(node, name, phyDataTransmission, phyNotification)
        # probes
        perProbe = openwns.FUN.Node("perProbe", openwns.Probe.ErrorRate(
                name = "errorRate",
                prefix = "glue.packet",
                errorRateProvider = "lowerConvergence",
                commandName = "packetErrorRate",
                parentLogger=self.logger))

        topWindowProbe = openwns.FUN.Node("topWindowProbe", openwns.Probe.Window(
                "glue.topWindowProbe",
                "glue.unicastTop",
                windowSize=.25,
                parentLogger=self.logger))

        topDelayProbe = openwns.FUN.Node("delayProbe", openwns.Probe.Packet(
                "glue.topDelayProbe",
                "glue.unicastTop",
                parentLogger=self.logger))

        bottomWindowProbe = openwns.FUN.Node("bottomWindowProbe", openwns.Probe.Window(
                "glue.bottomWindowProbe",
                "glue.bottom",
                windowSize=.25,
                parentLogger=self.logger))

        bottomDelayProbe = openwns.FUN.Node("bottomDelayProbe", openwns.Probe.Packet(
                "glue.bottomDelayProbe",
                "glue.bottom",
                parentLogger=self.logger))

        # create Buffer, ARQ and CRC

        broadcastBuffer = openwns.FUN.Node("broadcastBuffer", openwns.Buffer.Dropping(
            size = bufferSize,
            lossRatioProbeName = 'glue.broadcastBufferLoss',
            sizeProbeName = 'glue.broadcastBufferSize',
            parentLogger=self.logger))

        subFUN = openwns.FUN.FUN()

        unicastBuffer = openwns.FUN.Node("unicastBuffer", openwns.Buffer.Dropping(
            size = bufferSize,
            lossRatioProbeName = 'glue.unicastBufferLoss',
            sizeProbeName = 'glue.unicastBufferSize',
            parentLogger=self.logger))

        inSequenceChecker = openwns.FUN.Node("inSeuqenceChecker", openwns.Tools.InSequenceChecker(parentLogger=self.logger))

        arq = openwns.FUN.Node("arq", openwns.ARQ.StopAndWaitRC(resendTimeout=resendTimeout, parentLogger=self.logger))

        subFUN.add(unicastBuffer)
        subFUN.add(inSequenceChecker)
        subFUN.add(arq)

        subFUN.connect(unicastBuffer, inSequenceChecker)
        subFUN.connect(inSequenceChecker, arq)

        print "New config"
        groupFU = openwns.Group.Group(subFUN, "unicastBuffer")
        groupFU.bottomPorts.append(openwns.Group.PortConnector("arq",
                                                               openwns.ARQ.StopAndWaitRC.Data,
                                                               openwns.ARQ.StopAndWaitRC.Data))
        groupFU.bottomPorts.append(openwns.Group.PortConnector("arq",
                                                               openwns.ARQ.StopAndWaitRC.Ack,
                                                               openwns.ARQ.StopAndWaitRC.Ack))

        flowSeparatorFU = openwns.FlowSeparator.FlowSeparator(glue.KeyBuilder.KeyBuilder("unicastUpperConvergence"),
                                                              openwns.FlowSeparator.PrototypeCreator('group', groupFU),
                                                              parentLogger=self.logger)

        flowSeparator = openwns.FUN.Node('FlowSeparator', flowSeparatorFU)

        arqMux = openwns.FUN.Node("arqMux", openwns.Multiplexer.Dispatcher(opcodeSize = 0, parentLogger=self.logger))
        crc = openwns.FUN.Node("crc", openwns.CRC.CRC("lowerConvergence", lossRatioProbeName='glue.crcLoss', parentLogger=self.logger))

        self.lowerConvergence = openwns.FUN.Node(
            "lowerConvergence",
            glue.Glue.Lower2Copper(unicastRouting = self.unicastUpperConvergence.commandName,
                         broadcastRouting = self.broadcastUpperConvergence.commandName,
                         blocking = True,
                         parentLogger = self.logger,
                         enabled = self.loggerEnabled))

        # add probes
        self.fun.add(perProbe)
        # add Buffer, ARQ and CRC to fun
        self.fun.add(unicastBuffer)
        self.fun.add(broadcastBuffer)
        self.fun.add(topWindowProbe)
        self.fun.add(topDelayProbe)
        self.fun.add(flowSeparator)
        self.fun.add(arqMux)
        self.fun.add(crc)
        self.fun.add(bottomWindowProbe)
        self.fun.add(bottomDelayProbe)
        self.fun.add(self.lowerConvergence)

        # connect unicast path
        self.fun.connect(self.unicastUpperConvergence, topWindowProbe)
        self.fun.connect(topWindowProbe, topDelayProbe)
        self.fun.connect(topDelayProbe, flowSeparator)
        self.fun.connect(flowSeparator, openwns.ARQ.StopAndWaitRC.Data, arqMux)
        self.fun.connect(flowSeparator, openwns.ARQ.StopAndWaitRC.Ack, arqMux)
        self.fun.connect(arqMux, self.dispatcher)
        # connect broadcast path
        self.fun.connect(self.broadcastUpperConvergence, broadcastBuffer)
        self.fun.connect(broadcastBuffer, self.dispatcher)
        # connect common path
        self.fun.connect(self.dispatcher, crc)
        self.fun.connect(crc, perProbe)
        self.fun.connect(perProbe, bottomWindowProbe)
        self.fun.connect(bottomWindowProbe, bottomDelayProbe)
        self.fun.connect(bottomDelayProbe, self.lowerConvergence)

        self.logger.level = 3
