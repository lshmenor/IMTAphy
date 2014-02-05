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

class Interval:
    def __init__(self, lowerBound, upperBound, intervalType = "[]"):
        self.lowerBound = lowerBound
        self.upperBound = upperBound
        self.intervalType = intervalType
    
    def __contains__(self, value):
        if(value < self.lowerBound):
            return False
        if(value == self.lowerBound and self.intervalType[0] == '('):
            return False
        if(value > self.upperBound):
            return False
        if(value == self.upperBound and self.intervalType[1] == ')'):
            return False

        return True
    lowerBound = None
    upperBound = None
    intervalType = None
