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

import unittest
from wnsbase.rcs.Bazaar import Bazaar

import commands
import os

class BazaarTests(unittest.TestCase):

    def setUp(self):
        self.pid = os.getpid()
        c = ["rm -rf /tmp/__%s_bazaarTests" % str(self.pid),
             "mkdir /tmp/__%s_bazaarTests" % str(self.pid),
             "mkdir /tmp/__%s_bazaarTests/master" % str(self.pid),
             "mkdir /tmp/__%s_bazaarTests/testplayground" % str(self.pid),
             "cd /tmp/__%s_bazaarTests/master;bzr init ." % str(self.pid),
             "touch /tmp/__%s_bazaarTests/master/AFile" % str(self.pid),
             "cd /tmp/__%s_bazaarTests/master;bzr add AFile" % str(self.pid),
             "cd /tmp/__%s_bazaarTests/master;bzr commit -m 'Inital Version' AFile" % str(self.pid),
             ]
        self.runCommands(c)
        self.originalPath = os.getcwd()
        os.chdir("/tmp/__%s_bazaarTests/testplayground" % str(self.pid))

    def tearDown(self):
        os.chdir(self.originalPath)
        c = ["rm -rf /tmp/__%s_bazaarTests" % str(self.pid)]
        self.runCommands(c)

    def testSetGetPath(self):
        bzr = Bazaar("", "testcategory", "testbranch", "testrevision")
        bzr.setPath("./theBranch")
        self.failUnless(bzr.getPath() == os.path.abspath("./theBranch"),
                        "Path was not correctly set in Bazaar Object")

    def testGetFQRN(self):
        bzr = Bazaar("./theBranch", "testcategory", "testbranch", "testrevision")
        bzr.get("../master")
        fqrn = bzr.getFQRN()
        self.failUnless(fqrn == "file:///tmp/__%s_bazaarTests/master/" % str(self.pid), "FQRN is not correct. Was %s" % fqrn)

    def testGetTreeVersion(self):
        bzr = Bazaar("./theBranch", "testcategory", "testbranch", "testrevision")
        bzr.get("../master")
        treeVersion = bzr.getTreeVersion()
        self.failUnless(treeVersion == "file:///tmp/__%s_bazaarTests/master/" % str(self.pid), "TreeVersion is not correct. Was %s" % treeVersion)

    def testGetVersion(self):
        bzr = Bazaar("./theBranch", "testcategory", "testbranch", "testrevision")
        version = bzr.getVersion()
        self.failUnless(version == "testcategory--testbranch--testrevision", "Version is not correct. Was %s" % version)

    def testGetCategory(self):
        bzr = Bazaar("./theBranch", "testcategory", "testbranch", "testrevision")
        category = bzr.getCategory()
        self.failUnless(category == "testcategory", "Category is not correct. Was %s" % category)

    def testGetBranch(self):
        bzr = Bazaar("./theBranch", "testcategory", "testbranch", "testrevision")
        branch = bzr.getBranch()
        self.failUnless(branch == "testbranch", "Branch is not correct. Was %s" % branch)

    def testGetRevision(self):
        bzr = Bazaar("./theBranch", "testcategory", "testbranch", "testrevision")
        revision = bzr.getRevision()
        self.failUnless(revision == "testrevision", "Revision is not correct. Was %s" % revision)

    def testGet(self):
        bzr = Bazaar("./theBranch", "testcategory", "testbranch", "testrevision")
        bzr.get("../master")

        self.failUnless(os.path.exists("./theBranch/AFile"),
                        "File AFile could not be found after branching")

    def testGetDelayedPathInject(self):
        bzr = Bazaar("", "testcategory", "testbranch", "testrevision")

        bzr.setPath("./theBranch")

        bzr.get("../master")

        self.failUnless(os.path.exists("./theBranch/AFile"),
                        "File AFile could not be found after branching")

    def testStatus(self):
        bzr = Bazaar("./theBranch", "testcategory", "testbranch", "testrevision")
        bzr.get("../master")

        status = str(bzr.status())
        self.failUnless(status == "",
                        "Status output should be empty but was %s" % status)

        c = ["echo 'Dieter was here' >> ./theBranch/AFile"]

        self.runCommands(c)

        status = str(bzr.status())
        self.failUnless(status == " M  AFile",
                        "Status output should have AFile modified but was %s" % status)

    def testLint(self):
        bzr = Bazaar("./theBranch", "testcategory", "testbranch", "testrevision")
        bzr.get("../master")

        lint = str(bzr.lint())
        self.failUnless(lint == "",
                        "Lint output should be empty but was %s" % lint)

        c = ["echo 'Dieter was here' >> ./theBranch/BFile"]

        self.runCommands(c)

        lint = str(bzr.lint())
        self.failUnless(lint == "?   BFile",
                        "Lint output should have BFile unknown but was %s" % lint)

    def testMissing(self):
        bzr = Bazaar("./theBranch", "testcategory", "testbranch", "testrevision")
        bzr.get("../master")

        bzrB = Bazaar("./theBranchB", "testcategory", "testbranch", "testrevision")
        bzrB.get("../master")

        missing = str(bzr.missing("../../master"))

        self.failUnless(missing == "",
                        "Missing output should be empty but was %s" % missing)

        missing = str(bzrB.missing("../../master"))

        self.failUnless(missing == "",
                        "Missing output should be empty but was %s" % missing)


        # COMMIT ONE CHANGE ON theBranchB
        c = ["cd ./theBranchB; echo 'Changes on another branch' >> AFile",
             "cd ./theBranchB; bzr commit -m 'Changed on theBranchB'"]

        self.runCommands(c)

        missing = str(bzrB.missing("../../master"))

        expectedOutput = "You have 1 extra revision\(s\).*Changed on theBranchB"
        self.failUnless(self.outputMatches(missing, expectedOutput),
                        "Missing output should list revision on BranchB but was %s" % missing)

        # PUSH THE CHANGE TO THE MASTER AND MAKE IT VISIBLE FOR theBranch
        c = [ "cd ./theBranchB; bzr push ../../master" ]

        self.runCommands(c)

        missing = str(bzrB.missing("../../master"))

        self.failUnless(missing == "",
                        "Missing output should be empty but was %s" % missing)

        missing = str(bzr.missing("../../master"))

        expectedOutput = "You are missing 1 revision\(s\).*Changed on theBranchB"
        self.failUnless(self.outputMatches(missing, expectedOutput),
                        "Missing output should list revision on BranchB but was %s" % missing)

    def testUpdate(self):
        bzr = Bazaar("./theBranch", "testcategory", "testbranch", "testrevision")
        bzr.get("../master")

        bzrB = Bazaar("./theBranchB", "testcategory", "testbranch", "testrevision")
        bzrB.get("../master")

        missing = str(bzr.missing("../../master"))

        self.failUnless(missing == "",
                        "Missing output should be empty but was %s" % missing)

        missing = str(bzrB.missing("../../master"))

        self.failUnless(missing == "",
                        "Missing output should be empty but was %s" % missing)


        # COMMIT ONE CHANGE ON theBranchB
        c = ["cd ./theBranchB; echo 'Changes on another branch' >> AFile",
             "cd ./theBranchB; bzr commit -m 'Changed on theBranchB'"]

        self.runCommands(c)

        bzrB.push("../../master")

        # rcs.Bazaar implements the update function by call pull
        # This method is somewhat ill named for backward compatibility reasons
        # Nevertheless, here we go.
        missing = str(bzr.missing("../../master"))

        # PRECONDITION : We miss the revision before we update
        expectedOutput = "You are missing 1 revision\(s\).*Changed on theBranchB"
        self.failUnless(self.outputMatches(missing, expectedOutput),
                        "Missing output should list revision on BranchB but was %s" % missing)

        bzr.update()

        missing = str(bzr.missing("../../master"))

        self.failUnless(missing == "",
                        "Missing output should be empty but was %s" % missing)

    def outputMatches(self, output, regexp):
        import re
        exp = re.compile(regexp, re.DOTALL)
        result = re.search(exp, output)
        return not result == None

    def runCommands(self, commandList):
        for command in commandList:
            (errcode, output) = commands.getstatusoutput(command)
            assert errcode==0, "Could not execute %s\n\n%s" % (command, output)
