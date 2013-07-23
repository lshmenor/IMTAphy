import openwns.FUN
import openwns
import ltea.dll.harq

class PhyInterfaceRx(openwns.FUN.FunctionalUnit, openwns.StaticFactoryClass):
    def __init__(self, parentLogger = None, dumpChannel = False, processingDelay=None):
        openwns.StaticFactoryClass.__init__(self, 'ltea.l2s.PhyInterfaceRx')
        openwns.FUN.FunctionalUnit.__init__(self, functionalUnitName = "PhyInterfaceRx")

        self.harq = ltea.dll.harq.HARQ(parentLogger = parentLogger)
        self.logger = openwns.logger.Logger(moduleName = "LTEA",
                                            name = "PhyInterfaceRx",
                                            enabled = True,
                                            parent = parentLogger)
        self.dumpChannel = dumpChannel
        self.processingDelay = processingDelay

    def enableChannelGainProbing(self):
        self.dumpChannel = True

    def disableChannelGainProbing(self):
        self.dumpChannel = False
