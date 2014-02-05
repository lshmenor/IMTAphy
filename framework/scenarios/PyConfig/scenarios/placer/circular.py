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
import math,random

class CircularPlacer(scenarios.interfaces.INodePlacer):
    """
    Place a number of nodes on a circle with the given radius.
    The placement will then be rotated by the rotate argument
    """

    def __init__(self, numberOfNodes, radius, rotate = 0.0):
        """
        @type  numberOfNodes: int
        @param numberOfNodes: The number of nodes on the circle

        @type  radius: float
        @param radius: The radius is in Meters [m]

        @type  rotate: float
        @param rotate: Rotate the final result by rotate in radiant [0..2pi]
        """

        self.center = openwns.geometry.position.Position(x = 0.0, y = 0.0, z = 0.0)
        self.numberOfNodes = numberOfNodes
        self.radius = radius
        self.rotate = rotate

    def setCenter(self, center):
        self.center = center

    def getPositions(self):
        positions = []
        for s in xrange(self.numberOfNodes):
            x = self.radius * math.cos(float(s)/self.numberOfNodes * 2*math.pi)
            y = self.radius * math.sin(float(s)/self.numberOfNodes * 2*math.pi)
            v = openwns.geometry.position.Vector(x = x, y = y, z = 0.0)
            p = v.turn2D(self.rotate).toPosition()
            positions.append(p)

        return [p + self.center for p in positions]


    def isInside(self, position):

        return isInCircle(position, self.radius, self.center)

def isInCircle(position, radius, center):
    vector = (position - center)
    return vector.length() <= radius



class CircularAreaPlacer(scenarios.interfaces.INodePlacer):
    """
    Place a number of nodes on a circle with the given radius.
    The placement will then be rotated by the rotate argument
    """

    def __init__(self, numberOfNodes, radius, minDistance = 0.0, rotate = 0.0):
        """
        @type  numberOfNodes: int
        @param numberOfNodes: The number of nodes on the circle

        @type  radius: float
        @param radius: The radius is in Meters [m]

        @type  rotate: float
        @param rotate: Rotate the final result by rotate in radiant [0..2pi]
        """

        self.center = openwns.geometry.position.Position(x = 0.0, y = 0.0, z = 0.0)
        self.numberOfNodes = numberOfNodes
        self.radius = radius
        self.rotate = rotate

        self.minDistance = minDistance
    def setCenter(self, center):
        self.center = center

    def getPositions(self):

        positions = []
        stationcounter = 0
        while stationcounter < self.numberOfNodes:
            x = random.random() * 2 * self.radius - self.radius
            y = random.random() * 2 * self.radius - self.radius
            if (x*x + y*y) < self.radius*self.radius:
                v = openwns.geometry.position.Vector(x = x, y = y , z = 0.0)
                p = v.turn2D(self.rotate).toPosition()
                positions.append(p)
                stationcounter=stationcounter +1
               
            else:
                pass

        return [pos + self.center for pos in positions]

    def isInside(self, position):

        return isInCircle(position, self.radius, self.center)


