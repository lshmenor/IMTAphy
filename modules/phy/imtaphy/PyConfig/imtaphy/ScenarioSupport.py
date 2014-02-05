################################################################################
# This file is part of IMTAphy
# _____________________________________________________________________________
#
# Copyright (C) 2010
# Institute of Communication Networks (LKN)
# Department of Electrical Engineering and Information Technology (EE & IT)
# Technische Universitaet Muenchen
# Arcisstr. 21
# 80333 Muenchen - Germany
# http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
#
# _____________________________________________________________________________
#
#   IMTAphy is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   IMTAphy is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
#
#################################################################################
# partly based on
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

import openwns.geometry.position
import scenarios.ituM2135
import math
import random

speedOfLight = 3e8

def isInCircleInXyPlane(bsPos, msPos, radius):
    if math.sqrt(math.pow(bsPos.x - msPos.x, 2) + math.pow(bsPos.y - msPos.y, 2)) <= radius:
        return True
    else:
        return False
    

def isInHexagon(position, radius, center):
    """ returns true if position is located within hexagon boundaries.
    Can be used to correct random placement of UTs within circle!=hexagon
    Adapted from openWNS
    """
    # rotates hexagon by corrAngle. 0=radius to the right, flat top
    corrAngle = 30.0 / 180*math.pi
    
    vector = (position-center)/radius      # from center to position; normalized
    length = vector.length2D()
    cos30deg = 0.86602540378443865         # =cos(30deg)=sin(60deg)
    # First the easy cases:
    # position even outside the circle touching the corners
    if length > 1.0:
        return False
    # position inside the circle that lies within hexagon
    if length<=cos30deg:
        return True
    # check if in between
    angle  = vector.angle2D()-corrAngle    # 0=right; pi/2=up; pi=left; -pi/2=down
    angleReduced = angle % (math.pi/3.0)   # in [0..60deg]
    x = length*math.cos(angleReduced)
    y = length*math.sin(angleReduced)
    maxy = (1.0-x)*2.0*cos30deg
    isIn = (y<=maxy)
    return isIn


