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

import openwns.module
import openwns.simulator
import imtaphy.Feedback
import dll

class BlerConfig:
    shiftQPSKdB = 0.2
    shiftQAM16dB = 0.2
    shiftQAM64dB = 0.2
    assumeNumPRBs = 50


class IMTAphy(openwns.module.Module):
    channelConfig = None
    downlinkFeedbackManager = imtaphy.Feedback.LTERel8DownlinkFeedbackManager(enabled = False)
    uplinkStatusManager = imtaphy.Feedback.LTERel10UplinkChannelStatusManager(enabled = False)
    blerConfig = None
    
    def __init__(self):
        super(IMTAphy, self).__init__("imtaphy", "imtaphy")
        self.blerConfig = BlerConfig()
        
    ## TODO: find a way to configure the FeedbackManager in a nicer way. Maybe make it a 
    ## member of the channel instead of a singleton


# add the Module in order to get it loaded
openwns.simulator.OpenWNS.modules.imtaphy = IMTAphy()
