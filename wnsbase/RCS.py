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
import posix
import commands
import subprocess
import StringIO

from rcs.RCSInterface import RCS
from rcs.Output import *
from rcs.Bazaar import Bazaar

class No(RCS):
	def __init__(self, category="Unknown", branch="Unknown", revision="Unknown", patchLevel="Unknown"):
		self.category = category
		self.branch = branch
		self.revision = revision
		self.patchLevel = patchLevel
		self.version = '--'.join([self.category, self.branch, self.revision])
		self.FQRN = 'local/'+self.revision+'--patch-'+self.patchLevel

        def setPath(self, path):
                pass

        def getPath(self):
                pass

        def missing(self, url, switches={}, revision=""):
                pass

        def status(self, switches={}):
                pass

        def lint(self):
                pass

        def update(self, fromRepository=""):
                pass

        def merge(self, fromRepository=""):
                pass

        def get(self, url):
                pass

        def push(self, url):
                pass

        def getFQRN(self):
                return self.FQRN

        def getTreeVersion(self):
                return self.FQRN

        def getVersion(self):
                return self.version

        def getCategory(self):
                return self.category

        def getBranch(self):
                return self.branch

        def getRevision(self):
                return self.revision

        def getPatchLevel(self):
                return self.patchLevel

        def isPinned(self):
                return False

        def getPinnedPatchLevel(self):
                pass

class GNUArch(RCS):
	def __init__(self, path, category, branch, revision, pinnedPatchLevel=None):
		# we prefer "tla" over "baz"
		if commands.getstatusoutput("type tla")[0] == 0:
			self.arch = "tla"
		elif commands.getstatusoutput("type baz")[0] == 0:
			self.arch = "baz"
		else:
			raise ("Found none of the follwing supported GNUArch binaries: [tla, baz]")

		self.setPath(path)
		self.category = category
		self.branch = branch
		self.revision = revision
		self.version = self.category + "--" + self.branch + "--" + self.revision

		self.pinnedPatchLevel = pinnedPatchLevel
		self.patchLevel = None

	def setPath(self, path):
		self.path = os.path.abspath(path)

	def getPath(self):
		return self.path

	def __startup(self):
		self.treeVersion = self.getTreeVersion()
		self.category = self.getCategory()
		self.branch = self.getBranch()
		self.patchLevel = self.getPatchLevel()
		self.treeRoot = self.getTreeRoot()

	def __exec(self, command, switches, args):
		cwd = os.path.abspath(os.curdir)

		if (not os.path.exists(self.path)):
			if (not command == "get"):
				raise ProjectPathError("Error executing tla %s. The project path %s does not exist." % (command, self.path))
		else:
			os.chdir(self.path)

		switchString = [it + " " + switches[it] for it in switches]
		finalCommand = (" ").join([self.arch, command] + switchString + args)

		p = subprocess.Popen(finalCommand, shell=True,
				     stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=True)

		returnValue = Output(p.stdout, p.stderr)
		p.wait()
		os.chdir(cwd)
		return returnValue

	def missing(self, rcsUrl, switches = {}, version = ""):
		self.__startup()
		switches["-d"] = self.path
	        return self.__exec("missing", switches, [version])

	def status(self, switches = {}):
		self.__startup()

		args = []
	        if self.arch == "tla":
			command = "changes"
			switches["-d"] = self.path
		elif self.arch == "baz":
		        command = "status"
			args.append("self.path")
		return self.__exec(command, switches, args)

	def commit(self, switches = {}, files = []):
		self.__startup()

		switches["-d"] = self.path
		if len(files) > 0:
			files = ["--"] + files
		return self.__exec("commit", switches, files)

	def update(self, fromRepository=""):
		self.__startup()

		return self.__exec("update", {"-d":self.path}, [])

        def merge(self, fromRepository=""):
                return self.update()

	def ancestryGraph(self, switches = {}, specialRevision=""):
		self.__startup()

		switches["-d"] = self.path
		return self.__exec("ancestry-graph", switches, [specialRevision])

	def replay(self, specialRevision="", switches={}):
		self.__startup()

		switches["-d"] = self.path
		return self.__exec("replay", switches, [specialRevision])

	def starMerge(self, revision, switches={}):
		self.__startup()

		switches["-d"] = self.path
		return self.__exec("star-merge", switches, [revision])

	def tag(self, newBranch):
		self.__startup()

		return self.__exec("tag", {"--setup":""}, [self.getTreeVersion(), newBranch])

	def get(self, url):

		if self.pinnedPatchLevel != None:
			if self.patchLevel == 0:
				return self.__exec("get", {}, [url + "--base-" + str(self.pinnedPatchLevel), self.path])
			else:
				return self.__exec("get", {}, [url + "--patch-" + str(self.pinnedPatchLevel), self.path])

		return self.__exec("get", {}, [url, self.path])

        def push(self, url):
                pass

	def lint(self, options = {}):
		self.__startup()

		if self.arch == "tla":
			command = "tree-lint"
		elif self.arch == "baz":
			command = "lint"
		return self.__exec(command, options, [self.path])

	def getTreeVersion(self):
		switch = ""
		if self.arch == "baz":
			switch = "-d"
		return str(self.__exec("tree-version", {switch : self.path}, []))

	def getArchive(self):
		self.__startup()

		return self.treeVersion.split('/')[0]

	def getVersion(self):
		return self.version

	def getRevision(self):
		return self.revision

	def getBranch(self):
		return self.branch

	def getCategory(self):
		return self.category

	def getPatchLevel(self):
		tmp = str(self.__exec("logs", {"-r" : "", "-d" : self.path}, [])).split("\n")[0]
		patchLevel = tmp.replace("patch-", "")
		patchLevel = patchLevel.replace("base-", "")
		return patchLevel

	def getFQRN(self):
		if self.pinnedPatchLevel != None:
			self.patchLevel = self.pinnedPatchLevel

		treeVersion = self.getTreeVersion()

		if self.patchLevel == None:
			return treeVersion

		if self.patchLevel == 0:
			return treeVersion + '--base-'+ str(self.patchLevel)
		else:
			return treeVersion + '--patch-'+ str(self.patchLevel)

	def getTreeRoot(self):
		return str(self.__exec("tree-root " + self.path, {}, []))

	def cacheRev(self):
		self.__startup()

		return self.__exec("cacherev ", {}, [self.FQRN])

	def getCachedRevs(self):
		self.__startup()

		return self.__exec("cachedrevs ", {}, [self.treeVersion])

	def logs(self):
		self.__startup()

		return self.__exec("logs ", {}, [])

	def catlog(self,revision):
		self.__startup()

		return self.__exec("cat-log ",{}, [revision])

	def isPinned(self):
		return self.pinnedPatchLevel != None

	def getPinnedPatchLevel(self):
		return self.pinnedPatchLevel

