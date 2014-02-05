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
from wnsbase.playground.Tools import *

import wnsbase.playground.Core
import wnsbase.playground.plugins.Command

core = wnsbase.playground.Core.getCore()

class StatusCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "Show status in source code managed by the version control system for all known projects."
        # make 'changes' command an alias for the 'status' command
        wnsbase.playground.plugins.Command.Command.__init__(self, "status", usage, usage)

        self.addOption("", "--if",
                       type="string", dest = "if_expr", metavar = "EXPR", default = None,
                       help = "restrict commands to affect only projects that match EXPR (can be: 'python', 'bin', 'lib', 'none', 'changed', 'scons', 'ask', 'bzr', 'tla').")

        self.addOption("", "--diffs",
                       action = "store_const", dest = "diffs", const = '--diffs', default = '',
                       help=" when using --status, show status of changed files.")

    def run(self, arg = 'unused'):
        def runForEach(project):
            return self.changesChecker(project)

        print "Searching changes. A summary will be listed at the end ..."
        projectChanges = []
        projectChanges.extend(core.foreachProject(runForEach))

        print
        for ii in projectChanges:
            if len(ii.result) > 0:
                print "Changes in: " + ii.dirname
                for change in ii.result:
                    print "  " + change

    def changesChecker(self, project):
        sys.stdout.write("Checking for changes in " + project.getDir() + " ...")
        sys.stdout.flush()
        changes = []
        foundChanges = False
        for line in project.getRCS().status({self.options.diffs:""}):
            if line.startswith('*') or line.strip(" ") == "":
                continue

            if line.startswith("Format Repository"):
                continue

            changes.append(line)
            foundChanges = True

        if foundChanges:
            sys.stdout.write(" " + str(len(changes)) + " files changed\n")
        else:
            sys.stdout.write(" no changes\n")
        return changes


class ChangesCommand(StatusCommand):

    def __init__(self):
        usage = "This command is deprecated but still in use due to backward compatibility.\n       Use 'status' command instead."
        wnsbase.playground.plugins.Command.Command.__init__(self, "changes", usage, usage)

        self.addOption("", "--if",
                       type="string", dest = "if_expr", metavar = "EXPR", default = None,
                       help = "restrict commands to affect only projects that match EXPR (can be: 'python', 'bin', 'lib', 'none', 'changed', 'scons', 'ask', 'bzr', 'tla').")

        self.addOption("", "--diffs",
                       action = "store_const", dest = "diffs", const = '--diffs', default = '',
                       help=" when using --changes, show diffs of changed files.")

    def run(self, arg = 'unused'):
        print
        print "WARNING: This command is deprecated. Please use 'status' instead."
        print

        StatusCommand.run(self, arg)
