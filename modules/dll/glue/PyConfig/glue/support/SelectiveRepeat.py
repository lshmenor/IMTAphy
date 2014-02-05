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

import glue.Glue
import glue.Trigger
import glue.Routing
import glue.BERMeasurementReporting

class SelectiveRepeatARQ(glue.Glue.Component):
    def __init__(self, _node, _name, _phyDataTransmission, _phyNotification, _bufferSize = 20, _sarFragmentSize = 160):
        super(SelectiveRepeatARQ, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        unicastBuffer = openwns.FUN.Node("unicastBuffer", openwns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'glue.unicastBufferLoss', sizeProbeName = 'glue.unicastBufferSize'))
        broadcastBuffer = openwns.FUN.Node("broadcastBuffer", openwns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'glue.broadcastBufferLoss', sizeProbeName = 'glue.broadcastBufferSize'))
        unicastTopWindowProbe = openwns.FUN.Node("unicastTopWindowProbe", openwns.Probe.Window("glue.unicastTopWindowProbe", "glue.unicastTop", windowSize=.25))
        unicastTopDelayProbe = openwns.FUN.Node("unicastTopDelayProbe", openwns.Probe.Packet("glue.unicastTopDelayProbe", "glue.unicastTop"))
        broadcastTopWindowProbe = openwns.FUN.Node("broadcastTopWindowProbe", openwns.Probe.Window("glue.broadcastTopWindowProbe", "glue.broadcastTop", windowSize=.25))
        broadcastTopDelayProbe = openwns.FUN.Node("broadcastTopDelayProbe", openwns.Probe.Packet("glue.broadcastTopDelayProbe", "glue.broadcastTop"))
        arq = openwns.FUN.Node("arq", openwns.ARQ.SelectiveRepeat(useProbe = True, probeName = "glue.ARQTransmissionAttempts", resendTimeout = 0.00001))
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
        self.fun.add(unicastTopWindowProbe)
        self.fun.add(unicastTopDelayProbe)
        self.fun.add(broadcastBuffer)
        self.fun.add(broadcastTopWindowProbe)
        self.fun.add(broadcastTopDelayProbe)
        self.fun.add(arq)
        self.fun.add(crc)
        self.fun.add(bottomWindowProbe)
        self.fun.add(bottomDelayProbe)
        self.fun.add(self.lowerConvergence)

        # connect unicast path
        self.unicastUpperConvergence.connect(unicastBuffer)
        unicastBuffer.connect(unicastTopWindowProbe)
        unicastTopWindowProbe.connect(unicastTopDelayProbe)
        unicastTopDelayProbe.connect(arq)
        arq.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(broadcastTopWindowProbe)
        broadcastTopWindowProbe.connect(broadcastTopDelayProbe)
        broadcastTopDelayProbe.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(bottomWindowProbe)
        bottomWindowProbe.connect(bottomDelayProbe)
        bottomDelayProbe.connect(self.lowerConvergence)

