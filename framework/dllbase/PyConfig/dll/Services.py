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

import openwns.logger

class Service(object):
    nameInServiceFactory = None
    serviceName = None

class Association(Service):
    logger = None

    def __init__(self, name = '', parent = None):
        self.nameInServiceFactory = 'dll.services.control.Association'
        self.serviceName = 'ASSOCIATION'+name
        self.logger = openwns.logger.Logger("DLL", self.serviceName, True, parent)

class InterferenceCache(Service):
    nameInServiceFactory = 'dll.services.management.InterferenceCache'

    alphaLocal = None
    alphaRemote= None
    notFoundStrategy = None
    logger = None

    def __init__(self, serviceName, alphaLocal, alphaRemote, esm, parent = None):
        self.serviceName = serviceName
        self.alphaLocal = alphaLocal
        self.alphaRemote = alphaRemote
        self.logger = openwns.logger.Logger("DLL", self.serviceName, True, parent)
        self.notFoundStrategy = ConstantValue()
        self.esmFunc = esm

class ESMFunc(object):
    __plugin__ = None

    def __init__(self, phyModeMapper):
        self.phyModeMapper = phyModeMapper

class ESMStub(ESMFunc):
    __plugin__ = 'dll.services.management.InterferenceCache.ESMStub'

class LinearESM(ESMFunc):
    __plugin__ = 'dll.services.management.InterferenceCache.LinearESM'

class LogESM(ESMFunc):
    __plugin__ = 'dll.services.management.InterferenceCache.LogESM'

class MIESM(ESMFunc):
    __plugin__ = 'dll.services.management.InterferenceCache.MIESM'

    def __init__(self, phyModeMapper):
        ESMFunc.__init__(self, phyModeMapper)

class ConstantValue(object):
    __plugin__ = 'dll.services.management.InterferenceCache.ConstantValue'
    
    averageCarrier = None
    averageInterference = None
    deviationCarrier = None
    deviationInterference = None
    averagePathloss = None

class Complain(object):
    __plugin__ = 'dll.services.management.InterferenceCache.Complain'
    

class InterferenceCacheDropin(InterferenceCache):
    def __init__(self):
        super(InterferenceCacheDropin,self).__init__(serviceName = "INTERFERENCECACHE",
                                                     alphaLocal  = 0.2,
                                                     alphaRemote = 0.05,
                                                     esm = ESMStub(None))
        self.notFoundStrategy = Complain()

