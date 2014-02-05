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
import openwns.logger
import openwns.scheduler.APCStrategy
import openwns.scheduler.DSAStrategy
import copy

class PowerCapabilities(object):
    maxPerSubband = None
    nominalPerSubband = None
    maxOverall = None

    def __init__(self,
                 maxPerSubband = "0.0 mW",
                 nominalPerSubband = "0.0 mW",
                 maxOverall = "0.0 mW"):
        self.maxPerSubband = maxPerSubband
        self.nominalPerSubband = nominalPerSubband
        self.maxOverall = maxOverall

class Strategy(object):
    nameInStrategyFactory = None
    symbolDuration = None
    txMode = None # True for outgoing schedulers (Scheduler-TX) in BS and UT; False for uplink master scheduler (Scheduler-RX) in BS, because this does not really transmit packets
    logger = None
    powerControlSlave = None # True for slave scheduler in UT
    excludeTooLowSINR = None
    dsastrategy = None
    dsafbstrategy = None
    apcstrategy = None

    def __init__(self,
                 txMode = True,
                 symbolDuration = None,
                 parentLogger = None,
                 powerControlSlave = False,
                 excludeTooLowSINR = False,
                 apcstrategy   = openwns.scheduler.APCStrategy.DoNotUseAPC(),
                 dsastrategy   = openwns.scheduler.DSAStrategy.DoNotUseDSA(),
                 dsafbstrategy = openwns.scheduler.DSAStrategy.DoNotUseDSA(),
                 **kw):
        self.symbolDuration = symbolDuration
        self.txMode = txMode
        self.logger = openwns.logger.Logger("WNS", "Strategy", True, parentLogger)
        self.powerControlSlave = powerControlSlave
        self.excludeTooLowSINR = excludeTooLowSINR
        self.setAPCStrategy(apcstrategy)
        self.setDSAStrategy(dsastrategy)
        self.setDSAFallbackStrategy(dsafbstrategy)
        assert dsafbstrategy.requiresCQI == False, "dsafbstrategy must not require CQI"
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        myParentLogger = copy.deepcopy(parentLogger) # original object shares logger instance
        self.logger = openwns.logger.Logger("WNS", "Strategy", True, myParentLogger)
        #self.dsastrategy.setParentLogger(self.logger)
        ### above: BS1.L2.tdd100.RS-TX.Strategy.DSAStrategy.LinearFFirst
        ### below: BS1.L2.tdd100.RS-TX.DSAStrategy.LinearFFirst
        self.dsastrategy.setParentLogger(myParentLogger)
        self.dsafbstrategy.setParentLogger(openwns.logger.Logger("WNS", "FB", True, myParentLogger))
        self.apcstrategy.setParentLogger(myParentLogger)
    def setDSAStrategy(self, dsastrategy):
        self.dsastrategy = copy.deepcopy(dsastrategy)
        self.dsastrategy.setParentLogger(self.logger)
    def setDSAFallbackStrategy(self, dsafbstrategy):
        self.dsafbstrategy = copy.deepcopy(dsafbstrategy)
        self.dsafbstrategy.setParentLogger(self.logger)
    def setAPCStrategy(self, apcstrategy):
        self.apcstrategy = copy.deepcopy(apcstrategy)
        self.apcstrategy.setParentLogger(self.logger)


# QoS enabled (new style)
class StaticPriority(Strategy):
    subStrategies = None
    numberOfPriorities = None
    def __init__(self, txMode = True, subStrategies = [], parentLogger = None, powerControlSlave = False, **kw):
        super(StaticPriority,self).__init__(txMode = txMode, powerControlSlave=powerControlSlave, **kw)
        attrsetter(self, kw)
        self.nameInStrategyFactory = "StaticPriority"
        self.subStrategies = []
        self.logger = openwns.logger.Logger("WNS", "SP", True, parentLogger)
        # priority here is only used for the logger name
        priority = 0
        for subStrategy in subStrategies:
            mySubStrategy = copy.deepcopy(subStrategy) # original object shares logger instance
            logger = openwns.logger.Logger("WNS", "SP[%d]"%priority, True, self.logger)
            mySubStrategy.setParentLogger(logger)
            self.subStrategies.append(mySubStrategy)
            priority = priority+1

