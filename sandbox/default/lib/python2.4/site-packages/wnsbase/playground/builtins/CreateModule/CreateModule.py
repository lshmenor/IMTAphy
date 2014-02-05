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
import shutil
import subprocess
from wnsbase import FilePatcher

#from wnsbase.playground.Tools import *

import wnsbase.rcs.Bazaar

import wnsbase.playground.Core
import wnsbase.playground.plugins.Command
core = wnsbase.playground.Core.getCore()

class CreateModuleCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog createModule TARGET\n\n"
        rationale = "Create a new module from project ModuleTemplate."

        usage += rationale
        usage += """Use this command to create your new module
                 """

        wnsbase.playground.plugins.Command.Command.__init__(self, "createmodule", rationale, usage)

    def initConfigFile(self):
        print "You have not yet configured CreateModule. Now initializing"

        c = core.getConfig()
        c.add_section("builtin.CreateModule")

        output = subprocess.Popen(["bzr", "whoami"], stdout=subprocess.PIPE).communicate()[0].rstrip()

        defaultMaintainer = core.userFeedback.askForInput("Who will be the default maintainer of a new module?", output)

        dbl = "bzr://bazaar.comnets.rwth-aachen.de/" + str(os.environ['USER']).upper()

        defaultBranchLocation = core.userFeedback.askForInput("Where will be the default branch location", dbl)

        c.set("builtin.CreateModule", "defaultBranchLocation", defaultBranchLocation)

        c.set("builtin.CreateModule", "defaultMaintainer", defaultMaintainer)

        core.updateConfig()
        
    def run(self):

        c = core.getConfig()

        if not c.has_section("builtin.CreateModule"):
            self.initConfigFile()

        moduleName = core.userFeedback.askForInput("What is the name of your module", "ProjModule")

        branchLocation = core.userFeedback.askForInput("Where should the branch be pushed", c.get("builtin.CreateModule", "defaultBranchLocation")) 

        sdkLocation = core.userFeedback.askForInput("Where should the files be locate in the SDK", "./modules/dll/")

        maintainer = core.userFeedback.askForInput("Who is the maintainer of the module", c.get("builtin.CreateModule", "defaultMaintainer"))
        template = core.userFeedback.askForChoice("Which template should be used?",
                                                  {"moduleTemplate" : "moduleTemplate",
                                                   "binaryTemplate" : "binaryTemplate",
                                                   "systemTestTemplate" : "systemTestTemplate"},
                                                  "binaryTemplate")

        dest = os.path.join(sdkLocation, moduleName)

        self._copyTemplate(dest, template)
        
        self._initBranch(dest, template)

        self._patchProject(moduleName, branchLocation, dest, maintainer, template)
        
        self._appendToProjectsPy(moduleName, branchLocation, dest, template)

        core.projects = core.readProjectsConfig(core.projectsFile)

        core.fixConfigurationLinks()

        pushTarget = "%s/%s" % (branchLocation, moduleName)

        if not core.userFeedback.askForReject("Do you want to push the project to %s" % pushTarget):
            # Push initial version
            self._pushProject(dest, pushTarget)


    def _copyTemplate(self, sdkLocation, template):

        orig = os.path.join(core.getPathToSDK(), "wnsbase", "playground", "builtins", "CreateModule", template)

        shutil.copytree(orig, sdkLocation)

    def _initBranch(self, destination, template):

        curdir = os.getcwd()

        os.chdir(destination)

        try:
            if template == "systemTestTemplate":
                os.symlink(os.path.join(core.getPathToSDK(), 'sandbox', 'dbg', 'bin', 'openwns'),
                           'openwns')
                os.symlink(os.path.join(core.getPathToSDK(), 'sandbox', 'opt', 'bin', 'openwns'),
                           'fast-openwns')
            else:
                os.symlink(os.path.join(core.getPathToSDK(), 'SConscript'),
                           'SConscript')
        except OSError:
            pass

        subprocess.check_call(["bzr", "init"])

        subprocess.check_call(["bzr", "add"])

        subprocess.check_call(["bzr", "commit", "-m", "'Initial version from template'"])

        os.chdir(curdir)

    def _patchProject(self, moduleName, branchLocation, dest, maintainer, template):

        if template == "moduleTemplate":
            self._patchModuleTemplate(moduleName, branchLocation, dest, maintainer)

        if template == "binaryTemplate":
            self._patchBinaryTemplate(moduleName, branchLocation, dest, maintainer)

    def _patchBinaryTemplate(self, moduleName, branchLocation, dest, maintainer):

        p = FilePatcher.FilePatcher(os.path.join(dest, "MAINTAINER"), "John Doe <jdoe@doh.no>", maintainer).replaceAll()

        filesToPatch = [os.path.join(dest, "README"),
                        os.path.join(dest, "SConscript"),]

        for filename in filesToPatch:

            p = FilePatcher.FilePatcher(filename,
                                        "projname",
                                        moduleName.lower(),
                                        ignoreCase=False
                                        ).replaceAll()

        curdir = os.getcwd()

        os.chdir(dest)
        
        subprocess.check_call(["bzr", "ignore", ".objs", ".sconsign.dblite", "build", "include", "config/private.py", "config/pushMailRecipients.py"])
 
        subprocess.check_call(["bzr", "commit", "-m", "'Applied your settings to the template'"])

        os.chdir(curdir)

    def _patchModuleTemplate(self, moduleName, branchLocation, dest, maintainer):

        p = FilePatcher.FilePatcher(os.path.join(dest, "MAINTAINER"), "John Doe <jdoe@doh.no>", maintainer).replaceAll()

        filesToPatch = [os.path.join(dest, "config", "libfiles.py"),
                        os.path.join(dest, "src", "ProjNameModule.hpp"),
                        os.path.join(dest, "src", "ProjNameModule.cpp"),
                        os.path.join(dest, "src", "SimulationModel.hpp"),
                        os.path.join(dest, "src", "SimulationModel.cpp"),
                        os.path.join(dest, "PyConfig", "projname", "simulationmodel.py"),
                        os.path.join(dest, "PyConfig", "projname", "__init__.py")]

        for filename in filesToPatch:

            p = FilePatcher.FilePatcher(filename,
                                        "PROJNAME",
                                        moduleName.upper(),
                                        ignoreCase=False
                                        ).replaceAll()

            p = FilePatcher.FilePatcher(filename,
                                        "ProjName",
                                        moduleName,
                                        ignoreCase=False
                                        ).replaceAll()

            p = FilePatcher.FilePatcher(filename,
                                        "projname",
                                        moduleName.lower(),
                                        ignoreCase=False
                                        ).replaceAll()


        curdir = os.getcwd()

        os.chdir(dest)
        
        subprocess.check_call(["bzr", "mv", "PyConfig/projname", "PyConfig/%s" % moduleName.lower() ])

        subprocess.check_call(["bzr", "mv", "src/ProjNameModule.hpp", "src/%sModule.hpp" % moduleName ])

        subprocess.check_call(["bzr", "mv", "src/ProjNameModule.cpp", "src/%sModule.cpp" % moduleName ])

        subprocess.check_call(["bzr", "ignore", ".objs", ".sconsign.dblite", "build", "include", "config/private.py", "config/pushMailRecipients.py"])
 
        subprocess.check_call(["bzr", "commit", "-m", "'Applied your settings to the template'"])

        os.chdir(curdir)


    def _appendToProjectsPy(self, moduleName, branchLocation, destination, template):

        kind = ""
        if template == "moduleTemplate":
            kind = "Library"

        if template == "binaryTemplate":
            kind = "Binary"

        if template == "systemTestTemplate":
            kind = "SystemTest"

        tmp = ""
        for ch in moduleName:
            if ch.isalnum():
                tmp += ch
        moduleName = tmp
        entry =  "\n"
        entry += "%s = %s('%s','%s', '%s',\n" % (moduleName, kind, destination, moduleName, branchLocation)
        entry += "                 RCS.Bazaar('%s',\n" % (destination)

        if kind == "Library":
            entry += "                            '%s', '%s', '%s'), '%s')\n" % ( moduleName, 'deprecated', 'deprecated', moduleName.upper())
        else:
            entry += "                            '%s', '%s', '%s'))\n" % ( moduleName, 'deprecated', 'deprecated')
        
        sourceName = os.path.join(core.getPathToSDK(), 'config', 'projects.py')
        origPPy = open(sourceName, "a")

        origPPy.write(entry)
        origPPy.write("all.append(%s)\n" % moduleName)
        origPPy.close()

    def _pushProject(self, destination, target):

        curdir = os.getcwd()

        os.chdir(destination)
        
        subprocess.check_call(["bzr", "push", target])

        os.chdir(curdir)
