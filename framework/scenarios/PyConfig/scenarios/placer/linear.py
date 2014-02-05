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

import scenarios.interfaces
import openwns.geometry.position
import math

class LinearPlacer(scenarios.interfaces.INodePlacer):
    """
    Place a number of nodes on the given positions.
    """

    def __init__(self, numberOfNodes = 1, positionsList = [1], rotate = 0.0):
        """
        @type  numberOfNodes: int
        @param numberOfNodes: The number of nodes on the circle

        @Type: position: float
        @param position: distance from BS in Meters for every single node

        @type  rotate: float
        @param rotate: Rotate the final result by rotate in radiant [0..2pi]
        """

        self.center = openwns.geometry.position.Position(x = 1000.0, y = 1000.0, z = 0.0)
        self.numberOfNodes = numberOfNodes
        self.positionsList = positionsList
        self.rotate = rotate

    def setCenter(self, center):
        self.center = center

    def getPositions(self):
        positions = []
        for i in xrange(self.numberOfNodes):
            x = self.positionsList[i]
            y = 0.0
            v = openwns.geometry.position.Vector(x = x, y = y, z = 0.0)
            p = v.turn2D(self.rotate).toPosition()
            positions.append(p)

        return [p + self.center for p in positions]

    def isInside(self, position):
        for i in xrange(self.numberOfNodes):
            x = self.positionsList[i]
            y = 0.0
            v = openwns.geometry.position.Vector(x = x, y = y, z = 0.0)
            p = v.turn2D(self.rotate).toPosition()
            if p.x + self.center.x == position.x:
                return True
        return False
