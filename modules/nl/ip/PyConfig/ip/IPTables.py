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

import openwns
import openwns.pyconfig
from Logger import Logger

class Chain(object):

    logger = None

    rules = None

    def __init__(self, name, parentLogger=None):
        self.logger = Logger(name, True, parentLogger)
        self.rules = []

    def addRule(self, filter, target, ruleTag):
        filter.setLogger(self.logger)
        target.setLogger(self.logger)
        self.rules.append(Rule(filter, target, ruleTag))

class OutputChain(Chain):
    __plugin__ = "ip.iptables.outputchain"

    def __init__(self, parentLogger=None):
        Chain.__init__(self, "OutputChain", parentLogger)

class InputChain(Chain):
    __plugin__ = "ip.iptables.inputchain"

    def __init__(self, parentLogger=None):
        Chain.__init__(self, "InputChain", parentLogger)

class ForwardChain(Chain):
    __plugin__ = "ip.iptables.forwardchain"

    def __init__(self, parentLogger=None):
        Chain.__init__(self, "ForwardChain", parentLogger)

class PreroutingChain(Chain):
    __plugin__ = "ip.iptables.preroutingchain"

    def __init__(self, parentLogger=None) :
        Chain.__init__(self, "PreroutingChain", parentLogger)

class PostroutingChain(Chain):
    __plugin__ = "ip.iptables.postroutingchain"

    def __init__(self, parentLogger=None):
        Chain.__init__(self, "PostroutingChain", parentLogger)

class Rule:

    def __init__(self, filter, target, ruleTag):
        self.filter = filter
        self.target = target
        self.ruleTag = ruleTag

class SourceDestinationFilter:

    __plugin__ = "sourceDestinationFilter"

    def __init__(self, source, sourceMask, destination, destinationMask):
        self.source = source
        self.sourceMask = sourceMask
        self.destination = destination
        self.destinationMask = destinationMask

    def setLogger(self, parentlogger):
        pass

class AcceptsAllFilter:

    __plugin__ = "acceptsAllFilter"

    def __init__(self):
        pass

    def setLogger(self, parentLogger):
        self.logger = Logger("AcceptsAllFilter", True, parentLogger)


class LoggingTarget:

    __plugin__ = "loggingTarget"

    def __init__(self):
        pass

    def setLogger(self, parentLogger=None):
        self.logger = Logger("LoggingTarget", True, parentLogger)

class DLLFlowIDTarget:

    __plugin__ = "dllFlowIDTarget"

    def __init__(self):
        pass

    def setLogger(self, parentLogger=None):
        self.logger = Logger("DLLFlowIDTarget", True, parentLogger)
