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

class AlohaComponent(glue.Glue.Component):
    """Component with Aloha MAC

    This configuration contains (in addtion to lowerConvergence and
    upperConvergence) a dropping buffer with configurable size, a
    Stop-and-Wait ARQ and CRC in order to throw away broken
    packets. Furthermore a Aloha MAC 'controls' the medium access"""

    arq = None

    def __init__(
        self,
        node,
        name,
        phyDataTransmission,
        phyNotification,
        arqResendTimeout = 0.01,
        alohaWaitingTime = 0.05,
        bufferSize = 500*1024*8):

        super(AlohaComponent, self).__init__(node, name, phyDataTransmission, phyNotification)
        # create Buffer, ARQ and CRC
        unicastBuffer = openwns.FUN.Node("unicastBuffer", openwns.Buffer.Dropping(sizeUnit = 'Bit', size = bufferSize, lossRatioProbeName = 'glue.unicastBufferLoss', sizeProbeName = 'glue.unicastBufferSize'))
        broadcastBuffer = openwns.FUN.Node("broadcastBuffer", openwns.Buffer.Dropping(sizeUnit = 'Bit', size = bufferSize, lossRatioProbeName = 'glue.broadcastBufferLoss', sizeProbeName = 'glue.broadcastBufferSize'))
        self.arq = openwns.FUN.Node("arq", openwns.ARQ.StopAndWait(parentLogger = self.logger, resendTimeout = arqResendTimeout))
        crc = openwns.FUN.Node("crc", openwns.CRC.CRC("lowerConvergence", lossRatioProbeName='glue.crcLoss'))
        alohaMAC = glue.Glue.Aloha(commandName = "alohaMAC", maximumWaitingTime = alohaWaitingTime, parentLogger = self.logger)
        
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
        self.fun.add(alohaMAC)
        self.fun.add(self.lowerConvergence)

        # connect unicast path
        self.unicastUpperConvergence.connect(self.bottleNeckDetective)
        self.bottleNeckDetective.connect(unicastBuffer)
        unicastBuffer.connect(self.arq)
        self.arq.connect(self.dispatcher)
        # connect broadcast path
        self.broadcastUpperConvergence.connect(broadcastBuffer)
        broadcastBuffer.connect(self.dispatcher)
        # connect common path
        self.dispatcher.connect(crc)
        crc.connect(alohaMAC)
        alohaMAC.connect(self.lowerConvergence)


