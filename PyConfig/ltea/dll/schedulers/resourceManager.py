import openwns
import openwns.logger

class ResourceManagerBase:
    nameInFactory = "ltea.dll.schedulers.resourceManager.ResourceManagerBase"
    logger = None
    loggerName = "NoResourceManager"
    
    def __init__(self):
        pass
    
    def setParentLogger(self, parentLogger):
        self.logger = openwns.logger.Logger("LTEA", self.loggerName, True, parent = parentLogger)