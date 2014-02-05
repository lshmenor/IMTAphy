import openwns.logger
import openwns
import openwns.SAR


class RadioBearerID(openwns.StaticFactoryClass):
    def __init__(self):
        openwns.StaticFactoryClass.__init__(self, "ltea.rlc.RadioBearerID")

class FullQueue(object):

    def __init__(self, packetProbeCommandName, windowProbeCommandName, 
            pdcpCommandName, parentLogger = None):

        self.nameInQueueFactory = "FullQueue"
        self.windowProbeCommandName = windowProbeCommandName
        self.packetProbeCommandName = packetProbeCommandName
        self.pdcpCommandName = pdcpCommandName
        self.headerSize = 24 # See SegmentingQueue below for info
        self.logger = openwns.logger.Logger("WNS", "FullQueue", True, parentLogger);

class SegmentingQueue(object):
    """
    This class was part of openWNS (open Wireless Network Simulator)
    _____________________________________________________________________________
    
    Copyright (C) 2004-2007
    Chair of Communication Networks (ComNets)
    Kopernikusstr. 5, D-52074 Aachen, Germany
    phone: ++49-241-80-27910,
    fax: ++49-241-80-22242
    email: info@openwns.org
    www: http://www.openwns.org

    openWNS is free software; you can redistribute it and/or modify it under the
    terms of the GNU Lesser General Public License version 2 as published by the
    Free Software Foundation;

    openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
    details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    Dynamic segmentation encapsulate in a scheduler queue
    Can be used as implementation for 3GPP LTE R8 RLC

    fixedHeaderSize : Every PDU that possibly contains multiple SDUs has a header of at least this size
    extenstionHeaderSize: If segments are concatenated this size is added additionally for each extra SDU
    byteAlignHeader: If set to True then the total header size will be extended such that totalHeaderSize mod 8 = 0
    """

    nameInQueueFactory = None
    logger = None
    sizeProbeName = None
    overheadProbeName = None
    delayProbeName = None
    TxRx = None
    localIDs = None
    minimumSegmentSize = None # used to ask for resources of at least this size
    usePadding = False
    isDropping = False

    def setLocalIDs(self, localIDs):
        self.localIDs = localIDs

    def addLocalIDs(self, localIDs):
        self.localIDs.update(localIDs)

    def __init__(self, segmentHeaderFUName, segmentHeaderCommandName, queueSize, parentLogger = None, **kw):
        super(SegmentingQueue,self).__init__()
        self.localIDs = {}
        self.nameInQueueFactory = "SegmentingQueue"
        self.logger = openwns.logger.Logger("WNS", "SegmentingQueue", True, parentLogger);
        self.sizeProbeName = 'SegmentingQueueSize'
        self.overheadProbeName = 'SegmentingQueueOverhead'
        self.minimumSegmentSize = 32 # Bits

        # The queue models PDCP, RLC and MAC header overhead
        # PDCP: 8 bit AFTER header compression: R10 TS 36.323 Section 6.2.1 for 7 bit SN
        pdcpHeaderSize = 8
        # RLC:  8 bit per PDU for 5 bit SN and another 12 bit per concatenation + 4 bit
        # for uneven number of segments to have a byte aligned header size: 
        # R10 TS 36.322 Section 6.2.1.3 Unacknowledged Mode
        rlcHeaderSize = 8
        self.extensionHeaderSize = 12 
        self.byteAlignHeader = True 
        # MAC:8 bit if only one RLC PDU is multiplexed into the MAC PDU: R10 TS 36.321 Section 6.2.1
        # "There is one L/F field per MAC PDU subheader except for the last subheader"
        # Uncomment other options to model MAC multiplexing overhead
        # One user data MAC-SDU  
        macHeaderSize = 8 
        # In this order: One control MAC-SDU (7 bit length field) and one user data MAC-SDU
        #self.macHeaderSize = 24 
        # In this order: One user data MAC-SDU and one control MAC-SDU 
        #self.macHeaderSize = 32 

        self.fixedHeaderSize = pdcpHeaderSize + rlcHeaderSize + macHeaderSize

        # 24 bit CRC per 6144 bit code block (R10 TS 36.212 Section 5.1.2)
        # are already considered in the link-to-system model and are therefore not added here.

        self.segmentHeaderFUName = segmentHeaderFUName
        self.segmentHeaderCommandName = segmentHeaderCommandName
        self.isDropping = False
        self.queueSizeLimitBits = queueSize # must be big enough so that during one TTI scheduler cannot empty it 

class ReorderingWindow(openwns.SAR.ReorderingWindow):

    def __init__(self, snFieldLength, parentLogger = None):
        super(ReorderingWindow, self).__init__(snFieldLength, parentLogger)
        # self.tReordering can take values from 0ms to 200ms
        # According to 3GPP TS 36.331 V8.5.0 (2009-03)
        # However SRB1 and SRB2 default radio bearer configurations use 35ms
        # This is also described in TS 36.331
        self.tReordering = 0.035
        self.logger = openwns.logger.Logger('LTEA', 'UM.Reordering', True, parentLogger)

class UnacknowledgedMode(openwns.SAR.SegAndConcat):

    """LTE UM as described in 3GPP TS 36.322 V8.5.0
    The size of and number PDUs passed to lower layers is calculated as follows:
    The totalsize to be segmented is the length of the SDU received from upper layers
    plus the sduLengthAddition (default 0). This total length is cut into pieces of
    segmentSize. The last segment can possible be smaller than the segment size.
    Each segment is then prepended by a header of length headerSize.
    """

    def __init__(self, segmentSize, headerSize, commandName, delayProbeName = None, parentLogger = None):
        super(UnacknowledgedMode, self).__init__(segmentSize, headerSize, commandName, delayProbeName, parentLogger)
        self.__plugin__ = "ltea.rlc.UnacknowledgedMode"
        self.logger = openwns.logger.Logger('LTEA', 'UM', True, parentLogger)
        self.sduLengthAddition = 0
        # long serial number option chosen for safety here. If too many segments
        # are on the way, the segments will not be reassembled if field length
        # is too short.
        # todo dbn: This should be set to the short option (5) for VoIP. In
        # general we need simulator parameter settings per QoS class
        self.reorderingWindow = ReorderingWindow(snFieldLength = 10, parentLogger = self.logger)
        self.isSegmenting = False
        self.segmentDropRatioProbeName = "ltea.rlc.um.segmentDropRatio"
        self.reorderingWindowSizeProbeName = "ltea.rlc.um.reorderingWindowSize"
        self.reassemblyBufferSizeProbeName = "ltea.rlc.um.reassemblyBufferSize"
