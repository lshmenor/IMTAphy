import openwns.FUN
import openwns

import ltea.dll.rlc
import ltea.dll.harq

class RoundRobinScheduler(openwns.FUN.FunctionalUnit, openwns.StaticFactoryClass):
    def __init__(self, txPowerdBmPerPRB, queueSize = None, mcsOffset = 0.0, harqFactor=10.0, tilt = 0.0, parentLogger = None):
        openwns.StaticFactoryClass.__init__(self, 'ltea.mac.RoundRobinScheduler')
        openwns.FUN.FunctionalUnit.__init__(self, functionalUnitName = "Scheduler")

        if queueSize == None:
            queueSize = 250000 # should be always big enough

        self.harq = ltea.dll.harq.HARQ(parentLogger = parentLogger)
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = "RoundRobinScheduler",
                                            enabled = True,
                                            parent = None)

        # these are the names for the RLC FU's (ltea.dll.rlc.UnacknowledgedMode) FU and command names, see component.py
        self.segmentingQueue = ltea.dll.rlc.SegmentingQueue("rlcUnacknowledgedMode", "rlcUnacknowledgedMode", queueSize)
        self.txPowerdBmPerPRB = txPowerdBmPerPRB

        self.mcsOffset = mcsOffset
        self.harqFactor = harqFactor
        self.tilt = tilt

    def setParentLogger(self, parentLogger):
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = "RoundRobinScheduler",
                                            enabled = True,
                                            parent = parentLogger)

class MUmimoScheduler(openwns.FUN.FunctionalUnit, openwns.StaticFactoryClass):
    def __init__(self, txPowerdBmPerPRB, queueSize = None, parentLogger = None):
        openwns.StaticFactoryClass.__init__(self, 'ltea.mac.MUmimoScheduler')
        openwns.FUN.FunctionalUnit.__init__(self, functionalUnitName = "Scheduler")

        if queueSize == None:
			queueSize = 250000 # should be always big enough
        self.harq = ltea.dll.harq.HARQ(parentLogger = parentLogger)
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = "MUmimoScheduler",
                                            enabled = True,
                                            parent = None)

        # these are the names for the RLC FU's (ltea.dll.rlc.UnacknowledgedMode) FU and command names, see component.py
        self.segmentingQueue = ltea.dll.rlc.SegmentingQueue("rlcUnacknowledgedMode", "rlcUnacknowledgedMode", queueSize)
        self.txPowerdBmPerPRB = txPowerdBmPerPRB

    def setParentLogger(self, parentLogger):
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = "MUmimoScheduler",
                                            enabled = True,
                                            parent = parentLogger)
                                            
class MUmimoScheduler2(openwns.FUN.FunctionalUnit, openwns.StaticFactoryClass):
    def __init__(self, txPowerdBmPerPRB, queueSize = None, parentLogger = None):
        openwns.StaticFactoryClass.__init__(self, 'ltea.mac.MUmimoScheduler2')
        openwns.FUN.FunctionalUnit.__init__(self, functionalUnitName = "Scheduler")

        if queueSize == None:
            queueSize = 250000 # should be always big enough
        self.harq = ltea.dll.harq.HARQ(parentLogger = parentLogger)
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = "MUmimoScheduler2",
                                            enabled = True,
                                            parent = None)

        # these are the names for the RLC FU's (ltea.dll.rlc.UnacknowledgedMode) FU and command names, see component.py
        self.segmentingQueue = ltea.dll.rlc.SegmentingQueue("rlcUnacknowledgedMode", "rlcUnacknowledgedMode", queueSize)
        self.txPowerdBmPerPRB = txPowerdBmPerPRB

    def setParentLogger(self, parentLogger):
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = "MUmimoScheduler",
                                            enabled = True,
                                            parent = parentLogger)
class MUmimoRandomScheduler(openwns.FUN.FunctionalUnit, openwns.StaticFactoryClass):
    def __init__(self, txPowerdBmPerPRB, queueSize = None, parentLogger = None):
        openwns.StaticFactoryClass.__init__(self, 'ltea.mac.MUmimoRandomScheduler')
        openwns.FUN.FunctionalUnit.__init__(self, functionalUnitName = "Scheduler")
        
        if queueSize == None:
			queueSize = 250000 # should be always big enough
        self.harq = ltea.dll.harq.HARQ(parentLogger = parentLogger)
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = "MUmimoRandomScheduler",
                                            enabled = True,
                                            parent = None)

        # these are the names for the RLC FU's (ltea.dll.rlc.UnacknowledgedMode) FU and command names, see component.py
        self.segmentingQueue = ltea.dll.rlc.SegmentingQueue("rlcUnacknowledgedMode", "rlcUnacknowledgedMode", queueSize)
        self.txPowerdBmPerPRB = txPowerdBmPerPRB

    def setParentLogger(self, parentLogger):
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = "MUmimoRandomScheduler",
                                            enabled = True,
                                            parent = parentLogger)