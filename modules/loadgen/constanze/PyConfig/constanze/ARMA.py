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

import openwns.distribution
import distribution.CDFTables

import sys # for sys.path(), returns list of python directories
import os; # for os.getcwd(), returns Unix current working dir

# @author rs,sgm
# this class describes the traffic within one of the MMPP states:
class MMPPstateParams(object):
    iat = None # Distribution
    packetSize = None # Distribution

    def __init__(self, iatDistribution, packetSizeDistribution):
        self.iat = iatDistribution
        self.packetSize = packetSizeDistribution

# @author rs,sgm
# base class
class MMPPparams(object):
    fromFile = False
    startState = None # this is for all chains
    #startStates = None # if someone needs separate control
    numberOfChains = None # #chains: each chain generates traffic independently
    transitionMatrix = None # rate values that specify state transitions
    transitionDistribution = "NegExp(X)" # "NegExp" is standard for MMPP. But we are more generic.
    # transitionDistribution can be e.g.
    # "Pareto(1.2,X,0.0,1e100)"
    # "Uniform(1.5*X,0.5*X)"
    # "Norm(X,X*X)"

# @author rs,sgm
class MMPPparamsComplete(MMPPparams):
    numberOfStates = None
    transitionMatrix = None
    stateList = None

    def __init__(self, numberOfChains, numberOfStates, transitionMatrix, stateList):
        self.fromFile = False
        self.numberOfChains = numberOfChains
        self.numberOfStates = numberOfStates
        self.startState = -1 # -1=random
        self.transitionMatrix = transitionMatrix
        assert len(self.transitionMatrix) == self.numberOfStates
        self.stateList = stateList
        assert len(self.stateList) == self.numberOfStates

# @author rs,sgm
class MMPPexampleONOFF(MMPPparams):
    numberOfStates = 2
    transitionMatrix = ((0.0, 100),
                        (100, 0.0))
    numberOfChains = 1
    startState = -1 # -1=random
    #startStates = (-1,-1,-1,-1,-1) # for each chain; -1 means random
    stateList = None

    def __init__(self):
        self.fromFile = False
        self.stateList = []
        #for stateindex in xrange(numberOfStates):
        iatDistribution_0 = openwns.distribution.Fixed(5.0e+12) # OFF
        iatDistribution_1 = openwns.distribution.Fixed(100e-6)  # ON
        packetSizeDistribution_0 = openwns.distribution.Fixed(8) # bits
        packetSizeDistribution_1 = openwns.distribution.Fixed(1024) # bits
        stateParam_0 = MMPPstateParams(iatDistribution_0, packetSizeDistribution_0)
        stateParam_1 = MMPPstateParams(iatDistribution_1, packetSizeDistribution_1)
        self.stateList.append(stateParam_0)
        self.stateList.append(stateParam_1)
        ### debug output:
        #print "startStates="; print self.startStates
        #print "transitionMatrix="; print self.transitionMatrix

# @author rs,sgm
class MMPPexample01(MMPPparams):
    # sample configuration to show how to specify MMPP traffic
    # based on mmpp_example.gdf but
    # scaled rate down by 0.01 and transitions by 0.1
    numberOfStates = 5
    transitionMatrix = (( 0.0000, 2.0e+1,  1.0e+1, 2.0e+1, 1.0e+1),
                        ( 5.0e+1, 0.0000,  5.0e+1, 5.0e+1, 5.0e+1),
                        ( 5.0e+1, 5.0e+1,  0.0000, 5.0e+1, 5.0e+1),
                        (15.0e+1, 5.0e+1, 15.0e+1, 0.0000, 5.0e+1),
                        ( 5.0e+1, 5.0e+1,  5.0e+1, 5.0e+1, 0.0000))
    numberOfChains = 1
    startState = -1 # -1=random
    #startStates = (-1,-1,-1,-1,-1) # for each chain; -1 means random
    stateList = None

    def __init__(self):
        self.fromFile = False
        self.stateList = []
        #for stateindex in xrange(self.numberOfStates):
        iatDistribution = [
            openwns.distribution.NegExp(5.0e-4), # [s]
            openwns.distribution.NegExp(1.2e-4), # [s]
            openwns.distribution.NegExp(1.0e+6), # [s]
            openwns.distribution.NegExp(2.0e-4), # [s]
            openwns.distribution.NegExp(1.0e-4), # [s]
            ]
        packetSizeDistribution = openwns.distribution.Fixed(1024) # bits
        for stateindex in xrange(self.numberOfStates):
            stateParam = MMPPstateParams(iatDistribution[stateindex], packetSizeDistribution)
            self.stateList.append(stateParam)

        ### debug output:
        #print "startStates="; print self.startStates
        #print "transitionMatrix="; print self.transitionMatrix

