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

# aliases
import openwns.simulator as simulator
import sys
import os

Simulator = simulator.OpenWNS
getSimulator = simulator.getSimulator
setSimulator = simulator.setSimulator

def getPyConfigPath():
    for path in sys.path:
        path = os.path.abspath(path)
        if 'PyConfig' in path and os.path.exists(path):
            return path
    # should not be reached if path is available
    raise("No path in sys.path found which contains 'PyConfig'")

def dB(ratio):
    return str(ratio) + " dB"
def fromdB(str):
    i = str.index('dB')
    return(float(str[0:i-1]))

def dBm(power):
    return str(power) + " dBm"
def fromdBm(str):
    i = str.index('dBm')
    return(float(str[0:i-1]))

def W(power):
    return str(power) + " W"
def fromW(str):
    i = str.index('W')
    return(float(str[0:i-1]))

def mW(power):
    return str(power) + " mW"
def frommW(str):
    i = str.index('mW')
    return(float(str[0:i-1]))

class StaticFactoryClass(object):

    __slots__ = ["nameInStaticFactory", "__plugin__"]

    nameInStaticFactory = None

    __plugin__ = None

    def __init__(self, name):
        self.nameInStaticFactory = name
        self.__plugin__ = name