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
import copy

# The strategies for Adaptive Power Control (APC)
class APCStrategy(object):
    nameInAPCStrategyFactory = None
    logger = None
    def __init__(self, parentLogger = None, **kw):
        #self.logger = openwns.logger.Logger("WNS", "APCStrategy", True, parentLogger)
        self.logger = openwns.logger.Logger("WNS", ".".join(["APCStrategy",self.nameInAPCStrategyFactory]), True, parentLogger)
        attrsetter(self, kw)
    def setParentLogger(self,parentLogger = None):
        #self.logger = openwns.logger.Logger("WNS", "APCStrategy", True, parentLogger)
        self.logger = openwns.logger.Logger("WNS", ".".join(["APCStrategy",self.nameInAPCStrategyFactory]), True, parentLogger)
        self.logger.enabled = parentLogger.enabled
        #print "APCStrategy.setParentLogger():",self.logger.name

class DoNotUseAPC(APCStrategy):
    requiresCQI = False
    def __init__(self, **kw):
        self.nameInAPCStrategyFactory = "DoNotUseAPC"
        super(DoNotUseAPC,self).__init__(**kw)

class UseNominalTxPower(APCStrategy):
    requiresCQI = False
    def __init__(self, **kw):
        self.nameInAPCStrategyFactory = "UseNominalTxPower"
        super(UseNominalTxPower,self).__init__(**kw)

class UseMaxTxPower(APCStrategy):
    requiresCQI = False
    def __init__(self, **kw):
        self.nameInAPCStrategyFactory = "UseMaxTxPower"
        super(UseMaxTxPower,self).__init__(**kw)

class APCSlave(APCStrategy):
    requiresCQI = False
    def __init__(self, **kw):
        self.nameInAPCStrategyFactory = "APCSlave"
        super(APCSlave,self).__init__(**kw)

class FCFSMaxPhyMode(APCStrategy):
    requiresCQI = True
    def __init__(self, **kw):
        self.nameInAPCStrategyFactory = "FCFSMaxPhyMode"
        super(FCFSMaxPhyMode,self).__init__(**kw)

class FairSINR(APCStrategy):
    requiresCQI = True
    fair_sinrdl = None
    fair_sinrul = None
    def __init__(self, fairsinrdl = 18.2, fairsinrul = 13.0, **kw):
        self.nameInAPCStrategyFactory = "FairSINR"
        super(FairSINR,self).__init__(**kw)
        self.fair_sinrdl = fairsinrdl
        self.fair_sinrul = fairsinrul

class LTE_UL(APCStrategy):
    requiresCQI = False
    alpha = 1.0
    pNull = "-106 dBm"
    maxInterference = "-200 dBm"
    sinrMargin = "0.0 dB"
    minimumPhyMode = 0

    def __init__(self, **kw):
        self.nameInAPCStrategyFactory = "LTE_UL"
        super(LTE_UL,self).__init__(**kw)


