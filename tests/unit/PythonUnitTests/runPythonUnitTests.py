#!/usr/bin/env python
import sys
import os
import unittest

sandboxPath = '../../../sandbox'

class SubdirWithTests:

    def __init__(self, pathToScan, pythonPath):
        self.pathToScan = pathToScan
        self.pythonPath = pythonPath

class PythonUnitTests:
    subdirs = [ 
                SubdirWithTests(sandboxPath + '/default/lib/python2.4/site-packages/wnsbase',
                                sandboxPath + '/default/lib/python2.4/site-packages'),

		SubdirWithTests(sandboxPath + '/dbg/lib/PyConfig',
                                sandboxPath + '/dbg/lib/PyConfig'),
                ]

    allTests = unittest.TestSuite()

    def __init__(self):
        for subdir in self.subdirs:
            sys.path.append(subdir.pythonPath)
            self.addSubdir(subdir)

    def addSubdir(self, subdir):

        for root, dirs, files in os.walk(subdir.pathToScan):
            if root.find('/tests') != -1:
                for fileName in files:
                    if fileName.endswith('py') and fileName != '__init__.py':
                        pathName = os.path.join(root, fileName)
                        self.addModule(subdir, pathName)

    def addModule(self, subdir, modulePath):

        moduleName = modulePath.replace(subdir.pythonPath + '/', '').replace('.py', '').replace('/', '.')
        mod = __import__(moduleName, globals(), locals(), [''])
        suite = unittest.defaultTestLoader.loadTestsFromModule(mod)
        self.allTests.addTest(suite)

    def runTests(self, v):
        return unittest.TextTestRunner(verbosity=v).run(self.allTests)

if "-v" in sys.argv:
    verbosity = 2
else:
    verbosity = 1

if PythonUnitTests().runTests(verbosity).wasSuccessful():
    sys.exit(0)
else:
    sys.exit(1)
