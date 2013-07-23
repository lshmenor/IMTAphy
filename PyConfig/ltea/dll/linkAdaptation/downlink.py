import openwns
import openwns.logger

class LinkAdapation:
    Rel10CSIports = None
    Rel8AntennaPorts = None

    def __init__(self, rel8Ports = 1, rel10Ports = 0):
        self.Rel10CSIports = rel10Ports
        self.Rel8AntennaPorts = rel8Ports

    def setParentLogger(self, parentLogger):
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = self.loggerName + "LA",
                                            enabled = True,
                                            parent = parentLogger)        
        
    
class SINRThreshold(LinkAdapation):
    nameInFactory = "ltea.dll.linkAdaptation.downlink.SINRThreshold"

    def __init__(self, threshold_dB = 0, rel8Ports = 1, rel10Ports = 0):
        LinkAdapation.__init__(self, rel8Ports, rel10Ports)
        self.threshold = threshold_dB # positive values shoudl mean more conservative (i.e. estimated SINR = SINR - threshold)
        self.loggerName = "DLsinrThreshold"
                                            
class BLERadaptive(LinkAdapation):
    nameInFactory = "ltea.dll.linkAdaptation.downlink.BLERAdaptive"
                                            
    def __init__(self, threshold_dB = 0, offsetDelta = 0.04, updateInterval = 5, targetBLER = 0.1, rel8Ports = 1, rel10Ports = 0):
        LinkAdapation.__init__(self, rel8Ports, rel10Ports)
        
        self.threshold = threshold_dB # positive values shoudl mean more conservative (i.e. estimated SINR = SINR - threshold)
        self.offsetDelta = offsetDelta
        self.updateInterval = updateInterval
        self.targetBLER = targetBLER
        self.loggerName = "BLERAdaptive"
