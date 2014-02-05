###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2007
# Chair of Communication Networks (ComNets)
# Kopernikusstr. 16, D-52074 Aachen, Germany
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

import scenarios.interfaces
import scenarios.channelmodel

class IndoorHotspotChannelModelCreator(scenarios.channelmodel.SingleChannelModelCreator):
    
    def __init__(self, transceiverTypes):
        import rise.scenario.Pathloss
        from rise.scenario import Shadowing
        from rise.scenario import FastFading
        transceiverPairs = transceiverTypes
        scenarios.channelmodel.SingleChannelModelCreator.__init__(
            self, transceiverPairs, rise.scenario.Pathloss.ITUInH(), Shadowing.No(), FastFading.No())

class UrbanMicroChannelModelCreator(scenarios.channelmodel.SingleChannelModelCreator):
    
    def __init__(self, transceiverTypes):
        import rise.scenario.Pathloss
        from rise.scenario import Shadowing
        from rise.scenario import FastFading
        transceiverPairs = transceiverTypes
        scenarios.channelmodel.SingleChannelModelCreator.__init__(
            self, transceiverPairs, rise.scenario.Pathloss.ITUUMi(), Shadowing.No(), FastFading.No())

class UrbanMacroChannelModelCreator(scenarios.channelmodel.SingleChannelModelCreator):
    
    def __init__(self, transceiverTypes):
        import rise.scenario.Pathloss
        from rise.scenario import Shadowing
        from rise.scenario import FastFading
        transceiverPairs = transceiverTypes
        scenarios.channelmodel.SingleChannelModelCreator.__init__(
                self, transceiverPairs, rise.scenario.Pathloss.ITUUMa(), Shadowing.No(), FastFading.No())

class RuralMacroChannelModelCreator(scenarios.channelmodel.SingleChannelModelCreator):
    
    def __init__(self, transceiverTypes):
        import rise.scenario.Pathloss
        from rise.scenario import Shadowing
        from rise.scenario import FastFading
        transceiverPairs = transceiverTypes
        scenarios.channelmodel.SingleChannelModelCreator.__init__(
            self, transceiverPairs, rise.scenario.Pathloss.ITURMa(), Shadowing.No(), FastFading.No())

class SuburbanMacroChannelModelCreator(scenarios.channelmodel.SingleChannelModelCreator):
    
    def __init__(self, transceiverTypes):
        import rise.scenario.Pathloss
        from rise.scenario import Shadowing
        from rise.scenario import FastFading
        transceiverPairs = transceiverTypes
        scenarios.channelmodel.SingleChannelModelCreator.__init__(
            self, transceiverPairs, rise.scenario.Pathloss.ITUSMa(), Shadowing.No(), FastFading.No())

