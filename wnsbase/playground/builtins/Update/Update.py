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
core = wnsbase.playground.Core.getCore()

class UpdateCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog update\n\n"
        rationale = "Update the project support modules."

        usage += rationale

        usage += """

Updates the SDK and rereads the project configuration
afterwards
"""
        wnsbase.playground.plugins.Command.Command.__init__(self, "update", rationale, usage)

    def run(self):
        core = wnsbase.playground.Core.getCore()
        projects = core.getProjects()

        myProject = projects.root
        if myProject.getRCS().isPinned():
            sys.stdout.write("\nSkipping module in %s, because it is pinned to %i\n\n"
                             % (myProject.getDir(), myProject.getRCS().getPinnedPatchLevel()))
            return
        sys.stdout.write("Checking for new patches in: %s ... " % ("./"))
        sys.stdout.flush()
        missing = str(myProject.getRCS().missing(myProject.getRCSUrl(), {"-s":""}))
        if(missing != ""):
            print "Found:"
            print missing
            print "\nRetrieving new patches for './' ... "
            try:
                myProject.getRCS().update(myProject.getRCSUrl()).realtimePrint()
            except wnsbase.rcs.Bazaar.BzrMergeNeededException, e:
                if (not core.userFeedback.askForReject("These branches have diverged! Do you want me to merge?")):
                    myProject.getRCS().merge(fromRepository=myProject.getRCSUrl()).realtimePrint()
        else:
            print "None"

        # Maybe projects.py has changed. Trigger reload
        core.readProjectsConfig(core.projectsFile)