class Scenario:
        # downtilt is not specified in M.2135 but generally accepted values
        # can be found 3GPP TR 36.814 or in the Winner+ document about "Calibration
        # for IMT-Advanced Evaluations"
        def __init__(self, scenarioName, numberOfCircles, msHeight):
            self.scenarioName = scenarioName
            self.msHeight = msHeight

            self.xMin = self.yMin = 0
            
            if scenarioName == 'InH':
                self.bsPlacer = scenarios.ituM2135.IndoorHotspotBSPlacer()
                self.bsHeight = 6.0
                self.downtilt = None
                self.bsPerPRBTxPowerdBm = 21 - 10.0*math.log10(100) # 21dBm per 20MHz BW which consists of 100 180 kHz PRBs
                self.msTotalTxPowerdBm = 21
                self.centerFreqHz = 3.4E09
                self.msSpeedKmh = 3 # ITU-R M.2135 8.4.2.1
                self.wavelengthMeters = speedOfLight / float(self.centerFreqHz)
            elif scenarioName == 'UMi':
                self.bsPlacer = scenarios.ituM2135.UrbanMicroBSPlacer(numberOfCircles=numberOfCircles)
                self.bsHeight = 10.0
                self.downtilt = 12.0 / 180.0 * math.pi
                self.bsPerPRBTxPowerdBm = 41 - 10.0*math.log10(50) # 41dBm per 10MHz BW which consists of 50 180 kHz PRBs
                self.msTotalTxPowerdBm = 24
                self.centerFreqHz = 2.5E09
                self.msSpeedKmh = 3 # ITU-R M.2135 8.4.2.1
                self.wavelengthMeters = speedOfLight / float(self.centerFreqHz)
                
            elif scenarioName == 'UMa':
                self.bsPlacer = scenarios.ituM2135.UrbanMacroBSPlacer(numberOfCircles=numberOfCircles)
                self.bsHeight = 25.0
                self.downtilt = 12.0 / 180.0 * math.pi
                self.bsPerPRBTxPowerdBm = 46 - 10.0*math.log10(50) # 46dBm per 10MHz BW which consists of 50 180 kHz PRBs
                self.msTotalTxPowerdBm = 24
                self.centerFreqHz = 2.0E09
                self.msSpeedKmh = 30 # ITU-R M.2135 8.4.2.1
                self.wavelengthMeters = speedOfLight / float(self.centerFreqHz)
                
            elif scenarioName=='SMa':
                self.bsPlacer = scenarios.ituM2135.SuburbanMacroBSPlacer(numberOfCircles=numberOfCircles)
                self.bsHeight = 35.0
                self.downtilt = 6.0 / 180.0 * math.pi
                self.bsPerPRBTxPowerdBm = 46 - 10.0*math.log10(50) # 46dBm per 10MHz BW which consists of 50 180 kHz PRBs
                self.msTotalTxPowerdBm = 24
                self.centerFreqHz = 2.0E09
                self.msSpeedKmh = 90 # ITU-R M.2135 8.4.2.1, TODO: indoor users should have 3 km/h,
                self.wavelengthMeters = speedOfLight / float(self.centerFreqHz)
                
            elif scenarioName == 'RMa':
                self.bsPlacer = scenarios.ituM2135.RuralMacroBSPlacer(numberOfCircles=numberOfCircles)
                self.bsHeight = 35.0
                self.downtilt = 6.0 / 180.0 * math.pi
                self.bsPerPRBTxPowerdBm = 46 - 10.0*math.log10(50) # 46dBm per 10MHz BW which consists of 50 180 kHz PRBs
                self.msTotalTxPowerdBm = 24
                self.centerFreqHz = 800E06
                self.msSpeedKmh = 120 # ITU-R M.2135 8.4.2.1
                self.wavelengthMeters = speedOfLight / float(self.centerFreqHz)
            else: #todo: change to exception
                print "Value specified for scenarioName is not a valid scenario type"
                sys.exit(1)
            
            

            if scenarioName != 'InH':
                # compute max scenario dimensions so that with the center cell in
                # the middle the leftmost position (taking all rings around center
                # as well as the wrap-around into account) is > (0,0)
                self.xMax = self.yMax = 1 * ((numberOfCircles*2)+2) * self.bsPlacer.interSiteDistance
            
                epsilon = 0.000001
                self.azimuths = [-60.0, 60.0, 180.0 - epsilon]

            else: # see M.2135 p. 12 for InH definition
                  self.xMax = 120
                  self.yMax = 50
                  self.azimuths = [0.0]

            self.bsPlacer.setCenter(self.getCenter())


        def extendBoundingBoxToMultiplesOf(self, xBins, yBins):
            self.xMax = self.xMax - (self.xMax % xBins) + xBins
            self.yMax = self.yMax - (self.yMax % xBins) + yBins

            # update bsPlacer:
            self.bsPlacer.setCenter(self.getCenter())


        def getAzimuths(self):
            return self.azimuths

        def getCenterFrequencyHz(self):
            return self.centerFreqHz

        def getBoundingBox(self):
            return openwns.geometry.position.BoundingBox(xmin = 0.0, xmax = self.xMax,
                                                         ymin = 0.0, ymax = self.yMax,
                                                         zmin = 0, zmax = 20)

        def getCenter(self):
            center = openwns.geometry.position.Position(x = self.xMax/2.0, y = self.yMax/2.0, z = 0.0)
            return center

        def getInterSiteDistance(self):
            return self.bsPlacer.interSiteDistance

        def getMinDistance(self):
            #	According to M.2135 we should keep a scenario-dependent minimum distance between
            #	mobiles and their serving cell base station in the x-y plane. We enforce this
            #	minimum distance for all mobiles to all cells because it is much easier and also
            #	highly unlikely that a mobile that close would be associated to a different BS.
            minDistances = { # according to M.2135 Table 8-2
                "InH": 3,
                "UMi": 10,
                "UMa": 25,
                "RMa": 35,
                "SMa": 35
                }
            return minDistances[self.scenarioName]


