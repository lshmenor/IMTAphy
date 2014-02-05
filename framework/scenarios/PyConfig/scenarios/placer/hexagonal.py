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

class HexagonalPlacer(scenarios.interfaces.INodePlacer):

    def __init__(self, numberOfCircles, interSiteDistance, rotate = 0.0):
        """ 
        @type  numberOfCircles: int
        @param numberOfCircles: The number of circles around the center cell

        @type  interSiteDistance: float
        @param interSiteDistance: The cell interSiteDistance is in Meters [m]

        @type  rotate: float
        @param rotate: Rotate the final result by rotate in radiant [0..2pi]
        """
        assert numberOfCircles >= 0
        assert interSiteDistance > 0
        assert 0 <= rotate <= 2.0*math.pi # rotates the final result by corrAngle

        self.numberOfCircles = numberOfCircles
        self.interSiteDistance = interSiteDistance

        self.rotate = rotate

        self.center = openwns.geometry.position.Position(x = 1000.0, y = 1000.0, z = 0.0)

    def setCenter(self, center):
        self.center = center

    def _transformHexCoordinates(self, i, j, gridDistance):
        return openwns.geometry.position.Vector(math.sqrt(3)/2.0*j,float(i)+0.5*float(j), 0.0)*gridDistance

    def getPositions(self):
        centralBSPosition = openwns.geometry.position.Position(x = 0.0, y = 0.0, z = 0.0)
        posList = [centralBSPosition] # central BS position

        for circle in xrange(1, self.numberOfCircles +1): #  xrange(numberOfCircles)=0..numberOfCircles-1
            i=circle; j=0 # start vector
            di=-1
            dj=1 # delta vector
            for edge in xrange(6): # 0..5
                for way in xrange(circle):
                    v = self._transformHexCoordinates(i,j, self.interSiteDistance)
                    p = v.turn2D(self.rotate).toPosition()

                    posList.append(p)

                    i=i+di; j=j+dj

                # turn delta vector by 60 degrees right
                odi=di; odj=dj
                di=-odj
                dj=odi+odj

        return [pos + self.center for pos in posList]

    def getCenterCellPosition(self):

        """
        Return BS position of center cell
        """
        return self.center

    def isInside(self, position):

        return isInHexagon(position, float(self.interSiteDistance) / math.sqrt(3), self.center, self.rotate)


def isInHexagon(position, radius, center, corrAngle = 0.0):
    """ returns true if position is located within hexagon boundaries.
        Can be used to correct random placement of UTs within circle!=hexagon
    """
    assert 0 <= corrAngle <= 2.0*math.pi   # rotates hexagon by corrAngle. 0=radius to the right, flat top
    vector = (position-center)/radius      # from center to position; normalized
    length = vector.length2D()
    cos30deg = 0.86602540378443865         # =cos(30deg)=sin(60deg)
    if length>1.0:
        return False
    if length<=cos30deg:
        return True
    angle  = vector.angle2D()-corrAngle    # 0=right; pi/2=up; pi=left; -pi/2=down
    angleReduced = angle % (math.pi/3.0)   # in [0..60deg]
    x = length*math.cos(angleReduced)
    y = length*math.sin(angleReduced)
    maxy = (1.0-x)*2.0*cos30deg
    isIn = (y<=maxy)
    return isIn

def createAreaScanMobility(steps, radius, minDistance, center, corrAngle):
    import rise.Mobility
    from scenarios.placer.circular import isInCircle
    mobility = None
    radius = radius * 2 / math.sqrt(3)
    xMin = center.x - radius
    yMin = center.y - radius

    stepSize = 2.0 * radius / float(steps)

    for x in xrange(steps + 1):
        for y in xrange(steps + 1):
            pos = openwns.geometry.position.Position(xMin + x * stepSize, yMin + y * stepSize, 0.0)
            if isInHexagon(pos, radius, center, corrAngle) and not isInCircle(pos, minDistance, center):
                if mobility is None:
                    mobility = rise.Mobility.EventList(pos)
                else:
                    mobility.addWaypoint(0.01 * (x+1) * (y+1), pos)
    return mobility
                
                
                
class HexagonalAreaPlacer(scenarios.interfaces.INodePlacer):

    def __init__(self, numberOfNodes, interSiteDistance, minDistance = 0.0, rotate = 0.0):
        """ 
        @type  numberOfCircles: int
        @param numberOfCircles: The number of circles around the center cell

        @type  interSiteDistance: float
        @param interSiteDistance: The cell interSiteDistance is in Meters [m]

        @type  rotate: float
        @param rotate: Rotate the final result by rotate in radiant [0..2pi]
        """
        assert numberOfNodes >= 0
        assert interSiteDistance > 0
        assert 0 <= rotate <= 2.0*math.pi # rotates the final result by corrAngle

        self.interSiteDistance = interSiteDistance
        self.numberOfNodes = numberOfNodes
        self.rotate = rotate
        self.minDistance = minDistance

        self.center = openwns.geometry.position.Position(x = 1000.0 , y = 1000.0, z = 0.0)

    def setCenter(self, center):
        self.center = center

    def _transformHexCoordinates(self, i, j, gridDistance):
        return openwns.geometry.position.Vector(math.sqrt(3)/2.0*j,float(i)+0.5*float(j), 0.0)*gridDistance

    def getPositions(self):

        positions = []
        stationcounter = 0
        while stationcounter < self.numberOfNodes:
            x = random.random() * self.interSiteDistance * 2.0 / math.sqrt(3) - self.interSiteDistance / math.sqrt(3)
            y = random.random() * self.interSiteDistance * 2.0 / math.sqrt(3) - self.interSiteDistance / math.sqrt(3)
            ptemp =  openwns.geometry.position.Position(x=x, y=y, z=0.0)
            if isInHexagon(position=ptemp, radius=self.interSiteDistance / math.sqrt(3), center=openwns.geometry.position.Position(0.0,0.0,0.0), corrAngle = self.rotate) == True:
                if x*x + y*y > self.minDistance*self.minDistance:
                    v = openwns.geometry.position.Vector(x = x, y = y , z = 0.0)
                    p = v.turn2D(self.rotate).toPosition()
                    positions.append(p)
                    stationcounter=stationcounter +1
                
            else:
                pass



        return [pos + self.center for pos in positions]

    def isInside(self, position):

        return isInHexagon(position, float(self.interSiteDistance) / math.sqrt(3), self.center, self.rotate)


