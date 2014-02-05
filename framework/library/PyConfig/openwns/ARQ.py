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

"""Various ARQ implementations as well as a PiggyBacker

Available implementations are:
ARQ.NoARQ - No ARQ at all
ARQ.StopAndWait - Stop and wait style ARQ
ARQ.CumulativeACK - As defined in 802.16
ARQ.SelectiveRepeat - Operating with selective repeats
"""
import math
import FUN

from pyconfig import attrsetter
from logger import Logger

class ARQ(object):
    """Base class for all ARQs

    ARQ.resendTimeout: time after which the ARQ should try to resend a
    PDU if no ACK was received
    """
    resendTimeout = 0.1

    useSuspendProbe = False
    suspendProbeName = "timeBufferEmpty"
    arqStatusCollector = None
    logger = None

    def __init__(self, **kw):
        attrsetter(self, kw)

class NoStatusCollection:
    __plugin__ = "NoStatusCollection"
    logger = None

    def __init__(self, parentLogger = None, loggerEnabled=True):
        self.logger = Logger('WNS', 'ARQStat', loggerEnabled, parentLogger)

class StatusCollectorCounter:
    __plugin__ = "StatusCollectorCounter"
    logger = None

    def __init__(self, parentLogger = None, loggerEnabled=True, **kw):
        self.logger = Logger('WNS', 'ARQStat', loggerEnabled, parentLogger)
        attrsetter(self, kw)

class StatusCollectorTwoSizesWindowed:
    __plugin__ = "StatusCollectorTwoSizesWindowed"
    logger = None
    windowSize = None
    minSamples = None
    insufficientSamplesReturn = None
    frameSizeThreshold = None

    def __init__(self, parentLogger = None, loggerEnabled=True, **kw):
        self.logger = Logger('WNS', 'ARQStat', loggerEnabled, parentLogger)
        attrsetter(self, kw)

    def setParams(self,  windowSize, minSamples, insufficientSamplesReturn, frameSizeThreshold):
        self.windowSize = windowSize
        self.minSamples = minSamples
        self.insufficientSamplesReturn = insufficientSamplesReturn
        self.frameSizeThreshold = frameSizeThreshold

class NoARQ(ARQ):
    __plugin__ = 'wns.arq.None'
    name = "NoARQ"

    windowSize = 0

    def __init__(self, parentLogger = None, **kw):
        super(NoARQ,self).__init__()
        self.logger = Logger('WNS', 'No-ARQ', True, parentLogger)
        self.arqStatusCollector = NoStatusCollection();
        attrsetter(self, kw)

class StopAndWait(ARQ):
    __plugin__ = 'wns.arq.StopAndWait'
    name = "StopAndWait"

    bitsPerIFrame = 2
    bitsPerRRFrame = 2

    def __init__(self, parentLogger = None, statusCollector=NoStatusCollection, **kw):
        super(StopAndWait,self).__init__()
        self.logger = Logger('WNS', 'SNW-ARQ', True, parentLogger)
        self.arqStatusCollector = statusCollector(self.logger)
        attrsetter(self, kw)

class StopAndWaitRC(ARQ):
    __plugin__ = 'wns.arq.StopAndWaitRC'
    name = "StopAndWaitRC"

    bitsPerIFrame = 2
    bitsPerRRFrame = 2

    class Ack(FUN.Port):
        name = 'Ack'

    class Data(FUN.Port):
        name = 'Data'

    def __init__(self, parentLogger = None, statusCollector=NoStatusCollection, **kw):
      super(StopAndWaitRC,self).__init__()
      self.logger = Logger('WNS', 'SNW-ARQ', True, parentLogger)
      #self.arqStatusCollector = statusCollector(self.logger)
      attrsetter(self, kw)

class GoBackN(ARQ):
    __plugin__ = 'wns.arq.GoBackN'
    name = "GoBackN"

    windowSize = 16
    sequenceNumberSize = 2*windowSize

    useProbe = False
    probeName = "xxxTransmissionAttempts"

    def __init__(self, parentLogger = None, statusCollector=NoStatusCollection, **kw):
        super(GoBackN,self).__init__()
        self.logger = Logger('WNS', 'GBN-ARQ', True, parentLogger)
        self.arqStatusCollector = statusCollector(self.logger)
        attrsetter(self, kw)

class CumulativeACK(ARQ):
    __plugin__ = 'wns.arq.CumulativeACK'
    name = "CumulativeACK"

    windowSize = 1024
    sequenceNumberSize = 2*windowSize

    def __init__(self, parentLogger = None, statusCollector=NoStatusCollection,**kw):
        super(CumulativeACK,self).__init__()
        self.logger = Logger('WNS', 'CUM-ARQ', True, parentLogger)
        self.arqStatusCollector = statusCollector(self.logger)
        attrsetter(self, kw)

class SelectiveRepeat(ARQ):
    __plugin__ = 'wns.arq.SelectiveRepeat'
    name = "SelectiveRepeat"

    windowSize = 1024
    sequenceNumberSize = 2*windowSize
    useProbe = False
    probeName = "xxxTransmissionAttempts"
    ackDelayProbeName = "xxxAckDelay"
    RTTProbeName = "xxxRoundTripTime"
    commandSize = None

    def __init__(self, parentLogger = None, statusCollector=NoStatusCollection, **kw):
        super(SelectiveRepeat,self).__init__()
        self.logger = Logger('WNS', 'SR-ARQ', True, parentLogger)
        self.arqStatusCollector = statusCollector(self.logger)
        attrsetter(self, kw)
        if (self.commandSize == None):
          # reserve command space for the max length of the sequence number
            self.commandSize = int(math.ceil(math.log(self.sequenceNumberSize)/math.log(2)))
          # plus one additional bit for the command type (I or ACK)
            self.commandSize += 1

        assert self.sequenceNumberSize >= 2*self.windowSize, """You misconfigured the sequenceNumberSize.
        It needs to be at least two times the windowSize"""

        assert pow(2,(self.commandSize-1)) >= self.sequenceNumberSize, "Misconfigured or Mis-calculated commandSize."


class PiggyBacker(object):
     """ACKs from an ARQ are send piggy backed with I frames.

     self.arq: Name of the ARQ FU in the FUN
     self.name: Name for logging purpose
     self.bitsIfPiggyBacked: overhaed in bits if an ACK is sent piggy backed
     self.bitsIfNotPiggyBacked: overhaed in bits if an ACK is not sent piggy
     backed
     self.addACKPDUSize: If set to true the size of the ACK PDU is added to the
     PDU the ACK is being piggybacked on
     """

     __plugin__ = 'wns.arq.PiggyBacker'
     name = "PiggyBacker"

     arq = None
     bitsIfPiggyBacked = 0
     bitsIfNotPiggyBacked = 1
     addACKPDUSize = True

     def __init__(self, name, arq, **kw):
         self.name = name
         self.arq = arq
         attrsetter(self, kw)
