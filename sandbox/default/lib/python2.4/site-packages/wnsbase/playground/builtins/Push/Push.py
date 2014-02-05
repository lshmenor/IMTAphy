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
import wnsbase.playground.plugins.Command

from wnsbase.playground.builtins.Update.Update import UpdateCommand

import wnsbase.playground.Core
core = wnsbase.playground.Core.getCore()

class PushCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog push\n\n"
        rationale = "Push the whole project tree and all its modules to remote location from projects.py."

        usage += rationale

        usage += """

Push uploads all projects to the URL provided in projects.py. Use --noAsk option to suppress questions.
Use together with --configFile to supply a different projects.py file containing different target locations.
Use --create-prefix option if target directory does not exist.
See ./playground --help for more information.
"""
        wnsbase.playground.plugins.Command.Command.__init__(self, "push", rationale, usage)

        self.optParser.add_option("", "--create-prefix",
                                  dest = "createPrefix", default = False,
                                  action = "store_true",
                                  help = "create new remote repository if not present")

    def run(self):
        def push(project, otherProjects):
            otherURL = None
            if otherProjects is None:
                otherURL = project.getRCSUrl()
            else:
                print "Checking for another branch"

                otherProject = None

                for p in otherProjects.all:
                    if project.getDir() == p.getDir():
                        otherProject = p

                if otherProject is None:
                    print "WARNING: The alternate projects file does not contain %s" % project.getDir()
                    print "Skipping %s" % project.getDir()
                else:
                    otherURL = otherProject.getRCSUrl()


            rcs = project.getRCS()

            if rcs.isPinned():
                sys.stdout.write("\nSkipping module in %s, because it is pinned to %s\n\n"
                                 % (project.getDir(), rcs.getPinnedPatchLevel()))
                return
            checkForConflictsAndExit(".")
            core = wnsbase.playground.Core.getCore()
            if otherURL is not None:
                warning = "Do you really want to push " + project.getDir() + " to " + otherURL
                if (core.userFeedback.askForConfirmation(warning)):
                    print "\nPushing '" + project.getDir() + " to " + otherURL + "' ..."
                    rcs.push(otherURL, self.options.createPrefix).realtimePrint()

        core._process_hooks("_pre_push")

        core.foreachProject(push, otherProjects = core.otherProjects)