class SubStrategy:
    logger = None
    useHARQ = False
    __plugin__ = "NONE"
    def __init__(self, **kw):
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "SubStrategy", True, parentLogger)

class RoundRobin(SubStrategy):
    __plugin__ = "RoundRobin"
    blockSize = 1 # number of pdus taken out out queue for one cid per round
    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "RoundRobin", True, parentLogger)
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "RoundRobin", True, parentLogger)

class PersistentVoIP(SubStrategy):
    __plugin__ = "PersistentVoIP"

    class ResourceGrid(object) :
        
        class LinkAdaptation(object):
            def __init__(self, plugin, reduceMCS = True):
                self.__plugin__ = plugin
                self.reduceMCS = reduceMCS
                self.alpha = 0
                self.pNull = "4 dBm"

        class TBChoser(object):
            def __init__(self, plugin, returnRandom = False):
                self.__plugin__ = plugin
                self.returnRandom = returnRandom

        class PreviousTBC(TBChoser):
            def __init__(self, fallbackChoser, returnRandom = False):
                PersistentVoIP.ResourceGrid.TBChoser.__init__(self, "Previous", returnRandom)
                self.fallbackChoser = PersistentVoIP.ResourceGrid.TBChoser(fallbackChoser)

        def __init__(self, tbChoser, linkAdaptation, reduceMCS = True, returnRandom = False, parentLogger = None):
            self.tbChoser = PersistentVoIP.ResourceGrid.TBChoser(tbChoser, returnRandom)
            self.linkAdaptation = PersistentVoIP.ResourceGrid.LinkAdaptation(
                linkAdaptation, reduceMCS)

            self.logger = openwns.logger.Logger("WNS", "ResourceGrid", True, parentLogger)

    def __init__(self, parentLogger = None, voicePDUSize = 368, **kw):
        self.numberOfFrames = 20
        self.voicePDUSize = voicePDUSize
        self.logger = openwns.logger.Logger("WNS", "PersistentVoIP", True, parentLogger)
        self.resourceGrid = PersistentVoIP.ResourceGrid("First", "AtStart", self.logger)
        self.harq = None
        attrsetter(self, kw)

    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "PersistentVoIP", True, parentLogger)
        


# As long as DSA has resources for any CID of a USER, try scheduling this USER.
# If no more resources are left, try the next USER.
class DSADrivenRR(SubStrategy):
    __plugin__ = "DSADrivenRR"
    
    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "DSADrivenRR", True, parentLogger)
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "DSADrivenRR", True, parentLogger)

class Disabled(SubStrategy):
    __plugin__ = "DisabledStrategy"
    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "DisabledStrategy", True, parentLogger)
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "DisabledStrategy", True, parentLogger)

class ExhaustiveRoundRobin(SubStrategy):
    __plugin__ = "ExhaustiveRoundRobin"
    blockSize = 1000000 # don't ask.
    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "ExhaustiveRR", True, parentLogger)
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "ExhaustiveRR", True, parentLogger)

# begin example "wimac.tutorial.experiment2.staticFactory.substrategy.ProportionalFair.openwns.Scheduler.py"
class ProportionalFair(SubStrategy):
    __plugin__ = "ProportionalFair"
# end example
    blockSize = 1000000
    # 0.0 = no history; 0.9 = factor of older pastDataRates to keep
    historyWeight = 0.9
    # 0.0=MaxThroughput; 1.0=ProportionalFair
    scalingBetweenMaxTPandPFair = 1.0
    # indicates whether goal is rate (True) or resource (False) fairness
    rateFairness = True
    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "ProportionalFair", True, parentLogger)
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "ProportionalFair", True, parentLogger)

