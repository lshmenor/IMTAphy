#!/usr/bin/env python
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

""" A unit testing framework for the special evalutation of tests in
the context of WNS.

This framework provides a way to arrange tests around the results of
simultions performed with WNS.

Here is a simple example script:

<schnipp>
#! /usr/bin/env python

# this is needed, so that the script can be called from everywhere
import os
import sys
base, tail = os.path.split(sys.argv[0])
os.chdir(base)

# Append the python sub-dir of WNS--main--x.y ...
sys.path.append('../../../python')

# ... because the module WNS unit test framework is located there.
import WNSUnit

# create a system test
# see ../../python/WNSUnit.py for deeper understanding
testSuite = WNSUnit.ProbesTestSuite(sandboxPath = '../../../sandbox',
                                    configFile = 'config.py',
                                    readProbes = True,
                                    shortDescription = 'Provide a very short description ...')

someExpectation = WNSUnit.Expectation('IP_EndToEndIncomingDelay_SC1_PDF.dat',
                                      ['probe.trials > 15','probe.trials < 17'],
                                      'dbg')
testSuite.addTest(someExpectation)

if __name__ == '__main__':
    # if you need to change the verbosity do it here
    verbosity = 1

    WNSUnit.verbosity = verbosity

    # Create test runner
    testRunner = WNSUnit.TextTestRunner(verbosity=verbosity)

    # Finally, run the tests.
    testRunner.run(testSuite)
</schnapp>

There are three levels of system test suites available:
    - SystemTestSuite (performs no automatic check at all)
    - ProbesTestSuite (performs checks regarding probes)
    - PedanticProbesTestSuite (like ProbesTestSuite, but more restrictive)

Normally you want to use ProbesTestSuite.
"""

import unittest
import sys
import os
import time
import datetime
import shutil
import subprocess
import time
import Probe

class Output(object):
    def __init__(self):
        self.stderr = sys.stderr
        self.stdout = sys.stdout

    def writeOut(self, msg):
        if verbosity > 1:
            self.stdout.write(msg)

    def writeErr(self, msg):
        if verbosity > 1:
            self.stderr.write(msg)


# create local output object
verbosity = 2
output = Output()


class TextTestRunner(unittest.TextTestRunner):
    """ Has a special variable disabledTests used to record disabled
    tests
    """

    activeRunner = None
    """ runner in method 'run' (get by calling getActiveRunner) """

    @staticmethod
    def getActiveRunner():
        """ Raise an exception if no runner is currently in its 'run'
        method
        """

        if TextTestRunner.activeRunner != None:
            return TextTestRunner.activeRunner
        else:
            raise Exception("No WNSUnit.TextTestRunner in method 'run' currently ...")


    def __init__(self, *args, **kwds):
        super(TextTestRunner, self).__init__(*args, **kwds)
        self.disabledSuites = []


    def addDisabledSuite(self, suite):
        self.disabledSuites.append(suite)

    def run(self, *args, **kwds):
        """ Run the test and provide disabled statistics
        """
        assert(TextTestRunner.activeRunner == None)
        TextTestRunner.activeRunner = self
        result = unittest.TextTestRunner.run(self, *args, **kwds)
        # statistic on disabled tests
        count = len(self.disabledSuites)
        if count > 0:
            # print in every case:
            output.stderr.write("Warning: You had " + str(count) + " suites disabled.\n")
            for suite in self.disabledSuites:
                output.stderr.write("\n         TestSuite: " + suite.getName() + "\n")
                output.stderr.write("         Reason: " + suite.disabledReason + "\n")
        assert(TextTestRunner.activeRunner == self)
        TextTestRunner.activeRunner = None
        return result


class TestSuite(unittest.TestSuite):
    """ Nothing special

    simple forward
    """
    pass


