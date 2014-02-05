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
import os
import subprocess
import optparse
import re
if sys.version_info < (2, 4):
    from sets import Set as set
import exceptions
import ConfigParser

import wnsbase.rcs.Bazaar

from wnsbase.playground.Tools import *

import builtins
import plugins.Command
import Logger
import sys

class Core:
    """ This the core of the openwns-sdk project tree management tool 'playground.py'.

    Core is basically a plugin loader. It implements just a few commands by itself.
    """

    def __init__(self):
        """ Initialization of members. No other functionality.
        """
        usage = ""
        usage += "The list below shows global available options.\n"

        self.optParser = optparse.OptionParser(usage = usage)
        self.plugins = []
        self.hooks = {}
        self.pluginPaths = []
        self.commands = {}
        self.ifExpr = None
        self.warnings = []
        self.command = None

        major = int(sys.version_info[0])
        minor = int(+sys.version_info[1])
        if minor > 5:
            self.addPluginPath(os.path.join("/usr","local","lib", "python%d.%d" % (major, minor), "dist-packages", "openwns", "wrowser", "playgroundPlugins"))
            self.addPluginPath(os.path.join("/usr","lib", "python%d.%d" % (major, minor), "dist-packages", "openwns", "wrowser", "playgroundPlugins"))
        else:
            self.addPluginPath(os.path.join("/usr","lib", "python%d.%d" % (major, minor), "site-packages", "openwns", "wrowser", "playgroundPlugins"))


    def startup(self):
        """ Loads builtins, plugins and configuration. Setup of the project tree.

        The startup phase loads builtin commands and all plugins. You can add paths
        to search for plugins by using the addPluginPath method. The command line is
        parsed and some global switches are extracted before the command line is
        passed to the selected command.

        The command to execute is selected by the first argument of the stripped
        command line. This means you can place the global options anywhere on the
        command line. Ensure that the first non-global is a valid command name. If
        no command is found the help text is shown to the user.

        After builtins, plugins and the command line has been processed Core reads
        the project configuration at "config/projects.py" and checks if all projects
        are available. If not missing projects will be fetched.

        Finally the preReq commands in projects.py are executed.
        """

        self.logger = Logger.Logger("Core", Logger.Silent)

        argv = sys.argv

        self.printCommands = False

        self.projectsFile = "config/projects.py"

        self.userFeedback = UserMadeDecision()

        self.pluginArgs = []
        i = 1
        while i < len(argv):
            a = argv[i]
            if a.startswith('--configFile'):
                self.projectsFile = a.split("=")[1]
            elif a == "--noAsk":
                self.userFeedback = AcceptDefaultDecision()
            elif a.startswith("--if"):
                self.ifExpr = a.split("=")[1]
            elif a == "--commands":
                self.printCommands = True
            elif a == "--debug":
                self.logger = Logger.Logger("Core", Logger.Debug)
            elif a == "--warnings":
                self.logger = Logger.Logger("Core", Logger.Warning)
            else:
                self.pluginArgs.append(a)
            i += 1

        if os.path.exists(os.environ["HOME"]):
            self.configFile = os.path.join(os.environ["HOME"], ".wns", "playground.config")
        else:
            self.configFile = os.path.join("/tmp/%s_playground.config" % os.environ["HOME"].replace(os.sep, "_"))

        self.projects = None

        # Will be set if commands need a second projects configuration
        # For example: missing and upgrade from another branch
        self.otherProjects = None

        self._loadConfigFile()
        self.addPluginPath("./wnsbase/playground/plugins")
        # Read and add additional paths from the user's config
        c = self.getConfig()
        if c.has_section("AdditionalPluginPaths"):
            for (k,p) in c.items("AdditionalPluginPaths"):
                if(os.access(p, os.F_OK)):
                    self.addPluginPath(p)
        self._loadBuiltins()
        self._setupCommandLineOptions()
        self._loadPlugins()

        if (self.printCommands == True):
            print " ".join(self.commands.keys())
            self.shutdown(0)

        if len(self.pluginArgs) > 0:
            commandName = self.pluginArgs[0]
            if not self.commands.has_key(commandName):
                if not self.pluginArgs[0] == "--help":
                    print "\nERROR: Unknown Command %s" % commandName
                self.printUsage()
            else:
                self.command = self.commands[commandName]
                self.command.startup(self.pluginArgs[1:])
        else:
            self.printUsage()

        if (self.projectsFile == "config/projects.py"):
            if not os.path.exists(self.projectsFile):
                self._process_hooks("_projects_py_not_available")
            # if there is no hook we need to make a symlink to the template
            if not os.path.exists(self.projectsFile):
                os.symlink('projects.py.template', "config/projects.py")
        else:
            if not os.path.exists(self.projectsFile):
                print "Cannot open configuration file " + str(self.projectsFile)

        self.projects = self.readProjectsConfig(self.projectsFile)

        missingProjects = self.checkForMissingProjects()

        self.updateMissingProjects(missingProjects)

        # install necessary files
        # must happen after missing projects ...
        for preCommand, sourcePath in self.getProjects().prereqCommands:
            savedDir = os.getcwd()
            os.chdir(sourcePath)

            p = subprocess.Popen(preCommand, shell=True,stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, close_fds=True)
            (stdin, stdout) = (p.stdin, p.stdout)
            line = stdout.readline()
            while line:
                line = stdout.readline()
            os.chdir(savedDir)

    def run(self):
        """ Runs the command selected in the startup phase.
        """
        try:
            self.command.run()
        except wnsbase.rcs.Bazaar.BzrException, bzrException:
            print ""
            print "Error! Bazaar reports:"
            print ""
            print bzrException

    def shutdown(self, returnCode):
        """ Shutdown the core. Shutdown the selected command.
        """
        if not self.command is None:
            self.command.shutdown()
        for warning in self.warnings:
            print warning
        sys.exit(returnCode)

    def printUsage(self):
        """ Show the help text.

        Shows the help text for all commands and for the global switches
        """

        print "\nUsage : playground COMMAND options"
        print "\n\nYou can use one of following commands. Use COMMAND --help to get"
        print "detailed help for the command\n\n"
        for commandname, command in sorted(self.commands.items(), lambda x, y: cmp(x[0], y[0])):
            print "   " + commandname.ljust(20) + ":\t" + command.rationale

        print "\n\nThere are some global options that are available for all commands"
        self.optParser.print_help()

        self.shutdown(1)

    def addPluginPath(self, path):
        """ Add a path to search for plugins.

        During the startup phase all directories added in this way are scanned
        for plugins.
        """
        self.pluginPaths.append(path)

    def setPathToSDK(self, pathToSDK):
        self.pathToSDK = pathToSDK

    def getPathToSDK(self):
        return self.pathToSDK

    def _loadConfigFile(self):
        if not os.path.exists(self.configFile):
            f = open(self.configFile, "w")
            f.write("# Configuration file of playground\n")
            f.close()
        self.configParser = ConfigParser.SafeConfigParser()
        self.configParser.read(self.configFile)

    def _loadBuiltins(self):
        """ Load builtins.
        """
        self._loadPluginsInDir("./wnsbase/playground/builtins", "wnsbase.playground.builtins")

    def _loadPlugins(self):
        """ Load plugins.
        """
        for pluginPath in self.pluginPaths:
            self._loadPluginsInDir(pluginPath)

    def _loadPluginsInDir(self, pluginsDir, targetPackage="wnsbase.playground.plugins"):
        self.logger.logDebug("Loading plugins in dir : %s" % pluginsDir)
        """ Load plugins in a specific directory.

        pluginsDir : The directory to scan
        targetPackage : Inject the plugin into this package. Package must exist and
        must be importable.
        """
        if os.path.exists(pluginsDir):
            plugins.__path__.append(str(pluginsDir))
            for (dirname, topLevelDirs, files) in os.walk(pluginsDir):
                break

            if topLevelDirs.count('.arch-ids') > 0:
                topLevelDirs.remove('.arch-ids')

            sys.path.append(pluginsDir)
            for plugin in topLevelDirs:
                try:
                    if str(plugin) in plugins.__dict__.keys():
                        self.logger.logWarning("WARNING: Unable to load plugin '%s' from '%s/%s'." % (plugin, pluginsDir, plugin))
                        self.logger.logWarning("A package with this name already exists. You should rename")
                        self.logger.logWarning("the containing directory to some other name")
                    self.logger.logDebug("Loading plugin %s" % plugin)
                    exec "import %s.%s" % (targetPackage, str(plugin)) in globals(), locals()
                except exceptions.SystemExit:
                    self.shutdown(1)
                except:
                    tmpWarning = "WARNING: Unable to load '" + str(plugin) + "' plugin from '" + pluginsDir + "'. Ignored."
                    tmpWarning += "\n   " + str(sys.exc_info()[0])
                    tmpWarning += "\n   " + str(sys.exc_info()[1])
                    tmpWarning += "\n   " + str(sys.exc_info()[2].tb_frame)
                    self.warnings.append(tmpWarning)

            sys.path.pop()

    def hasPlugin(self, pluginName):
        """ Returns True if Core has loaded the plugin pluginName.

        pluginName : Name of the plugin (string)
        Returns True if plugin was loaded, else returns False
        """
        return (self.plugins.count(pluginName) > 0)

    def registerPlugin(self, pluginName):
        """ Register a plugin.

        Each plugin must register itself with a unique name to avoid duplicates. You
        should us hasPlugin in your plugin to test if the plugin has already been
        loaded. If it has not been loaded use registerPlugin to register it and then
        completely initialise your plugin.

        pluginName : The plugin to register (string)
        """
        if self.plugins.count(pluginName) > 0:
            print "Error! Pluging %s already registered." % pluginName
            print
            print "This could happen if you have a plugin installed to several places that are"
            print "read by playground. Some plugin did violate the hasPlugin/registerPlugin"
            print "protocol of playground."
            self.shutdown(1)
        else:
            self.plugins.append(pluginName)

    def registerCommand(self, command):
        """ Register a command.

        Plugins may register an arbitrary number of commands.
        Use this method to register a new command. Each command must implement
        wnsbase.playground.plugins.Command. Duplicate registration of commands
        is an error.

        command : The command to register (wnsbase.playground.plugins.Command)
        """
        if self.commands.has_key(command.name) > 0:
            print "Error! Command %s already registered." % command.name
            print
            print "This could happen if you have a plugin installed to several places that are"
            print "read by playground or if two plugins try to register a command with the same"
            print "name."
            print
            print "Note : You can use wnsbase.playground.Core.getCore().replaceCommand(command)"
            print "to replace an exisiting command"
            self.shutdown(1)
        else:
            self.commands[command.name] = command

    def replaceCommand(self, command):
        """ Replace an exisiting command.

        Plugins may replace already exisiting commands.
        Use this method to replace an exisiting command. Each command must implement
        wnsbase.playground.plugins.Command. Replacing a command that is was not registered before
        is an error.
        You can force to not allow command replacement by adding disableCommandReplacement to the
        core section of your playground.config: This looks like this:

        [core]
        disableCommandReplacement = True
        """
        if self.getConfig().has_option("core", "disableCommandReplacement"):
            if self.getConfig().get("core", "disableCommandReplacement") == "True":
                self.logger.logWarning("Warning! The replacement of command %s was disabled" % command.name)
                return

        if not self.commands.has_key(command.name) > 0:
            self.registerCommand(command)
        else:
            self.commands[command.name] = command

    def installHook(self, hookname, callable):
        if not self.hooks.has_key(hookname):
            self.hooks[hookname] = []
        self.hooks[hookname].append(callable)

    def _process_hooks(self, hookname):
        if self.hooks.has_key(hookname):
            for callable in self.hooks[hookname]:
                callable()

    def getConfig(self):
        """ Return the global config parser
        """
        return self.configParser

    def updateConfig(self):
        """ Write the config file to disk and reparse it.
        """
        f = open(self.configFile, "w")
        self.configParser.write(f)
        f.close()
        self._loadConfigFile()

    def getProjects(self):
        """ Get the parsed contents of config/projects.py

        returns projects
        """
        return self.projects

    def checkForMissingProjects(self):
        """Iterate over all projects return a list of missing directory names.

        returns a list of missing projects ([ wnsbase.playground.Project.Project ])
        """

        return [project for project in self.projects.all if not os.path.isdir(project.getDir())]

    def updateMissingProjects(self, missingProjects):
        """Fetch missing projects.

        Projects that are listed in projects.py but not found in the file system will
        be fetched.

        missingProjects : Projects to fetch ( [wnsbase.playground.project.Project, ...] )
        """
        if not len(missingProjects):
            return

        print "Warning: According to '%s' the following directories are missing:" % (self.projectsFile)
        for project in missingProjects:
            print "  " + project.getDir() + " (from URL: " + project.getRCSUrl() + ")"

        if self.userFeedback.askForConfirmation("Try fetch the according projects?"):
            self.getMissingProjects(missingProjects)
        else:
            print "Not trying to fetch missing projects."
            print "Exiting"
            self.shutdown(0)

    def getMissingProjects(self, missingProjects):
        """For each project in missingProjects, try to resolve the archive and retrieve it.

        missingProjects : Projects to fetch ( [wnsbase.playground.project.Project, ...] )
        """

        for project in missingProjects:

            basedir = os.path.abspath(os.path.join(project.getDir(), ".."))

            if project.alias != None:
                newLink = os.path.join(basedir, project.alias)
                if os.path.exists(newLink) or os.path.islink(newLink):
                    print "\nError: For this project I need to make a symlink!!"
                    print "Symlink would be: " + project.getDir() + " -> "  + project.alias + " in " + os.getcwd()
                    print "Error: " + project.alias + " already exists. Please move it out of the way."
                    print "Run '" + " ".join(sys.argv) + "' afterwards again."
                    self.shutdown(1)

            print "Fetching project: " + project.getRCSUrl()

            try:
                project.getRCS().get(project.getRCSUrl())
            except wnsbase.rcs.Bazaar.BzrGetException, e:
                print "\n\n!!! Unable to retrieve project %s !!!\n\n" % project.getRCSUrl()
                print e
                sys.exit(1)

            if project.alias != None:
                curDir = os.getcwd()
                os.chdir(basedir)
                linkDest = project.getDir().split("/")[-1]
                os.symlink(linkDest, project.alias)
                os.chdir(curDir)

        self.fixConfigurationLinks()

    def fixConfigurationLinks(self):
        """ Symlink config.pushMailRecipients.py to its template if not found
        """
        self.foreachProject(self._linkPushMailRecipientsPy)

    def foreachProject(self, fun, **args):
        """ Execute a function on each project

        fun : Callable to execute in each directory
        args : Any arguments to pass to fun when it is called
        returns wnsbase.playground.Tools.ForEachResult
        """
        return self.foreachProjectIn(self.projects.all, fun, **args)

    def foreachProjectIn(self, projectList, fun, **args):
        """run function fun for each of the projects.

        change directory to each project directory, calling fun
        with the projects as parameter. You may use the --if option
        to control if a project is included.

        projectList : A list of projects to execute fun in ( [wnsbase.playground.Project.Project] )
        fun : Callable to execute in each directory
        args : Any arguments to pass to fun when it is called
        returns wnsbase.playground.Tools.ForEachResult
        """
        results = []
        for project in projectList:
            if not self.includeProject(project):
                continue

            cwd = os.path.abspath(os.curdir)
            os.chdir(project.getDir())

            results.append(ForEachResult(dirname = project.getDir(), result = fun(project, **args)))

            os.chdir(cwd)

        return results

    def includeProject(self, project):
        """ Check the --if switch and decide if a project is included.

        project : The project to test (wnsbase.playground.Project)
        """
        if self.ifExpr is None:
            return True

        # we evaluate --if expressions using the python eval function.
        # thus, any valid python expression may be used. python2.3 allows
        # us to specify a dictionary to be used as globals during evaluation.
        # that means, that we can build a dictionary with the test results
        # in advance, and use this dictionary later during evaluation.
        # to limit the test evaluation to tests that are only used in
        # the expression, we first search for used tests within the
        # expression string. this is done in the following lines. after that
        # we loop over the tokenized testNames and run the tests, filling
        # the context dictionary.
        #
        # this works, but is not very nice. python2.4 allows us to use
        # anything talking the getattr protocol to be used as context.
        # this would allow us to do real lazy evaluation of tests.
        # think of the following expression:
        #   --if="changed and ask"
        # using the current approach, the user would be asked for *every*
        # project. even if the 'changed' test fails for most of them...
        #
        # -> get python2.4 now, dude! \o/

        token = re.split('\W+', self.ifExpr)
        token = [it for it in token if it not in ('and', 'or', 'not', '')]
        token = set(token)

        context = {}
        for testName in token:
            tester = Tester(project)
            if not hasattr(tester, testName):
                print "Unknown test in --if expression:", testName
                self.shutdown(1)

            context[testName] = getattr(tester, testName)()

        try:
            return eval(self.ifExpr, context)
        except SyntaxError, e:
            print "Syntax error in --if expression:", e
            self.shutdown(1)

    def getDirectoryDepth(self, path):
        """ For a directory get the depth relative to the root of openwns-sdk

        path : Directory to check
        returns Relative depth the the root of openwns-sdk (int)
        """
        # calculate the depth with respect to the testbed dir
        normalizedPath = os.path.normpath(path).replace(self.getPathToSDK(), "")
        normalizedPath.lstrip("/")
        # by splitting at the slashes we can find the depth
        return len(normalizedPath.split(os.sep))

    def getRelativePathToPlayground(self, path):
        """ Get the relative path to the root of openwns-sdk for a subdir path.

        path : Directory to check
        returns pathname (str)
        """
        depth = self.getDirectoryDepth(path)
        return os.path.normpath(("/").join([".."]*depth))

    def readProjectsConfig(self, filename):
        """ Read the projects config file config/projects.py
        """
        self._process_hooks("_pre_parse_project")

        sys.path.append("config")
        foobar = {}
        execfile(filename, foobar)
        sys.path.remove("config")

        self._process_hooks("_post_parse_project")

        return Dict2Class(foobar)

    def _linkPushMailRecipientsPy(self, project):
        """ Possibly link config/pushMailRecipients.py in a project to the global openwns-sdk/config/pushMailRecipients.py

        If a project does not have a config/pushMailRecipients.py a link to the global openwns-sdk/config/pushMailRecipients.py
        is created.

        project : The project to check (wnsbase.playground.Project.Project)
        """
        linksrc = os.path.join(self.getRelativePathToPlayground(project.getDir()), "config", "pushMailRecipients.py")
        linktarget = os.path.join("config", "pushMailRecipients.py")

        installLink(linksrc, linktarget)

    def _setupCommandLineOptions(self):
        """ Setup the internal optParser
        """
        # modifying options
        self.optParser.add_option("-f", "--configFile",
                                  type="string", dest = "configFile", metavar = "FILE", default = "config/projects.py",
                                  help = "choose a configuration file (e.g., --configFile=config/projects.py)")
        self.optParser.add_option("", "--if",
                                  type="string", dest = "if_expr", metavar = "EXPR", default = None,
                                  help = "restrict commands to affect only projects that match EXPR (can be: 'python', 'bin', 'lib', 'none', 'changed', 'scons', 'ask', 'bzr', 'tla').")
        self.optParser.add_option("", "--noAsk",
                                  action = "store_true", dest = "noAsk", default = False,
                                  help = "Do not ask user. Accept all default answers. Use this for automation.")
        self.optParser.add_option("", "--commands",
                                  action = "store_true", dest = "noAsk", default = False,
                                  help = "Space separated list of available commands (use for Bash completion).")
        self.optParser.add_option("", "--debug",
                                  action = "store_true", dest = "noAsk", default = False,
                                  help = "Enable debug output. Also enables warnings.")
        self.optParser.add_option("", "--warnings",
                                  action = "store_true", dest = "noAsk", default = False,
                                  help = "Show warnings.")

theCore = Core()

def getCore():
    """ Get the one and only core object.
    returns wnsbase.playground.Core.Core
    """
    return theCore

