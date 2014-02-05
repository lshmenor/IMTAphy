###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2007
# Chair of Communication Networks (ComNets)
# Kopernikusstr. 16, D-52074 Aachen, Germany
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
##############################################################################

from wnsbase.Interface import Interface, abstractmethod

class RCS(Interface):

    @abstractmethod
    def setPath(self, path):
        pass

    @abstractmethod
    def getPath(self):
        pass

    @abstractmethod
    def missing(self, url, switches={}, revision=""):
        pass

    @abstractmethod
    def status(self, switches={}):
        pass

    @abstractmethod
    def lint(self):
        pass

    @abstractmethod
    def update(self, fromRepository=""):
        pass

    @abstractmethod
    def merge(self, fromRepository=""):
        pass

    @abstractmethod
    def get(self, url):
        pass

    @abstractmethod
    def push(self, url, createPrefix = False):
        pass

    @abstractmethod
    def getFQRN(self):
        pass

    @abstractmethod
    def getTreeVersion(self):
        pass

    @abstractmethod
    def getVersion(self):
        pass

    @abstractmethod
    def getCategory(self):
        pass

    @abstractmethod
    def getBranch(self):
        pass

    @abstractmethod
    def getRevision(self):
        pass

    @abstractmethod
    def getPatchLevel(self):
        pass

    @abstractmethod
    def isPinned(self):
        pass

    @abstractmethod
    def getPinnedPatchLevel(self):
        pass
