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
import constanze.node

import tcp.TCP

import ltea.nodes.IPv4Component
import ip.Address
import ip.AddressResolver

import imtaphy.Logger
import imtaphy.Station

import scenarios.interfaces

import ltea.dll.component

import openwns.node

class UE(openwns.node.Node):
    
    def __init__(self, pos, speed, directionOfTravel, antenna, scheduler, receiver, windowSize, settlingTime, config, processingDelay = 0.002999, fullBuffer = False):
        openwns.node.Node.__init__(self, "UE")

        self.logger = openwns.logger.Logger("LTEA", "UE%d" % self.nodeID, True)

        self.name += str(self.nodeID)

        self.setProperty("Type", "UE")

        self.dll = ltea.dll.component.ueLayer2(node = self, name = "UE", scheduler = scheduler, 
            windowSize = windowSize, settlingTime = settlingTime, processingDelay = processingDelay, parentLogger = self.logger, fullBuffer = fullBuffer)

        stationPhyLogger = imtaphy.Logger.Logger(self.name + ".stationPhy")
        self.stationPhy = imtaphy.Station.StationPhy(self, self.name + ".PHY",
                                                     pos,
                                                     stationPhyLogger,
                                                     phyDataTransmissionName = "tx",
                                                     phyDataReceptionName = "rx",
                                                     stationType = "MS", #TODO: is this necessary?
                                                     speed = speed,
                                                     directionOfTravel = directionOfTravel,
                                                     antenna = antenna,
                                                     receiver = receiver)

        # TODO: make these loggers nicer
        antennaLogger = imtaphy.Logger.Logger(self.name + ".antenna")
        self.stationPhy.antenna.logger = antennaLogger

        if not fullBuffer:
            self._setupIP(windowSize, settlingTime)
            self._setupTCP(False) #config.useTCP)
            self._setupLoad()

    def _setupIP(self, probeWindow, settlingTime):
        assert self.nodeID < 65000, "Only 65000 Terminals supported currently :-) !"
        domainName = "UT" + str(self.nodeID) + ".lte.wns.org"
        ipAddress = ip.Address.ipv4Address("192.168.0.1") + self.nodeID
        self.nl = ltea.nodes.IPv4Component.IPv4Component(self, domainName + ".ip", domainName, probeWindow, settlingTime, useDllFlowIDRule = False)
        
        self.nl.logger.enabled = False
        
        self.nl.addDLL(_name = "lte",
                       # Where to get IP Adresses
                       _addressResolver = ip.AddressResolver.VirtualDHCPResolver("LTEARAN"),
                       # Name of ARP zone
                       _arpZone = "LTEARAN",
                       # We cannot deliver locally to other UTs without going to the gateway
                       _pointToPoint = True,
                       _dllDataTransmission = self.dll.dataTransmission,
                       _dllNotification = self.dll.notification)

        self.nl.addRoute("0.0.0.0", "0.0.0.0", "192.168.254.254", "lte")

    def _setupTCP(self, useTCP):
        if useTCP == True:
            self.tl = tcp.TCP.TCPComponent(self, "tcp", self.nl.dataTransmission, self.nl.notification)
        else:
            self.tl = tcp.TCP.UDPComponent(self, "udp", self.nl.dataTransmission, self.nl.notification)

        self.tl.addFlowHandling(
            _dllNotification = self.dll.notification,
            _flowEstablishmentAndRelease = self.dll.flowEstablishmentAndRelease)

    def _setupLoad(self):
        domainName = "UE" + str(self.nodeID) + ".ltea.wns.org"
        self.load = constanze.node.ConstanzeComponent(self, domainName + ".constanze", self.logger)

    def setPosition(self, position):
        self.mobility.mobility.setCoords(position)      

    def getPosition(self):
        return self.mobility.mobility.getCoords()

    def addTraffic(self, binding, load):
        self.load.addTraffic(binding, load)
