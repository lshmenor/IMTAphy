# -*- coding: utf-8 -*-
################################################################################
# This file is part of IMTAphy
# _____________________________________________________________________________
#
# Copyright (C) 2010
# Institute of Communication Networks (LKN)
# Department of Electrical Engineering and Information Technology (EE & IT)
# Technische Universitaet Muenchen
# Arcisstr. 21
# 80333 Muenchen - Germany
# http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
# 
# _____________________________________________________________________________
#
#   IMTAphy is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   IMTAphy is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
#
#################################################################################

import imtaphy.Logger

class LTERel8DownlinkFeedbackManager:
    name = "DefaultLteRel8DownlinkFeedbackManager"
    
    def __init__(self, 
                 enabled = False,
                 precodingMode = "ClosedLoopCodebookBased", # or "SingleAntenna" or "NoPrecoding"
                 numPrbsPerSubband = 2,
                 cqiUpdateFrequency = 5,
                 rankUpdateFrequency = 10,
                 feedbackTotalDelay = 6):
        self.enabled = enabled
        self.precodingMode = precodingMode
        self.numPrbsPerSubband = numPrbsPerSubband
        self.cqiUpdateFrequency = cqiUpdateFrequency
        self.rankUpdateFrequency = rankUpdateFrequency
        self.feedbackTotalDelay = feedbackTotalDelay
    

    
class LTERel10UplinkChannelStatusManager:
    name = "DefaultLteRel10UplinkChannelStatusManager"
    
    def __init__(self,
                 enabled = False,
                 precodingMode = "NoPrecoding", # or "NoPrecoding"
                 srsUpdateFrequency = 5,
                 statusDelay = 3):
        self.enabled = enabled
        self.precodingMode = precodingMode
        self.srsUpdateFrequency = srsUpdateFrequency
        self.statusDelay = statusDelay
        self.logger = imtaphy.Logger.Logger("ULChannelStatus")
        
class PU2RCFeedbackManager:
    name = "PU2RCFeedbackManager"
    
    def __init__(self, 
                 enabled = False,
                 pmis = range(16),
                 precodingMode = "ClosedLoopCodebookBased", # or "SingleAntenna" or "NoPrecoding"
                 numPrbsPerSubband = 2,
                 cqiUpdateFrequency = 5,
                 rankUpdateFrequency = 10,
                 feedbackTotalDelay = 6):
        self.enabled = enabled
        self.pmis = pmis
        self.precodingMode = precodingMode
        self.numPrbsPerSubband = numPrbsPerSubband
        self.cqiUpdateFrequency = cqiUpdateFrequency
        self.rankUpdateFrequency = rankUpdateFrequency
        self.feedbackTotalDelay = feedbackTotalDelay
        

class FixedPMIPRBFeedbackManager:
    name = "FixedPMIPRBFeedbackManager"
    
    def __init__(self, 
                 enabled = False,
                 pmis = range(16),
                 randomize = False,
                 fixedRank = 0,
                 numPrbsPerSubband = 2,
                 cqiUpdateFrequency = 5,
                 rankUpdateFrequency = 10,
                 feedbackTotalDelay = 6):
        self.enabled = enabled
        self.pmis = pmis
        self.randomize = randomize
        self.fixedRank = fixedRank
        self.precodingMode = "ClosedLoopCodebookBased" # the base class needs this
        self.numPrbsPerSubband = numPrbsPerSubband
        self.cqiUpdateFrequency = cqiUpdateFrequency
        self.rankUpdateFrequency = rankUpdateFrequency
        self.feedbackTotalDelay = feedbackTotalDelay
                
                
                