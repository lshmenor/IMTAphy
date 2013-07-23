import openwns.FUN
import openwns

import ltea.dll.rlc
import ltea.dll.harq

class UE(openwns.FUN.FunctionalUnit, openwns.StaticFactoryClass):
    harq = None
    segmentingQueue = None
    logger = None
    loggerName = None
    totalTxPowerdBm = None

    def __init__(self, totalTxPowerdBm, queueSize = None, harq = None, parentLogger = None):
        openwns.FUN.FunctionalUnit.__init__(self, functionalUnitName = "Scheduler")
        openwns.StaticFactoryClass.__init__(self, 'ltea.mac.scheduler.uplink.UE')

        self.totalTxPowerdBm = totalTxPowerdBm

        if queueSize == None:
            queueSize = 250000 # should be always big enough
        
        if harq == None:
            self.harq = ltea.dll.harq.HARQ(parentLogger = parentLogger)
        else:
            self.harq = harq

        # these are the names for the RLC FU's (ltea.dll.rlc.UnacknowledgedMode) FU and command names, see component.py

        self.queue = ltea.dll.rlc.SegmentingQueue("rlcUnacknowledgedMode", "rlcUnacknowledgedMode", queueSize)
        self.loggerName = "UplinkScheduler"

    def setParentLogger(self, parentLogger):
        self.harq.setParentLogger(parentLogger)
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = self.loggerName,
                                            enabled = True,
                                            parent = parentLogger)
        self.loggerName = "UplinkUEScheduler"
        

class SchedulerBase(openwns.FUN.FunctionalUnit):
    linkAdaptation = None
    logger = None
    loggerName = None
    alpha = None
    P0dBmPerPRB = None
    pucchSize = None
    prachPeriod = None
    srsPeriod = None
    Ks = None
    resourceManager = None
    pathlossEstimationMethod = None
    weightingFactor = None
    
    def __init__(self, linkAdaptation, 
                 alpha, P0dBmPerPRB, Ks = 0.0, pucchSize = 4, 
                 prachPeriod = 10, srsPeriod = 10, parentLogger = None,
                 resourceManager = None, pathlossEstimationMethod = "WBL", weightingFactor = 0.02):
        openwns.FUN.FunctionalUnit.__init__(self, functionalUnitName = "ULScheduler")

        self.linkAdaptation = linkAdaptation
        self.alpha = alpha
        self.P0dBmPerPRB = P0dBmPerPRB
        self.pucchSize = pucchSize
        self.prachPeriod = prachPeriod
        self.srsPeriod = srsPeriod
        self.Ks = Ks
        self.pathlossEstimationMethod = pathlossEstimationMethod
        self.weightingFactor = weightingFactor
        
        if resourceManager is None:
            self.resourceManager = ltea.dll.schedulers.resourceManager.ResourceManagerBase()
        else:
            self.resourceManager = resourceManager    
        
    def setParentLogger(self, parentLogger):
        self.resourceManager.setParentLogger(parentLogger)
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = self.loggerName + "UplinkScheduler",
                                            enabled = True,
                                            parent = parentLogger)
    

class RoundRobin(SchedulerBase, openwns.StaticFactoryClass):
    threegppCalibration = None
    def __init__(self, linkAdaptation, alpha = 1.0, 
                 P0dBmPerPRB = -106, threegppCalibration = False,  Ks = 0.0, 
                 pucchSize = 4, prachPeriod = 10, srsPeriod = 10, parentLogger = None,
                 resourceManager = None, pathlossEstimationMethod = "WBL", weightingFactor = 0.02):
        SchedulerBase.__init__(self, linkAdaptation, alpha, P0dBmPerPRB, Ks, pucchSize, prachPeriod, srsPeriod, parentLogger, resourceManager, pathlossEstimationMethod, weightingFactor)
        openwns.StaticFactoryClass.__init__(self, 'ltea.mac.scheduler.uplink.RoundRobin')
        
        self.threegppCalibration = threegppCalibration
        self.loggerName = "RoundRobin"
