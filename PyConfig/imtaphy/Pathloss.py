# -*- coding: utf-8 -*-
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

import math
from openwns.pyconfig import attrsetter

class Pathloss:
    minPathloss = None
    
class M2135Pathloss:
    """
    Pathloss models from ITU-R Report M.2135
    """
    nameInChannelFactory = "imtaphy.pathloss.M2135"
    SMaBuildingHeight = 10
    SMaStreetWidth = 20
    RMaBuildingHeight = 5
    RMaStreetWidth = 20
    UMaBuildingHeight = 20
    UMaStreetWidth = 20
    feederLoss = 2 #2dB feeder loss
    
    def __init__(self, **kw):
        attrsetter(self, kw)
        
    

class No(Pathloss):
    nameInChannelFactory = "imtaphy.pathloss.No"

class SingleSlope(Pathloss):
    nameInChannelFactory = "imtaphy.pathloss.SingleSlope"
    frequencyFactorLOS = None
    frequencyFactorNLOS = None
    distanceFactorLOS = None
    distanceFactorNLOS = None
    offsetLOS = None
    offsetNLOS = None
    
    def __init__(self, fLOS, fNLOS, dLOS, dNLOS, offsetLOS, offsetNLOS, minPathloss = 0):
        self.frequencyFactorLOS = fLOS
        self.frequencyFactorNLOS = fNLOS
        self.distanceFactorLOS = dLOS
        self.distanceFactorNLOS = dNLOS
        self.offsetLOS = offsetLOS
        self.offsetNLOS = offsetNLOS
        self.minPathloss = minPathloss

class R4092042(Pathloss):
    nameInChannelFactory = "imtaphy.pathloss.R4092042"
    # frequencyFactorLOS = None
    # frequencyFactorNLOS = None
    distanceFactor1 = None
    distanceFactor2 = None
    offset1 = None
    offset2 = None
    apartmentWallsLoss = None
    housesBoxList = None


    def __init__(self, d1, d2, offset1, offset2, apartmentWalls = 7, minPathloss = 0, housesBoxList=[]):
        #self.frequencyFactorLOS = fLOS
        #self.frequencyFactorNLOS = fNLOS
        self.distanceFactor1 = d1
        self.distanceFactor2 = d2
        self.offset1 = offset1
        self.offset2 = offset2
        self.apartmentWallsLoss = apartmentWallsLoss
        self.minPathloss = minPathloss
        self.housesBoxList = housesBoxList


class DenmarkIndoor(SingleSlope):
    def __init__(self):
        #super(DenmarkIndoor, self)
        self.frequencyFactorLOS = 20 #fLOS
        self.frequencyFactorNLOS = 20 #fNLOS
        self.distanceFactorLOS = 18.7 #dLOS
        self.distanceFactorNLOS =  20 # dNLOS
        self.offsetLOS = 46.8 + 20.0 * math.log10(1./5000.) # offsetLOS
        self.offsetNLOS = 46.4 + 20.0 * math.log10(1./5000.) # offsetNLOS
        self.minPathloss = 45 # minpathloss




class Home4By4ScenarioModel(SingleSlope):
    def __init__(self, scalingFactor):
        self.frequencyFactorLOS = 20 #fLOS
        self.frequencyFactorNLOS = 20 #fNLOS
        self.distanceFactorLOS = 18.7 #dLOS
        self.distanceFactorNLOS =  20 # dNLOS
        self.offsetLOS = 46.8 + 20.0 * math.log10(1./5000.) - 18.7 * math.log10(scalingFactor) # offsetLOS   // Added -5 for NLOS1 Case of WINNER  //  Added -20dB to convert from m to dm,  
        self.offsetNLOS = 46.4 + 20.0 * math.log10(1./5000.) - 20 * math.log10(scalingFactor) # offsetNLOS  // Added -5 for NLOS1 Case of WINNER  // Added -20dB to convert from m to dm, // CONSIDER THE FEMTOCELLS ON THE CORRIDORS
        self.minPathloss = 45 # minpathloss    