class SystemTestSuite(TestSuite):
    """ Test suite and test fixture

    This is most basic WNS system test suite. A system is defined as
    the configuration available from self.configFile. This class is
    test suite and test fixture at the same time. It is a test suite
    because it holds all the tests to be executed within a test
    fixture. And it is a test fixture because it prepares the
    surrounding (runs the simulation and reads the probes).

    The simualtions run by this TestSuite are dbg and opt versions
    with the respective configuration. The probes are read after the
    simulations are run. All this is performed within the 'run' method
    of the SystemTestSuite.

    Within the run method, but before tests are actually run, the
    method prepareSystemTestSuite is called is called.

    Results of running the debug version of the simulator found below
    the 'sandboxPath' with the specified configFile are placed in
    'output_dbg_'+configFile

    Results of running the optimized version of the simulator found
    below the 'sandboxPath' with the specified configFile are placed
    in 'output_opt_'+configFile
    """

    class PDFProbes(object):
        """ A little internal helper """

        def __init__(self, dirname):
            super(ProbesTestSuite.PDFProbes, self).__init__()
            self.dirname = dirname
            self.probes = Probe.readAllProbes(dirname)

    def __init__(
        self,
        sandboxPath = "../../sandbox",
        configFile = "config.py",
        runSimulations = True,
        shortDescription = "Please provide a short description!!!",
        disabled = False,
        disabledReason = "You MUST provide a reason for disabled tests!!!",
        workingDir = None,
        readProbes = False
        ):
        """
        Parameters:

        sandboxPath: path to sandbox to launch openwns from

        configFile: this file will be used for the test two
        simulations (dbg and opt) will be performed with this
        configuration

        runSimulation: Useful when writing tests. If simulations
        have been performed (which is normally time consuming one
        might want to switch simulating of while writing new test
        cases based on the current output.
        """

        super(SystemTestSuite, self).__init__()

        self.sandboxPath = os.path.abspath(sandboxPath)
        self.configFile = configFile
        self.runSimulations = runSimulations
        self.shortDescription = shortDescription
        self.disabled = disabled
        self.disabledReason = disabledReason
        if workingDir == None:
            self.workingDir = os.getcwd()
        else:
            self.workingDir = os.path.join(os.getcwd(), workingDir )
        self.__readProbes = readProbes
        # default name is the working dir
        self.name = self.workingDir
        self.dbgOutputDir = "output_dbg_" + self.configFile
        self.optOutputDir = "output_opt_" + self.configFile
        self.referenceOutputDir = "referenceOutput_" + self.configFile
        self.dbgProbes = None
        self.optProbes = None
        self.referenceProbes = None
        # will be set to false if something went wrong ...
        self.simulationsWorkedOut = True

    def addTest(self, testCase):
        """ This injects the system test into the test case. Thus
        allowing every test to access the system test (fixture). This
        is useful to get access to the probes.
        """
        testCase.systemTestSuite = self
        unittest.TestSuite.addTest(self, testCase)


    def run(self, *args, **kwds):
        """ Runs simulations. If successful: Calls prepapreTestSuite
        here and in each derived class (if exists). After this, tests
        are run.
        """
        oldDir = os.getcwd()
        os.chdir(self.workingDir)
        output.writeErr("\n**********************************************************************\n")
        output.writeErr("SystemTestSuite: " + self.__getShortName(53) + "\n")
        output.writeErr("Configuration: " + self.configFile + "\n")
        output.writeErr("Description: " + self.__cutString(self.shortDescription, 57, False) + "\n")
        output.writeErr("----------------------------------------------------------------------\n")

        if self.disabled == False:
            output.writeErr("Preparation phase:\n")

            self.__runSims()

            if self.simulationsWorkedOut == True:
                self.__callPrepareTestSuite()

            output.writeErr("Test phase:\n")
            # finally, really run the tests
            unittest.TestSuite.run(self, *args, **kwds)
        else:
            # add to to disabled suites in active runner
            TextTestRunner.getActiveRunner().addDisabledSuite(self)
            output.writeErr("Suite is disabled: " + self.disabledReason + "\n")
            output.writeErr("Disabled.")

        output.writeErr("\n**********************************************************************\n")

        os.chdir(oldDir)
        # finally free the memory (When having a huge number of tests
        # (>5000) this became an issue since the python interpreter
        # consumed around 1 GB of memory
        self.dbgProbes = {}
        self.optProbes = {}
        self.referenceProbes = {}
        # This might be dangerous since we don't know, if the test
        # objects are still expected to be in the suite. However it seems to work
        self._tests = []


    def prepareSystemTestSuite(self):
        """ If readProbes ==  True, Read probes for dbg and opt and reference probes if available
        """
        if self.__readProbes == True:
            self.__readAvailableProbes()
            self.readReferenceProbesIfAvailable()


    def getName(self):
        """ Return the name
        """
        return self.name


    def readReferenceProbesIfAvailable(self):
        """ Read reference probes if directory exists, otherwise
        self.referenceProbes == None
        """
        if os.path.exists(self.referenceOutputDir) and self.referenceProbes == None:
            output.writeErr("Reading reference probes (this may take a while) ... ")
            self.referenceProbes = SystemTestSuite.PDFProbes(self.referenceOutputDir)
            output.writeErr("Done.\n")


    # private stuff

    def __cutString(self, text, maxLength, cutHead = True):
        """ Cut a text at the head or the tail so that it is no longer
        but maxLength chars.
        """
        length = len(text)
        if (length > maxLength) and (length > 3):
            if cutHead == True:
                return "..." + text[length-maxLength+3:]
            else:
                return text[:maxLength-4] + "..."
        else:
            return text


    def __getShortName(self, maxLength, cutHead = True):
        return self.__cutString(self.getName(), maxLength, cutHead)


    def __readAvailableProbes(self):
        """ Read dbg and opt probes
        """
        output.writeErr("Reading dbg probes (this may take a while) ... ")
        self.dbgProbes = SystemTestSuite.PDFProbes(self.dbgOutputDir)
        output.writeErr("Done.\n")

        output.writeErr("Reading opt probes (this may take a while) ... ")
        self.optProbes = SystemTestSuite.PDFProbes(self.optOutputDir)
        output.writeErr("Done.\n")


    def __runSims(self):
        # Returns True if simulation run was ok (or if no simulations
        # needed to be performed.
        if(self.runSimulations):
            self.__runSimulations()

    def __runSimulations(self):
        """ Runs dbg and opt simulations to prepare the output dir

        If the simulation fails, the test will not stop but print the
        error message.
        """
        output.writeErr("Running simulations (no test, just preparing output) in debugging and\noptimized mode (may take very long):\n")
        # two tests one for dbg
        dbgSimulation = Simulation(wns = os.path.join(self.sandboxPath, "dbg", "bin", "openwns"),
                                   configFile = self.configFile,
                                   outputDir = self.dbgOutputDir)

        # and one for opt
        optSimulation = Simulation(wns = os.path.join(self.sandboxPath, "opt", "bin", "openwns"),
                                   configFile = self.configFile,
                                   outputDir = self.optOutputDir)

        dbgResult = self.__runSimulation(dbgSimulation)
        optResult = self.__runSimulation(optSimulation)

        # Both results are fake tests ...
        self.addTest(dbgResult)
        self.addTest(optResult)

        if self.simulationsWorkedOut == True:
            dbgSeconds = dbgSimulation.getDurationInSeconds()
            optSeconds = optSimulation.getDurationInSeconds()
            output.writeErr("opt:dbg = 1:" + str(round((dbgSeconds/optSeconds), 3)) + "\n")

    def __runSimulation(self, sim):
        try:
            sim.run()
            # queue test to show simulation worked out
            return FakeTest(success = True, shortDescription = "Simulation")

        except SimulationException, simException:
            # only catch simualation exceptions, other exceptions
            # should be handled above us
            # clear all tests, don't execute them
            self._tests = []
            # queue a fake test in order to signal the unittest
            # system that something went wrong
            errorMsg = "Simulation failed. Reason:\n" + str(simException)
            self.simulationsWorkedOut = False
            return FakeTest(success = False, shortDescription = "Simulation", errorMsg = errorMsg)

    def __callPrepareTestSuite(self):
        """ This calls all 'prepareSystemTestSuite' methods (if available at
        the class) beginning with this class and descending into any
        derived class.
        """
        mro = []

        # loop over all classes in inhertiance tree ...
        for classObject in type(self).mro():
            mro.append(classObject)
            # ... until we arrive here
            if classObject == SystemTestSuite:
                break

        # change the order (start with this class)
        mro.reverse()

        # call "prepareSystemTestSuite" (if exists) for each class
        for classObject in mro:
            if classObject.__dict__.has_key("prepareSystemTestSuite"):
                classObject.prepareSystemTestSuite(self)


