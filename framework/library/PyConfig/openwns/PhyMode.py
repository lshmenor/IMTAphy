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
from openwns.interval import Interval

unitTestSymbolDuration = 1.389e-05
unitTestSubCarriersPerSubChannel = 192

class PhyModeDropin1:
    nameInPhyModeFactory = "test.PhyMode.PhyMode"
    modulation = "BPSK"
    coding = "Testing-3/4"
    symbolDuration = unitTestSymbolDuration
    subCarriersPerSubChannel = unitTestSubCarriersPerSubChannel
    snr2miMapping = None # mapping class object
    mi2perMapper  = None # mapping class object

class PhyModeDropin2:
    nameInPhyModeFactory = "test.PhyMode.PhyMode"
    modulation = "QAM16"
    coding = "Testing-3/4"
    symbolDuration = unitTestSymbolDuration
    subCarriersPerSubChannel = unitTestSubCarriersPerSubChannel
    snr2miMapping = None # mapping class object
    mi2perMapper  = None # mapping class object

class PhyModeDropin3:
    nameInPhyModeFactory = "test.PhyMode.PhyMode"
    modulation = "QAM64"
    coding = "Testing-3/4"
    symbolDuration = unitTestSymbolDuration
    subCarriersPerSubChannel = unitTestSubCarriersPerSubChannel
    snr2miMapping = None # mapping class object
    mi2perMapper  = None # mapping class object

# copied and stripped down from rise*/addOn/PyConfig/rise/PhyMode.py
class PhyModeMapperDropin:
    nameInPhyModeMapperFactory = "test.plmapping.PhyModeMapper"

    mapEntries = None
    symbolDuration = None
    subCarriersPerSubChannel = None
    minimumSINR = None # below this SINR there will be no useful transmission (PER too high)
    # ^ in practice this can depend on the codeWordLength
    snr2miMapper = None # mapper objects
    mi2perMapper = None # mapper objects
    dynamicTable = False # True: calculate PhyModes=f(bl,PER)

    class MapEntry:
        sinrInterval = None
        phyMode = None

    def __init__(self, **kw):
        self.mapEntries = []
        self.symbolDuration = unitTestSymbolDuration
        self.subCarriersPerSubChannel = unitTestSubCarriersPerSubChannel
        # the next two mappers can be overwritten by extra arguments of the constructor:
        #self.snr2miMapper = rise.SNR2MI.default # The default is currently specified there
        #self.mi2perMapper = rise.CoderSpecification.defaultCoderMapping # The default is currently specified there
        attrsetter(self, kw)

    def setMinimumSINR(self, sinr):
        self.minimumSINR = sinr

    def addPhyMode(self, sinrInterval, phyMode):
        # items must be added in order of ascending sinr ranges
        tmp = self.MapEntry()
        tmp.sinrInterval = sinrInterval
        tmp.phyMode = phyMode
        tmp.phyMode.symbolDuration = self.symbolDuration
        tmp.phyMode.subCarriersPerSubChannel = self.subCarriersPerSubChannel
        # ensure that the PhyMode class uses the same mappers:
        #tmp.phyMode.snr2miMapping = self.snr2miMapper
        #tmp.phyMode.coderMapping = self.mi2perMapper
        self.mapEntries.append(tmp)
