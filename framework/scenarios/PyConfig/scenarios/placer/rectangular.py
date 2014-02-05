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

class RectangularPlacer(scenarios.interfaces.INodePlacer):

    def __init__(self, w, h, rotate = 0.0):
        self.w = w
        self.h = h
        self.rotate = rotate
        self.center = openwns.geometry.position.Position(1000.0, 1000.0, 0.0)

    def setCenter(self, pos):
        self.center = pos

    def getPositions(self):
        positions = []
        positions.append(openwns.geometry.position.Vector(0.0, 0.0, 0.0))
        positions.append(openwns.geometry.position.Vector(0.0, self.h, 0.0))
        positions.append(openwns.geometry.position.Vector(self.w, 0.0, 0.0))
        positions.append(openwns.geometry.position.Vector(self.w, self.h, 0.0))
        positions = [v.turn2D(self.rotate).toPosition() for v in positions]
        positions = [p + self.center for p in positions]
        return positions

    def isInside(self, position):

        return isInRectangle(position, self.w, self.h, self.center)

def isInRectangle(position, w, h, center):
    p = position - center
    p.x += float(w)/2.0
    p.y += float(h)/2.0
    if (p.x >= 0.0) and (p.x <= w) and (p.y >= 0.0) and (p.y <= h):
        return True
    else:
        return False

def createAreaScanMobility(stepsX, stepsY, w, h, center):
    import rise.Mobility
    from scenarios.placer.circular import isInCircle
    mobility = None

    stepSizeX = float(w)/float(stepsX)
    stepSizeY = float(h)/float(stepsY)

    for x in xrange(stepsX + 1):
        for y in xrange(stepsY + 1):
            pos = openwns.geometry.position.Position(x * stepSizeX + center.x - float(w)/2.0, y * stepSizeY + center.y - float(h)/2.0, 0.0)
            bs1 = openwns.geometry.position.Position(center.x - 30.0, center.y, center.z)
            bs2 = openwns.geometry.position.Position(center.x + 30.0, center.y, center.z)
            if isInRectangle(pos, w, h, center) and not isInCircle(pos, 3, bs1) and not isInCircle(pos, 3, bs2):
                if mobility is None:
                    mobility = rise.Mobility.EventList(pos)
                else:
                    mobility.addWaypoint(0.01 * (x+1) * (y+1), pos)
    return mobility


class RectangularAreaPlacer(scenarios.interfaces.INodePlacer):

    def __init__(self, numberOfNodes, w, h, minDistance = 0.0, rotate = 0.0, perBS = True):
        self.numberOfNodes = numberOfNodes
        self.w = w
        self.h = h
        self.rotate = rotate
        self.center = openwns.geometry.position.Position(0.0, 0.0, 0.0)
        self.minDistance = minDistance
        self.perBS = perBS
        
    def setCenter(self, pos):
        self.center = pos

    def getPositions(self):
        positions = []
        stationcounter = 0
        while stationcounter < self.numberOfNodes:
            x = random.random() * self.w - self.w/2
            y = random.random() * self.h - self.h/2
            if x*x + y*y > self.minDistance * self.minDistance:
                 
                v = openwns.geometry.position.Vector(x=x , y=y , z = 0.0)

                p = v.turn2D(self.rotate).toPosition()
                positions.append(p)

                stationcounter = stationcounter + 1

        return [pos + self.center for pos in positions]

    def isInside(self, position):

        return isInRectangle(position, self.w, self.h, self.center)