class LogParser:
    """This class can parse the output of the tla cat-log <revision-spec> command and make
    its different fields available in a dict. The most commonly wanted fields can be retrieved
    via access methods.
    """

    def creator(self):
        """returns the name of the creator of the patch"""
        return self.contents['Creator'][0]

    def date(self):
        """returns the creation date of the patch"""
        return self.contents['Date'][0]

    def patch(self):
        """returns the revision of the patch"""
        return self.contents['Revision'][0]

    def dump(self):
        """debug method. prints all data that was read from the patchlog"""
        print self.contents

    def fileChanged(self, fileName):
        """returns True if the string fileName was anywhere among the removed, modified or new files."""
        keys = [ 'Removed-files', 'Modified-files','New-files' ]
        changed = False
        for k in keys:
            if self.contents.has_key(k):
                files = self.contents[k]
                for f in files:
                    if f.count(fileName):
                        changed = True
        return changed

    def __init__(self, patchLog):
        """Constructor, requires the patchlog as an iterable data structure (file object)"""
        self.contents = {}

        for line in patchLog:
            if line.strip()=='':
                return

            if not line.startswith('    '):
                keyword = line.split(':')[0]
                fieldContent = line.lstrip(keyword+':').strip()
            else:
                fieldContent = line.strip()

            if self.contents.has_key(keyword):
                self.contents[keyword].append(fieldContent)
            else:
                self.contents[keyword] = [fieldContent]
