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

import openwns.FUN
from openwns.pyconfig import attrsetter

class PortConnector(object):
    def __init__(self, fuName, fuPort, groupPort):
        self.fuName = fuName
        self.fuPort = fuPort
        self.groupPort = groupPort

class Group(openwns.FUN.FunctionalUnit):
    __plugin__ = "wns.Group"
    name = "Group"

    fun = None
    """SubFun configuration to be built by this Group"""

    top = None
    """Top FU in the SubFUN"""

    bottom = None
    """Bottom FU in the SubFUN"""

    def __init__(self, fun, top = None, bottom = None, **kw):
        super(Group, self).__init__()
        self.fun = fun
        self.top = top
        self.bottom = bottom

        self.topPorts = []
        """Contains a list of PortConnectors"""

        self.bottomPorts = []
        """Contains a list of PortConnectors"""

        attrsetter(self, kw)