class ProbesTestSuite(SystemTestSuite):
    """ Test suite for system tests with automatic probe checking

    This test suite automatically generates a number of tests for a
    specific system.
    """
    probesToBeExcluded = None

    def removeProbeTest(self, ProbeName):
        self.probesToBeExcluded.append(ProbeName)

    def __init__(
        self,
        sandboxPath = "../../sandbox",
        configFile = "config.py",
        maximumRelativeError = 1E-6,
        runSimulations = True,
        shortDescription = "Please provide a short description!!!",
        disabled = False,
        disabledReason = "You MUST provide a reason for disabled tests!!!",
        requireReferenceOutput = True,
        workingDir = None,
        checkCPUCycles = False,
        CPUCycleTolerance = 0.2
        ):
        """ Setup system test with automatic probe checking

        maximumRelativeError: see below
        """

        super(ProbesTestSuite, self).__init__(
            sandboxPath = sandboxPath,
            configFile = configFile,
            runSimulations = runSimulations,
            shortDescription = shortDescription,
            disabled = disabled,
            disabledReason = disabledReason,
            workingDir = workingDir,
            readProbes = True)

        self.probesToBeExcluded = ['wns.Memory_TimeSeries.dat', 
                                    'wns.Memory_Moments.dat',
                                    'wns.SimTimePerRealTime_TimeSeries.dat',
                                    'wns.SimTimePerRealTime_Moments.dat']
                                    
        # Check no code leeds to significant runtime changes etc.
        # but runtime on modern CPUs can vary
        if checkCPUCycles:
            self.simulatorPerformanceProbes = ['wns.cpuCycles_Moments.dat']
            self.simulatorPerformanceTolerance = CPUCycleTolerance
        else:
            self.simulatorPerformanceProbes = []
            self.probesToBeExcluded.append('wns.cpuCycles_Moments.dat')
                                    
        self.maximumRelativeError = maximumRelativeError
        self.__requireReferenceOutput = requireReferenceOutput

        ## write a csv file that can be used to compare the outputs of the opt, dbg and reference sims
        ## with wrowser
        csvFile = file('testResults_'+self.configFile+'.csv','w')
        csvFile.write('id, name\n')
        csvFile.write('./referenceOutput_'+self.configFile+', REFERENCE\n')
        csvFile.write('./output_dbg_'+self.configFile+', DBG\n')
        csvFile.write('./output_opt_'+self.configFile+', OPT\n')
        csvFile.close()



    def prepareSystemTestSuite(self):
        """
             1) Optional: If you don't have a directory
                'referenceOutput_'+configFile it will ask you if you want to copy
                from 'output_dbg_'+configFile as initial reference output.
             2) Register all probe tests at the test suite.
        """
        if self.__requireReferenceOutput == True:
            self.__checkReferenceOutputExists()
            self.readReferenceProbesIfAvailable()
        self.__generateAutomaticTests()


    # private stuff

    def __checkReferenceOutputExists(self):
        """ check if reference output exists. If not, offer to create
        from dbg output.
        """
        if not os.path.exists(self.referenceOutputDir):
            answer = ""
            output.writeOut("\n")
            output.writeOut("The directory " + self.referenceOutputDir + " dose not exists.\n")
            output.writeOut("This probably means you have not run this test suite before.\n")
            output.writeOut("I need this directory in order to run all tests. It is expected\n")
            output.writeOut("that you have your reference output there. I can offer you\n")
            output.writeOut("to copy the results that have just been created in " + self.dbgOutputDir + ".\n")
            while answer.lower() not in ["y", "n"]:
                answer = raw_input("\nShould I copy " + self.dbgOutputDir + " to " + self.referenceOutputDir + "? (y/n) ")
            if answer.lower() == "y":
                shutil.copytree(self.dbgOutputDir, self.referenceOutputDir)
            else:
                output.writeOut("I will not be able to run tests against reference output.\n")
                output.writeOut("These test will be disabled. Also self.referenceProbes will not be available (None).\n")

            output.writeOut("\n")


    def __generateAutomaticTests(self):
        """ Register all tests probe tests at the test suite.

        A test is the comparision of a property (e.g. mean or
        variance) of two probes (e.g. dbg vs opt of
        IP_End_to_End_Delay). The intersection of the probes of two
        directories are compared against each other (in the event some
        probes are not available, for whatsoever reason). The values
        are compared with a maximum relative error. For dbg vs. opt
        tests, the maximum relative error is set 0.0.
        """
        # check that any probes have been read
        if len(self.dbgProbes.probes.keys()) == 0:
            raise Exception("No probes to compare!")

        if self.referenceProbes != None:
            self.__generateTestsForProbes(
                referenceProbeNames = set(self.referenceProbes.probes.keys()),
                referenceFlavour = 'reference',
                actualProbeNames = set(self.dbgProbes.probes.keys()),
                actualFlavour = 'dbg',
                maximumRelativeError = self.maximumRelativeError)

            self.__generateTestsForProbes(
                referenceProbeNames = set(self.referenceProbes.probes.keys()),
                referenceFlavour = 'reference',
                actualProbeNames = set(self.optProbes.probes.keys()),
                actualFlavour = 'opt',
                maximumRelativeError = self.maximumRelativeError)

        # special case: dbg vs. opt must fit 100%, no relative error allowed!
        self.__generateTestsForProbes(
                referenceProbeNames = set(self.optProbes.probes.keys()),
                referenceFlavour = 'dbg',
                actualProbeNames = set(self.optProbes.probes.keys()),
                actualFlavour = 'opt',
                maximumRelativeError = 0.0)


    def __generateTestsForProbes(
	self,
	referenceProbeNames,
	referenceFlavour,
	actualProbeNames,
	actualFlavour,
        maximumRelativeError):
        """ Generate a number of tests for all probes in
        referenceOutput and actualOutput

        All probes in actualOutput are compared with the probes found
        in referenceOutput. The following criteria are checked:
        - Are all probes available
        For probes that are available check equality for:
        - Trials
        - Mean
        - Variance
        """

        self.addTest(DirectoryContentsAreEqual(
	    self.__dict__[referenceFlavour + "Probes"].dirname,
            self.__dict__[actualFlavour + "Probes"].dirname,
            self.probesToBeExcluded))

        # create tests for each probe contained in both sets
        probeNamesAvailableInBoth = referenceProbeNames.intersection(actualProbeNames)

        for probeName in probeNamesAvailableInBoth:
            if probeName in self.simulatorPerformanceProbes:
                if referenceFlavour == "reference" and actualFlavour == "opt" :
                    maxRelativeError = self.simulatorPerformanceTolerance
                else:
                    continue
            else:
                maxRelativeError = maximumRelativeError 
                
            # exclude removed Probes
            if probeName in self.probesToBeExcluded :
                continue
            # TableProbes are currently not supported, TimeSeries not intended
            if not (isinstance(self.__dict__[referenceFlavour+"Probes"].probes[probeName],
                Probe.TableProbe)) and (not 
            isinstance(self.__dict__[referenceFlavour+"Probes"].probes[probeName],
                Probe.TimeSeriesProbe)):
              
                self.addTests(ProbesAreAlmostEqual.getAllTests(
                    probeName,
                    referenceFlavour,
                    probeName,
                    actualFlavour,
                    maxRelativeError))


