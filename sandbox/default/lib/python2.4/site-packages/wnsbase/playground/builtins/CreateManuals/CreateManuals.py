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

import wnsbase.playground.Core
import wnsbase.playground.Project
import shutil
import ConfigParser
import StringIO
import os.path
import re
import textwrap
import subprocess
import glob

from wnsbase.playground.builtins.CPPDocumentation.CPPDocumentation import prepareExamples

class CreateManualsCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog cppdocu\n\n"
        rationale = "Build the users manual and the developers guide."

        usage += rationale
        usage += """ Builds the users manual and the developers guide. HTML
and PDF output can be found in ./sandbox/default/doc.
"""
        wnsbase.playground.plugins.Command.Command.__init__(self, "createmanuals", rationale, usage)

        self.workingdir = '.createManualsWorkingDir'
    def startup(self, args):
        wnsbase.playground.plugins.Command.Command.startup(self, args)

        errorOccured = False
        # Needs make, pdflatex
        try:
            print "Checking make Version"
            print ""
            retcode = subprocess.check_call(['make', '-v'])
            if retcode != 0:
                print "Cannot determine make version. Is it installed?"
                errorOccured = True
        except (OSError, subprocess.CalledProcessError):
            print "Cannot find make."
            errorOccured = True

        try:
            print "Checking pdflatex Version"
            print ""
            retcode = subprocess.check_call(['pdflatex', '-version'])
            if retcode != 0:
                print "Cannot determine pdflatex version. Is it installed?"
                errorOccured = True
        except (OSError, subprocess.CalledProcessError):
            print "Cannot find pdflatex."
            errorOccured = True

        if errorOccured == True:
            print "You need to have the doxygen and dot tool installed."
            print ""
            print "Try:"
            print ""
            print "  apt-get install make"
            print "  apt-get install texlive-full"
            print ""
            exit(1)

    def run(self):
        core = wnsbase.playground.Core.getCore()
        print "Identifying projects for documentation ..."
        # find all documentation projects:
        docProjects = []
        masterDocumentationProject = None
        for project in core.getProjects().all:
            if isinstance(project, (wnsbase.playground.Project.Library,
                                    wnsbase.playground.Project.Binary,
                                    wnsbase.playground.Project.Documentation,
                                    wnsbase.playground.Project.SystemTest)):
                print "... found: " + project.getDir()
                docProjects.append(project)
                if isinstance(project, wnsbase.playground.Project.MasterDocumentation):
                    # we can have only 1 (in words: one) master documentation project
                    assert masterDocumentationProject == None
                    masterDocumentationProject = project

        # we need exactly one master documentation project
        assert masterDocumentationProject != None

        import doxygenParser
        p = doxygenParser.Parser()

        # remove old working dir
        shutil.rmtree(self.workingdir, True)
        os.mkdir(self.workingdir)

        prepareExamples(self.workingdir, docProjects)

        cur = os.getcwd()
        os.chdir(masterDocumentationProject.getDir())

        makefiles = [n for n in glob.glob("./Makefile*") if not n.endswith("~")]

        for makefile in makefiles:
            os.chdir(masterDocumentationProject.getDir())
            print "Cleaning..."
            print "make -f %s clean" % makefile
            retcode = subprocess.call("make -f %s clean" % makefile, shell=True)

            if retcode != 0:
                print "\n\nThere were errors during cleanup\n\n"
                exit(1)

            print "Generating HTML for %s" % makefile
            retcode = subprocess.call("make -f %s html" % makefile, shell=True)

            if retcode != 0:
                print "\n\nThere were errors during processing of documentation\n\n"
                exit(1)

            print "Generating PDF for %s" % makefile
            retcode = subprocess.call("make -f %s latex" % makefile, shell=True)

            if retcode != 0:
                print "\n\nThere were errors during processing of documentation\n\n"
                exit(1)

            os.chdir(os.path.join(masterDocumentationProject.getDir(), makefile.replace("Makefile", "build"), "latex"))

            retcode = subprocess.call("make all-pdf", shell=True)

            if retcode != 0:
                print "\n\nThere were errors when executin latex\n\n"
                exit(1)

        os.chdir(cur)

        if not os.path.exists("sandbox/default/doc"):
            os.makedirs("sandbox/default/doc")

        containedFiles = glob.glob("sandbox/default/doc/*") + glob.glob("sandbox/default/doc/.*")
        # Do not touch the api subdirectory. This is managed by CPPDocumentation command
        toBeDeleted = [f for f in containedFiles if not f.endswith("api")]

        for file in toBeDeleted:
            if os.path.isdir(file):
                shutil.rmtree(file)
            else:
                os.remove(file)

        for makefile in makefiles:
            builddir = makefile.replace("Makefile", "build")

            suffix = makefile.replace("Makefile", "").replace("./","")
            if suffix == "DevelopersGuide":
                suffix = ""

            if not os.path.exists("sandbox/default/doc/%s" % suffix):
                os.makedirs("sandbox/default/doc/%s" % suffix)
            # Install the guide to the sandbox's documentation directory
            source = os.path.join(masterDocumentationProject.getDir(), builddir, "html")
            toBeCopied = glob.glob(source + "/*") + glob.glob(source + "/.*")
            for file in toBeCopied:
                if os.path.isdir(file):
                    subdirname = os.path.basename(file)
                    shutil.copytree(file, "sandbox/default/doc/%s" % suffix + "/" + subdirname)
                else:
                    shutil.copy(file, "sandbox/default/doc/%s" % suffix)

            source = os.path.join(masterDocumentationProject.getDir(), builddir, "latex")
            pdfFiles = glob.glob(source + "/*.pdf")
            for pdf in pdfFiles:
                shutil.copy(pdf, "sandbox/default/doc/%s" % suffix)