class SARSelectiveRepeatARQ(glue.Glue.Component):
    def __init__(self, _node, _name, _phyDataTransmission, _phyNotification, _bufferSize = 20, _sarFragmentSize = 160):
        super(SARSelectiveRepeatARQ, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        unicastBuffer = openwns.FUN.Node("unicastBuffer", openwns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'glue.unicastBufferLoss', sizeProbeName = 'glue.unicastBufferSize'))
        unicastTopWindowProbe = openwns.FUN.Node("unicastTopWindowProbe", openwns.Probe.Window("glue.unicastTopWindowProbe", "glue.unicastTop", windowSize=.25))
        unicastTopDelayProbe = openwns.FUN.Node("unicastTopDelayProbe", openwns.Probe.Packet("glue.unicastTopDelayProbe", "glue.unicastTop"))
        unicastSar = openwns.FUN.Node("unicastSar", openwns.SAR.Fixed(_sarFragmentSize))
        broadcastBuffer = openwns.FUN.Node("broadcastBuffer", openwns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'glue.broadcastBufferLoss', sizeProbeName = 'glue.broadcastBufferSize'))
        broadcastTopWindowProbe = openwns.FUN.Node("broadcastTopWindowProbe", openwns.Probe.Window("glue.broadcastTopWindowProbe", "glue.broadcastTop", windowSize=.25))
        broadcastTopDelayProbe = openwns.FUN.Node("broadcastTopDelayProbe", openwns.Probe.Packet("glue.broadcastTopDelayProbe", "glue.broadcastTop"))
        broadcastSar = openwns.FUN.Node("broadcastSar", openwns.SAR.Fixed(_sarFragmentSize))
        arq = openwns.FUN.Node("arq", openwns.ARQ.SelectiveRepeat(useProbe = True, probeName = "glue.ARQTransmissionAttempts", resendTimeout = 0.00001))
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
        self.fun.add(unicastTopWindowProbe)
        self.fun.add(unicastTopDelayProbe)
        self.fun.add(unicastSar)
        self.fun.add(broadcastBuffer)
        self.fun.add(broadcastTopWindowProbe)
        self.fun.add(broadcastTopDelayProbe)
        self.fun.add(broadcastSar)
        self.fun.add(arq)
        self.fun.add(crc)
        self.fun.add(bottomWindowProbe)
        self.fun.add(bottomDelayProbe)
        self.fun.add(self.lowerConvergence)

        # connect unicast path
        self.unicastUpperConvergence.connect(unicastBuffer)
        unicastBuffer.connect(unicastTopWindowProbe)
        unicastTopWindowProbe.connect(unicastTopDelayProbe)
        unicastTopDelayProbe.connect(unicastSar)
        unicastSar.connect(arq)
        arq.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(broadcastTopWindowProbe)
        broadcastTopWindowProbe.connect(broadcastTopDelayProbe)
        broadcastTopDelayProbe.connect(broadcastSar)
        broadcastSar.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(bottomWindowProbe)
        bottomWindowProbe.connect(bottomDelayProbe)
        bottomDelayProbe.connect(self.lowerConvergence)

class SARSelectiveRepeatARQTrigger(glue.Glue.Component):
    def __init__(self, _node, _name, _phyDataTransmission, _phyNotification, _bufferSize = 20, _sarFragmentSize = 160):
        super(SARSelectiveRepeatARQTrigger, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        buffer = openwns.FUN.Node("buffer", openwns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'glue.unicastBufferLoss', sizeProbeName = 'glue.unicastBufferSize'))
        topWindowProbe = openwns.FUN.Node("topWindowProbe", openwns.Probe.Window("glue.topWindowProbe", "glue.unicastTop", windowSize=.25))
        topDelayProbe = openwns.FUN.Node("topDelayProbe", openwns.Probe.Packet("glue.topDelayProbe", "glue.unicastTop"))
        sar = openwns.FUN.Node("sar", openwns.SAR.Fixed(_sarFragmentSize))
        arq = openwns.FUN.Node("arq", openwns.ARQ.SelectiveRepeat(useProbe = True, probeName = "glue.ARQTransmissionAttempts", resendTimeout = 0.00001))
        crc = openwns.FUN.Node("crc", openwns.CRC.CRC("lowerConvergence", lossRatioProbeName='glue.crcLoss'))
        bottomWindowProbe = openwns.FUN.Node("bottomWindowProbe", openwns.Probe.Window("glue.bottomWindowProbe", "glue.bottom", windowSize=.25))
        bottomDelayProbe = openwns.FUN.Node("bottomDelayProbe", openwns.Probe.Packet("glue.bottomDelayProbe", "glue.bottom"))
        trigger = openwns.FUN.Node("trigger", glue.Trigger.Trigger("lowerConvergence", "sar"))
        
        self.lowerConvergence = openwns.FUN.Node(
            "lowerConvergence",
            glue.Glue.Lower2Copper(unicastRouting = self.unicastUpperConvergence.commandName,
                         broadcastRouting = self.broadcastUpperConvergence.commandName,
                         blocking = True,
                         parentLogger = self.logger,
                         enabled = self.loggerEnabled))
        
        # add
        self.fun.add(buffer)
        self.fun.add(topWindowProbe)
        self.fun.add(topDelayProbe)
        self.fun.add(sar)
        self.fun.add(arq)
        self.fun.add(crc)
        self.fun.add(bottomWindowProbe)
        self.fun.add(bottomDelayProbe)
        self.fun.add(trigger)
        self.fun.add(self.lowerConvergence)
        # connect
        self.unicastUpperConvergence.connect(buffer)
        buffer.connect(topWindowProbe)
        topWindowProbe.connect(topDelayProbe)
        topDelayProbe.connect(sar)
        sar.connect(arq)
        arq.connect(crc)
        crc.connect(bottomWindowProbe)
        bottomWindowProbe.connect(bottomDelayProbe)
        bottomDelayProbe.connect(self.lowerConvergence)

