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

import scenarios.placer
import unittest
import math
import random
# WARNING: This is a statistic test. There is always a probability it could fail! We tolerate 1% error
# If you are sure everything is still OK: Increase "count"

# info: for the mean tests, the reference value does not mean anything (like mean=0), so I take the range of uniform distribution and tolerance as
# reference for the mean tests. like width and height for rectangular and interSiteDistance for hexagonal.

class RectangularAreaPlacerTest(unittest.TestCase):

    count = 5E5
    tolerance = 0.01

    def setUp(self):
        self.placer = scenarios.placer.RectangularAreaPlacer(int(self.count), 100, 100, 10)
        random.seed(1234)
        
    def testMeanVar(self):
        pos = self.placer.getPositions()
        
        sum_x = 0
        sum_y = 0
        for p in pos:
          sum_x = sum_x + p.x
          sum_y = sum_y + p.y
          
        meanX = sum_x / self.count
        meanY = sum_y / self.count  
        
        self.assert_(abs(meanX) < 100.0 * self.tolerance)
        self.assert_(abs(meanY) < 100.0 * self.tolerance)

        sum_x2 = 0
        sum_y2 = 0
        for p in pos:
            sum_x2 = sum_x2 + (p.x - meanX) * (p.x - meanX)
            sum_y2 = sum_y2 + (p.y - meanY) * (p.y - meanY)
        
        varX = sum_x2 / (self.count - 1)
        varY = sum_y2 / (self.count - 1)

        self.assert_(abs(varX- 856.0)<856.0 * self.tolerance)
        self.assert_(abs(varY- 856.0)<856.0 * self.tolerance)



class HexagonalAreaPlacerTest(unittest.TestCase):

    count = 5E5
    tolerance = 0.01
    def setUp(self):
        self.placer = scenarios.placer.HexagonalAreaPlacer(int(self.count), 100, 10)
        random.seed(1234)
        
    def testMeanVar(self):
        pos = self.placer.getPositions()
        
        sum_x = 0
        sum_y = 0
        for p in pos:
          sum_x = sum_x + p.x
          sum_y = sum_y + p.y
          
        meanX = sum_x / self.count
        meanY = sum_y / self.count  
         
        self.assert_(abs(meanX-1000.0) < 100.0 * self.tolerance)
        self.assert_(abs(meanY-1000.0) < 100.0 * self.tolerance)

        sum_x2 = 0
        sum_y2 = 0
        for p in pos:
            sum_x2 = sum_x2 + (p.x - meanX) * (p.x - meanX)
            sum_y2 = sum_y2 + (p.y - meanY) * (p.y - meanY)
        
        varX = sum_x2 / (self.count - 1)
        varY = sum_y2 / (self.count - 1)

        self.assert_(abs(varX- 720.0)<720.0 * self.tolerance)
        self.assert_(abs(varY- 720.0)<720.0 * self.tolerance)




class CircularAreaPlacerTest(unittest.TestCase):

    count = 5E5
    tolerance = 0.01
    def setUp(self):
        self.placer = scenarios.placer.CircularAreaPlacer(int(self.count), 100, 10)
        random.seed(1234)
        
    def testMeanVar(self):
        pos = self.placer.getPositions()
        
        sum_x = 0
        sum_y = 0
        for p in pos:
          sum_x = sum_x + p.x
          sum_y = sum_y + p.y
          
        meanX = sum_x / self.count
        meanY = sum_y / self.count  
         
        self.assert_(abs(meanX) < 100.0 * self.tolerance)
        self.assert_(abs(meanY) < 100.0 * self.tolerance)

        sum_x2 = 0
        sum_y2 = 0
        for p in pos:
            sum_x2 = sum_x2 + (p.x - meanX) * (p.x - meanX)
            sum_y2 = sum_y2 + (p.y - meanY) * (p.y - meanY)
        
        varX = sum_x2 / (self.count - 1)
        varY = sum_y2 / (self.count - 1)
        self.assert_(abs(varX- 2520.0)<2520.0 * self.tolerance)
        self.assert_(abs(varY- 2520.0)<2520.0 * self.tolerance)