class PedanticProbesTestSuite(ProbesTestSuite):
    """ Like SystemTestSuite but more restrictive

    Additional features:
    - maximum relative error is set fixed to 1E-9
    - check: probe.trials > 0 (because this means most likely they
      don't record anything)
    """

    def __init__(
        self,
        sandboxPath = "../../sandbox",
        configFile = "config.py",
        runSimulations = True,
        shortDescription = "Please provide a short description!!!",
        disabled = False,
        disabledReason = "You MUST provide a reason for disabled tests!!!"
        ):
        super(PedanticProbesTestSuite, self).__init__(
            sandboxPath = sandboxPath,
            configFile = configFile,
            maximumRelativeError = 1E-9,
            runSimulations = runSimulations,
            shortDescription = shortDescription,
            disabled = disabled,
            disabledReason = disabledReason)


    def prepareSystemTestSuite(self):
        if self.referenceProbes != None:
            self.__addProbesHaveTrialsTests(self.referenceProbes)

        self.__addProbesHaveTrialsTests(self.dbgProbes)
        self.__addProbesHaveTrialsTests(self.optProbes)


    # private stuff

    def __addProbesHaveTrialsTests(self, probe):
        for ii in probe.probes.values():
            self.addTest(ProbeHasTrials(ii))


class TestCase(unittest.TestCase):
    """ A forward
    """
    pass


