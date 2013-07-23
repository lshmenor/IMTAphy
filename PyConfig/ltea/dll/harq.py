import openwns
from openwns.logger import Logger

class ChaseCombiningDecoder(openwns.StaticFactoryClass):

    def __init__(self, parentLogger = None):
        openwns.StaticFactoryClass.__init__(self, "ltea.ChaseCombiningDecoder")
        self.logger = openwns.logger.Logger("LTEA", "ChaseCombining", True, parentLogger)


class HARQEntity:

    def __init__(self, parentLogger=None):
        self.decoder = ChaseCombiningDecoder(parentLogger=parentLogger)

class HARQ(openwns.StaticFactoryClass):
    def __init__(self, parentLogger=None):
        openwns.StaticFactoryClass.__init__(self, "ltea.harq")
        self.numSenderProcesses = 8
        self.numReceiverProcesses = 8 
        self.numRVs = 1
        self.retransmissionLimit = 3
        # delay consists of 1ms for the feedback transmission itself, 3ms for the typical eNB processing
        # time minus a little epsilon to allow after 1ms transmission + 3ms UE delay + 1ms transmission + 3ms
        # eNB processing the retransmission is available 8m of the initial transmission (UL basically same story)
        feedbackTransmission = 0.001
        processingDelay = 0.003
        epsilon = 0.0001 
        self.feedbackDecodingDelay = feedbackTransmission + processingDelay - epsilon
        self.harqEntity = HARQEntity(parentLogger)
        self.harqEntity.retransmissionLimit = self.retransmissionLimit
        self.logger = openwns.logger.Logger("LTEA", "HARQ", True, parentLogger)
        
    def setParentLogger(self, parentLogger):
        self.logger = openwns.logger.Logger("LTEA", "HARQ", True, parentLogger)