class SARSelectiveRepeatARQTriggerReporting(glue.Glue.Component):
    def __init__(self, _node, _name, _phyDataTransmission, _phyNotification, _bufferSize = 20, _sarFragmentSize = 160):
        super(SARSelectiveRepeatARQTriggerReporting, self).__init__(_node, _name, _phyDataTransmission, _phyNotification)
        # create
        # control plane
        trigger = openwns.FUN.Node("trigger", glue.Trigger.Trigger("BERMeasurementReporting", "sar"))
        berMeasurementReporting = openwns.FUN.Node("BERMeasurementReporting", glue.BERMeasurementReporting.BERMeasurementReporting("lowerConvergence"))

        # user plane
        buffer = openwns.FUN.Node("buffer", openwns.Buffer.Dropping(size = _bufferSize, lossRatioProbeName = 'glue.unicastBufferLoss', sizeProbeName = 'glue.unicastBufferSize'))
        topWindowProbe = openwns.FUN.Node("topWindowProbe", openwns.Probe.Window("glue.topWindowProbe", "glue.unicastTop", windowSize=.25))
        topDelayProbe = openwns.FUN.Node("topDelayProbe", openwns.Probe.Packet("glue.topDelayProbe", "glue.unicastTop"))
        sar = openwns.FUN.Node("sar", openwns.SAR.Fixed(_sarFragmentSize))
        arq = openwns.FUN.Node("arq", openwns.ARQ.SelectiveRepeat(useProbe = True, probeName = "glue.ARQTransmissionAttempts", resendTimeout = 0.00001))

        planeDispatcher = openwns.FUN.Node("planeDispatcher", openwns.Multiplexer.Dispatcher(1))
        routing = openwns.FUN.Node("routing", glue.Routing.Routing("unicastUpperConvergence", allowRouteChange = True))
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
        # control plane
        self.fun.add(trigger)
        self.fun.add(berMeasurementReporting)

        # user plane
        self.fun.add(buffer)
        self.fun.add(topWindowProbe)
        self.fun.add(topDelayProbe)
        self.fun.add(sar)
        self.fun.add(arq)

        self.fun.add(planeDispatcher)
        self.fun.add(routing)
        self.fun.add(crc)
        self.fun.add(bottomWindowProbe)
        self.fun.add(bottomDelayProbe)
        self.fun.add(self.lowerConvergence)

        # connect
        # control plane
        trigger.connect(berMeasurementReporting)
        berMeasurementReporting.connect(planeDispatcher)

        # user plane
        self.unicastUpperConvergence.connect(buffer)
        buffer.connect(topWindowProbe)
        topWindowProbe.connect(topDelayProbe)
        topDelayProbe.connect(sar)
        sar.connect(arq)
        arq.connect(planeDispatcher)

        planeDispatcher.connect(routing)
        routing.connect(crc)
        crc.connect(bottomWindowProbe)
        bottomWindowProbe.connect(bottomDelayProbe)
        bottomDelayProbe.connect(self.lowerConvergence)