class SystemTestCase(TestCase):
    """ This test case allows for access to the test fixture

    In order to write a SystemTestCase for WNS do the following:
    a) derive from WNSUnit.SystemTestCase
    b) have no constructor!
    c) implement 'def runTest(self):' (will be called by WNS.TextTestRunner.run())
    d) optional - implement 'def description(self):'
    NOTE: If you need a constuctor:
      1) You MUST call the constructor of the this class
      2) The variable self.systemTestSuite is not accessible in the constructor!

    Important: In the method 'def testRun(self)' you have access to
    the following important objects (besides all other available in
    SystemTestSuite):

    self.systemTestSuite.dbgProbes (probes from dbg run)
    self.systemTestSuite.optProbes (probes from opt run)
    self.systemTestSuite.referenceProbes (reference probes)

    To access a certain probe call it with its filename:
    probe = self.systemTestSuite.dbgProbes.probes['IPEndToEndDelay_SC1_PDF.dat']

    You can also retrieve the dirname (where the probes are located):
    dirname = self.systemTestSuite.dbgProbes.dirname

    see description at the top of the module to see how this fits into a system test
    """
    def __init__(self, *args, **kwds):
        super(SystemTestCase, self).__init__(*args, **kwds)
        # will be set by SystemTestSuite.addTest(testCase)
        self.systemTestSuite = None

    def shortDescription(self):
        return "(" + self.systemTestSuite.getName() + ") " + self.description()


class DirectoryContentsAreEqual(SystemTestCase):
    """ Asserts the content of two directories is equal

    Checks if two directories have the same content. If not lists the
    difference between both. No recursion is done. Only toplevel files
    are considered.
    """
    def __init__(self, referenceDir, actualDir, filterItems):
        super(DirectoryContentsAreEqual, self).__init__("runTest")
        self.referenceDir = referenceDir
        self.actualDir = actualDir
        self.filterItems =  filterItems

    def description(self):
        return "Check equality of " + self.referenceDir + " and " + self.actualDir


    def runTest(self):
        ref = set([ii for ii  in os.listdir(self.referenceDir) if not ii in self.filterItems])
        act = set([ii for ii  in os.listdir(self.actualDir) if not ii in self.filterItems])

        errorMsg = "\n  Files in " + self.referenceDir + " but not in " + self.actualDir + ": "
        errorMsg += ", ".join(ref.difference(act))
        errorMsg += "\n  Files in " + self.actualDir + " but not in " + self.referenceDir + ": "
        errorMsg += ", ".join(act.difference(ref))
        self.assertTrue(ref == act, errorMsg)


class ProbeTest(SystemTestCase):
    """ Tests with one probe (use only to derive)

    Simplify tests dealing with one probe. To write a test derive from
    it and implement the test function:

    class MyProbeTest(ProbeTest):

        # the test function
        def runTest(self):
            failMessage = 'mean <= 0 (' + str(self.probe.mean) + ')'
            self.AssertTrue(self.probe.mean > 0, failMessage)

        # you might provide a short description
        def description(self):
            return self.probe.filename + ': mean > 0'
    """

    def __init__(self, probe, methodName="runTest"):
        super(ProbeTest, self).__init__(methodName)
        self.probe = probe


class TwoProbeTest(SystemTestCase):
    """ Tests with two pobes (use only to derive)

    Simplify tests dealing with two probe. To write a test derive from
    it and implement the test function:

    class MyProbeTest(ProbeTest):

        # the test function
        def runTest(self):
            failMessage = 'meanA <= meanB (' + str(self.probeA.mean) + ' <= ' + str(self.probeB.mean) + ')'
            self.AssertTrue(self.probeA.mean > self.probeB.mean, failMessage)

        # you might provide a short description
        def description(self):
            return self.probeA.filename + ' vs ' + self.probeB.fileName + ': meanA > meanB'
    """

    def __init__(self, probeA, probeB, methodName="runTest"):
        super(TwoProbeTest, self).__init__(methodName)
        self.probeA = probeA
        self.probeB = probeB


