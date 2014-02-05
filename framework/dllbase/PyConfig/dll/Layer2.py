###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2007
# Chair of Communication Networks (ComNets)
# Kopernikusstr. 5, D-52074 Aachen, Germany
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

from openwns.pyconfig import attrsetter

import openwns.node
import openwns.logger
class LinkHandler:

    type = "wns.ldk.SimpleLinkHandler"

    isAcceptingLogger = None
    sendDataLogger = None
    wakeupLogger = None
    onDataLogger = None

    traceCompoundJourney = False

    def __init__(self, parentLogger = None):
        self.isAcceptingLogger = openwns.logger.Logger("WNS", "LinkHandler", True, parentLogger)
        self.sendDataLogger = openwns.logger.Logger("WNS", "LinkHandler", True, parentLogger)
        self.wakeupLogger = openwns.logger.Logger("WNS", "LinkHandler", True, parentLogger)
        self.onDataLogger = openwns.logger.Logger("WNS", "LinkHandler", True, parentLogger)

class Layer2( openwns.node.Component ):
    __associationID = -1

    # station ID, user must make sure this is set uniquely. Used for Probes Access Control
    stationID = None

    # a list of associations
    associations = None

    # String, can be "BS", "FRS" or "UT"
    stationType = None

    # an integer to denote how far away from the BS we are
    ring = None

    # Configuration of the FUN residing in the Layer2
    fun = None

    # Services to communicate with the PHY
    phyDataTransmission = None
    phyNotification = None
    phyMeasurements = None

    # Services offered to the NL
    dataTransmission = None
    notification = None

    # Services offered to the TL
    flowHandler = None
    flowEstablishmentAndRelease = None

    # MAC Adress
    address = None

    # Control Services
    controlServices = None

    # Management Services
    managementServices = None

    # Logger
    logger = None

    linkHandler = None

    # Name of the UpperConvergence FU
    upperConvergenceName = None

    #
    # layer2 Constructor
    #
    def __init__(self, node, stationName, parentLogger = None, **kw):
        self.nameInComponentFactory = None
        super(Layer2,self).__init__(node, stationName)
        self.dataTransmission = stationName + ".dllDataTransmission"
        self.notification = stationName + ".dllNotification"

        self.flowHandler = stationName + ".dllFlowHandler"
        self.flowEstablishmentAndRelease = stationName + ".dllFlowEstablishmentAndRelease"

        self.controlServices = []
        self.managementServices = []
        self.logger = openwns.logger.Logger("DLL", "L2", True, parentLogger)
        self.linkHandler = LinkHandler(self.logger)
        self.upperConvergenceName = 'upperConvergence'
        attrsetter(self, kw)

    def setPhyDataTransmission(self, serviceName):
        self.phyDataTransmission = serviceName

    def setPhyNotification(self, serviceName):
        self.phyNotification = serviceName

    def getAssociationID(self):
        self.__associationID += 1
        return self.__associationID

    def setStationID(self, number):
        if self.stationID is not None: raise AssertionError, "Do you really want to re-set the stationID?"
        self.stationID = number
        self.address = self.stationID

    def setRing(self, _ring):
        if self.ring is not None: raise AssertionError, "Do you really want to re-set the ring?"
        self.ring = _ring

    def setStationType(self, _stationType):
        if self.stationType is not None: raise AssertionError, "Do you really want to re-set the stationType?"
        self.stationType = _stationType
