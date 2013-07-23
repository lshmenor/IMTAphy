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

import ltea.nodes.EPCgw
import ip.BackboneHelpers
import constanze.node
import constanze.traffic
import openwns.qos

def createEPCandTraffic(simulator, offeredDLtrafficBps, offeredULtrafficBps, packetSize, probeWindow, settlingTime, useTCP = False, enableLogger = True):
    # EPCgw, see lte/creators/BS.py
    epcgw = ltea.nodes.EPCgw(probeWindow, settlingTime, useTCP = useTCP)
    
    epcgw.logger.enabled = enableLogger
    
    simulator.simulationModel.nodes.append(epcgw)

    if useTCP:
        # 1014 and 777 are port numbers
        listenerBindingInEPCgw = constanze.node.TCPListenerBinding(1024, parentLogger = epcgw.logger)
    else:
        listenerBindingInEPCgw = constanze.node.UDPListenerBinding(777,  parentLogger = epcgw.logger)

    listenerInEPCgw = constanze.node.Listener(epcgw.nl.domainName, parentLogger = epcgw.logger)
    epcgw.load.addListener(listenerBindingInEPCgw, listenerInEPCgw)

    # the EPCgw has to be able to find the MAC addresses of the mobiles, for that it needs to
    # get the remote DataTransmissionService
    # TODO: make this nicer
    remoteServiceName = simulator.simulationModel.getNodesByProperty("Type", "UE")[0].dll.dataTransmission
    epcgw.tunnel.setUEdllDataTransmissionServiceName(remoteServiceName)

    for eNB in simulator.simulationModel.getNodesByProperty("Type", "eNB"):
        epcgw.tunnel.addBS(eNB)


    for ue in simulator.simulationModel.getNodesByProperty("Type", "UE"):
        # UL traffic    
        ulBinding = constanze.node.UDPClientBinding(ue.nl.domainName,
                                                    epcgw.nl.domainName,
                                                    777,
                                                    parentLogger=ue.logger,
                                                    qosClass=openwns.qos.undefinedQosClass)

        ulTraffic = constanze.traffic.CBR(offset=0, # startTrafficOffset,
                                          throughput = offeredULtrafficBps,
                                          packetSize = packetSize * 8)
        ue.addTraffic(ulBinding, ulTraffic) # calls addTraffic in lte/nodes/UE.py

        if useTCP:
            # 1014 and 777 are port numbers
            listenerBindingUE = constanze.node.TCPListenerBinding(1024, parentLogger = ue.logger)
        else:
            listenerBindingUE = constanze.node.UDPListenerBinding(777,  parentLogger = ue.logger)

        listenerInUE = constanze.node.Listener(ue.nl.domainName, parentLogger = ue.logger)
        ue.load.addListener(listenerBindingUE, listenerInUE)

        # DL traffic
        dlBinding = constanze.node.UDPClientBinding(epcgw.nl.domainName,
                                                    ue.nl.domainName,
                                                    777,
                                                    parentLogger=epcgw.logger,
                                                    qosClass=openwns.qos.undefinedQosClass)
        
        dlTraffic = constanze.traffic.CBR(offset=0, # startTrafficOffset,
                                          throughput = offeredDLtrafficBps,
                                          packetSize = packetSize * 8)
        epcgw.addTraffic(dlBinding, dlTraffic) 

    # DHCP, ARP, DNS for IP
    ip.BackboneHelpers.createIPInfrastructure(simulator, "LTEARAN")