# MPEG2 traffic with GOP=IBBPBB, framerate=30/s.
# representative in short-scale (<1s), but missing longer range (scene) changes
# @author rs,sgm
class MMPPMPEG2(MMPPparams):
    # sample configuration to show how to specify MMPP traffic
    # based on mmpp_mpeg2.gdf
    numberOfStates = 6
    transitionMatrix = ((0.0000, 3.0e+1, 1.0e-1, 1.0e-1, 1.0e-1, 1.0e-1),
                        (1.0e-1, 0.0000, 3.0e+1, 1.0e-1, 1.0e-1, 1.0e-1),
                        (1.0e-1, 1.0e-1, 0.0000, 3.0e+1, 1.0e-1, 1.0e-1),
                        (1.0e-1, 1.0e-1, 1.0e-1, 0.0000, 3.0e+1, 1.0e-1),
                        (1.0e-1, 1.0e-1, 1.0e-1, 1.0e-1, 0.0000, 3.0e+1),
                        (3.0e+1, 1.0e-1, 1.0e-1, 1.0e-1, 1.0e-1, 0.0000))
    transitionDistribution = "Fixed(X)" # constant state changes
    numberOfChains = 1
    startState = 0 # -1=random
    stateList = None

    def __init__(self):
        self.fromFile = False
        self.stateList = []
        #for stateindex in xrange(self.numberOfStates):
        iatDistribution = [ # arg in [s]
            openwns.distribution.NegExp(0.038e-3), # I
            openwns.distribution.NegExp(0.192e-3), # B
            openwns.distribution.NegExp(0.192e-3), # B
            openwns.distribution.NegExp(0.066e-3), # P
            openwns.distribution.NegExp(0.192e-3), # B
            openwns.distribution.NegExp(0.192e-3), # B
            ]
        packetSizeDistribution = [ # arg in [bits]
            openwns.distribution.Fixed(1024),
            openwns.distribution.Fixed(1024),
            openwns.distribution.Fixed(1024),
            openwns.distribution.Fixed(1024),
            openwns.distribution.Fixed(1024),
            openwns.distribution.Fixed(1024),
            ]
        for stateindex in xrange(self.numberOfStates):
            stateParam = MMPPstateParams(iatDistribution[stateindex], packetSizeDistribution[stateindex])
            self.stateList.append(stateParam)

