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

from Logger import Logger
from openwns.node import Node
from openwns.node import Component
from openwns.node import NoRadio

class VirtualARP(Component):

    nameInComponentFactory = "ip.VARP"

    subnetIdentifier = None

    resolveDelay = 0.0

    logger = None

    def __init__(self, node, subnetIdentifier, resolveDelay):
        super(VirtualARP, self).__init__(node, subnetIdentifier)
        self.subnetIdentifier = subnetIdentifier
        self.resolveDelay = resolveDelay
        self.logger = Logger(self.subnetIdentifier, True, node.logger)

class VirtualARPServer(Node, NoRadio):
    def __init__(self, name, subnetIdentifier, resolveDelay=0.0):
        Node.__init__(self, name)
        varp = VirtualARP(self, subnetIdentifier, resolveDelay)
        self.setProperty("Type", "Virtual")