class HARQRetransmission(SubStrategy):
    __plugin__ = "HARQRetransmission"
    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "HARQRetransmission", True, parentLogger)
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "HARQRetransmission", True, parentLogger)

class HARQUplinkRetransmission(SubStrategy):
    __plugin__ = "HARQUplinkRetransmission"
    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "HARQUplinkRetransmission", True, parentLogger)
        attrsetter(self, kw)

    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "HARQUplinkRetransmission", True, parentLogger)

class HARQUplinkSlaveRetransmission(SubStrategy):
    __plugin__ = "HARQUplinkSlaveRetransmission"
    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "HARQUplinkSlaveRetransmission", True, parentLogger)
        attrsetter(self, kw)

    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "HARQUplinkSlaveRetransmission", True, parentLogger)

# TODO:
class EqualTimeRoundRobin(SubStrategy):
    __plugin__ = "EqualTimeRoundRobin"
    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "EqualTimeRR", True, parentLogger)
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "EqualTimeRR", True, parentLogger)

# TODO:
class EqualRateRoundRobin(SubStrategy):
    __plugin__ = "EqualRateRoundRobin"
    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "EqualRateRR", True, parentLogger)
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "EqualRateRR", True, parentLogger)

# TODO: First Come First Serve (FCFS)
class FCFS(SubStrategy):
    __plugin__ = "FCFS"
    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "FCFS", True, parentLogger)
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "FCFS", True, parentLogger)

# TODO: Earliest Deadline First (EDF)
class EDF(SubStrategy):
    __plugin__ = "EDF"
    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "EDF", True, parentLogger)
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        self.logger = openwns.logger.Logger("WNS", "EDF", True, parentLogger)


######################################################

class NoGrouper:
    nameInGrouperFactory = "NoGrouper"
    beamforming = False


class Grouper(object):
    nameInGrouperFactory = None
    friendliness_dBm = None
    MonteCarloSim = None
    beamforming = None
    uplink = None
    logger = None

    def __init__(self, parentLogger = None, **kw):
        super(Grouper,self).__init__()
        self.friendliness_dBm = "-95 dBm"
        self.MonteCarloSim = False
        self.beamforming = True
        self.uplink = False
        self.logger = openwns.logger.Logger("WNS", "SpatialGrouper", True, parentLogger);
        attrsetter(self, kw)

class RelayMetaGrouper(Grouper):
    internalStrategy = None
    def __init__(self, internalStrategy, parentLogger = None):
        super(RelayMetaGrouper, self).__init__(parentLogger = parentLogger)
        self.nameInGrouperFactory = 'RelayMetaGrouper'
        self.internalStrategy = internalStrategy

class Treebased(Grouper):
    def __init__(self, **kw):
        super(Treebased,self).__init__(**kw)

class SINRHeuristic(Treebased):
    def __init__(self, **kw):
        super(SINRHeuristic,self).__init__(**kw)
        self.nameInGrouperFactory = "SINRHeuristic"
        
class GreedyGrouper(Grouper):
    def __init__(self, **kw):
        super(GreedyGrouper,self).__init__(**kw)
        self.nameInGrouperFactory = "GreedyGrouper"
        
class DoAGrouper(Treebased):
    minAngleDegree = None
    weight = None
    strategy = None

    def __init__(self, minAngleDegree, weight, **kw):
        super(DoAGrouper,self).__init__(**kw)
        self.minAngleDegree = minAngleDegree
        self.weight = weight
        # Strategy for the cost function
        self.strategy = 0 # 0 means average cost for the group
        #self.strategy = 1 # 1 means cost of group member with maximum cost

class DoAHeuristicLinearCost(DoAGrouper):
    def __init__(self, minAngleDegree, weight, **kw):
        super(DoAHeuristicLinearCost,self).__init__(minAngleDegree, weight, **kw)
        self.nameInGrouperFactory = "DoAHeuristicLinearCost"

