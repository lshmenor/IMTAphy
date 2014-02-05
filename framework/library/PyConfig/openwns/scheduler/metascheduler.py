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

class InitVals(object):
    def __init__(self, c = "0.0 dBm" , i = "0.0 dBm" , pl = "0.0 dB"):
        self.c = c
        self.i = i
        self.pl = pl

class MetaScheduler(object):
 
    def __init__(self, initialICacheValues = InitVals()):
        self.initialICacheValues = initialICacheValues
        #self.logger = openwns.logger.Logger("WNS", "MetaScheduler", True, parentLogger);

class NoMetaScheduler(MetaScheduler):
    plugin = "NoMetaScheduler"
    
    def __init__(self, initialICacheValues = InitVals()):
        MetaScheduler.__init__(self, initialICacheValues)

class GreedyMetaScheduler(MetaScheduler):
    plugin = "GreedyMetaScheduler"
  
    def __init__(self, initialICacheValues = InitVals()):
        MetaScheduler.__init__(self, initialICacheValues)

class MaxRegretMetaScheduler(MetaScheduler):
    plugin = "MaxRegretMetaScheduler"
  
    def __init__(self, initialICacheValues = InitVals()):
        MetaScheduler.__init__(self, initialICacheValues)
        
class HighCwithHighIMetaScheduler(MetaScheduler):
    plugin = "HighCwithHighIMetaScheduler"
  
    def __init__(self, initialICacheValues = InitVals()):
        MetaScheduler.__init__(self, initialICacheValues)