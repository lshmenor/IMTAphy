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
##############################################################################

import os
import commands
import shutil
import sys

def searchPathToSDK(path):
    rootSign = ".thisIsTheRootOfWNS"
    while rootSign not in os.listdir(path):
        if path == os.sep:
            # arrived in root dir
            return None
        path, tail = os.path.split(path)
    return os.path.abspath(path)

def installEnvironment():
    if not os.path.exists("sandbox"):
        os.mkdir("sandbox")

    # Create ~/.wns if the users got a home
    if os.path.exists(os.environ["HOME"]):
        if not os.path.exists(os.path.join(os.environ["HOME"], ".wns")):
            os.mkdir(os.path.join(os.environ["HOME"], ".wns"))

def installWNSBase(sandboxDir):
    """ Installs wnsbase package to the sandbox.

    openWNS-sdk provides the python package wnsbase. This includes python classes that
    are available for all modules within your openWNS-sdk. Everytime you invoke playground.py
    openWNS-sdk/wnsbase is installed openWNS-sdk/sandbox/default/lib/python2.4/site-packages/wnsbase.
    """

    # Install all files in wnsbase to sandbox

    sandboxSrcSubDir = os.path.join("default", "lib", "python2.4", "site-packages", "wnsbase")
    if commands.getstatusoutput("rm -rf " + str(os.path.join(sandboxDir,sandboxSrcSubDir)))[0] != 0:
        print "\nERROR:"
        print "Unable to remove path for wnsbase Python module in sandbox"

    if commands.getstatusoutput("mkdir -p " + str(os.path.join(sandboxDir, sandboxSrcSubDir)))[0] != 0:
        print "\nERROR:"
        print "Unable to create path for wnsbase Python module in sandbox"

    if commands.getstatusoutput("cp -R wnsbase/* " + str(os.path.join(sandboxDir,sandboxSrcSubDir)))[0] != 0:
        print "\nERROR:"
        print "Unable to install wnsbase files to sandbox"

if __name__ == "__main__":

    pathToSDK = searchPathToSDK(os.path.abspath(os.path.dirname(sys.argv[0])))

    if pathToSDK == None:
        print "Error! You are note within an openWNS-SDK. Giving up"
        exit(1)

    installEnvironment()

    installWNSBase(os.path.join(pathToSDK, "sandbox"))

    sys.path.append(os.path.join(pathToSDK, "sandbox", "default", "lib", "python2.4", "site-packages"))

    import wnsbase.playground.Core

    core = wnsbase.playground.Core.getCore()

    pywnsPluginsPath = os.path.join("sandbox", "default", "lib", "python2.4", "site-packages", "pywns", "playgroundPlugins")

    core.setPathToSDK(pathToSDK)

    core.addPluginPath(pywnsPluginsPath)

    core.addPluginPath(os.path.join(os.environ["HOME"], ".wns", "playgroundPlugins"))

    core.startup()

    core.run()

    core.shutdown(0)
