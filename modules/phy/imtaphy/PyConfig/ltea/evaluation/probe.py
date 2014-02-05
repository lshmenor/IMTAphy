import openwns.Probe
import openwns.logger
from openwns.pyconfig import attrsetter


class Window(openwns.Probe.Probe):
    """ This probing FU measures the throughput in a windowed fashion

    6 Values are measured:
    incoming, outgoing and aggreagted throughput in Compounds and Bit
    per second.

    The values are averaged over 'windowSize'. Evaluation starts after
    'windowSize' and is done every 'windowSize' seconds. T

    The 6 values are measured as follows:

    -------------              -------------
    | measuring |              |   peer    |
    |    FU     |  aggregated  |    FU     |
    |           |<-------------|---------| |
    -------------              -------------
      |       ^                          ^
      v       |                          |
     outgoing incoming                   incoming from
                                         measuring FU

    """

    __plugin__ = 'ltea.helper.probe.Window'
    """ Name in the static factory """

    incomingBitThroughputProbeName = None
    """ Bus name for the incoming bit throughput measurement """

    incomingCompoundThroughputProbeName = None
    """ Bus name for the incoming compound throughput measurement """

    outgoingBitThroughputProbeName = None
    """ Bus name for the outgoing bit throughput measurement """

    outgoingCompoundThroughputProbeName = None
    """ Bus name for the outgoing compound throughput measurement """

    aggregatedBitThroughputProbeName = None
    """ Bus name for the aggregated bit throughput measurement """

    aggregatedCompoundThroughputProbeName = None
    """ Bus name for the aggregated compound throughput measurement """

    relativeBitsGoodputProbeName = None
    """ Bus name for the relative bit goodput (aggregated/outgoing) measurement """

    relativeCompoundsGoodputProbeName = None
    """ Bus name for the relative compounds goodput (aggregated/outgoing) measurement """
    
    settlingTime = None
    """ Time after which first window starts"""

    windowSize = None
    """ Length of the sliding window to use for averaging """
    
    prefix = None
    """ Probe name prefix """

    def __init__(self, name, prefix, commandName=None, windowSize = 1.0, settlingTime = 0.0, parentLogger=None, moduleName='LTEA', **kw):
        super(Window,self).__init__(name, commandName)
        self.logger = openwns.logger.Logger(moduleName, name, True, parentLogger)
        self.windowSize = windowSize

        self.prefix = prefix

        self.incomingBitThroughputProbeName = self.prefix + ".window.incoming.bitThroughput"
        self.incomingCompoundThroughputProbeName = self.prefix + ".window.incoming.compoundThroughput"
        self.outgoingBitThroughputProbeName = self.prefix + ".window.outgoing.bitThroughput"
        self.outgoingCompoundThroughputProbeName = self.prefix + ".window.outgoing.compoundThroughput"
        self.aggregatedBitThroughputProbeName = self.prefix + ".window.aggregated.bitThroughput"
        self.aggregatedCompoundThroughputProbeName = self.prefix + ".window.aggregated.compoundThroughput"
        self.relativeBitsGoodputProbeName = self.prefix + ".window.relative.bitGoodput"
        self.relativeCompoundsGoodputProbeName = self.prefix + ".window.relative.compoundGoodput"
        self.settlingTime = settlingTime

        attrsetter(self, kw)
