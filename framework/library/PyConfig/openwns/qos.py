###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2007
# Chair of Communication Networks (ComNets)
# Kopernikusstr. 5, D-52074 Aachen, Germany
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

from openwns.pyconfig import attrsetter

# one element of the classes:
class QoSClass:
    name = None
    number = None
    priority = None
    def __init__(self, name, number, **kw):
        self.name   = name
        self.number = number
        attrsetter(self, kw)

undefinedQosClass = QoSClass("UNDEFINED",0,priority=0)

class QoSClasses:
    mapEntries = None

    def __init__(self, **kw):
        self.mapEntries = []
        self.mapEntries.append(undefinedQosClass)
        attrsetter(self, kw)
    
    def addQoSClass(self, name, number):
        tmp = QoSClass(name, number)
        self.mapEntries.append(tmp)

    def getNumberOfQosClasses(self):
        return len(self.mapEntries)

    def getMaxPriority(self):
        maxprio = 0
        for mapEntry in self.mapEntries:
            if mapEntry.priority>maxprio:
                maxprio=mapEntry.priority
        return maxprio

    def getQosClassByName(self, name):
        qosClass = undefinedQosClass
        for mapEntry in self.mapEntries:
            if mapEntry.name == name:
                qosClass = mapEntry
        return qosClass

    def verifyQosName(self, name):
        qosClass = undefinedQosClass
        for mapEntry in self.mapEntries:
            if mapEntry.name == name:
                qosClass = mapEntry
        return qosClass.name

    def getPriorityByQosClassName(self, name):
        priority = priorityWorst
        for mapEntry in self.mapEntries:
            if mapEntry.name == name:
                priority = mapEntry.priority
        return priority
