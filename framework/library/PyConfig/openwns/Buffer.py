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
import openwns.FUN

class Buffer(openwns.FUN.FunctionalUnit):
    size = 1
    sizeUnit = 'PDU'
    lossRatioProbeName = 'NoNameSpecified'
    droppedPDUWindowDuration = 0.20 # seconds
    probeDroppedPDUInterval  = 0.10 # seconds
    sizeProbeName = 'NoNameSpecified'
    logger = None
    probingEnabled = True
    localIDs = None

    def setLocalIDs(self, localIDs):
        self.localIDs = localIDs

    def addLocalIDs(self, localIDs):
        self.localIDs.update(localIDs)

    def __init__(self, parentLogger = None, **kw):
        super(Buffer, self).__init__()
        self.localIDs = {}
        self.logger = openwns.logger.Logger("WNS", "Buffer", True, parentLogger)
        attrsetter(self, kw)

class Bounded(Buffer):
    __plugin__ = 'wns.buffer.Bounded'
    name = "Bounded"
    def __init__(self, **kw):
        super(Bounded, self).__init__(**kw)

class Dropping(Buffer):
    __plugin__ = 'wns.buffer.Dropping'
    name = "Dropping"

    drop = 'Tail'
    def __init__(self, **kw):
        super(Dropping, self).__init__(**kw)


