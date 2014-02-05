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

from pyconfig import attrsetter
import openwns.logger
import openwns.FUN

class OpcodeProvider(object):
    __plugin__ = 'wns.multiplexer.OpcodeProvider'

    opcodeSize = None

    def __init__(self, opcodeSize, **kw):
        self.opcodeSize = opcodeSize
        attrsetter(self, kw)


class OpcodeSetter(object):
    __plugin__ = 'wns.multiplexer.OpcodeSetter'

    opcode = None
    opcodeProvider = None

    def __init__(self, opcode, opcodeProvider, **kw):
        self.opcode = opcode
        self.opcodeProvider = opcodeProvider
        attrsetter(self, kw)


class Dispatcher(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.multiplexer.Dispatcher'

    opcodeSize = None
    logger = None
    opcodeLogger = None

    def __init__(self, opcodeSize, parentLogger=None, logName = "Dispatcher", moduleName = "WNS", **kw):
        super(Dispatcher,self).__init__()
        assert type(opcodeSize) == type(1)

        self.opcodeSize = opcodeSize
        self.logger = openwns.logger.Logger(moduleName, logName, True, parentLogger)
        self.opcodeLogger = openwns.logger.Logger(moduleName, logName, True, self.logger)
        attrsetter(self, kw)


class FrameDispatcher(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.multiplexer.FrameDispatcher'

    opcodeSize = None
    logger = None
    opcodeLogger = None

    def __init__(self, opcodeSize, **kw):
        super(FrameDispatcher,self).__init__()
        self.opcodeSize = opcodeSize
        self.logger = openwns.logger.Logger("WNS", "FrameDispatcher", True)
        self.opcodeLogger = openwns.logger.Logger("WNS", "OpcodeSetter", True, self.logger)
        attrsetter(self, kw)