def processFile(parser, path, fileName):
    curr = os.getcwd()
    fullFileName = os.path.join(path, fileName)
    fullFileName = fullFileName.replace(curr, ".")
    parser.parseFile(fullFileName)

def findDocumentation(parser, path):

    for root, dirs, files in os.walk(path):
        for f in files:
            if f.endswith('.hpp') or f.endswith('.cpp') or f.endswith('.txt'):
                processFile(parser, root, f)

class DoxygenConfigParser:
    """Parser for doxygen config files"""
    def __init__(self, filename):
        self.parser = ConfigParser.ConfigParser()
        text = open(filename).read()
        # the Python's ConfigParser expects the file to have
        # at least one section. Doxygen's config files don't
        # have such a section. Hence we prepend a dummy section
        newFileContent = StringIO.StringIO("[dummy]\n" + text)
        self.parser.readfp(newFileContent, filename)

    def get(self, parameter):
        """Provides access to the parameters of a doxygen config file"""
        return self.parser.get("dummy", parameter)

    def getSplit(self, parameter):
        """Provides access to the parameters of a doxygen config file (returns list of parameters)"""
        return self.parser.get("dummy", parameter).replace("\\", "").replace("\n", "").split()

    def options(self):
        return self.parser.options("dummy")

    def has_option(self, parameter):
        return self.parser.has_option("dummy", parameter)

    def set(self, parameter, value):
        self.parser.set("dummy", parameter, self.__listToString(value))

    def append(self, parameter, value):
        oldParameters = ""
        if self.has_option(parameter):
            oldParameters = self.get(parameter)

        self.set(parameter, oldParameters + " " + self.__listToString(value))

    def prepend(self, parameter, value):
        oldParameters = ""
        if self.has_option(parameter):
            oldParameters = self.get(parameter)

        self.set(parameter, self.__listToString(value) + " " + oldParameters)

    def __listToString(self, any):
        if not isinstance(any, str):
            return " ".join(any)
        else:
            return any