# MPEG2 traffic with GOP=IBBPBB, framerate=30/s.
# Discrete-Time MMPP version.
# representative in short-scale (<1s), but missing longer range (scene) changes
# @author rs,sgm
class DTMMPPMPEG2(MMPPparams):
    # sample configuration to show how to specify MMPP traffic
    # based on mmpp_mpeg2.gdf
    numberOfStates = 6
    # deterministic transitions:
    transitionMatrix = ((0.0, 1.0, 0.0, 0.0, 0.0, 0.0),
                        (0.0, 0.0, 1.0, 0.0, 0.0, 0.0),
                        (0.0, 0.0, 0.0, 1.0, 0.0, 0.0),
                        (0.0, 0.0, 0.0, 0.0, 1.0, 0.0),
                        (0.0, 0.0, 0.0, 0.0, 0.0, 1.0),
                        (1.0, 0.0, 0.0, 0.0, 0.0, 0.0))
    slotTime = 1.0/30.0 # 1/framerate
    numberOfChains = 1
    startState = 0 # -1=random
    stateList = None

    def __init__(self):
        self.fromFile = False
        self.stateList = []
        #for stateindex in xrange(self.numberOfStates):
        iatDistribution = [ # arg in [s]
            openwns.distribution.NegExp(0.038e-3), # I
            openwns.distribution.NegExp(0.192e-3), # B
            openwns.distribution.NegExp(0.192e-3), # B
            openwns.distribution.NegExp(0.066e-3), # P
            openwns.distribution.NegExp(0.192e-3), # B
            openwns.distribution.NegExp(0.192e-3), # B
            ]
        packetSizeDistribution = [ # arg in [bits]
            openwns.distribution.Fixed(1024),
            openwns.distribution.Fixed(1024),
            openwns.distribution.Fixed(1024),
            openwns.distribution.Fixed(1024),
            openwns.distribution.Fixed(1024),
            openwns.distribution.Fixed(1024),
            ]
        for stateindex in xrange(self.numberOfStates):
            stateParam = MMPPstateParams(iatDistribution[stateindex], packetSizeDistribution[stateindex])
            self.stateList.append(stateParam)

# data traffic with self-similar behaviour
# representative in time scales from 1ms..100s
# no MMPP in the strong sense
# @author rs,sgm
class MMPPdata(MMPPparams):
    # configuration for data traffic
    # based on mmpp_pareto_01.gdf
    numberOfStates = 2
    transitionMatrix = ((0.0, 100),
                        (100, 0.0))
    transitionDistribution = "Pareto(1.2,X,0.0,1e100)" # heavy-tailed state changes
    numberOfChains = 10 # superposition of C sources
    startState = -1
    stateList = None

    def __init__(self):
        self.fromFile = False
        self.stateList = []
        #for stateindex in xrange(self.numberOfStates):
        iatDistribution = [ # arg in [s]
            openwns.distribution.Fixed(5.0e+12), # OFF
            openwns.distribution.NegExp(100e-6), # ON
            ]
        packetSizeDistribution = [ # arg in [bits]
            openwns.distribution.Fixed(8),
            distribution.CDFTables.IPPacketSizeDataTraffic(), # IP packets
            ]
        for stateindex in xrange(self.numberOfStates):
            stateParam = MMPPstateParams(iatDistribution[stateindex], packetSizeDistribution[stateindex])
            self.stateList.append(stateParam)
        ### debug output:
        #print "numberOfChains=",self.numberOfChains

# Self-similar traffic
# superposition of many ON-OFF-models with heavy tailed distributions
# proven to be the best generator method:
# [G.Horn et al., "An empirical comparison of generators for self-similar simulated traffic", Performance Evaluation 64 (2007-Feb), pp 162-190]
# representative in time scales from 1ms..100s
# no MMPP in the strong sense
# @author rs
class MMPPSelfSimilar(MMPPparams):
    # configuration for data traffic
    # based on mmpp_pareto_01.gdf
    numberOfStates = 2
    transitionMatrix = ((0.0, 100),
                        (100, 0.0))
    transitionDistribution = "Pareto(A,X,0.0,1e100)" # heavy-tailed state changes
    # Hurst Parameter H=(3-alpha)/2 <=> alpha=3-2*H
    alpha = None # A
    hurst = None # H
    numberOfChains = 300 # superposition of C sources
    startState = -1 # random
    stateList = None

    def __init__(self, alpha=None, hurst=None):
        self.fromFile = False
        if ((alpha == None) and (hurst == None)): # no specification of self-similarity-degree
            alpha = 1.2 # default => H=0.9
        assert ((alpha != None) and (hurst == None)) or ((alpha == None) and (hurst != None))
        if (alpha != None): # no default
            self.alpha = alpha
            self.hurst = (3.0-alpha)/2.0
        if (hurst != None): # no default
            self.hurst = hurst
            self.alpha = (3.0-2.0*hurst)
        #replace(old, new[, count])
        self.transitionDistribution = self.transitionDistribution.replace('A',"%f" % self.alpha)
        self.stateList = []
        #for stateindex in xrange(self.numberOfStates):
        iatDistribution = [ # arg in [s]
            openwns.distribution.Fixed(5.0e+12), # OFF
            openwns.distribution.NegExp(100e-6), # ON
            ]
        packetSizeDistribution = [ # arg in [bits]
            openwns.distribution.Fixed(8),
            distribution.CDFTables.IPPacketSizeDataTraffic(), # IP packets
            ]
        for stateindex in xrange(self.numberOfStates):
            stateParam = MMPPstateParams(iatDistribution[stateindex], packetSizeDistribution[stateindex])
            self.stateList.append(stateParam)
        ### debug output:
        #print "MMPPSelfSimilar: a=",self.alpha," H=",self.hurst," TD=",self.transitionDistribution

