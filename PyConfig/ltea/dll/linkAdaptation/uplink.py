import openwns
import openwns.logger

class LinkAdapationBase:
    logger = None
    threshold_dB = None

    def __init__(self, threshold_dB):
        self.threshold_dB = threshold_dB # positive values should mean more conservative (i.e. estimated SINR = SINR - threshold)

    
    def initLogger(self, loggerName):
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = loggerName,
                                            enabled = True,
                                            parent = None)
    
class SINRThreshold(LinkAdapationBase):
    nameInFactory = "ltea.dll.linkAdaptation.uplink.SINRThreshold"
                                            
    def __init__(self, threshold_dB = 0):
        LinkAdapationBase.__init__(self, threshold_dB)
        self.initLogger("ULSINRThresholdLA")
        

class Adaptive(LinkAdapationBase):
    nameInFactory = "ltea.dll.linkAdaptation.uplink.Adaptive"
    fastCrossingWeight = None
    crossingThreshold = None
    longTimeWeight = None
                        
                      
    def __init__(self, fastCrossingWeight, longTimeWeight, crossingThreshold, threshold_dB = 0):
        LinkAdapationBase.__init__(self, threshold_dB)
        self.initLogger("ULAdaptive")
        self.fastCrossingWeight = fastCrossingWeight
        self.crossingThreshold = crossingThreshold
        self.longTimeWeight = longTimeWeight