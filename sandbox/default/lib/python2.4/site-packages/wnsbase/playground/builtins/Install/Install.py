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

import sys
import os
from wnsbase.playground.Tools import *

import subprocess

import wnsbase.playground.Core
core = wnsbase.playground.Core.getCore()

class InstallCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog install\n\n"
        rationale = "Compile and install all projects to the sandbox"

        usage += rationale
        usage += """This command starts the build process and installs all files
to the sandbox. Use --flavour=FLAVOUR to select a build flavour. Choose one of the following:

dbg (default) : Build with debug symbols
opt           : Build optimized version without debug symbols and assures
optassuremsg  : like opt, but with all logging and assures enabled
"""
        wnsbase.playground.plugins.Command.Command.__init__(self, "install", rationale, usage)

        self.optParser.add_option("-j", "--jobs",
                                  type = "int", dest = "jobs", default = os.sysconf('SC_NPROCESSORS_ONLN'),
                                  help = "use JOBS parallel compilation jobs", metavar = "JOBS")

        self.optParser.add_option("", "--flavour",
                                  type="string", dest = "flavour", metavar = "TYPE", default = "dbg",
                                  help = "choose a flavour (TYPE=[dbg|opt|optassuremsg]) to operate with.")

        self.optParser.add_option("", "--static",
                                  dest = "static", default = False,
                                  action = "store_true",
                                  help = "build static executable")

        self.optParser.add_option("", "--arch32",
                                  dest = "arch32", default = False,
                                  action = "store_true",
                                  help = "build 32bit executable")

        self.optParser.add_option("", "--scons",
                                  dest = "scons", default = "",
                                  help="options forwarded to scons.(e.g. --scons=callgrind=True)")

        self.optParser.add_option("", "--sandboxDir",
                                  type = "string", dest = "sandboxDir", metavar = "DIRECTORY", default = "",
                                  help = "Choose directory to store the sandbox.")


    def run(self):
        sconsOptions = self.options.flavour
        sconsOptions += " --warn=no-missing-sconscript"
        if self.options.jobs != None:
            sconsOptions += ' -j ' + str(self.options.jobs)
        if self.options.sandboxDir != '':
            sconsOptions += ' sandboxDir=' + str(self.options.sandboxDir)
            defaultInstalls = os.path.join(self.options.sandboxDir, 'default')
            sconsOptions += ' ' + defaultInstalls
        else:
            sconsOptions += ' default'
        if self.options.scons != '':
            sconsOptions += ' ' + self.options.scons
        if self.options.static:
            sconsOptions += ' static=True'
        if self.options.arch32:
            sconsOptions += ' arch32=True'

        command = 'scons ' + sconsOptions
        print 'Executing: ', command 

	p = subprocess.Popen(command, shell=True, stdin=subprocess.PIPE, close_fds=True)

        p.wait()

        if p.returncode != 0:
            sys.exit(p.returncode)


    def runProjectHook(self, project, hookName):
        if not hasattr(project, hookName):
            return True

        hook = getattr(project, hookName)
        if not callable(hook):
            return True

        print "Running '%s' hook for project %s" % (hookName, project.getDir())
        return hook()

