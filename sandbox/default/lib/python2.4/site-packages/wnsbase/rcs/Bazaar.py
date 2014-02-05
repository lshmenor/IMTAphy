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

from RCSInterface import RCS
from Output import Output
from ProjectPathError import ProjectPathError

import os
import subprocess
import StringIO

import bzrlib.branch

class BzrException(Exception):
	def __init__(self, errorcode, errormessage, command):
		self.errorcode = errorcode
		self.errormessage = errormessage
		self.command = command

	def __str__(self):
		return "--\nExecuted command:\n%s\nErrorcode %s (message follows):\n%s\n--\n" % (str(self.command), str(self.errorcode), str(self.errormessage))

class BzrGetException(BzrException):
	pass

class BzrMissingException(BzrException):
	pass

class BzrMergeNeededException(BzrException):
	pass

class Bazaar(RCS):
	def __init__(self, path, category, branch, revision, pinnedPatchLevel=None):
		self.cmd = "bzr"
		self.category = category
		self.branch = branch
		self.revision = revision
		self.pinnedPatchLevel = pinnedPatchLevel
		self.version = self.category + "--" + self.branch + "--" + self.revision

		self.setPath(path)

	def setPath(self, path):
		self.path = os.path.abspath(path)

	def getPath(self):
		return self.path

	def __exec(self, command, switches, args):
		cwd = os.path.abspath(os.curdir)
		if (not os.path.exists(self.path)):
			if (not command == "branch"):
				raise ProjectPathError("Error executing bzr %s. The project path %s does not exist." % (command, self.path))
		else:
			os.chdir(self.path)

		switchString = [it + "=" + switches[it] for it in switches]

		finalCommand = (" ").join([self.cmd, command] + switchString + args)

		p = subprocess.Popen(finalCommand, shell=True,
				     stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=True)

		returnValue = Output(p.stdout, p.stderr)
		p.wait()
		os.chdir(cwd)
		if p.returncode != 0:
			raise BzrException(p.returncode, str(returnValue), finalCommand)
		return returnValue

	def _updateVersionInfo(self):
		foobar = {}
		output = self.__exec("version-info", {"--format":"python"}, [])
		exec(str(output), foobar)
		if foobar.has_key("version_info"):
			self.patchLevel = foobar["version_info"]["revno"]

	def missing(self, url, switches={}, revision=""):
		if self.pinnedPatchLevel != None:
			return Output(StringIO.StringIO("This branch is pinned to " + str(url) + " at revision " + str(self.pinnedPatchLevel)), StringIO.StringIO())

		try:
			retval = self.__exec("missing", {"--log-format" : "short"}, [url])
		except BzrException, e:
			# If any patches are missing or you have any extra patches
			# Bazaar returns with errorcode 1. Output is on stdout.
			if not (e.errorcode == 1):
				raise BzrMissingException(e.errorcode, e.errormessage, e.command)
			return e.errormessage

		return Output(StringIO.StringIO(""), StringIO.StringIO())

	def status(self, switches={}):
		return self.__exec("status", {}, ["--short"])

	def lint(self):
		statusoutput = self.status()
		lintoutput = ""
		for line in str(statusoutput).split("\n"):
			if line.startswith("?"):
				lintoutput += line + "\n"
		lintoutput += str(self.__exec("conflicts", {}, []))
		tmp = Output(StringIO.StringIO(lintoutput), StringIO.StringIO())
		return tmp

	def update(self, fromRepository=""):
		# BZR update outputs partly on std error, we redirect this
		try:
			updateResult = self.__exec("pull", {}, [fromRepository, "2>&1"])
		except BzrException, e:
			if (e.errorcode == 3):
				raise BzrMergeNeededException(e.errorcode, e.errormessage, e.command)
		return updateResult

	def merge(self, fromRepository=""):
		# BZR update outputs partly on std error, we redirect this
		mergeResult = self.__exec("merge", {}, [fromRepository, "2>&1"])
		return mergeResult

	def get(self, url):
		namedArgs = {}

		if self.pinnedPatchLevel != None:
			namedArgs = { "--revision" : str(self.pinnedPatchLevel) }

		try:
			returnValue = self.__exec("branch", namedArgs, [url, self.path])
		except BzrException, e:
			raise BzrGetException(e.errorcode, e.errormessage, e.command)
		self._updateVersionInfo()
		return returnValue

	def push(self, url, createPrefix = False):
		if createPrefix:
			return self.__exec("push", {}, ["--create-prefix", url, "2>&1"])
		else:
			return self.__exec("push", {}, [url, "2>&1"])

	def getFQRN(self):
		branch, relpath = bzrlib.branch.Branch.open_containing(self.path)

		url = branch.get_parent()

                if url==None:
                        # We have no known parent branch location.
                        # We are our own parent, Fry ;)
                        return self.path
		return url

	def getTreeVersion(self):
		self._updateVersionInfo()
		return self.getFQRN()

	def getVersion(self):
		return self.version

	def getCategory(self):
		return self.category

	def getBranch(self):
		return self.branch

	def getRevision(self):
		return self.revision

	def getPatchLevel(self):
		self._updateVersionInfo()
		return str(self.patchLevel)

	def isPinned(self):
		return self.pinnedPatchLevel != None

	def getPinnedPatchLevel(self):
		return self.pinnedPatchLevel
