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

import openwns.pyconfig
import openwns.FUN
import openwns.logger

class Synchronizer(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.tools.Synchronizer'
    name = "Synchronizer"
    def __init__(self, **kw):
        super(Synchronizer,self).__init__(**kw)

class Stub(object):
    __plugin__ = 'wns.tools.Stub'
    name = "Stub"

class Forwarder(object):
    __plugin__ = 'wns.tools.Forwarder'
    name = "Forwarder"

    def __init__(self, *args, **kw):
        pass

class Padding(object):
    __plugin__ = 'wns.tools.Padding'
    name = "Padding"

    size = 1

    def __init__(self, **kw):
        openwns.pyconfig.attrsetter(self, kw)

class FlowControl(object):
    """
    attic code. to be removed.
    """
    __plugin__ = 'wns.tools.FlowControl'
    name = "FlowControl"

    numBitsIfTransparent = 0
    numBitsIfRNR = 0
    numBitsIfRR = 0


class Gate(object):
    __plugin__ = 'wns.tools.Gate'
    name = "Gate"

class Stutter(object):
    __plugin__ = 'wns.tools.MMMyGGGeneration'

    interval = 1.0
    flowControl = None

    def __init__(self, flowControl, **kw):
        self.flowControl = flowControl
        openwns.pyconfig.attrsetter(self, kw)

class BottleNeckDetective(object):
    __plugin__ = 'wns.ldk.tools.BottleNeckDetective'
    name = "BottleNeckDetective"

    offset = None

    observationInterval = None

    logger = None

    def __init__(self, offset, observationInterval, parentLogger):
        self.offset = offset
        self.observationInterval = observationInterval
        self.logger = openwns.logger.Logger("WNS", self.name, True, parentLogger)

class Overhead(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.ldk.tools.Overhead'

    overhead = None
    name = None
    logger = None

    def __init__(self, overhead, commandName, parentLogger = None, name = "Overhead", **kw):
        super(Overhead, self).__init__(commandName = commandName)
        self.name = name
        self.logger = openwns.logger.Logger("WNS", self.name, True, parentLogger)
        self.overhead = overhead
        openwns.pyconfig.attrsetter(self, kw)

class Consumer(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.ldk.tools.Consumer'

    incoming = None
    outgoing = None

    def __init__(self, incoming = True, outgoing = True):
        super(Consumer, self).__init__(commandName = 'Consumer')
        self.incoming = incoming
        self.outgoing = outgoing

class ConstantDelay(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.ldk.tools.ConstantDelay'

    logger = None
    delayDuration = None

    def __init__(self, delayDuration,
                 functionalUnitName = 'ConstantDelay',
                 commandName = 'ConstandDelayCommand',
                 parentLogger = None, logName = 'ConstantDelay', moduleName = 'WNS',
                 **kw):
        super(ConstantDelay, self).__init__(functionalUnitName = functionalUnitName, commandName = commandName)
        self.delayDuration = delayDuration
        self.logger = openwns.logger.Logger(moduleName, logName, True, parentLogger)

        openwns.pyconfig.attrsetter(self, kw)

class Compressor(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.ldk.tools.Compressor'

    reduction = None
    byteAlign = None
    name = None
    logger = None

    def __init__(self, reduction, byteAlign = False, 
                 commandName = 'EmptyCommand', parentLogger = None, name = "Compressor", **kw):
        super(Compressor, self).__init__(commandName = commandName)
        self.name = name
        self.logger = openwns.logger.Logger("WNS", self.name, True, parentLogger)
        self.reduction = reduction
        self.byteAlign = byteAlign
        openwns.pyconfig.attrsetter(self, kw)

class InSequenceChecker(openwns.FUN.FunctionalUnit):
    """In Sequence Checker FU"""

    __plugin__ = 'wns.ldk.tools.InSequenceChecker'
    name = "InSequenceChecker"

    logger = None
    """Logger configuration"""

    def __init__(self, enabled = True, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("WNS", "InSequenceChecker", enabled, parentLogger)
        openwns.pyconfig.attrsetter(self, kw)
