import os
import shutil
import subprocess
import wnsbase.playground.plugins.Command
from wnsbase.playground.Tools import *
import wnsbase.playground.Core
core = wnsbase.playground.Core.getCore()

class PyCoDocumentationCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog pycodocu\n\n"
        rationale = "Creates documentation for the PyConfig configuration tree"

        usage += rationale

        usage += """

EpyDoc is used to create the documentation of all PyConfig classes located under
sandbox/FLAVOUR/lib/PyConfig. You can find the documentation in sandbox/FLAVOUR/PyCoDoc.

Note: EpyDoc currently produces a lot of warnings.
"""
        wnsbase.playground.plugins.Command.Command.__init__(self, "pycodocu", rationale, usage)

        self.optParser.add_option("", "--flavour",
                                  type="string", dest = "flavour", metavar = "TYPE", default = "dbg",
                                  help = "choose a flavour (TYPE=[dbg|opt|prof|...]) to operate with.")

    def startup(self, args):
        wnsbase.playground.plugins.Command.Command.startup(self, args)

        errorOccured = False
        # Needs epydoc and dot
        try:
            print "Checking EpyDoc Version"
            print ""
            retcode = subprocess.check_call(['epydoc', '--version'])
            if retcode != 0:
                print "Cannot determine epydoc version. Is it installed?"
                errorOccured = True
        except (OSError, subprocess.CalledProcessError):
            print "Cannot find epydoc."
            errorOccured = True

        print ""
        try:
            print "Checking dot version"
            print ""
            retcode = subprocess.check_call(['dot', '-V'])
            if retcode != 0:
                print "Cannot determine dot version. Is it installed?"
                errorOccured = True
        except OSError:
            print "Cannot find dot."
            errorOccured = True

        if errorOccured == True:
            print "You need to have the doxygen and dot tool installed."
            print ""
            print "Try:"
            print ""
            print "  apt-get install python-epydoc"
            print "  apt-get install graphviz"
            print ""
            exit(1)

    def run(self):

        print "Install PyConfig"
        command = "scons %sPyConfig" % self.options.flavour
        result = runCommand(command)
        if not (result is None):
            print "Could not execute %s" % command

        command = "touch sandbox/%s/lib/PyConfig/__init__.py" % self.options.flavour
        result = runCommand(command)
        if not (result is None):
            print "Could not execute %s" % command

        destination = "sandbox/default/doc/api/PyCoDoc"
        package = "sandbox/%s/lib/PyConfig/" % self.options.flavour

        print "Creating Python Documentation"
        if os.path.exists(destination):

            shutil.rmtree(destination)
        os.makedirs(destination)

        command = "PYTHONPATH=$PYTHONPATH:%s epydoc -o %s --html --show-sourcecode --simple-term --name=openWNS --graph=all %s" % (package, destination, package)
        result = runCommand(command)
        if not (result is None):
            print "Could not execute %s" % command

        command = "rm -f sandbox/%s/lib/PyConfig/__init__.py" % self.options.flavour
        result = runCommand(command)
        if not (result is None):
            print "Could not execute %s" % command

