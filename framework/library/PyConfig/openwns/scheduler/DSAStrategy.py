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
import openwns.logger

# The strategies for Dynamic Subcarrier Assignment (DSA)
class DSAStrategy(object):
    nameInDSAStrategyFactory = None
    oneUserOnOneSubChannel = None
    adjacentSubchannelsOnUplink = None
    logger = None
    def __init__(self, parentLogger = None, oneUserOnOneSubChannel = True, adjacentSubchannelsOnUplink = False, **kw):
        #self.logger = openwns.logger.Logger("WNS", "DSAStrategy", True, parentLogger)
        self.logger = openwns.logger.Logger("WNS", ".".join(["DSAStrategy",self.nameInDSAStrategyFactory]), True, parentLogger)
        self.oneUserOnOneSubChannel = oneUserOnOneSubChannel
        self.adjacentSubchannelsOnUplink = adjacentSubchannelsOnUplink
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        #self.logger = openwns.logger.Logger("WNS", "DSAStrategy", True, parentLogger)
        self.logger = openwns.logger.Logger("WNS", ".".join(["DSAStrategy",self.nameInDSAStrategyFactory]), True, parentLogger)
        self.logger.enabled = parentLogger.enabled

# default if DSA is not required (e.g. for old strategies)
class DoNotUseDSA(DSAStrategy):
    requiresCQI = False
    def __init__(self, **kw):
        self.nameInDSAStrategyFactory = "DoNotUseDSA"
        super(DoNotUseDSA,self).__init__(**kw)

# the only DSA strategy useful for the UL slave scheduler (UT.RSTX)
class DSASlave(DSAStrategy):
    requiresCQI = False
    def __init__(self, **kw):
        self.nameInDSAStrategyFactory = "DSASlave"
        super(DSASlave,self).__init__(**kw)
        oneUserOnOneSubChannel = True # must be enforced

# this strategy is used for meta scheduling
class DSAMeta(DSAStrategy):
    requiresCQI = False
    def __init__(self, **kw):
        self.nameInDSAStrategyFactory = "DSAMeta"
        super(DSAMeta,self).__init__(**kw)
        

# The most simple DSA strategy.
# Just start with subChannel 0 and go up linearly, no matter how the CQI is
class LinearFFirst(DSAStrategy):
    requiresCQI = False
    useRandomChannelAtBeginning = None
    def __init__(self, useRandomChannel = False, **kw):
        self.nameInDSAStrategyFactory = "LinearFFirst"
        super(LinearFFirst,self).__init__(**kw)
        self.useRandomChannelAtBeginning = useRandomChannel

# Use channel with best effSINR
# Since we only request one subchannel effSINR = SINR
class BestEffSINR(DSAStrategy):
    requiresCQI = False
    def __init__(self, **kw):
        self.nameInDSAStrategyFactory = "BestEffSINR"
        super(BestEffSINR,self).__init__(**kw)

#random DSA
class Random(DSAStrategy):
    requiresCQI = False
    def __init__(self, **kw):
        self.nameInDSAStrategyFactory = "Random"
        super(Random,self).__init__(**kw)
        
#Fixed equal share DSA
class Fixed(DSAStrategy):
    requiresCQI = False
    timeFirst = False
    def __init__(self, **kw):
        self.nameInDSAStrategyFactory = "Fixed"
        super(Fixed,self).__init__(**kw)

#Position aware Fixed equal share DSA
class NearbyFirst(DSAStrategy):
    requiresCQI = False
    def __init__(self, **kw):
        self.nameInDSAStrategyFactory = "NearbyFirst"
        super(NearbyFirst,self).__init__(**kw)

#Position aware Fixed equal share DSA
class FarFirst(DSAStrategy):
    requiresCQI = False
    def __init__(self, **kw):
        self.nameInDSAStrategyFactory = "FarFirst"
        super(FarFirst,self).__init__(**kw)

#DSA with interference estimation for each Slot
class HighCwithHighI(DSAStrategy):
    requiresCQI = False
    def __init__(self, **kw):
        self.nameInDSAStrategyFactory = "HighCwithHighI"
        super(HighCwithHighI,self).__init__(**kw)

# CQI-aware subchannel assignment
class BestChannel(DSAStrategy):
    requiresCQI = True
    useRandomChannelAtBeginning = None
    def __init__(self, useRandomChannel = False, **kw):
        self.nameInDSAStrategyFactory = "BestChannel"
        super(BestChannel,self).__init__(**kw)
        self.useRandomChannelAtBeginning = useRandomChannel

# CQI-aware subchannel assignment
class BestCapacity(DSAStrategy):
    requiresCQI = True
    useRandomChannelAtBeginning = None
    def __init__(self, useRandomChannel = False, **kw):
        self.nameInDSAStrategyFactory = "BestCapacity"
        super(BestCapacity,self).__init__(**kw)
        self.useRandomChannelAtBeginning = useRandomChannel

