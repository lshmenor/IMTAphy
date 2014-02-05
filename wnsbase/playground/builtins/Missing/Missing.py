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

import wnsbase.playground.Core
core = wnsbase.playground.Core.getCore()


class MissingCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog missing\n\n"
        rationale = "Check for missing patches in the source repository."

        usage += rationale
        usage += """

Missing checks if you are missing any patches in your master source code repository.
If for a project the underlying version control is GNUArch it will report any patches
that have been committed and that you do not have applied to your local source tree.

If you use Bazaar missing additionally lists the patches that you have commited locally
but have not yet pushed to the remote location. So pay attention to the output wether
patches are local or remote
"""
        wnsbase.playground.plugins.Command.Command.__init__(self, "missing", rationale, usage)

    def run(self):
        def checkMissing(project, otherProjects):
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
                    print "Executing missing on parent branch"
                    otherURL = project.getRCSUrl()
                else:
                    otherURL = otherProject.getRCSUrl()

            print "Missing in", project.getDir(), "..."
            print project.getRCS().missing(otherURL ,{"-s":""})

        core = wnsbase.playground.Core.getCore()

        core._process_hooks("_pre_missing")

        core.foreachProject(checkMissing, otherProjects = core.otherProjects)
