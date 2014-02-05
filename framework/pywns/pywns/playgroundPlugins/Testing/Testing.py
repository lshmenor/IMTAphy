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
import pywns.MemCheck
import pywns.WNSUnit

import wnsbase.playground.Core
core = wnsbase.playground.Core.getCore()

class RunTestsCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog runtests\n\n"
        rationale = "Runs the test suite."

        usage += rationale

        usage += """
Runs all the tests. This includes unittests for both Python and C++
and the system tests.
"""
        wnsbase.playground.plugins.Command.Command.__init__(self, "runtests", rationale, usage)

        self.addOption("", "--executable",
                       type="string", dest = "executable", default = "./openwns",
                       help = "The executable that is to be called (default : \"./openwns\")")
    def run(self):
        # create test collector
        import pywns.WNSUnit
        import wnsbase.playground.Project

        tests = []
        for project in core.getProjects().all:
            if isinstance(project, wnsbase.playground.Project.SystemTest):
                tests.append(project)

        testCollector = pywns.WNSUnit.SystemTestCollector(suiteConfig = "systemTest.py",
                                                          suiteName = "testSuite")
        testCollector.setTests(tests)

        # Add PyConfig unit tests
        pyUnit = pywns.WNSUnit.ExternalProgram(dirname = "tests/unit/PythonUnitTests/",
                                               command = "./runPythonUnitTests.py -v",
                                               description = "PyConfig Unit Tests",
                                               includeStdOut = True)
        testCollector.addTest(pyUnit)


        # Add C++ unit tests

        cppUnit = pywns.WNSUnit.ExternalProgram(dirname = "tests/unit/unitTests/",
                                                command = self.options.executable + " -f config.py -t -y'WNS.masterLogger.backtrace.enabled=True'",
                                                description = "C++ unit tests",
                                                includeStdOut = True)
        testCollector.addTest(cppUnit)

        pywns.WNSUnit.verbosity = 2

        # you can get the beast even more verbose by enabling this:
        # testCollector.testRunner.verbosity = 2

        print "Starting test suites ..."
        print "NOTE: you may see slow progress since the tests run simulations"

        result = testCollector.run()
        if (len(result.errors) == 0) and (len(result.failures) == 0):
            sys.exit(0)
        else:
            sys.exit(1)

class RunLongTestsCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog runlongtests\n\n"
        rationale = "Runs the 'long' test suite."

        usage += rationale

        usage += """
Runs all the tests in the long test suite.
"""
        wnsbase.playground.plugins.Command.Command.__init__(self, "runlongtests", rationale, usage)

        self.addOption("", "--executable",
                       type="string", dest = "executable", default = "./openwns",
                       help = "The executable that is to be called (default : \"./openwns\")")
    def run(self):
        # create test collector
        import pywns.WNSUnit
        import wnsbase.playground.Project

        tests = []
        for project in core.getProjects().all:
            if isinstance(project, wnsbase.playground.Project.SystemTest):
                tests.append(project)

        testCollector = pywns.WNSUnit.SystemTestCollector(suiteConfig = "systemLongTest.py",
                                                          suiteName = "testSuite")
        testCollector.setTests(tests)

        pywns.WNSUnit.verbosity = 2

        # you can get the beast even more verbose by enabling this:
        # testCollector.testRunner.verbosity = 2

        print "Starting test suites ..."
        print "NOTE: you may see slow progress since the tests run simulations"

        result = testCollector.run()
        if (len(result.errors) == 0) and (len(result.failures) == 0):
            sys.exit(0)
        else:
            sys.exit(1)

class MemcheckCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog memcheck\n\n"
        rationale = "Run the memchecker valgrind on the unit tests."

        usage += rationale

        usage += """
Runs the unittests under the memory checker valgrind. Use this to detect
memory leaks or memory access errors. Upstream patches must be free of any
memory errors.
Note that with this command certain errors from third party libraries are
suppressed since these are out our of control.
"""
        wnsbase.playground.plugins.Command.Command.__init__(self, "memcheck", rationale, usage)

        self.addOption("", "--executable",
                       type="string", dest = "executable", default = "./openwns",
                       help = "The executable that is to be called (default : \"./openwns\")")

        self.addOption("", "--suppressions",
                       type="string", dest = "suppressions", default = "",
                       help = "A (comma separated) list of valgrind suppression files (e.g.: /usr/lib/valgrind/python.supp)")

    def run(self):
        suppressionsFileList = []
        if not self.options.suppressions == "":
            suppressionsFileList = [ ii.strip() for ii in self.options.suppressions.split(',') ]
        r = pywns.MemCheck.Runner(args=[self.options.executable, "-tv"],
                                  cwd="tests/unit/unitTests",
                                  suppressions=suppressionsFileList)
        returncode = r.run()
        sys.exit(returncode)
