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

from wnsbase.playground.Tools import *
import sys

import wnsbase.playground.Core
core = wnsbase.playground.Core.getCore()

class LintCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog lint\n\n"
        rationale = "Check for inconsistencies in version control."

        usage += rationale
        usage += """

lint gives you an overview of the health of your source tree. It shows you files
that are not managed by your version control systems. Names that violate naming
conventions or unknown files. This command is very usefull before committing you
changes somewhere. You should always use this before committing, to see that you
did not miss anything.
"""
        wnsbase.playground.plugins.Command.Command.__init__(self, "lint", rationale, usage)

    def run(self):
        def lint(project):
            return linter(project)

        def linter(project):
            sys.stdout.write ("Linting" + project.getDir() + " ... ")
            result = str(project.getRCS().lint())
            if result == "":
                sys.stdout.write(" OK\n")
            else:
                sys.stdout.write(" Fail\n")
            return result

        print "Linting all project trees. A summary will be listed at the end ..."
        lintedResults = core.foreachProject(lint)
        print
        print
        for ii in lintedResults:
            if ii.result != "":
                print "Lints in " + ii.dirname + ":"
                print ii.result
                print
                print
