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
###############################################################################

import os
import wnsbase.RCS as RCS

def installLink(linksrc, linktarget):

    if not os.path.exists(linktarget) and os.path.exists(os.path.dirname(linktarget)):
        # Broken symlink?
        if os.path.lexists(linktarget):
            os.remove(linktarget)
        try:
            os.symlink(linksrc, linktarget)
        except:
            print "Cannot create symlink %s pointing to %s" % (linktarget, linksrc)
            print "Current working dir is %s" % os.getcwd()
            raise

def Stripper(something):
    """iterate over something, yielding stripped, non-empty elements, that don't start with a hash.
    """

    for it in something:
        it = it.strip()
        if len(it) and not it.startswith("#"):
            yield it

class ForEachResult:
    """ Helper class that stores the results of some command executed
    in a certain directory """
    def __init__(self, dirname, result):
        self.dirname = dirname
        self.result = result

class UserMadeDecision:
    """ This class asks the user for feedback. Invoke askForConfirmation if you want
    to offer yes as the default decision. Invoke askForReject if you want no to be
    the default decision."""
    def askForConfirmation(self, question):
        """ Returns true if user answers yes to the question"""
        answer = raw_input(question + " (Y/n) ")
        while answer.lower() not in  ["n", "y", '']:
            answer = raw_input(question + " (Y/n) ")
        return (answer.lower() == 'y') or (answer == '')

    def askForReject(self, question):
        """ Returns false if user answers yes to the question"""
        answer = raw_input(question + " (y/N) ")
        while answer.lower() not in  ["n", "y", '']:
            answer = raw_input(question + " (y/N) ")
        return (answer.lower() == 'n') or (answer == '')

    def askForChoice(self, question, alternativesDict, defaultKey):
        answer = None
        keys = alternativesDict.keys()
        while answer not in xrange(len(keys)):
            for k in keys:
                if k==defaultKey:
                    print "[%i] %s (default)" % (keys.index(k), str(k))
                else:
                    print "[%i] %s" % (keys.index(k), str(k))

            answer = raw_input(question + " ")

            if answer == '':
                answer = keys.index(defaultKey)
            else:
                try:
                    answer = int(answer)
                except ValueError:
                    answer = None
        return alternativesDict[keys[answer]]

    def askForInput(self, question, default=None):
        answer = None
        if default is not None:
            answer = raw_input(question + " [ " + str(default) + " ] ? ")
            if answer == "":
                answer = default
        else:
            answer = raw_input(question + " ? ")
            
        return answer

class AcceptDefaultDecision:
    """ This always accepts default decisions. AskForConfirmation always returns
    True. AskForReject always returns False."""
    def askForConfirmation(self, question):
        """ Always returns true """
        return True

    def askForReject(self, question):
        """ Always retruns true """
        return True

    def askForChoice(self, question, alternativesDict, defaultKey):
        return alternativesDict[defaultKey]

    def askForInput(self, question, default=None):
        raise "Don't know how to apply --noAsk to askForInput!"

class CommandQueue:
    """ A queue for commands. """
    def __init__(self):
        self.queue = []

    def append(self, option, opt, arg, parser, command):

        self.queue.append((command, arg, option))

    def run(self):
        for command, arg, option in self.queue:
            command(arg)

class Dict2Class:
    def __init__(self, dictionary):
        for key in dictionary.iterkeys():
            if not key.startswith("__"):
                self.__dict__[key] = dictionary[key]

def runCommand(command):
    fh = os.popen(command)
    line = fh.readline()
    while line:
        print line.strip('\n')
        line = fh.readline()
    return fh.close()

def checkForConflicts(projectDir):
    """Returns a list with confilct files"""
    result = []
    for (dirname, dirs, files) in os.walk(projectDir):
        # conflicts cannot occure in '{arch}' or ".arch-ids" directory
        if not "{arch}" in dirname and not ".arch-ids" in dirname:
            for f in files:
                if f.endswith("rej"):
                    result.append(os.path.join(dirname, f))
        else:
            dirs=[]

    return result

def checkForConflictsAndExit(projectDir):
    conflicts = checkForConflicts(projectDir)
    if len(conflicts) > 0:
        print "Error: %s has conflicts:" % (os.path.abspath(projectDir))
        print conflicts
        sys.exit(1)

class Tester:
    def __init__(self, project):
        self.project = project

    def python(self):
        return self.project.getExe() == 'python'

    def lib(self):
        return self.project.getExe() == 'lib'

    def bin(self):
        return self.project.getExe() == 'bin'

    def none(self):
        return self.project.getExe() is None

    def changed(self):
        changes = []
        for line in self.project.getRCS().status():
            if line.startswith('*') or line.strip(" ") == "":
                continue
            changes.append(line)
            
        if len(changes) > 0:
            return True
        return False

    def scons(self):
        return os.path.exists(os.path.join(self.project.getDir(), 'SConstruct'))

    def ask(self):
        userFeedback = UserMadeDecision()
        return userFeedback.askForConfirmation("Execute command for project " + self.project.getDir() + " ?")

    def tla(self):
        if isinstance(self.project.getRCS(), RCS.GNUArch):
            return True

    def bzr(self):
        if isinstance(self.project.getRCS(), RCS.Bazaar):
            return True