def checkSectors(msPos, bsPos, azimuthsDegrees, minDistance, radius):
    for azimuth in azimuthsDegrees:
        cellCenter = bsPos + openwns.geometry.position.Vector(x = math.sin(azimuth / 180 * math.pi)*radius,
                                                              y = math.cos(azimuth / 180 * math.pi)*radius)
        # the min distance is in the X-Y plane according to M.2135 Table 8-2 footnote (2)
        if isInHexagon(msPos, radius, cellCenter) and not isInCircleInXyPlane(bsPos, msPos, minDistance):
            return True
    return False



def placeMobilesEquallyInCells(sites, scenarioConfig, probeConfig):
    mobiles = []
    for x in xrange(probeConfig.xBins):
        for y in xrange(probeConfig.yBins):
            pos = openwns.geometry.position.Position(x = x * (scenarioConfig.xMax - scenarioConfig.xMin) / probeConfig.xBins + scenarioConfig.xMin,
                                                     y = y * (scenarioConfig.yMax - scenarioConfig.yMin) / probeConfig.yBins + scenarioConfig.yMin,
                                                     z = scenarioConfig.msHeight)
            for bsPos in sites:
                if checkSectors(pos, bsPos, scenarioConfig.getAzimuths(),
                                            scenarioConfig.getMinDistance(), scenarioConfig.getInterSiteDistance() / 3):
                    mobiles.append(pos)

                    # we can go to next position. Before we permute the list in a way that
                    # the first element is the current position becuase at the next scanner
                    # position we will most likely look at the same cell
                    if bsPos != sites[0]:
                        newList = [bsPos]
                        newList.extend([p for p in sites if p != bsPos])
                        sites= newList
                    break # to next mobile
    return mobiles

def placeMobilesEquallyInRectangle(sites, scenarioConfig, probeConfig):
    mobiles = []
    for x in xrange(probeConfig.xBins):
        for y in xrange(probeConfig.yBins):
            pos = openwns.geometry.position.Position(x = x * (scenarioConfig.xMax - scenarioConfig.xMin) / probeConfig.xBins + scenarioConfig.xMin,
                                                     y = y * (scenarioConfig.yMax - scenarioConfig.yMin) / probeConfig.yBins + scenarioConfig.yMin,
                                                     z = scenarioConfig.msHeight)
            tooClose = False
            for bsPos in sites:
                if isInCircleInXyPlane(bsPos, pos, scenarioConfig.getMinDistance()):
                    tooClose = True
            if not tooClose:
                mobiles.append(pos)
    return mobiles


def placeMobilesUniformlyRandomlyInCells(numMS, sites, scenarioConfig):
    mobiles = []

    while len(mobiles) < numMS:
       pos = openwns.geometry.position.Position(x = random.randint(scenarioConfig.xMin, scenarioConfig.xMax),
                                                y = random.randint(scenarioConfig.yMin, scenarioConfig.yMax),
                                                z = scenarioConfig.msHeight)
       for bsPos in sites:
           if checkSectors(pos, bsPos, scenarioConfig.getAzimuths(),
                           scenarioConfig.getMinDistance(), scenarioConfig.getInterSiteDistance() / 3):
                mobiles.append(pos)
    return mobiles


def placeMobilesUniformlyRandomlyInRectangle(numMS, sites, scenarioConfig):
    mobiles = []
    while len(mobiles) < numMS:
       pos = openwns.geometry.position.Position(x = random.randint(scenarioConfig.xMin, scenarioConfig.xMax),
                                                y = random.randint(scenarioConfig.yMin, scenarioConfig.yMax),
                                                z = scenarioConfig.msHeight)
       tooClose = False
       for bsPos in sites:
           if isInCircleInXyPlane(bsPos, pos, scenarioConfig.getMinDistance()):
                tooClose = True
       if not tooClose:
           mobiles.append(pos)
    return mobiles

