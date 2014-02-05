#! /usr/bin/env python
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
###############################################################################
import os
import commands

class Project(object):

    def __init__ (self,
                  directory,
                  rcsSubDir,
                  rcsBaseUrl,
                  rcs,
                  executable,
                  generateDoc = True,
                  alias = None):


        self.directory = os.path.abspath(directory)

        self.rcsSubDir = rcsSubDir

        self.rcsBaseUrl = rcsBaseUrl

        self.rcsUrl = rcsBaseUrl.rstrip("/") + "/" + rcsSubDir

        self.rcs = rcs

        self.executable = executable

        self.alias = alias

        self.generateDoc = generateDoc

        self.addOns = []

        self.includeBaseName = None

    def getFQRN(self):
        return self.rcs.getFQRN()

    def getDir(self):
        return self.directory

    def getRCS(self):
        return self.rcs

    def getExe(self):
        return self.executable

    def getRCSBaseUrl(self):
        return self.rcsBaseUrl

    def getRCSSubDir(self):
        return self.rcsSubDir

    def getRCSUrl(self):
        return self.rcsUrl

    def registerAddOn(self, addonProject):
        self.addOns.append(addonProject)

    def hasAddOns(self):
        return (len(self.addOns) > 0)

    def getAddOns(self):
        return self.addOns

class Library(Project):
    def __init__(self, directory, rcsSubDir, rcsBaseUrl, rcs, includeBaseName = None):
        super(Library, self).__init__(directory = directory,
                                      rcsSubDir = rcsSubDir,
                                      rcsBaseUrl = rcsBaseUrl,
                                      rcs = rcs,
                                      executable = 'lib')
        self.includeBaseName = includeBaseName
        
class AddOn(Library):
    def __init__(self, baseProject, rcsSubDir, rcsBaseUrl, rcs, addOnDir = "addOn"):
        super(AddOn, self).__init__(os.path.join(baseProject.getDir(), addOnDir),
                                    rcsSubDir = rcsSubDir,
                                    rcsBaseUrl = rcsBaseUrl,
				    rcs = rcs)
        # handled by the "master" project
        self.executable = None
        # handled by the "master" project
        self.generateDoc = False
        self.rcs.setPath(self.getDir())

        baseProject.registerAddOn(self)
        self.includeBaseName = baseProject.includeBaseName

class Binary(Project):
    def __init__(self, directory, rcsSubDir, rcsBaseUrl, rcs):
        super(Binary, self).__init__(directory = directory,
                                     rcsSubDir = rcsSubDir,
                                     rcsBaseUrl = rcsBaseUrl,
                                     rcs = rcs,
                                     executable = 'bin')

class Python(Project):
    def __init__(self, directory, rcsSubDir, rcsBaseUrl, rcs, generateDoc = True, alias=None):
        super(Python, self).__init__(directory = directory,
                                     rcsSubDir = rcsSubDir,
                                     rcsBaseUrl = rcsBaseUrl,
                                     rcs = rcs,
                                     alias = alias,
                                     executable = 'python',
                                     generateDoc = generateDoc)

class SystemTest(Project):
    def __init__(self, directory, rcsSubDir, rcsBaseUrl, rcs):
        super(SystemTest, self).__init__(directory = directory,
                                         rcsSubDir = rcsSubDir,
                                         rcsBaseUrl = rcsBaseUrl,
                                         rcs = rcs,
                                         executable = None,
                                         generateDoc = False)

class Generic(Project):
    def __init__(self, directory, rcsSubDir, rcsBaseUrl, rcs, alias = None):
        super(Generic, self).__init__(directory = directory,
                                      rcsSubDir = rcsSubDir,
                                      rcsBaseUrl = rcsBaseUrl,
                                      rcs = rcs,
                                      alias = alias,
                                      executable = None,
                                      generateDoc = False)

class Documentation(Generic):
    def __init__(self, directory, rcsSubDir, rcsBaseUrl, rcs, alias = None):
        super(Documentation, self).__init__(directory = directory,
                                      rcsSubDir = rcsSubDir,
                                      rcsBaseUrl = rcsBaseUrl,
                                      rcs = rcs,
                                      alias = alias)
        self.generateDoc = True

class MasterDocumentation(Documentation):
    def __init__(self, directory, rcsSubDir, rcsBaseUrl, rcs, alias = None):
        super(MasterDocumentation, self).__init__(directory = directory,
                                                  rcsSubDir = rcsSubDir,
                                                  rcsBaseUrl = rcsBaseUrl,
                                                  rcs = rcs,
                                                  alias = alias)
        self.generateDoc = True

class Root(Generic):
    pass
