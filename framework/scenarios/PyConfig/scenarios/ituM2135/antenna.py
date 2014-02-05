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

import scenarios.antenna.isotropic
import scenarios.interfaces

class IndoorHotspotAntennaCreator(scenarios.antenna.IsotropicAntennaCreator):

    def __init__(self):
        scenarios.antenna.IsotropicAntennaCreator.__init__(self, [0.0, 0.0, 6.0])

    def create(self):
        antenna = scenarios.antenna.IsotropicAntennaCreator.create(self)
        return antenna

    def getAntenna(self):
        antenna = scenarios.antenna.IsotropicAntennaCreator.create(self)
        return antenna


class UrbanMicroAntennaCreator(scenarios.interfaces.IAntennaCreator):

    def __init__(self, sectorization):
        self.sectorization = sectorization
        self.antennalist = []
        if self.sectorization == True:
            self.azimuth = [30.0* 2.0 * 3.14 / 360.0, 150.0* 2.0 * 3.14 / 360.0, 270.0* 2.0 * 3.14 / 360.0]
        else:
            self.azimuth = [0.0]
    def create(self):
        import rise.Antenna
        # Downtilt 12 degrees (in rise up is 0 degree horizontal is 90 degree)
        downtilt = (12.0 + 90.0)/360.0 * 2 * 3.14159265
        for azimuth in self.azimuth:
            antenna = rise.Antenna.ITU("17.0 dB", [0.0, 0.0, 10.0], azimuth, downtilt)
            antenna.drawAntennaPattern = False
            self.antennalist.append(antenna)
        return self.antennalist

    def getAntenna(self):
        import rise.Antenna
        # Downtilt 12 degrees (in rise up is 0 degree horizontal is 90 degree)
        downtilt = (12.0 + 90.0)/360.0 * 2 * 3.14159265
        for azimuth in self.azimuth:
            antenna = rise.Antenna.ITU("17.0 dB", [0.0, 0.0, 10.0], azimuth, downtilt)
            antenna.drawAntennaPattern = False
            self.antennalist.append(antenna)
        return self.antennalist



class UrbanMacroAntennaCreator(scenarios.interfaces.IAntennaCreator):

    def __init__(self, sectorization):
        self.sectorization = sectorization
        self.antennalist = []
        if self.sectorization == True:
            self.azimuth = [30.0* 2.0 * 3.14 / 360.0, 150.0* 2.0 * 3.14 / 360.0, 270.0* 2.0 * 3.14 / 360.0]
        else:
            self.azimuth = [0.0]

    def create(self):
        import rise.Antenna
        # Downtilt 12 degrees (in rise up is 0 degree horizontal is 90 degree)
        downtilt = (12.0 + 90.0)/360.0 * 2 * 3.14159265

        for azimuth in self.azimuth:
            antenna = rise.Antenna.ITU("17.0 dB", [0.0, 0.0, 25.0], azimuth, downtilt)
            antenna.drawAntennaPattern = False
            self.antennalist.append(antenna)
        return self.antennalist

    def getAntenna(self):
        import rise.Antenna
        # Downtilt 12 degrees (in rise up is 0 degree horizontal is 90 degree)
        downtilt = (12.0 + 90.0)/360.0 * 2 * 3.14159265

        for azimuth in self.azimuth:

            antenna = rise.Antenna.ITU("17.0 dB", [0.0, 0.0, 25.0], azimuth, downtilt)
            antenna.drawAntennaPattern = False
            self.antennalist.append(antenna)
        return self.antennalist



class RuralMacroAntennaCreator(scenarios.interfaces.IAntennaCreator):

    def __init__(self, sectorization):
        self.sectorization = sectorization
        self.antennalist = []
        if self.sectorization == True:
            self.azimuth = [30.0* 2.0 * 3.14 / 360.0, 150.0* 2.0 * 3.14 / 360.0, 270.0* 2.0 * 3.14 / 360.0]
        else:
            self.azimuth = [0.0]

    def create(self):
        import rise.Antenna
        # Downtilt 12 degrees (in rise up is 0 degree horizontal is 90 degree)
        downtilt = (6.0 + 90.0)/360.0 * 2 * 3.14159265

        for azimuth in self.azimuth:
            antenna = rise.Antenna.ITU("17.0 dB", [0.0, 0.0, 35.0], azimuth, downtilt)
            antenna.drawAntennaPattern = False
            self.antennalist.append(antenna)
        return self.antennalist

    def getAntenna(self):
        # Downtilt 12 degrees (in rise up is 0 degree horizontal is 90 degree)
        downtilt = (6.0 + 90.0)/360.0 * 2 * 3.14159265

        for azimuth in self.azimuth:
            antenna = rise.Antenna.ITU("17.0 dB", [0.0, 0.0, 35.0], azimuth, downtilt)
            antenna.drawAntennaPattern = False
            self.antennalist.append(antenna)
        return self.antennalist


SuburbanMacroAntennaCreator = RuralMacroAntennaCreator
