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

defaultPairs = [("AP","AP"),("AP","FRS"),("AP","UT"),
                   ("UT","UT"),("UT","FRS"),("UT","AP"),
                   ("FRS","FRS"),("FRS","AP"),("FRS","UT"),("DropIn","DropIn")]

singlePair = [("default", "default")]

APandUT = [("AP", "UT"), ("UT", "AP")]

class SingleChannelModelCreator(scenarios.interfaces.IChannelModelCreator):
    
    def __init__(self, transceiverPairs, pathloss, shadowing, fastFading):
        self.transceiverPairs = transceiverPairs
        self.pathloss = pathloss
        self.shadowing = shadowing
        self.fastFading = fastFading
        
    def create(self):
        import rise.scenario.Propagation
        channelmodelConfigurations = []
        for pair in self.transceiverPairs:
            channelmodel = rise.scenario.Propagation.Configuration(self.pathloss, self.shadowing, self.fastFading)
            channelmodelConfiguration = rise.scenario.Propagation.ChannelModelConfiguration(pair, channelmodel)
            channelmodelConfigurations.append(channelmodelConfiguration)
        return channelmodelConfigurations


class PerPairChannelModelCreator(scenarios.interfaces.IChannelModelCreator):
    
    def __init__(self, channelmodelConfigurations):
        self.channelmodelConfigurations = channelmodelConfigurations
        
    def create(self):
        return self.channelmodelConfigurations

class SingleSlopeChannelModelCreator(SingleChannelModelCreator):
    
    def __init__(self, offset, distFactor, freqFactor = 0.0):
        
        import rise.Scenario
        from rise.scenario import Shadowing
        from rise.scenario import FastFading
        import rise.scenario.Pathloss
        from openwns.interval import Interval
        transceiverPairs = scenarios.channelmodel.defaultPairs

        pathloss = rise.scenario.Pathloss.SingleSlope(
            validFrequencies = Interval(0, 1E12),
            validDistances = Interval(0, 1E6),
            offset = offset, 
            freqFactor = freqFactor,
            distFactor = distFactor, 
            distanceUnit = "m", 
            minPathloss = "0.0 dB",
            outOfMinRange = rise.scenario.Pathloss.Deny(),
            outOfMaxRange = rise.scenario.Pathloss.Deny()
            )
        scenarios.channelmodel.SingleChannelModelCreator.__init__(
                self, transceiverPairs, pathloss, Shadowing.No(), FastFading.No())

class TestChannelModelCreator(SingleSlopeChannelModelCreator):
    
    def __init__(self):
	SingleSlopeChannelModelCreator.__init__(self, "41.9 dB", "23.8 dB")

class InHNLoSChannelModelCreator(SingleSlopeChannelModelCreator):
    
    def __init__(self):
	SingleSlopeChannelModelCreator.__init__(self, "22.13 dB", "43.3 dB")

