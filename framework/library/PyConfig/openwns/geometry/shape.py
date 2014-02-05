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

import wnsbase.Interface

class Interface(wnsbase.Interface.Interface):

    @wnsbase.Interface.abstractmethod
    def contains(self, position):
        """ Returns True if position is within the shape. Returns False if the position is outside
        
        @type point: openwns.geometry.position.Position
        @param point: The position to test
        """

class Box(Interface):

    def __init__(self, top, left, bottom, right):
        """ A simple box

        @type top: float
        @param top: Top coordinate

        @type bottom: float
        @param bottom: Bottom coordinate

        @type left: float
        @param left: Left coordinate

        @type right: float
        @param right: Right coordinate
        """

        self.top = top
        self.left = left
        self.bottom = bottom
        self.right = right

        assert self.top <= self.bottom, "Top is larger than bottom"
        assert self.left <= self.right, "Left is larger than right"

    def contains(self, position):
        if position.x >= self.top and position.x <= self.bottom and position.y >= self.left and position.y <= self.right:
            return True
        else:
            return False

