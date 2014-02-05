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

"""CRC implementation

   the class CRC configures the CRC implementation, which performs a random
   experiment. For that, it has to rely on the help of a friend, the PERProvider,
   which MUST be a Functional Unit that has the PERProviderInterface feature added
   to its Command so that the CRC can determine the Packet Error Ratio (PER) of the
   Packet it is currently inspecting.
"""
from pyconfig import attrsetter
from logger import Logger
import openwns.FUN

class CRC(openwns.FUN.FunctionalUnit):
    """This class can emulate the cyclic redundancy check (CRC)

    self.CRCsize:     Checksum size in Bits
    self.PERProvider: Name of the friend that provides the PER
    self.isDropping:  boolean select between DROPPING or MARKING behaviour
    """
    __plugin__ = "wns.crc.CRC"
    name = "CRC"

    CRCsize = 16
    PERProvider = "none"
    isDropping = True
    lossRatioProbeName = "NoNameSpecified"

    useSuspendProbe = False
    suspendProbeName = "timeBufferEmpty"

    logger = None
    localIDs = None

    def setLocalIDs(self, localIDs):
        self.localIDs = localIDs

    def addLocalIDs(self, localIDs):
        self.localIDs.update(localIDs)

    def __init__(self, PERProvider, functionalUnitName = None, commandName = None, parentLogger = None, **kw):
        super(CRC,self).__init__(functionalUnitName = functionalUnitName, commandName = commandName )
        self.PERProvider = PERProvider
        self.logger = Logger('WNS','CRC',True,parentLogger)
        self.localIDs = {}
        attrsetter(self, kw)

class CRCFilter(openwns.FUN.FunctionalUnit):
    __plugin__ = "wns.crc.CRCFilter"
    name = "CRCFilter"

    crc = None
    """Name of the CRC FU whose command is used to decide whether the
    transmission of the compound was successful or failed"""

    fus = None
    """Compounds that contain at least one activated command
    of one of these FUs won't be filtered out. fus is a list
    containing the names of these FUs."""

    useSuspendProbe = False
    suspendProbeName = "timeBufferEmpty"

    logger = None

    def __init__(self, crc, fus, parentLogger = None, **kw):
        super(CRCFilter,self).__init__()
        self.crc = crc
        self.fus = fus
        self.logger = Logger('WNS', 'CRCFilter', True, parentLogger)
        attrsetter(self, kw)
