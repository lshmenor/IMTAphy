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

import openwns.node
import dll.Layer2

import constanze.node
import tcp.TCP
import ltea.nodes.IPv4Component
import ip.AddressResolver

class Tunnel( dll.Layer2.Layer2 ):
    dataTransmission = None
    notification = None
    flowIDUnbindDelay = 60.0 # was 60.0
    dllDataTransmissions = None
    dllNotifications = None
    ueDllDataTransmissionServiceName = None

    def __init__(self, node, parentLogger = None):
        dll.Layer2.Layer2.__init__(self, node, "EPCgw", parentLogger = parentLogger)
        self.dataTransmission = "EPCgw.dllDataTransmission"
        self.notification = "EPCgw.dllNotification"
        self.nameInComponentFactory = "ltea.EPCgw"
        self.dllDataTransmissions = []
        self.dllNotifications = []
        self.logger.enabled=True


    def setUEdllDataTransmissionServiceName(self, serviceName):
        self.ueDllDataTransmissionServiceName = serviceName

    def addBS(self, bs):
        self.dllDataTransmissions.append(openwns.node.FQSN(bs, bs.dll.dataTransmission))
        self.dllNotifications.append(openwns.node.FQSN(bs, bs.dll.notification))

class EPCgw(openwns.node.Node):
    tunnel = None
    nl  = None
    load = None
    tl = None

    def __init__(self, probeWindow, settlingTime, useTCP=True):
        super(EPCgw, self).__init__("EPCgw")

        self.setProperty("Type", "EPCgw")

        # create Tunnel to reach the BSs
        self.tunnel = Tunnel(self, self.logger)
        address = 256*255 - 1
        self.tunnel.setStationID(address)

        # create Network Layer
        domainName = "EPCgw.ltea.wns.org"
        
        self.nl = ltea.nodes.IPv4Component.IPv4Component(self, domainName + ".ip",domainName, probeWindow, settlingTime, useDllFlowIDRule = True)
        self.nl.addDLL(_name = "tun",
                       # Where to get my IP Address
                       _addressResolver = ip.AddressResolver.FixedAddressResolver("192.168.254.254", "255.255.0.0"),
                       # ARP zone
                       _arpZone = "LTEARAN",
                       # We can deliver locally
                       _pointToPoint = False,
                       # DLL service names
                       _dllDataTransmission = self.tunnel.dataTransmission,
                       _dllNotification = self.tunnel.notification)

        if useTCP == True:
            self.tl = tcp.TCP.TCPComponent(self, "tcp", self.nl.dataTransmission, self.nl.notification)
        else:
            self.tl = tcp.TCP.UDPComponent(self, "udp", self.nl.dataTransmission, self.nl.notification)

        # create Loadgen
        self.load = constanze.node.ConstanzeComponent(self, domainName + ".constanze", self.logger)


    def addTraffic(self, binding, load):
        self.load.addTraffic(binding, load)
