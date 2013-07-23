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
import sys

class LinearAntennaArray:
    def __init__(self, type, numElements, wavelengthMeters, **kwargs):
        self.distancesToFirstElement = []
        self.slants = []
        
        if type is None: # this allows constructing an arbitrary antenna array by providing the distances and slants manually
            self.distancesToFirstElement = kwargs['distancesToFirstElement']
            self.slants = kwargs['slants']
       
        # the antenna array element separation seems unspecified in M.2135, this is from 36.814:
        elif str(type).upper() == "MOBILESTATIONITU":
            # default ITU antenna is vertically polarized (slant angle 90 degrees) with elementSpacingMeters distances between all elements
            self.distancesToFirstElement = [float(i) * 0.5 *float(wavelengthMeters) for i in range(numElements)]
            self.slants = [math.radians(90) for i in range(numElements)]        
            
        elif str(type).upper() == "BASESTATIONITU" or type == 0:
            # default ITU antenna is vertically polarized (slant angle 90 degrees) with elementSpacingMeters distances between all elements
            self.distancesToFirstElement = [float(i) * 10.0 *float(wavelengthMeters) for i in range(numElements)]
            self.slants = [math.radians(90) for i in range(numElements)]
            
        elif str(type).upper() == 'A' or type == 1: 
            # According to 3GPP TR 36.814 table A.3-1, base station antenna config A)
            # features co-polarized (vertically) antennas separated by 4 wavelengths
            self.distancesToFirstElement = [float(i*4) * float(wavelengthMeters) for i in range(numElements)]
            self.slants = [math.radians(90) for i in range(numElements)]
            
        elif str(type).upper() == 'B' or type == 2:
            # According to 3GPP TR 36.814 table A.3-1, base station antenna config B)
            # Grouped co-polarized: Two groups of co-polarized antennas. 10 wavelengths between center 
            # of each group. 0.5 wavelength separation within each groups
            numGroup1 = int(math.ceil(numElements /2))
            group1 = [float(i) * 0.5 * float(wavelengthMeters) for i in range(numGroup1)]
            group2 = [10.0 * float(wavelengthMeters) + float(i) * 0.5 * float(wavelengthMeters) for i in range(numElements - numGroup1)]
            print group1
            print group2
            self.distancesToFirstElement = group1 + group2
            self.slants = [math.radians(90) for i in range(numElements)]
            
        elif str(type).upper() == 'C' or type == 3:
            # According to 3GPP TR 36.814 table A.3-1, base station antenna config C)
            # features co-polarized (vertically) antennas separated by 4 wavelengths
            self.distancesToFirstElement = [float(i) * 0.5 * float(wavelengthMeters) for i in range(numElements)]
            self.slants = [math.radians(90) for i in range(numElements)]
            
        elif (str(type).upper() == 'D' or type == 4) or (str(type).upper() == 'E' or type == 5):
            if str(type).upper() == 'D' or type == 4:
                spacing = 4.0
            else:
                spacing = 0.5 # for E
            # According to 3GPP TR 36.814 table A.3-1, base station antenna config D)
            # Uncorrelated cross-polarized: Columns with +-45deg linearly polarized antennas Columns separated 4 wavelengths
            if (numElements % 2) != 0:
                raise Exception("Antenna Configuration D needs an even number of antenna elements")
            
            numGroups = numElements / 2
            slantAngles = [math.radians(+45.0), math.radians(-45.0)]
            for i in range(numGroups):
                for j in range(2):
                    self.distancesToFirstElement.append(float(i) * spacing * wavelengthMeters)
                    self.slants.append(slantAngles[j])
        else:
            raise Exception("Bad IMTAdvanced / 3GPP antenna configuration ID")

class IMTAdvancedAntenna(LinearAntennaArray):
    nameInAntennaFactory = "imtaphy.antenna.ITU"
    """
        Azimuth should be between -Pi and PI in radians.
     
        Downtilt should be between -PI/2 and PI/2 in radians. 0 degrees (0 rad) 
        downtilt is horizontal, -90 degrees (-PI/2 rad) is looking straight up,
        and +90 degrees (PI/2 rad) is looking straight into the ground.
        
        From 3GPP TR 36.814 (V9.0.0 2010-03) BS antenna downtilt
        ITU Indoor, indoor hotspot scenario (InH): N/A 
        ITU Microcellular, urban micro-cell scenario (Umi): 12deg 
        ITU Base coverage urban, Urban  macro-cell scenario (Uma): 12deg 
        ITU High speed, Rural macro-cell scenario (Rma): 6 deg 
        Case 1 3GPP 3D: 15 deg 
        Case 1 3GPP 2D: N/A
       
        Element spacing is in meters so it has to be adapted to the wave lenght to
        get, e.g., 0.5 lambda spacing
       
        """
    def __init__(self, type, azimuth, downtilt, antennaGain, numElements, wavelengthMeters, logger = None):
        LinearAntennaArray.__init__(self, type, numElements, wavelengthMeters)
        self.azimuth = azimuth
        self.downtilt = downtilt
        self.antennaGain = antennaGain
        self.logger = logger
        self.numElements = numElements

class Omnidirectional(LinearAntennaArray):
    nameInAntennaFactory = "imtaphy.antenna.Omnidirectional"
    
    def __init__(self, type, antennaGain, azimuth, numElements, wavelengthMeters, logger = None):
        LinearAntennaArray.__init__(self, type, numElements, wavelengthMeters)
        self.azimuth = azimuth
        self.antennaGain = antennaGain
        self.numElements = numElements
        self.logger = logger
        
class OmnidirectionalForTests(LinearAntennaArray):        
    nameInAntennaFactory = "imtaphy.antenna.Omnidirectional"
    
    def __init__(self, antennaGain = "0 dB", azimuth = 0, numElements = 1, elementSpacingMeters = 0.5*0.15, logger = None):
        distancesToFirstElement = [float(i) * elementSpacingMeters for i in range(numElements)]
        slants = [math.radians(90) for i in range(numElements)]
        LinearAntennaArray.__init__(self, type = None, numElements = None, wavelengthMeters = None, distancesToFirstElement = distancesToFirstElement, slants = slants)
        self.numElements = numElements
        self.azimuth = azimuth
        self.antennaGain = antennaGain
        self.logger = logger
        
      