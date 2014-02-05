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

class VirtualDHCP(Component):

    nameInComponentFactory = "ip.VDHCP"

    subnetIdentifier = None

    startAddress = None

    endAddress = None

    subnetMask = None

    unbindDelay = None

    logger = None

    def __init__(self, node, subnetIdentifier, startAddress, endAddress, subnetMask, unbindDelay = 60):
        super(VirtualDHCP, self).__init__(node, subnetIdentifier)
        self.subnetIdentifier = subnetIdentifier
        self.subnetMask = subnetMask
        self.startAddress = startAddress
        self.endAddress = endAddress
        self.unbindDelay = unbindDelay
        self.logger = Logger(self.subnetIdentifier, True, node.logger)

class VirtualDHCPServer(Node, NoRadio):
    def __init__(self, name, subnetIdentifier, startAddress, endAddress, subnetMask):
        Node.__init__(self, name)
        vdhcp = VirtualDHCP(self, subnetIdentifier, startAddress, endAddress, subnetMask)
        self.setProperty("Type", "Virtual")
