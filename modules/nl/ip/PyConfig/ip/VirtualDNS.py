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

class VirtualDNS(Component):

    nameInComponentFactory = "ip.VDNS"

    zoneIdentifier = None

    logger = None

    def __init__(self, node, zoneIdentifier):
        super(VirtualDNS, self).__init__(node, zoneIdentifier)
        self.zoneIdentifier = zoneIdentifier
        self.logger = Logger(self.zoneIdentifier, True, node.logger)


class VirtualDNSServer(Node, NoRadio):
    def __init__(self, name, zoneIdentifier):
        Node.__init__(self, name)
        vdns = VirtualDNS(self, zoneIdentifier)
        self.setProperty("Type", "Virtual")

