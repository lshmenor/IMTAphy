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

import ltea.dll.pdcp
import ltea.dll.l2s


import dll.Layer2
import dll.Services

import openwns.FUN
import openwns.Tools
import openwns.FlowSeparator
import openwns.Tools

import ltea.evaluation.probe
import ltea.dll.schedulers.uplink

# from openwns-lte:
def connectFUs(pairList):
    """
    pairList= [(A,B), (B,C)]
            
    results in a FUN like this
     A - B - C
    """
    for source, destination in pairList:
        source.connect(destination)

class eNBLayer2( dll.Layer2.Layer2 ):

    def __init__(self, node, name, dlScheduler, ulScheduler, windowSize, settlingTime, processingDelay, parentLogger=None, fullBuffer=False):
        dll.Layer2.Layer2.__init__(self, node, name, parentLogger)
        self.nameInComponentFactory = "ltea.Layer2"
        self.setStationID(node.nodeID)
        self.setStationType(node.getProperty("Type"))
        self.associations = []
        self.phyUsers = {}
        self.phyDataTransmissionName = "tx" 
        self.phyNotifyServiceName = "rx"

        # legacy: framework/dll/Layer2.cpp wants to read this:
        self.ring = -1


        # Build the FUN
        self.fun = openwns.FUN.FUN(self.logger)

        # overwrite name inherited from dll.Layer2.Layer2
        # the layer container that holds the FUN must be able to find the PDCP FU
        self.upperConvergenceName = "pdcp" 

        pdcp = ltea.dll.pdcp.eNB(self.logger)
        self.fun.add(pdcp)

        throughput = ltea.evaluation.probe.Window(name = 'throughput',
                                                  prefix = "ltea.total",
                                                  windowSize = windowSize,
                                                  settlingTime = settlingTime,
                                                  commandName = 'throughput')
        self.fun.add(throughput)

        packet = openwns.Probe.Packet(name = 'packet',
                                      prefix = 'ltea',
                                      commandName = 'packet')
        self.fun.add(packet)

        if not fullBuffer:
            # setup rlc FlowSeparator that helps the segmenting queue in the scheduler to provide RLC functionality
            rlcFlowSeparator = setupUnacklowdgedModeFlowSeparator(self.fun, "rlcUnacknowledgedMode", "rlcUnacknowledgedMode", "radioBearer", self.logger)
            self.fun.add(rlcFlowSeparator)

        dlScheduler.setParentLogger(self.logger)
        if fullBuffer: 
            dlScheduler.queue = ltea.dll.rlc.FullQueue(packet.commandName, throughput.commandName, pdcp.commandName)

        self.fun.add(dlScheduler)
        
        phyInterfaceRx = ltea.dll.l2s.PhyInterfaceRx(self.logger, dumpChannel=False, processingDelay=processingDelay)
        self.fun.add(phyInterfaceRx)
        
        # the UL scheduler gets added to the FUN but is not connected to any other FU in the dataplane
        ulScheduler.setParentLogger(self.logger)
        self.fun.add(ulScheduler)
        
        if not fullBuffer:
            connectFUs([
                    (pdcp, throughput),
                    (throughput, packet),
                    (packet, rlcFlowSeparator),
                    (rlcFlowSeparator, dlScheduler),
                    (dlScheduler, phyInterfaceRx),
                    ])
        else:
            connectFUs([
                    (throughput, packet),
                    (packet, dlScheduler),
                    (dlScheduler, phyInterfaceRx),
                    ])

    def setPhyDataTransmission(self, serviceName):
        """set the name of the PHY component for a certain mode"""
        self.phyDataTransmissionName = serviceName

    def setPhyNotification(self, serviceName):
        self.phyNotifyServiceName = serviceName

