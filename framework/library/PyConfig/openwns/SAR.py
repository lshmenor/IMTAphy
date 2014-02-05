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

from openwns.pyconfig import attrsetter
from openwns.logger import Logger
import openwns.FUN

class SAR(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.sar.SAR'

    segmentSize = 42
    logger = None
    preserving = True
    headerSize = 1

    useSuspendProbe = False
    suspendProbeName = "timeBufferEmpty"

    def __init__(self, segmentSize, **kw):
        self.segmentSize = segmentSize
        attrsetter(self, kw)

class Fixed(SAR):
    __plugin__ = 'wns.sar.Fixed'
    name = "SAR Fixed"

    def __init__(self, segmentSize, parentLogger = None, **kw):
        super(Fixed, self).__init__(segmentSize, **kw)
        self.logger = Logger('WNS','FixedSAR',True, parentLogger)

class Soft(SAR):
    __plugin__ = 'wns.sar.Soft'
    name = "SAR Soft"
    PERProvider = "Name of the PER Provider Functional unit, e.g. the MACr used"
    useProbe  = False
    probeName = "PER"

    def __init__(self, segmentSize, PERProviderName, parentLogger = None, **kw):
        super(Soft, self).__init__(segmentSize, **kw)
        self.PERProvider = PERProviderName
        self.logger = Logger('WNS','SoftSAR',True, parentLogger)

class DynamicSAR(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.sar.DynamicSAR'
    name = 'Dynamic SAR'
    searchAlgo = 'wns.search.SimpleBinarySearch'

    def __init__(self, maxSegmentSize, parentLogger = None, **kw):
        self.maxSegmentSize = maxSegmentSize
        self.logger = Logger('WNS','DynamicSAR',True, parentLogger)

class ReorderingWindow(object):

    def __init__(self, snFieldLength, parentLogger = None):
        self.snFieldLength = snFieldLength
        self.tReordering = 0.035
        self.logger = openwns.logger.Logger('WNS', 'SegAndConcat.Reordering', True, parentLogger)

class SegAndConcat(openwns.StaticFactoryClass):

    """
    The size of and number PDUs passed to lower layers is calculated as follows:
    The totalsize to be segmented is the length of the SDU received from upper layers
    plus the sduLengthAddition (default 0). This total length is cut into pieces of
    segmentSize. The last segment can possible be smaller than the segment size.
    Each segment is then prepended by a header of length headerSize.
    """

    def __init__(self, segmentSize, headerSize, commandName, delayProbeName = None, parentLogger = None):
        openwns.StaticFactoryClass.__init__(self, "wns.sar.SegAndConcat")
        self.logger = openwns.logger.Logger('WNS', 'SegAndConcat', True, parentLogger)
        self.commandName = commandName
        self.segmentSize = segmentSize
        self.headerSize = headerSize
        self.sduLengthAddition = 0
        # long serial number option chosen for safety here. If too many segments
        # are on the way, the segments will not be reassembled if field length
        # is too short.
        # todo dbn: This should be set to the short option (5) for VoIP. In
        # general we need simulator parameter settings per QoS class
        self.reorderingWindow = ReorderingWindow(snFieldLength = 10, parentLogger = self.logger)
        self.isSegmenting = False
        self.delayProbeName = delayProbeName
        self.segmentDropRatioProbeName = "segmentDropRatio"
