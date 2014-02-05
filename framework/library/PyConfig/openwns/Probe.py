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

from pyconfig import attrsetter
from openwns.logger import Logger
import openwns.FUN

class Plugin(object):
    nameInPluginFactory = None


class Probe(openwns.FUN.FunctionalUnit):

    name = None
    localIDs = None
    logger = None

    def __init__(self, name, commandName=None):
        super(Probe,self).__init__(functionalUnitName = name, commandName=commandName)
        self.name = name
        self.localIDs = {}

    def setLocalIDs(self, localIDs):
        self.localIDs = localIDs

    def addLocalIDs(self, localIDs):
        self.localIDs.update(localIDs)


class Packet(Probe):
    __plugin__ = 'wns.probe.Packet'
    incomingDelayProbeName = None
    outgoingDelayProbeName = None
    incomingThroughputProbeName = None
    outgoingSizeProbeName = None
    incomingSizeProbeName = None
    
    prefix = None
    """ Probe name prefix """


    def __init__(self, name, prefix, commandName = None, parentLogger=None, moduleName='WNS', **kw):
        super(Packet,self).__init__(name, commandName)
                
        self.prefix = prefix
        self.incomingDelayProbeName = self.prefix + ".packet.incoming.delay"
        self.outgoingDelayProbeName = self.prefix + ".packet.outgoing.delay"
        self.incomingThroughputProbeName = self.prefix + ".packet.incoming.bitThroughput"
        self.outgoingSizeProbeName = self.prefix + ".packet.outgoing.size"
        self.incomingSizeProbeName = self.prefix + ".packet.incoming.size"
        self.logger = Logger(moduleName, name, True, parentLogger)
        attrsetter(self, kw)


class Window(Probe):
    """ This probing FU measures the throughput in a windowed fashion

    6 Values are measured:
    incoming, outgoing and aggreagted throughput in Compounds and Bit
    per second.

    The values are averaged over 'windowSize'. Evaluation starts after
    'windowSize' and is done every 'sampleInterval' seconds. The
    window is a sliding window, i.e. a sampleInterval higher than
    windowSize is possible.

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

    Note: If 'sampleInterval' is not set, it is set to 'windowSize'.
    """

    __plugin__ = 'wns.probe.Window'
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
    

    windowSize = None
    """ Length of the sliding window to use for averaging """

    sampleInterval = None
    """ Time between two puts into the probe """
    
    prefix = None
    """ Probe name prefix """

    def __init__(self, name, prefix, commandName=None, windowSize = 1.0, sampleInterval = None, parentLogger=None, moduleName='WNS', **kw):
        super(Window,self).__init__(name, commandName)
        self.logger = Logger(moduleName, name, True, parentLogger)
        self.windowSize = windowSize
        if sampleInterval == None:
            self.sampleInterval = self.windowSize
        else:
            self.sampleInterval = sampleInterval

        self.prefix = prefix

        assert(self.sampleInterval <= self.windowSize)

        self.incomingBitThroughputProbeName = self.prefix + ".window.incoming.bitThroughput"
        self.incomingCompoundThroughputProbeName = self.prefix + ".window.incoming.compoundThroughput"
        self.outgoingBitThroughputProbeName = self.prefix + ".window.outgoing.bitThroughput"
        self.outgoingCompoundThroughputProbeName = self.prefix + ".window.outgoing.compoundThroughput"
        self.aggregatedBitThroughputProbeName = self.prefix + ".window.aggregated.bitThroughput"
        self.aggregatedCompoundThroughputProbeName = self.prefix + ".window.aggregated.compoundThroughput"
        self.relativeBitsGoodputProbeName = self.prefix + ".window.relative.bitGoodput"
        self.relativeCompoundsGoodputProbeName = self.prefix + ".window.relative.compoundGoodput"

        attrsetter(self, kw)

class Constant(object):
    __plugin__ = "Constant"

    value  = None

    def __init__(self, value):
        self.value = value


class IDProvider(object):
    name = None
    provider = None

    def __init__(self, name, provider):
        self.name = name

        if type(provider) == type(1):
            provider = Constant(provider)

        self.provider = provider


class ErrorRate(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.probe.ErrorRate'
    name = None
    probeName = None
    errorRateProvider = None
    logger = None

    def __init__(self, name, prefix, errorRateProvider, commandName=None, parentLogger=None,  moduleName='WNS', **kw):
        super(ErrorRate,self).__init__(functionalUnitName = name, commandName=commandName)
        self.name = name
        self.probeName = prefix + "ErrorRate"
        self.errorRateProvider = errorRateProvider
        self.logger = Logger(moduleName, name, True, parentLogger)
        attrsetter(self, kw)

class PacketProbeBus(Packet):
    __plugin__ = 'wns.probe.PacketProbeBus'

    def __init__(self, name, prefix, commandName = None, parentLogger=None, moduleName='WNS', **kw):
        super(PacketProbeBus,self).__init__(name, prefix, commandName, parentLogger, moduleName, **kw)

class WindowProbeBus(Window):
    __plugin__ = 'wns.probe.WindowProbeBus'

    def __init__(self, name, prefix, commandName = None, windowSize = 1.0, sampleInterval = None, parentLogger=None, moduleName='WNS', **kw):
        super(WindowProbeBus,self).__init__(name, prefix, commandName, windowSize, sampleInterval, parentLogger, moduleName, **kw)


class Tick(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.probe.Tick'

    probeOutgoing = None
    logger = None

    def __init__(self, probeName, probeOutgoing = True, parentLogger = None):
        super(Tick, self).__init__(functionalUnitName = "Tick" + probeName, commandName = probeName)
        self.probeOutgoing = probeOutgoing
        self.logger = openwns.logger.Logger("WNS", "Tick", True, parentLogger)

class Tack(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.probe.Tack'

    probeAndCommandName = None
    probeOutgoing = None
    logger = None

    def __init__(self, probeName, probeOutgoing = True, parentLogger = None):
        super(Tack, self).__init__(functionalUnitName = "Tack" + probeName)
        self.probeAndCommandName = probeName
        self.probeOutgoing = probeOutgoing
        self.logger = openwns.logger.Logger("WNS", "Tack", True, parentLogger)


