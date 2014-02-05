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

import pyconfig
import FUN
import logger

class Concatenation(FUN.FunctionalUnit):
    __plugin__ = "wns.concatenation.Concatenation"
    """ Name in FU factory """

    maxSize = None
    """ Concatenation buffer limit in Bit """
    maxEntries = None
    """ Maximum number of entries per container """

    numBitsIfConcatenated = 8
    """ Number of bits for the container header """
    numBitsPerEntry = 8
    """ Number of bits per entry in the container """
    numBitsIfNotConcatenated = 0
    """ Number of bits to mark a non-container compound """
    entryPaddingBoundary = 8
    """ Entries are padded to the next multiple of entryPaddingBoundary, set to 1 to deactivate padding """
    countPCISizeOfEntries = True
    """ count the PCI size of each entry towards the complete size """
    
    logger = None
    """ FU logger """

    def __init__(self, maxSize, maxEntries,
                 functionalUnitName = "Concatenation",
                 commandName = "Concatenation",
                 parentLogger = None,
                 logName = "Concatenation",
                 moduleName = "WNS",
                 **kw):
        super(Concatenation, self).__init__(functionalUnitName = functionalUnitName, commandName = commandName)

        self.maxSize = maxSize
        self.maxEntries = maxEntries

        self.logger = logger.Logger(moduleName, logName, True, parentLogger)
        pyconfig.attrsetter(self, kw)
