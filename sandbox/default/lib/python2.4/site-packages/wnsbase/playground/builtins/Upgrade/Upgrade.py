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

class UpgradeCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog upgrade\n\n"
        rationale = "Update the whole project tree and all its modules."

        usage += rationale

        usage += """

Upgrade first uses the update command to update the project base. Afterwards
all project listed in the project configuration file will be update with new
patches from the remote repository (if any are available).
"""
        wnsbase.playground.plugins.Command.Command.__init__(self, "upgrade", rationale, usage)

    def run(self):
        core._process_hooks("_pre_upgrade")
        updateCommand = UpdateCommand()
        updateCommand.startup([])
        updateCommand.run()

        def upgrade(project, otherProjects):

            if otherProjects is None:
                otherURL = project.getRCSUrl()
            else:
                otherProject = None

                for p in otherProjects.all:
                    if project.getDir() == p.getDir():
                        otherProject = p

                if otherProject is None:
                    print "WARNING: The alternate projects file does not contain %s" % project.getDir()
                    print "Executing upgrade on parent branch"
                    otherURL = project.getRCSUrl()
                else:
                    otherURL = otherProject.getRCSUrl()

            rcs = project.getRCS()
            if rcs.isPinned():
                sys.stdout.write("\nSkipping module in %s, because it is pinned to %s\n\n"
                                 % (project.getDir(), rcs.getPinnedPatchLevel()))
                return
            sys.stdout.write("Checking for new patches in: %s ... " % (project.getDir()))
            sys.stdout.flush()
            missing = str(rcs.missing(otherURL , {"-s":""}))
            if(missing != ""):
                print "Found:"
                print missing
                checkForConflictsAndExit(".")
                print "\nRetrieving new patches for '" + project.getDir() + "' ..."
                try:
                    rcs.update(fromRepository=otherURL).realtimePrint()
                    checkForConflictsAndExit(".")
                except wnsbase.rcs.Bazaar.BzrMergeNeededException, e:
                    core = wnsbase.playground.Core.getCore()
                    if (not core.userFeedback.askForReject("These branches have diverged! Do you want me to merge?")):
                        rcs.merge(fromRepository=otherURL).realtimePrint()
            else:
                print "None"

        core.foreachProject(upgrade, otherProjects = core.otherProjects)