class Expectation(SystemTestCase):
    """ Formulate your expectations towards a probe

    Usage:
    someExpectation = WNSUnit.Expectation('IP_EndToEndIncomingDelay_SC1_PDF.dat',
                                          ['probe.trials > 15','probe.trials < 17'],
                                          'dbg')
    testSuite.addTest(someExpectation)

    ALL expectations must be met! This means here you have two Expectation:
    'probe.trials > 15'
    AND
    'probe.trials < 17'
    and they must both be met.

    Note: You can have even more complex Expectations. The two above
    expections can be re-written as:
    '(probe.trials > 15) and (probe.trials <17)'

    Or even more complex:
    '(probe.mean / probe.variance >= 0.04)'


    Via the flavour parameter you can choose which probes to use:
      - dbg
      - opt
      - reference
    """
    def __init__(self, probeName, listOfExpectations, flavour="dbg"):
        super(Expectation, self).__init__()
        self.probeName = probeName
        self.listOfExpectations = listOfExpectations
        self.flavour = flavour

    def description(self):
        return self.probeName + " expectations: " + str(self.listOfExpectations)

    def runTest(self):
        # error handling
        if not self.systemTestSuite.__dict__.has_key(self.flavour + "Probes"):
            msg = "Wrong flavour ("+ self.flavour +"). Choose one of 'dbg, opt, reference'."
            raise Exception(msg)
        if self.systemTestSuite.__dict__[self.flavour + "Probes"] == None:
            msg = "No probes for " + self.flavour + " available.\n"
            msg += "Try setting 'readProbes' to 'True' in the constructor of this SystemTestSuite."
            raise Exception(msg)

        # get probe
        self.probe = self.systemTestSuite.__dict__[self.flavour + "Probes"].probes[self.probeName]

        # prepare error message. to have a nice error message, all
        # values that occured in the evaluation string are printed
        for ee in self.listOfExpectations:
            attributes = dir(self.probe)
            errorMessage = "Expression '" + ee + "' didn't match.\n"
            errorMessage += "The following values were used for evaluation:\n"
            for attribute in attributes:
                if attribute in ee:
                    errorMessage += "  probe." + attribute + ": " + str(self.probe.__dict__[attribute]) + "\n"

            # finally the assert
            self.assertTrue(eval(ee, {}, {"probe":self.probe}), errorMessage)


class ProbesAreAlmostEqual(SystemTestCase):
    """ Compare two probes

    Usage:
    someProbesAreAlmostEqual = WNSUnit.ProbesAreAlmostEqual(referenceProbeName, referenceFlavour,
                                                            actualProbeName, actualFlavour,
                                                            maxRelError, prop)

    testSuite.addTest(someProbesAreAlmostEqual)

    referenceProbeName: Name of the Probe wich should be compared
    referenceFlavour:   Corresponding flavour for this Probe

    actualProbeName: Name of the other Probe which should be compared
    actualFlavour:   Corresponding flavour for this Probe

    maxRelError: Maximal relative aberration for a correct comparison
    prop:        Probe attribute which will compared



    Possible flavours properties are:
      - dbg
      - opt
      - reference

    Possible prop properties are:
      - mean
      - variance
      - trials
      **They must fit to the attributes of Probe.Probe
    """


    availableProps = ["mean", "variance", "trials"]
    """ This properties are used in getAllTests to create one test for
    each property. They must fit to the attributes of Probe.Probe
    """

    def __init__(self, referenceProbeName, referenceFlavour, actualProbeName, actualFlavour, maxRelError, prop):
        super(ProbesAreAlmostEqual, self).__init__(methodName="runTest")
        self.referenceProbeName = referenceProbeName
        self.referenceFlavour = referenceFlavour
        self.actualProbeName = actualProbeName
        self.actualFlavour = actualFlavour
        self.maxRelError = maxRelError
        self.prop = prop
        self.description_ = str(referenceFlavour + "_" + referenceProbeName + " vs " + actualFlavour + "_" +actualProbeName + ": " + self.prop)

    def description(self):
        return  self.description_

    def runTest(self):
        # get probe
        self.probeA = self.systemTestSuite.__dict__[self.referenceFlavour + "Probes"].probes[self.referenceProbeName]
        self.probeB = self.systemTestSuite.__dict__[self.actualFlavour + "Probes"].probes[self.actualProbeName]

        # get new describtion
        self.description_ = str(self.probeA.filename + " vs " + self.probeB.filename + ": " + self.prop)

        refVal = eval("self.probeA." + self.prop)
        actVal = eval("self.probeB." + self.prop)

        nominator = float(abs(refVal - actVal))
        denominator = float(refVal)

        # special handling for the relative error where the reference
        # value is '0'
        if (denominator != 0.0):
            relError =  nominator / denominator
        else:
            if (nominator == 0.0):
                relError = 0.0
            else:
                relError = float("infinity")

        errorMsg  = "\nMaximum relative error of " + str(self.maxRelError) + " exceeded."
        errorMsg += "\nRelative error was: " + str(relError) + " (reference: " + str(refVal) + ", actual: " + str(actVal) + ")"

        # compare absolute values
        self.assertTrue( abs(relError) <= abs(self.maxRelError), errorMsg )

    @staticmethod
    def getAllTests(referenceProbeName, referenceFlavour, actualProbeName, actualFlavour, maxRelError):
        """ Call this to automagically generate probe tests

        This will create PDFProbeAlmostEqual tests for referenceProbe
        vs. actualProbe with a maximum relative Error compared for
        each property. The properties are listed in availableProps.
        """
        tests = []
        for prop in ProbesAreAlmostEqual.availableProps:
            tests.append(ProbesAreAlmostEqual(referenceProbeName, referenceFlavour,
                                              actualProbeName, actualFlavour,
                                              maxRelError, prop))
        return tests


