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

class Output:
	def __init__(self, sout, serr):
		self._sout = sout
		self._serr = serr
		self._error = ""
		self._hasError = False

	def __str__(self):
		s = ""

		for i in self._sout:
			s += i

		if self._hasError:
			s = "stdout:\n%s\n\nstderr:\n%s\n" % (s, self.error)
		return s.strip("\n")

	def __iter__(self):
		for s in self._sout, self._serr:
			line = s.readline()
			while line:
				if s == self._serr:
					self._hasError = True
				yield line.strip('\n')
				line = s.readline()

	def hasError(self):
		for it in self._serr:
			self._error += it
		if self._error != "":
			self._hasError = True

		return self._hasError

	def getErrorMessage(self):
		if self.hasError():
			return self._error
		else:
			return ""

	def realtimePrint(self, prepend=""):
		for it in self:
			print prepend + it
		if self._hasError:
			raise Exception("An error during RCS action occured!")
