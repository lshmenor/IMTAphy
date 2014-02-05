###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2009
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

from openwns.pyconfig import Sealed
from openwns.pyconfig import attrsetter
import openwns.logger

class ARQFSM(Sealed):
    """Base class for all ARQ FSMs

    ARQFSM.resendTimeout: time after which the ARQ should try to resend a
    PDU if no ACK was received
    """
    resendTimeout = 0.1

    useSuspendProbe = False
    suspendProbeName = "timeBufferEmpty"

    logger = None

    def __init__(self, **kw):
        attrsetter(self, kw)

class StopAndWait(ARQFSM):
    __plugin__ = 'glue.arqfsm.stopandwait.FSMFU'
    name = "StopAndWait"

    bitsPerIFrame = 2
    bitsPerRRFrame = 2

    def __init__(self, parent = None, **kw):
        self.logger = openwns.logger.Logger("GLUE", "StopAndWait", True, parent)
        attrsetter(self, kw)

class SelectiveRepeat(ARQFSM):
    __plugin__ = 'glue.arqfsm.selectiverepeat.FSMFU'
    name = "SelectiveRepeat"

    bitsPerIFrame = 10
    bitsPerACKFrame = 10

    windowSize = 512
    sequenceNumberSize = 1024

    def __init__(self, parent = None, **kw):
        self.logger = openwns.logger.Logger("GLUE", "SelectiveRepeat", True, parent)
        attrsetter(self, kw)