class ProbeHasTrials(ProbeTest):
    """ Check if the number of trials for a probe is >0
    """

    def runTest(self):
        self.assertTrue(self.probe.trials > 0, "Number of trials: " + str(self.probe.trials))

    def description(self):
        return self.probe.filename + ": number of trials > 0"


class SimulationException(Exception):
    pass

class Simulation(object):
    """ Ensures a simulation runs without config error and segfault

    This will run the simulation and prepare the output directory. If
    the simulation fails, an exception is raised.
    """
    def __init__(
        self,
        wns = "../../sandbox/dbg/bin/openwns",
        configFile = "config.py",
        configPatches = [],
        outputDir = ""
        ):
        self.wns = wns
        self.configFile = configFile
        self.configPatches = ["WNS.masterLogger.enabled=True", "WNS.masterLogger.backtrace.enabled=True"] + configPatches
        self.outputDir = outputDir
        if self.outputDir != "":
            self.configPatches += ["WNS.outputDir = '" + self.outputDir + "'"]
        self.configPatch = '-y "' + '; '.join(self.configPatches) + '"'
        self.wnsParameters = self.configPatch

    def run(self):
        """ Run simulation
        """
        start = datetime.datetime.today()
        cmd = " ".join([self.wns, "-f", self.configFile, self.wnsParameters])
        print "Running: " + cmd
        stdout = "stdout.log"
        stderr = "stderr.log"
        process = subprocess.Popen(cmd, shell=True, stdout=open(stdout, "w"), stderr=open(stderr, "w"))
        status = process.poll()
        while(status == None):
            output.writeErr(".")
            time.sleep(1.0)
            status = process.poll()

        self.duration = datetime.datetime.today() - start
        output.writeErr(" " + str(self.duration) + " h")
        output.writeErr("\n")
        if status != 0:
            raise SimulationException(cmd + " failed!!:\n" + file(stderr).read())

    def getDurationInSeconds(self):
        return float(self.duration.seconds + 86400*self.duration.days + self.duration.microseconds*1E-6)


class FakeTest(SystemTestCase):
    """ Helper to inject tests that aren't really tests
    """
    def __init__(self, success, shortDescription = "FakeTest", errorMsg = ""):
        super(FakeTest, self).__init__("runTest")
        self.__description = shortDescription
        self.success = success
        self.errorMsg = errorMsg

    def runTest(self):
        self.assertTrue(self.success, self.errorMsg)

    def description(self):
        return self.__description


class TestCollector(object):
    """ This collector searches in dirname in all sub-dirs for a file
    suiteConfig and expects a variable 'testSuite' in this file. It
    builds a local masterTestSuite and finally executes the tests.
    """
    def __init__(
        self,
        dirname = "tests/system",
        suiteConfig = "systemTest.py",
        suiteName = "testSuite",
        filterDirs = [".arch-ids", ".", ".."]
        ):
        super(TestCollector, self).__init__()
        self.dirname = dirname
        self.suiteConfig = suiteConfig
        self.suiteName = suiteName
        self.filterDirs = filterDirs
        self.masterSuite = TestSuite()
        self.__noConfigurationFound = []
        self.__noSuiteFound = []
        self.testRunner = TextTestRunner(verbosity=1)

    def collect(self):
        items = os.listdir(self.dirname)
        for item in items:
            if not item in self.filterDirs:
                currentItem = os.path.join(self.dirname, item)
                if os.path.isdir(currentItem):
                    subDir = os.path.join(self.dirname, item)
                    if os.path.exists(os.path.join(subDir, self.suiteConfig)):
                        # found the right file
                        globalsDict = {"__name__": "systemTest"}
                        oldDir = os.getcwd()
                        os.chdir(subDir)
                        execfile(self.suiteConfig, globalsDict)
                        os.chdir(oldDir)
                        if globalsDict.has_key(self.suiteName):
                            self.masterSuite.addTest(globalsDict[self.suiteName])
                        else:
                            output.writeErr("Warning: Didn't find " + self.suiteName + " in: ")
                            output.writeErr(os.path.join(subDir, self.suiteConfig) + "\n")
                            self.__noSuiteFound.append(subDir)
                    else:
                        output.writeErr("Warning: No configuration named ")
                        output.writeErr(self.suiteConfig + " in: ")
                        output.writeErr(os.path.join(subDir, self.suiteConfig) + "\n")
                        self.__noConfigurationFound.append(subDir)
                else:
                    output.stderr.write("Warning: " + item + " is not a directory.\n")
                    output.stderr.write("You should only have directories here.\n")


    def run(self):
        status = self.testRunner.run(self.masterSuite)
        if(len(self.__noSuiteFound) > 0):
            output.stderr.write("\nWarning: You had " + str(len(self.__noSuiteFound)) + " configurations")
            output.stderr.write(" withtout a suite named " + self.suiteName +  ":\n")
            output.stderr.write("         (No tests will be run here)\n         ")
            output.stderr.write("         ".join([os.path.join(ii, self.suiteConfig) + "\n" for ii in self.__noSuiteFound]))

        if(len(self.__noConfigurationFound) > 0):
            output.stderr.write("\nWarning: You had " + str(len(self.__noConfigurationFound)) + " directories")
            output.stderr.write(" withtout a config file (" + self.suiteConfig +  "):\n")
            output.stderr.write("         (No tests will be run here)\n         ")
            output.stderr.write("         ".join([ii + "\n" for ii in self.__noConfigurationFound]))
        return status

    def addTest(self, test):
        self.masterSuite.addTest(test)


