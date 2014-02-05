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

class CSMACAComponent(glue.Glue.Component):
    """Component with CSMA/CA MAC

    This configuration contains (in addtion to lowerConvergence and
    upperConvergence) a dropping buffer with configurable size, a
    Stop-and-Wait ARQ and CRC in order to throw away broken
    packets. Furthermore a CSMA/CA MAC controls the medium access"""

    arq = None

    def __init__(self, node, name, phyDataTransmission, phyNotification, phyDataTransmissionFeedbackName, bufferSize = 500*1024*8):
        super(CSMACAComponent, self).__init__(node, name, phyDataTransmission, phyNotification)
        # probes
        perProbe = openwns.Probe.ErrorRate(
            name = "errorRate",
            prefix = "glue.packet",
            errorRateProvider = "lowerConvergence",
            commandName = "packetErrorRate")

        # create Buffer, ARQ and CRC
        unicastBuffer = openwns.FUN.Node("unicastBuffer", openwns.Buffer.Dropping(
            sizeUnit = 'Bit',
            size = bufferSize,
            lossRatioProbeName = 'glue.unicastBufferLoss',
            sizeProbeName = 'glue.unicastBufferSize'))

        broadcastBuffer = openwns.FUN.Node("broadcastBuffer", openwns.Buffer.Dropping(
            sizeUnit = 'Bit',
            size = bufferSize,
            lossRatioProbeName = 'glue.broadcastBufferLoss',
            sizeProbeName = 'glue.broadcastBufferSize'))

        self.arq = openwns.FUN.Node("arq", glue.Glue.StopAndWait(
            phyDataTransmissionFeedbackName = phyDataTransmissionFeedbackName,
            phyNotification = phyNotification,
            parentLogger = self.logger,
            # We wait at least SIFS+SlotTime
            shortResendTimeout = 25E-6,
            longResendTimeout = 44E-6))

        # CRC with 32 Bit (802.11)
        crc = openwns.FUN.Node("crc", openwns.CRC.CRC("lowerConvergence", lossRatioProbeName='glue.crcLoss', CRCsize = 4*8))
        # 24 Byte header (802.11)
        overhead = openwns.Tools.Overhead(overhead = 24*8, commandName = "overhead")
        csmaCAMAC = glue.Glue.CSMACAMAC(commandName = "csmaCAMAC", stopAndWaitARQName = self.arq.commandName, phyNotification = self.phyNotification, parentLogger = self.logger)
        
        self.lowerConvergence = openwns.FUN.Node(
            "lowerConvergence",
            glue.Glue.Lower2Copper(unicastRouting = self.unicastUpperConvergence.commandName,
                         broadcastRouting = self.broadcastUpperConvergence.commandName,
                         blocking = False,
                         parentLogger = self.logger,
                         enabled = self.loggerEnabled))
        
        # add Buffer, ARQ and CRC to fun
        self.fun.add(unicastBuffer)
        self.fun.add(broadcastBuffer)
        self.fun.add(self.arq)
        self.fun.add(crc)
        self.fun.add(csmaCAMAC)
        self.fun.add(overhead)
        self.fun.add(self.lowerConvergence)

        # connect unicast path
        self.unicastUpperConvergence.connect(unicastBuffer)
        unicastBuffer.connect(self.arq)
        self.arq.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(csmaCAMAC)
        csmaCAMAC.connect(overhead)
        overhead.connect(self.lowerConvergence)
