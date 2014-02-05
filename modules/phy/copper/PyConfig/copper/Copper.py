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

from openwns.module import Module
from openwns.pyconfig import Sealed, Frozen, attrsetter
import openwns.node
import openwns.logger


class Logger(openwns.logger.Logger):
    def __init__(self, name, enabled, parent = None, **kw):
        super(Logger, self).__init__("COPPER", name, enabled, parent, **kw)


class Wire(Sealed):
    name = None
    logger = None

    def __init__(self, name):
        super(Wire, self).__init__()
        self.name = name
        self.logger = Logger(name, True)


class Transceiver(openwns.node.Component):
    nameInComponentFactory = 'copper.Transceiver'

    wire = None
    transmitter = None
    receiver = None
    dataTransmission = None
    dataTransmissionFeedback = None
    notification = None

    def __init__(self, node, name, wire, ber, dataRate, sensingTime = 0.0):
        super(Transceiver, self).__init__(node, name)
        self.wire = wire
        self.transmitter = Transmitter(dataRate, sensingTime, self.logger)
        self.receiver = Receiver(ber, sensingTime, self.logger)
        self.dataTransmission = name + '.dataTransmission'
        self.dataTransmissionFeedback = name + '.dataTransmissionFeedback'
        self.notification = name + '.notification'


class Copper(Module):

    def __init__(self):
        super(Copper, self).__init__("copper", "copper")


class Transmitter(Sealed):
    logger = None
    dataRate = None # in Bit/s
    sensingTime = None # in s

    def __init__(self, dataRate, sensingTime, parentLogger):
        super(Transmitter, self).__init__()
        self.logger = Logger("Transmitter", True, parentLogger)
        self.dataRate = dataRate
        self.sensingTime = sensingTime

        assert(self.dataRate >= 0.0)
        assert(self.sensingTime >= 0.0)


class Receiver(Sealed):
    logger = None
    ber = None
    sensingTime = None

    def __init__(self, ber, sensingTime, parentLogger):
        super(Receiver, self).__init__()
        self.logger = Logger("Receiver", True, parentLogger)
        self.ber = ber
        self.sensingTime = sensingTime