class SystemTestCollector(object):
    """ This collector concatenates all given tests into a single
    master test and can execute this test.
    """
    def __init__(self,
                 suiteConfig = "systemTest.py",
                 suiteName = "testSuite"):
        super(SystemTestCollector, self).__init__()
        self.suiteConfig = suiteConfig
        self.suiteName = suiteName
        self.masterSuite = TestSuite()
        self.__noConfigurationFound = []
        self.__noSuiteFound = []
        self.testRunner = TextTestRunner(verbosity=1)

    def setTests(self, tests):
        for test in tests:
            if os.path.exists(os.path.join(test.getDir(), self.suiteConfig)):
                globalsDict =  {"__name__": "systemTest"}
                oldDir = os.getcwd()
                os.chdir(test.getDir())
                execfile(self.suiteConfig, globalsDict)
                os.chdir(oldDir)
                if globalsDict.has_key(self.suiteName):
                    self.masterSuite.addTest(globalsDict[self.suiteName])
                else:
                    output.writeErr("Warning: Didn't find " + self.suiteName + " in: ")
                    output.writeErr(os.path.join(test.getDir(), self.suiteConfig) + "\n")
                    self.__noSuiteFound.append(test.getDir())
            else:
                output.writeErr("Warning: No configuration named ")
                output.writeErr(self.suiteConfig + " in: ")
                output.writeErr(os.path.join(test.getDir(), self.suiteConfig) + "\n")
                self.__noConfigurationFound.append(test.getDir())

    def run(self):
        status = self.testRunner.run(self.masterSuite)
        if(len(self.__noSuiteFound) > 0):
            output.stderr.write("\nWarning: You had " + str(len(self.__noSuiteFound)) + " configurations")
            output.stderr.write(" withtout a suite named " + self.suiteName +  ":\n")
            output.stderr.write("         (No tests will be run here)\n         ")
            output.stderr.write("         ".join([os.path.join(ii, self.suiteConfig) + "\n" for ii in self.__noSuiteFound]))

        if(len(self.__noConfigurationFound) > 0):
            output.stderr.write("\nWarning: You had " + str(len(self.__noConfigurationFound)) + " system tests")
            output.stderr.write(" withtout a config file (" + self.suiteConfig +  "):\n")
            output.stderr.write("         (No tests will be run here)\n         ")
            output.stderr.write("         ".join([ii + "\n" for ii in self.__noConfigurationFound]))
        return status

    def addTest(self, test):
        self.masterSuite.addTest(test)

        

class ExternalProgram(TestCase):
    """ Runs and progam and assert the status code is 0

    Parameters:
    dirname: where to execute the program
    command: the command to be executed

    Test works as follows:
    1) Change into the dir
    2) Run the program
    3) Change back to old dir
    4) Check status code
    """

    def __init__(self, dirname, command, description, includeStdOut = False, *args, **kwds):
        super(ExternalProgram, self).__init__(*args, **kwds)
        self.command = command
        self.dirname = dirname
        self.description = description
        self.includeStdOut = includeStdOut


    def shortDescription(self):
        return self.dirname + ": " + self.description


    def runTest(self):
        oldDir = os.getcwd()
        os.chdir(self.dirname)
        stdout = "stdout.log"
        stderr = "stderr.log"
        process = subprocess.Popen(self.command, shell=True, stdout=open(stdout, "w"), stderr=open(stderr, "w"))
        status = process.wait()
        os.chdir(oldDir)

        errorMessage  = "'" + self.command + "' failed.\n"
        if self.includeStdOut == True:
            errorMessage += "stdout\n"
            errorMessage += "----------------------------------------------------------------------\n"
            errorMessage += file(os.path.join(self.dirname, stdout)).read()
            errorMessage += "\n"

        errorMessage += "stderr\n"
        errorMessage += "----------------------------------------------------------------------\n"
        errorMessage += file(os.path.join(self.dirname, stderr)).read()


        self.assertEqual(0, status, errorMessage)
