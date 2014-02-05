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
import sys
import shutil

from wnsbase.playground.Tools import *

import wnsbase.playground.Core
import wnsbase.playground.plugins.Command
core = wnsbase.playground.Core.getCore()

class ConsistencyCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog consistency\n\n"
        rationale = "Check if projects.py is consistent with file system contents."

        usage += rationale
        usage += """Use this command to check if the projects defined in projects.py are also present in the file system."""
        wnsbase.playground.plugins.Command.Command.__init__(self, "consistency", rationale, usage)

    def checkConsistency(self, project):
        # We cannot fix an addOn project separately. We will handle it with its
        # super project
        if isinstance(project, wnsbase.playground.Project.AddOn):
            return

        self.check(project)

    def check(self, project):
        """ Check if the project is consistent with its configuration in projects.py
        If not try to fix it.

        returns True if it was consistent or it was successfully fixed. Otherwise, False.
        """

	print "Checking consistency for %s" % project.getDir()

        # First check all the addOns
        if project.hasAddOns():
            print "Checking consistency of super project %s" % project.getDir()
            for addOn in project.getAddOns():
                self.check(addOn)

	parentBranchFileSystem = project.getRCS().getFQRN().rstrip("/")
	parentBranchProjectsPy = project.getRCSUrl()

	if parentBranchFileSystem != parentBranchProjectsPy:
	    print "WARNING! Inconsistency detected"
            print "  Directory     : %s" % project.getDir()
	    print "  Parent is     : %s" % parentBranchFileSystem
	    print "  but should be : %s" % parentBranchProjectsPy

            if core.userFeedback.askForConfirmation("Do you want to delete it?"):
                self.delete(project)
        else:
            return True

    def delete(self, project):
        if self.pendingChangesIn(project):
            print
            print "!!! Cannot delete project %s. There are open changed" % project.getDir()
            return False

        print "Checking for unpushed commits"
        if "extra revision" in project.getRCS().missing(project.getRCS().getFQRN().rstrip("/")):
            print
            print "!!! Cannot delete project %s. There are unpushed commits" % project.getDir()
            return False

        print "Checking if addOns would be deleted"
        if project.hasAddOns():
            # We can only delete ourself if all addOn projects are gone
            for addOn in project.getAddOns():
                if os.path.exists(addOn.getDir()):
                    print
                    print "!!! Cannot delete project %s. There are addOn directories present" % project.getDir()
                    return False

        # No uncommited changes, No unpushed commits, No addOns. It seems to be ok to delete the project
        # Cowards ask
        if core.userFeedback.askForConfirmation("Checks OK. Are you sure you want to delete?"):
            # Also remove the alias if there is one
            if project.alias is not None:
                shutil.rmtree(os.path.join(project.getDir(), "..", alias))

            shutil.rmtree(project.getDir())

            return True

        return False

    def pendingChangesIn(self, project):
        sys.stdout.write("Checking for changes in " + project.getDir() + " ...")
        sys.stdout.flush()
        changes = []
        foundChanges = False
        for line in project.getRCS().status():
            if line.startswith('*') or line.strip(" ") == "":
                continue
            print line
            changes.append(line)
            foundChanges = True

        if foundChanges:
            sys.stdout.write(" " + str(len(changes)) + " files changed\n")
            return True
        else:
            sys.stdout.write(" no changes\n")
            return False

        # Should not be reached. But better we say there are open changes
        # than have them accidently deleted.
        return True

    def run(self):
        # Check Consistency
        noAddons = [ p for p in core.getProjects().all if not isinstance(p, wnsbase.playground.Project.AddOn)]

	core.foreachProjectIn(noAddons, self.checkConsistency)
        # Fetch missing projects
        core.updateMissingProjects(core.checkForMissingProjects())
