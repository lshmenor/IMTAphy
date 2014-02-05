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

from openwns.evaluation import *

def installEvaluation(sim, maxPacketDelay, maxPacketSize, maxBitThroughput, maxPacketThroughput, delayResolution, sizeResolution, throughputResolution):

    sourceName = 'traffic.endToEnd.packet.incoming.size'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(PDF(name = sourceName,
                            description = 'packet size [Bit]',
                            minXValue = 0.0,
                            maxXValue = maxPacketSize,
                            resolution = sizeResolution))

    sourceName = 'traffic.endToEnd.packet.incoming.delay'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(PDF(name = sourceName,
                            description = 'end to end packet delay [s]',
                            minXValue = 0.0,
                            maxXValue = maxPacketDelay,
                            resolution = delayResolution))

    sourceName = 'traffic.endToEnd.window.incoming.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(PDF(name = sourceName,
                            description = 'average bit rate [Bit/s]',
                            minXValue = 0.0,
                            maxXValue = maxBitThroughput,
                            resolution = throughputResolution))

    sourceName = 'traffic.endToEnd.window.incoming.packetThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(PDF(name = sourceName,
                            description = 'average packet rate [Packets/s]',
                            minXValue = 0.0,
                            maxXValue = maxPacketThroughput,
                            resolution = throughputResolution))

    sourceName = 'traffic.windowedEndToEndIncomingBitThroughputTrace'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(TextTrace(key = sourceName,
                                  description = 'windowed bit rate [Bit/s]'))

    sourceName = 'traffic.windowedEndToEndIncomingPacketThroughputTrace'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.appendChildren(TextTrace(key = sourceName,
                                  description = 'windowed packet rate [Packets/s]'))
