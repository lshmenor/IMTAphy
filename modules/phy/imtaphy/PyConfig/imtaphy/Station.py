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

import openwns.node
import imtaphy.Antenna
import imtaphy.Receiver

class StationPhy(openwns.node.Component):
    nameInComponentFactory = "imtaphy.StationPhy"
    phyDataTransmissionName = None
    phyDataReceptionName = None
    logger = None
    stationType = None
    antenna = None
    speed = None
    directionOfTravel = None
    position = None
    receiver = None

    def __init__(self, node, name, position, logger, phyDataTransmissionName, phyDataReceptionName, 
                 stationType = "MS", speed=0.0, directionOfTravel = 0.0, antenna = None, receiver = None):
        super(StationPhy, self).__init__(node, name)
        self.phyDataTransmissionName =  phyDataTransmissionName
        self.phyDataReceptionName = phyDataReceptionName
        self.logger = logger
        self.stationType = stationType
        self.speed = speed
        self.directionOfTravel = directionOfTravel
        self.position = position
        if receiver is None:
            self.receiver = imtaphy.Receiver.LinearReceiver(filter = imtaphy.Receiver.MMSEFilter(), logger = logger)
        else:
            self.receiver = receiver
        if antenna is None:
            self.antenna = imtaphy.Antenna.Omnidirectional(logger = logger)
        else:
            self.antenna = antenna

class BaseStation(StationPhy):
    def __init__(self, node, name, position, logger, phyDataTransmissionName, phyDataReceptionName,  
                 antenna = None, receiver = None):
        super(BaseStation, self).__init__(node, name, position, logger, phyDataTransmissionName,
                                          phyDataReceptionName, stationType="BS", speed=0.0, directionOfTravel = 0.0, antenna = antenna, receiver = receiver)

class MobileStation(StationPhy):
    def __init__(self, node, name, position, logger, phyDataTransmissionName, phyDataReceptionName, 
                  speed=0.0, directionOfTravel = 0.0, antenna = None, receiver = None):
        super(MobileStation, self).__init__(node, name, position, logger, phyDataTransmissionName,
                                            phyDataReceptionName, stationType="MS", speed = speed, directionOfTravel = directionOfTravel, antenna = antenna, receiver = receiver) 

class ScannerStation(StationPhy):
    def __init__(self, node, name, position, logger, phyDataTransmissionName, phyDataReceptionName,  
                 speed=0.0, directionOfTravel = 0.0, antenna = None, receiver = None):
        super(ScannerStation, self).__init__(node, name, position, logger, phyDataTransmissionName,
                 phyDataReceptionName, stationType="MS", speed = speed, directionOfTravel = directionOfTravel, antenna = antenna , receiver = receiver)
