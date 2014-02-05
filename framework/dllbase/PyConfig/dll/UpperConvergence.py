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

class ConvBase(openwns.FUN.FunctionalUnit):
    logger = None

    def __init__(self, name, parent = None, **kw):
        super(ConvBase,self).__init__()
        self.logger = Logger(moduleName = "DLL",
                             name = name+"UpperConv",
                             enabled = True,
                             parent = parent)
        attrsetter(self, kw)

class No(ConvBase):
    __plugin__ = 'dll.upperconvergence.None'
    def __init__(self, parent = None, **kw):
        super(No,self).__init__("No", parent, **kw)

class UT(ConvBase):
    __plugin__ = 'dll.upperconvergence.UT'
    def __init__(self, parent = None, **kw):
        super(UT,self).__init__("UT", parent, **kw)

class AP(ConvBase):
    __plugin__ = 'dll.upperconvergence.AP'
    def __init__(self, parent = None, **kw):
        super(AP,self).__init__("AP", parent, **kw)
