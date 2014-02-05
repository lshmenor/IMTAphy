###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2007
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

from openwns.evaluation import *

def installEvaluation(sim, maxPacketDelay, maxPacketSize, maxBitThroughput, maxPacketThroughput, resolution=1000):

    for dist in ['hop', 'endToEnd']:
        sourceName = 'ip.%s.packet.incoming.delay' % dist
        node = openwns.evaluation.createSourceNode(sim, sourceName )
        node.appendChildren(PDF(name = sourceName,
                                description = 'Incoming packet delay [s]',
                                maxXValue = maxPacketDelay,
                                resolution = resolution,
                                minXValue = 0.0))

        sourceName = 'ip.%s.packet.outgoing.delay' % dist
        node = openwns.evaluation.createSourceNode(sim, sourceName )
        node.appendChildren(PDF(name = sourceName,
                                description = 'Outgoing packet delay [s]',
                                maxXValue = maxPacketDelay,
                                resolution = resolution,
                                minXValue = 0.0))

        sourceName = 'ip.%s.packet.incoming.size' % dist
        node = openwns.evaluation.createSourceNode(sim, sourceName )
        node.appendChildren(PDF(name = sourceName,
                                description = 'Incoming packet size [Bit]',
                                maxXValue = maxPacketSize,
                                resolution = resolution,
                                minXValue = 0.0))

        sourceName = 'ip.%s.packet.outgoing.size' % dist
        node = openwns.evaluation.createSourceNode(sim, sourceName )
        node.appendChildren(PDF(name = sourceName,
                                description = 'Outgoing packet size [Bit]',
                                maxXValue = maxPacketSize,
                                resolution = resolution,
                                minXValue = 0.0))

        for direction in ['incoming', 'outgoing', 'aggregated']:

            sourceName = 'ip.%s.window.%s.bitThroughput' % (dist, direction)
            node = openwns.evaluation.createSourceNode(sim, sourceName )
            node.appendChildren(PDF(name = sourceName,
                                    description = '%s throughput [Bit/s]' % direction.capitalize(),
                                    maxXValue = maxBitThroughput,
                                    resolution = resolution,
                                    minXValue = 0.0))

            sourceName = 'ip.%s.window.%s.compoundThroughput' % (dist, direction)
            node = openwns.evaluation.createSourceNode(sim, sourceName )
            node.appendChildren(PDF(name = sourceName,
                                    description = '%s throughput [Packet/s]' % direction.capitalize(),
                                    maxXValue = maxPacketThroughput,
                                    resolution = resolution,
                                    minXValue = 0.0))