# @author rs,sgm
# class to read the MMPP parameters from a GDF file located in
# ./framework/libwns--main--3.0/PyConfig/wns/markov/*.gdf
# and it is installed here: with scons!
# ./sandbox/lib/dbg/PyConfig/wns/markov
class MMPPparamsFromFile(MMPPparams):
    filename = None

    def __init__(self, filename):
        self.fromFile = True
        self.startState = -1 # -1 means random
        self.numberOfChains = 1 # default
        PyConfigDirIndex = self.getPyConfigDirIndex(sys.path)
        #print "path=" + ":".join(sys.path)
        #print "PyConfigDirIndex=" + ("%s" % PyConfigDirIndex)
        #os.path.split(os.getcwd())
        #cwd = os.getcwd()
        #print "cwd=" + cwd
        GdfFileDir  = sys.path[PyConfigDirIndex] + "/wns/markov/"
        assert os.path.exists(GdfFileDir)
        #print "GdfFileDir=" + GdfFileDir
        self.filename = GdfFileDir + filename
        assert os.path.isfile(self.filename)
        #print "GdfFileName=" + self.filename
        # idea: let python read the file and c++ take the datastructures

    # this Python function is also copied in MarkovContinuousTimeTrafficTest.cpp --- please keep consistent
    def getPyConfigDirIndex(self, list):
        for s in list:
            if 'PyConfig' in s:
                 if os.path.exists(s):
                     return list.index(s)
        return None

# @author rs,sgm
class DTMMPPexample01(MMPPparams):
    # sample configuration to show how to specify MMPP traffic
    # based on mmpp_example.gdf but
    # scaled rate down by 0.01 and transitions by 0.1
    numberOfStates = 5
    # matrix of probabilities (not rates as for CT-MMPP !)
    transitionMatrix = (
        (0.990, 0.001, 0.001, 0.001, 0.007),
        (0.002, 0.990, 0.002, 0.001, 0.005),
        (0.006, 0.002, 0.980, 0.007, 0.005),
        (0.009, 0.001, 0.002, 0.980, 0.008),
        (0.001, 0.002, 0.003, 0.004, 0.990))
    slotTime = 0.01
    numberOfChains = 1
    startState = -1 # -1=random
    #startStates = (-1,-1,-1,-1,-1) # for each chain; -1 means random
    stateList = None

    def __init__(self):
        self.fromFile = False
        self.stateList = []
        #for stateindex in xrange(self.numberOfStates):
        iatDistribution = [
            openwns.distribution.NegExp(1.0e-4), # [s]
            openwns.distribution.NegExp(2.0e-4), # [s]
            openwns.distribution.NegExp(5.0e-4), # [s]
            openwns.distribution.NegExp(1.0e-3), # [s]
            openwns.distribution.NegExp(1.0e+6), # [s]
            ]
        packetSizeDistribution = openwns.distribution.Fixed(1024) # bits
        for stateindex in xrange(self.numberOfStates):
            stateParam = MMPPstateParams(iatDistribution[stateindex], packetSizeDistribution)
            self.stateList.append(stateParam)

        ### debug output:
        #print "startStates="; print self.startStates
        #print "transitionMatrix="; print self.transitionMatrix