class UniformDistributedPlacerInHexagonalGrid(scenarios.interfaces.INodePlacer):

    """
    Place a number of nodes uniformly distributed on a given area.
    """
    def __init__(self, numberOfNodes, numberOfCircles, interSiteDistance, minDistance = 0.0, rotate=0.0):

        """
        @type  numberOfNodes: int
        @param numberOfNodes: The number of nodes in the scenario
        @type  numberOfCircles: int
        @param numberOfCircles: The number of circles surrounding the center cell
        @type  interSiteDistance: float
        @param interSiteDistance: The cells' interSiteDistance [m]
        @type  rotate: float
        @param rotate: Rotation of the hexagonal BS positioning layout

        """
        assert numberOfCircles >= 0
        assert interSiteDistance > 0
        assert 0 <= rotate <= 2.0*math.pi # rotates the final result by corrAngle

        self.numberOfNodes = numberOfNodes
        self.numberOfCircles = numberOfCircles
        self.interSiteDistance = interSiteDistance
        self.rotate = rotate
        self.center = openwns.geometry.position.Position(x = 1000.0, y = 1000.0, z = 0.0)
        self.minDistance = minDistance
        self.positions = []
        self.perBS = False
        self.bsPositions = []

    def setCenter(self, center):
        self.center = center
    
    def getPositions(self):

        if self.positions == []:
            return self._getPositions()
        else: # positions already calculated
            return self.positions

    def _getPositions(self):
        radius = self.interSiteDistance/math.sqrt(3)
        lowerXBound = self.center.x-((self.numberOfCircles*self.interSiteDistance)+radius)
        upperXBound = self.center.x+(self.numberOfCircles*self.interSiteDistance)+radius
        lowerYBound = self.center.y-((self.numberOfCircles*self.interSiteDistance)+radius)
        upperYBound = self.center.y+(self.numberOfCircles*self.interSiteDistance)+radius

        positions = []
        for node in xrange(self.numberOfNodes):
            utPositionIsValid = False

            while(not utPositionIsValid):
                xPos = random.uniform(lowerXBound,upperXBound)
                yPos = random.uniform(lowerYBound,upperYBound)
                utPos = openwns.geometry.position.Position(xPos, yPos)
                
                # find serving cell (in terms of shortes distance UE <--> BS)
                servingBSPosition = self._findServingBS(utPos)

                # minimum distance UE to serving BS (in horizontal terms): 25m
                if ((self.getPositionDistance(utPos, servingBSPosition) >= self.minDistance) and isInHexagon(utPos, radius, servingBSPosition, self.rotate)):

                    positions.append(utPos)
                    utPositionIsValid = True

        return positions

    def getPositionDistance(self, pos1, pos2):

        return math.sqrt((pos1.x-pos2.x)**2+(pos1.y-pos2.y)**2)

    def _findServingBS(self, _utPos):

        """
        Given the position of the UT find the BS of shortest distance
        and return the base station's position
        """


        """
        calculate list of BS positions
        self.interSiteDistance is in Meters [m]
        self.center is a Position(x,y,z) of center Base Station
        self.rotate is in radiant [0..2pi]
        self.rotate can be 0 (default) or e.g. math.pi/6.0 ( = 30 degrees)
        """

        self.bsPositions.append(self.center) # central BS position

        numInnerCells = (self.numberOfCircles*(self.numberOfCircles+1)/2)*6+1
        for circle in xrange(1,self.numberOfCircles+1): #  xrange(nCircles)=0..nCircles-1
            i=circle; j=0 # start vector
            di=-1; dj=1 # delta vector
            for edge in xrange(6): # 0..5
                for way in xrange(circle):
                    v = openwns.geometry.position.Vector(math.sqrt(3)/2.0*j,float(i)+0.5*float(j))*self.interSiteDistance
                    p = v.turn2D(self.rotate).toPosition() + self.center
                    self.bsPositions.append(p)
                    i=i+di; j=j+dj
                # turn delta vector by 60 degrees right
                odi=di; odj=dj
                di=-odj
                dj=odi+odj


        bsPos = self.center
        minDistance = self.numberOfCircles*self.interSiteDistance*10 # > than possible UT positions
        for bs in self.bsPositions:
            distance = self.getPositionDistance(_utPos, bs)

            if distance < minDistance:
                minDistance = distance
                bsPos = bs
        return bsPos


    def getUEsInCenterCell(self, utPosList):
        radius = self.interSiteDistance/math.sqrt(3)
        centerCellUEs = []
        for utPos in utPosList:
            if (isInHexagon(utPos, radius, self.center, self.rotate)):
                centerCellUEs.append(utPos)
        return centerCellUEs


    def setNumberOfNodes(self, _numOfNodes):
        self.numberOfNodes = _numOfNodes


    def isInside(self, position):

        return isInHexagon(position, float(self.interSiteDistance) / math.sqrt(3), self.center, self.rotate)


