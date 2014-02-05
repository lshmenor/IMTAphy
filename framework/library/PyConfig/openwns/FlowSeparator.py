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

from openwns.logger import Logger
from openwns.pyconfig import attrsetter
import openwns.FUN


# creator strategies

class Prototype(object):
    __plugin__ = None

    name = None
    fuName = None
    prototypeConfig = None

    def __init__(self, name, prototypeConfig):
        self.__plugin__ = 'prototypecreator'
        if name != None:
            self.name = name
            self.fuName = name
        else:
            self.name = prototypeConfig.commandName
            self.fuName = prototypeConfig.functionalUnitName
            
        self.prototypeConfig = prototypeConfig

class Config(Prototype):
    def __init__(self, name, prototypeConfig):
        Prototype.__init__(self, name, prototypeConfig)
        self.__plugin__ = 'configcreator'


# Notfound strategies

# base class
class NotFound(object):
    creator = None

    def __init__(self, creator):
        self.creator = creator

# 1a Backward compatibility
class PrototypeCreator(NotFound):
    __plugin__ = 'createonfirstcompound'
    def __init__(self, name, prototype):
        NotFound.__init__(self, Prototype(name, prototypeConfig = prototype))


class Complain(NotFound):
    __plugin__ = 'complain'

    def __init__(self):
        NotFound.__init__(self, creator = None)

class CreateOnFirstCompound(NotFound):
    __plugin__ = 'createonfirstcompound'

    def __init__(self, creator):
        NotFound.__init__(self, creator)

class CreateOnValidFlow(NotFound):
    __plugin__ = 'createonvalidflow'
    flowInfoProviderName = None
    def __init__(self, creator, fipName):
        NotFound.__init__(self, creator)
        self.flowInfoProviderName = fipName


class FlowSeparator(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.FlowSeparator'

    keyBuilder = None
    notFound = None
    logger = None

    def __init__(self, keyBuilder, notFound, logName = 'FlowSeparator', parentLogger = None, **kw):
        super(FlowSeparator,self).__init__()
        self.keyBuilder = keyBuilder
        self.notFound = notFound
        self.logger = Logger('WNS', logName, True, parentLogger)
        attrsetter(self,kw)

class OneFlow(object):
    __plugin__ = 'wns.fungenerictest.OneFlow'


class FlowGate(openwns.FUN.FunctionalUnit):
    __plugin__ = 'wns.FlowGate'

    keyBuilder = None
    logger = None

    def __init__(self, fuName, keyBuilder, commandName=None, parentLogger = None, **kw):
        super(FlowGate,self).__init__(functionalUnitName = fuName, commandName=commandName)
        self.keyBuilder = keyBuilder
        self.logger = Logger('WNS', 'FlowGate', True, parentLogger)
        attrsetter(self,kw)
