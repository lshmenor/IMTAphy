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

from openwns.geometry.shape import *
from openwns.geometry.position import *

import unittest

class BoxTests(unittest.TestCase):

    def setUp(self):
        self.box = Box(12, 45, 23, 85)

    def testContains(self):
        self.assertEquals(self.box.contains(Position(14, 46)), True)

        self.assertEquals(self.box.contains(Position(12, 46)), True)

        self.assertEquals(self.box.contains(Position(15, 45)), True)

        self.assertEquals(self.box.contains(Position(23, 46)), True)

        self.assertEquals(self.box.contains(Position(20, 85)), True)