class OfficeScenarioModelNL1_dm(SingleSlope):
    def __init__(self, scalingFactor):
        #super(DenmarkIndoor, self)
        self.frequencyFactorLOS = 20 #fLOS
        self.frequencyFactorNLOS = 20 #fNLOS
        self.distanceFactorLOS = 18.7 #dLOS
        self.distanceFactorNLOS =  36.8 # dNLOS
        self.offsetLOS = 46.8 + 20.0 * math.log10(1./5000.) - 5 - 18.7 * math.log10(scalingFactor)  # offsetLOS  // Added -20dB to convert from m to dm,
        self.offsetNLOS = 43.8 + 20.0 * math.log10(1./5000.) - 5 - 36.8 * math.log10(scalingFactor) # offsetNLOS  // Added -20dB to convert from m to dm,
        self.minPathloss = 45 # minpathloss

# TODO: NEED TO ADD NLOS2 FOR ROOM TO ROOM WALL SHADOWING OF WINNER SCENARIO IN OFFICE MODEL !!!


class DenseScenarioModel(R4092042):
    def __init__(self, scalingFactor, housesBoxList):
        #super(DenmarkIndoor, self)
        #self.frequencyFactorLOS = 0 #fLOS       # TODO: Take the frequency as 2GHZ. // No frequency factor !!
        #self.frequencyFactorNLOS = 0 #fNLOS
        self.distanceFactor1 = 37.6 #dLOS         
        self.distanceFactor2 =  20 # dNLOS
        self.offset1 = 15.3 - 37.6 * math.log10(scalingFactor)  # offsetNLOS  // Added -20dB to convert from m to dm,
        self.offset2 = 38.46 - 20 * math.log10(scalingFactor)  # SAME APARTMENT STRIP AS LOS  // Added -20dB to convert from m to dm,

        self.minPathloss = 45 # minpathloss     # TODO: Need to add floors and ceilings !!
        self.apartmentWallsLoss = (0.7/scalingFactor)
        print "apartmentWallsLoss", self.apartmentWallsLoss
        self.housesBoxList = housesBoxList

class FiveXFiveScenarioModel(R4092042):
    def __init__(self, scalingFactor, housesBoxList):
        #super(DenmarkIndoor, self)
        #self.frequencyFactorLOS = 0 #fLOS       # TODO: Take the frequency as 2GHZ. // No frequency factor !!
        #self.frequencyFactorNLOS = 0 #fNLOS
        self.distanceFactor1 = 37.6 #dLOS       
        self.distanceFactor2 =  20 # dNLOS
        self.offset1 = 15.3 - 37.6 * math.log10(scalingFactor)   # offsetNLOS  // Added -20dB to convert from m to dm,
        self.offset2 = 38.46 - 20 * math.log10(scalingFactor)  # SAME APARTMENT BLOCK AS LOS  // Added -20dB to convert from m to dm,
        self.apartmentWallsLoss = (0.7/scalingFactor)                                           

        self.minPathloss = 45 # minpathloss     # TODO: Need to add floors and ceilings !!
        print "apartmentWallsLoss", self.apartmentWallsLoss
        self.housesBoxList = housesBoxList
        
        
#class OfficeScenarioModelNL2(SingleSlope):
    #def __init__(self):
        ##super(DenmarkIndoor, self)
        #self.frequencyFactorLOS = 20 #fLOS
        #self.frequencyFactorNLOS = 20 #fNLOS
        #self.distanceFactorLOS = 18.7 #dLOS
        #self.distanceFactorNLOS =  60 # dNLOS
        #self.offsetLOS = 46.8 + 20.0 * math.log10(1./5000.) # offsetLOS
        #self.offsetNLOS = 66.4 + 20.0 * math.log10(1./5000.) # offsetNLOS
        #self.minPathloss = 45 # minpathloss
