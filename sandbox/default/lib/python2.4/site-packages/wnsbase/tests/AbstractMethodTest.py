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
import unittest
import wnsbase.Interface
from wnsbase.Interface import abstractmethod

class IA(wnsbase.Interface.Interface):
   @abstractmethod
   def methodA(self, aParameter):
     pass

class IB(wnsbase.Interface.Interface):
   @abstractmethod
   def methodB(self,aParameter):
      pass

class IAB(IA, IB):
   pass


class A(IA):
    def methodA(self, aParameter):
       pass

class BadA(IA):
    pass

class AB(IAB):
   def methodA(self, aParameter):
      pass

   def methodB(self, aParameter):
      pass

class BadAB(IAB):
   def methodA(self, aParameter):
      pass

class AB2(IA, IB):

   def methodA(self, aParameter):
      pass

   def methodB(self, aParameter):
      pass

class BadAB2(IA, IB):

   def methodB(self, aParameter):
      pass

class AbstractMethodTests(unittest.TestCase):


    def testSingleInheritance(self):
        # This must work
        a = A()

    def testMultipleInheritance(self):
        ab = AB2()

    def testMultipleInheritanceHierarchy(self):
        ab = AB()

    def testBadSingleInheritance(self):
        typeErrorThrown = False
        try:
            badA = BadA()
        except TypeError:
            typeErrorThrown = True
        assert(typeErrorThrown, "No TypeError was thrown when creating an instance of BadA")

    def testBadMultipleInheritance(self):
        typeErrorThrown = False
        try:
            badAB2 = BadAB2()
        except TypeError:
            typeErrorThrown = True
        assert(typeErrorThrown, "No TypeError was thrown when creating an instance of BadAB2")

    def testBadMultipleInheritanceHierarchy(self):
        typeErrorThrown = False
        try:
            badAB = BadAB()
        except TypeError:
            typeErrorThrown = True
        assert(typeErrorThrown, "No TypeError was thrown when creating an instance of BadAB")


