import openwns.FUN
import openwns

import ltea.dll.rlc
import ltea.dll.harq
import ltea.dll.schedulers.resourceManager

class SchedulerBase(openwns.FUN.FunctionalUnit):
    linkAdaptation = None
    harq = None
    txPowerdBmPerPRB = None
    segmentingQueue = None
    logger = None
    loggerName = None
    provideRel10DMRS = None
    resourceManager = None
    syncHARQ = None
    
    def __init__(self, linkAdaptation, 
                 txPowerdBmPerPRB, 
                 queueSize = None, 
                 harq = None, 
                 syncHARQ = True,
                 parentLogger = None, 
                 resourceManager = None):
        openwns.FUN.FunctionalUnit.__init__(self, functionalUnitName = "Scheduler")
        if queueSize == None:
            queueSize = 250000 # should be always big enough
        
        if harq == None:
            self.harq = ltea.dll.harq.HARQ(parentLogger = parentLogger)
        else:
            self.harq = harq
            
        if resourceManager is None:
            self.resourceManager = ltea.dll.schedulers.resourceManager.ResourceManagerBase()
        else:
            self.resourceManager = resourceManager    

        # these are the names for the RLC FU's (ltea.dll.rlc.UnacknowledgedMode) FU and command names, see component.py
        self.queue = ltea.dll.rlc.SegmentingQueue("rlcUnacknowledgedMode", "rlcUnacknowledgedMode", queueSize)
        self.txPowerdBmPerPRB = txPowerdBmPerPRB
        self.linkAdaptation = linkAdaptation
        self.syncHARQ = syncHARQ

    def setParentLogger(self, parentLogger):
        self.harq.setParentLogger(parentLogger)
        self.resourceManager.setParentLogger(parentLogger)
        self.linkAdaptation.setParentLogger(parentLogger)
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = self.loggerName + "DownlinkScheduler",
                                            enabled = True,
                                            parent = parentLogger)
    

class RoundRobin(SchedulerBase, openwns.StaticFactoryClass):
    prbsPerUser = None
    
    def __init__(self, linkAdaptation, 
                 txPowerdBmPerPRB, 
                 queueSize = None,
                 prbsPerUser = 0, # means all PRBs to one user per TTI
                 harq = None, 
                 syncHARQ = False,
                 parentLogger = None, 
                 resourceManager = None):
        SchedulerBase.__init__(self, linkAdaptation, txPowerdBmPerPRB, queueSize, harq, syncHARQ, parentLogger, resourceManager)
        openwns.StaticFactoryClass.__init__(self, 'ltea.mac.scheduler.downlink.RoundRobin')
        
        self.provideRel10DMRS = False
        self.loggerName = "RoundRobin"
        self.prbsPerUser = prbsPerUser

class ProportionalFair(SchedulerBase, openwns.StaticFactoryClass):
    throughputSmoothing = None
    def __init__(self, linkAdaptation, 
                 txPowerdBmPerPRB,
                 historyExponent = 1.0,
                 throughputSmoothing = 0.05, 
                 queueSize = None, 
                 harq = None, 
                 syncHARQ = True,
                 parentLogger = None,
                 resourceManager = None):
        SchedulerBase.__init__(self, linkAdaptation, txPowerdBmPerPRB, queueSize, harq, syncHARQ, parentLogger, resourceManager)
        openwns.StaticFactoryClass.__init__(self, 'ltea.mac.scheduler.downlink.ProportionalFair')
        
        self.throughputSmoothing = throughputSmoothing
        self.provideRel10DMRS = False
        self.loggerName = "ProportionalFair"
        self.historyExponent = historyExponent
        
        
        
class PU2RCScheduler(SchedulerBase, openwns.StaticFactoryClass):
    throughputSmoothing = None
    def __init__(self, linkAdaptation, 
                 txPowerdBmPerPRB, 
                 historyExponent = 1.0,
                 fillGrid = True,
                 throughputSmoothing = 0.05,
                 estimateOther = "no",
                 queueSize = None, 
                 harq = None, 
                 syncHARQ = True,
                 parentLogger = None,
                 resourceManager = None):
        SchedulerBase.__init__(self, linkAdaptation, txPowerdBmPerPRB, queueSize, harq, syncHARQ, parentLogger, resourceManager)
        openwns.StaticFactoryClass.__init__(self, 'ltea.mac.scheduler.downlink.PU2RCScheduler')
        
        self.throughputSmoothing = throughputSmoothing
        self.provideRel10DMRS = False
        self.loggerName = "PU2RCScheduler"       
        self.estimateOther = estimateOther
        self.historyExponent = historyExponent
        self.fillGrid = fillGrid 
       
class ZFScheduler(SchedulerBase, openwns.StaticFactoryClass):
    throughputSmoothing = None
    def __init__(self, linkAdaptation, 
                 txPowerdBmPerPRB, 
                 throughputSmoothing = 0.05, 
                 queueSize = None, 
                 harq = None, 
                 syncHARQ = True,
                 parentLogger = None,
                 resourceManager = None):
        SchedulerBase.__init__(self, linkAdaptation, txPowerdBmPerPRB, queueSize, harq, syncHARQ, parentLogger, resourceManager)
        openwns.StaticFactoryClass.__init__(self, 'ltea.mac.scheduler.downlink.ZFScheduler')
        
        self.throughputSmoothing = throughputSmoothing
        self.loggerName = "ZFScheduler"            
        self.provideRel10DMRS = True # we compute arbitrary ZF precoders so we have to provide precoded demodulation reference symbols
        
        