class DoAHeuristicPreferredAngle(DoAGrouper):
    def __init__(self, minAngleDegree, weight, **kw):
        super(DoAHeuristicPreferredAngle,self).__init__(minAngleDegree, weight, **kw)
        self.nameInGrouperFactory = "DoAHeuristicPreferredAngle"

class RegistryProxy(object):
    nameInRegistryProxyFactory = None



######################################################
### SimpleQueue (stores segmented PDUs of size<=segmentSize)
class SimpleQueue(object):
    nameInQueueFactory = None
    logger = None
    sizeProbeName = None
    TxRx = None
    localIDs = None

    def setLocalIDs(self, localIDs):
        self.localIDs = localIDs

    def addLocalIDs(self, localIDs):
        self.localIDs.update(localIDs)

    def __init__(self, parentLogger = None, **kw):
        self.localIDs = {}
        self.nameInQueueFactory = "SimpleQueue"
        self.logger = openwns.logger.Logger("WNS", "SimpleQueue", True, parentLogger);
        self.sizeProbeName = 'SimpleQueueSize'
        attrsetter(self, kw)

######################################################
### QueueProxy (passes all calls to other queues. Used to
### access uplink queues
class SegQueueConfig(object):
    def __init__(self, commandName):
        self.fixedHeaderSize = 0
        self.extensionHeaderSize = 0
        self.minimumSegmentSize = 1 # Bits
        self.byteAlignHeader = False
        self.usePadding = False
        self.segmentHeaderCommandName = commandName

class QueueProxy(object):  	
    nameInQueueFactory = "wns.scheduler.queue.QueueProxy"
    queueManagerServiceName = None
    supportsDynamicSegmentation = None
    segmentingQueueConfig = None
    
    def __init__(self, queueManagerServiceName, 
                supportsDynamicSegmentation = False,
                parentLogger = None):
        self.queueManagerServiceName = queueManagerServiceName
        self.supportsDynamicSegmentation = supportsDynamicSegmentation
        self.logger = openwns.logger.Logger("WNS", "QueueProxy", True, parentLogger);
        
    def setSegmentingQueueConfig(self, config):
        self.segmentingQueueConfig = config

### SegmentingQueue (stores unsegmented original PDUs and does segmentation on-the-fly)
class SegmentingQueue(object):
    """
    Dynamic segmentation encapsulate in a scheduler queue
    Can be used as implementation for 3GPP LTE R8 RLC

    fixedHeaderSize : Every PDU that possibly contains multiple SDUs has a header of at least this size
    extenstionHeaderSize: If segments are concatenated this size is added additionally for each extra SDU
    byteAlignHeader: If set to True then the total header size will be extended such that totalHeaderSize mod 8 = 0
    """

    nameInQueueFactory = None
    logger = None
    sizeProbeName = None
    overheadProbeName = None
    delayProbeName = None
    TxRx = None
    localIDs = None
    minimumSegmentSize = None # used to ask for resources of at least this size
    usePadding = False
    isDropping = False

    def setLocalIDs(self, localIDs):
        self.localIDs = localIDs

    def addLocalIDs(self, localIDs):
        self.localIDs.update(localIDs)

    def __init__(self, segmentHeaderFUName, segmentHeaderCommandName, parentLogger = None, **kw):
        super(SegmentingQueue,self).__init__()
        self.localIDs = {}
        self.nameInQueueFactory = "SegmentingQueue"
        self.logger = openwns.logger.Logger("WNS", "SegmentingQueue", True, parentLogger);
        self.sizeProbeName = 'SegmentingQueueSize'
        self.overheadProbeName = 'SegmentingQueueOverhead'
        self.minimumSegmentSize = 32 # Bits
        self.fixedHeaderSize = 8
        self.extensionHeaderSize = 12
        self.byteAlignHeader = False
        #self.sizeProbeName = 'schedulerQueueSize'
        self.segmentHeaderFUName = segmentHeaderFUName
        self.segmentHeaderCommandName = segmentHeaderCommandName
        self.isDropping = False
        attrsetter(self, kw) # new [rs]