class ueLayer2( dll.Layer2.Layer2 ):

    def __init__(self, node, name, scheduler, windowSize, settlingTime, processingDelay, parentLogger=None, fullBuffer=False):
        dll.Layer2.Layer2.__init__(self, node, name, parentLogger)
        self.nameInComponentFactory = "ltea.Layer2"
        self.setStationID(node.nodeID)
        self.setStationType(node.getProperty("Type"))
        self.associations = []
        self.phyUsers = {}
        self.phyDataTransmissionName = "tx" 
        self.phyNotifyServiceName = "rx"

        # legacy: framework/dll/Layer2.cpp wants to read this:
        self.ring = -1


        self.fun = openwns.FUN.FUN(self.logger)

        # overwrite name inherited from dll.Layer2.Layer2
        # the layer container that holds the FUN must be able to find the PDCP FU
        self.upperConvergenceName = "pdcp" 

        pdcp = ltea.dll.pdcp.UE(self.logger)
        self.fun.add(pdcp)

        throughput = ltea.evaluation.probe.Window(name = 'throughput',
                                                  prefix = "ltea.total",
                                                  windowSize = windowSize,
                                                  settlingTime = settlingTime,
                                                  commandName = 'throughput')
        self.fun.add(throughput)

        packet = openwns.Probe.Packet(name = 'packet',
                                      prefix = 'ltea',
                                      commandName = 'packet')
        self.fun.add(packet)

        if not fullBuffer:
            # setup rlc FlowSeparator that helps the segmenting queue in the scheduler to provide RLC functionality
            rlcFlowSeparator = setupUnacklowdgedModeFlowSeparator(self.fun, "rlcUnacknowledgedMode", "rlcUnacknowledgedMode", "radioBearer", self.logger)
            self.fun.add(rlcFlowSeparator)

        if not scheduler is None:
            scheduler.setParentLogger(self.logger)
            if fullBuffer:
                scheduler.queue = ltea.dll.rlc.FullQueue(packet.commandName, throughput.commandName, pdcp.commandName)
            self.fun.add(scheduler)


        self.phyInterfaceRx = ltea.dll.l2s.PhyInterfaceRx(self.logger, processingDelay = processingDelay)
        self.fun.add(self.phyInterfaceRx)
        
        if not fullBuffer:
            if not scheduler is None:
                connectFUs([
                        (pdcp, throughput),
                        (throughput, packet), 
                        (packet, rlcFlowSeparator),
                        (rlcFlowSeparator, scheduler),
                        (scheduler, self.phyInterfaceRx),
                        ])
            else:
                connectFUs([
                        (pdcp, throughput),
                        (throughput, packet), 
                        (packet, rlcFlowSeparator),
                        (rlcFlowSeparator, self.phyInterfaceRx),
                        ])
        else:
            connectFUs([
                    (throughput, packet),
                    (packet, scheduler),
                    (scheduler, self.phyInterfaceRx),
                    ])

    def setPhyDataTransmission(self, serviceName):
        """set the name of the PHY component for a certain mode"""
        self.phyDataTransmissionName = serviceName

    def setPhyNotification(self, serviceName):
        self.phyNotifyServiceName = serviceName

    def enableChannelGainProbing(self):
        self.phyInterfaceRx.enableChannelGainProbing()
    
    def disableChannelGainProbing(self):
        self.phyInterfaceRx.disableChannelGainProbing()
        


def setupUnacklowdgedModeFlowSeparator(fun, fuName, commandName, separatorName, logger):
    lowerSubFUN = openwns.FUN.FUN(logger)
        
    # Unacknowledged Mode FU
    segmentSize = 999999 # what for? we don't segment in this FU at all, um.isSegmenting == False
    um = ltea.dll.rlc.UnacknowledgedMode(segmentSize = segmentSize - 1,
                                        headerSize = 1,
                                        commandName=commandName,
                                        parentLogger = logger)
    um.sduLengthAddition = 16
    um = openwns.FUN.Node(fuName, um, commandName)
    
    lowerSubFUN.add(um)

    lowerGroup = openwns.Group.Group(lowerSubFUN, um.functionalUnitName, um.functionalUnitName)
    
    config = openwns.FlowSeparator.Config(fuName + 'Prototype', lowerGroup)

    # if validFlowNeeded:
    #     strategy = openwns.FlowSeparator.CreateOnValidFlow(config, fipName = 'FlowManagerBS')
    # else:
    strategy = openwns.FlowSeparator.CreateOnFirstCompound(config)

    flowSeparator = openwns.FlowSeparator.FlowSeparator(
        ltea.dll.rlc.RadioBearerID(),
        strategy,
        'rlcFlowSeparator',
        logger,
        functionalUnitName = separatorName,
        commandName = separatorName)

    return flowSeparator
    
