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

import openwns.FUN
import openwns.logger
from openwns.pyconfig import attrsetter

class CompoundSwitch(openwns.FUN.FunctionalUnit):
    __plugin__="dll.compoundSwitch.CompoundSwitch"

    logger = None
    onDataFilters = None
    sendDataFilters = None
    mustAccept = None

    def __init__(self, functionalUnitName = 'compoundSwitch', commandName = 'compoundSwitch', logName = 'CompoundSwitch', moduleName = 'DLL', parentLogger = None, **kw):
        super(CompoundSwitch, self).__init__(functionalUnitName = functionalUnitName, commandName = commandName)
        self.logger =  openwns.logger.Logger(moduleName, logName, True, parentLogger)
        self.onDataFilters = []
        self.sendDataFilters = []
        self.mustAccept = True
        attrsetter(self, kw)

    def connectOnDataFU(self, fu, filter):
        self.onDataFilters.append(filter)
        fu.connect(self)

    def connectSendDataFU(self, fu, filter):
        self.sendDataFilters.append(filter)
        self.connect(fu)


class Filter(object):
    __plugin__="dll.compoundSwitch.Filter"
    name = None
    def __init__(self, name):
        self.name = name

######### filter implementation ###################
class FilterAll(Filter):
    __plugin__="dll.compoundSwitch.filter.FilterAll"

    def __init__(self,name,**kw):
        super(FilterAll, self).__init__(name)
        attrsetter(self, kw)


class FilterNone(Filter):
    __plugin__="dll.compoundSwitch.filter.FilterNone"

    def __init__(self,name,**kw):
        super(FilterNone, self).__init__(name)
        attrsetter(self, kw)


class FilterName(Filter):
    __plugin__="dll.compoundSwitch.filter.FilterName"

    def __init__(self,name,**kw):
        super(FilterFilterName, self).__init__(name)
        attrsetter(self, kw)


class FilterCommand(Filter):
    __plugin__="dll.compoundSwitch.filter.FilterCommand"
    commandProvider = None

    def __init__(self, name, commandProvider, **kw):
        super(FilterCommand, self).__init__(name)
        self.commandProvider = commandProvider
        attrsetter(self, kw)

class FilterMACAddress(Filter):
    __plugin__ = "dll.compoundSwitch.filter.FilterMACAddress"

    """ Allows for filtering by the MAC address, which was set by the upperConvergence"""

    upperConvergenceCommandName = None
    """ The name of the upperConvergence Command, 'upperConvergence' by default"""

    filterMACAddress = None
    """ The MAC addres the filter should match on """

    filterTarget = None
    """ If true, filters the target of the compound; if false, filters for the source"""

    def __init__(self, name, filterMACAddress, filterTarget, upperConvergenceCommandName, **kw):
        super(FilterMACAddress, self).__init__(name)
        self.upperConvergenceCommandName = upperConvergenceCommandName
        self.filterMACAddress = filterMACAddress
        self.filterTarget = filterTarget
        attrsetter(self, kw)
