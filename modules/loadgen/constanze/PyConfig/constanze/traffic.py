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

import openwns.distribution
from MMPP import *

class Traffic(object):
    iat = None        # average interarrival time [seconds]
    packetSize = None # average packet size [bits]
    offset = None     # startTime [seconds]
    duration = None   # stopTime=startTime+duration [seconds]
    correctFRT = None # use statistically correct ForwardRecurrenceTime if True
    # duration=0.0 means: forever, no stopTime
    logger = None

    def __init__(self, parentLogger = None, **kw):
        self.logger = openwns.logger.Logger("CONST", "Traffic", True, parentLogger)
        #self.logger.level = 2
        attrsetter(self, kw)

# CBR is constant bit rate
# the most simple traffic
# absolutely no burstiness
# produces the best queueing results (delay, loss)
# unfortunately unrealistic in most cases
class CBR(Traffic):
    __plugin__ = "PointProcess" # plugin used in ConstanzeComponent.cpp
    correctFRT = True # use statistically correct ForwardRecurrenceTime if True

    def __init__(self, offset = 0.0, throughput = 1024, packetSize = 1024, duration = 0.0, parentLogger = None):
        throughput = float(throughput) # [bits/s]
        packetSize = float(packetSize) # [bits]
        self.offset = offset
        self.iat = openwns.distribution.Fixed(packetSize / throughput) # arg is mean interarrival time [s]
        self.packetSize = openwns.distribution.Fixed(packetSize)
        self.duration = duration;
        self.logger = openwns.logger.Logger("CONST", "TrafficGeneratorCBR", True, parentLogger)

# CBR0 is constant bit rate. All first packets come at once
class CBR0(Traffic):
    __plugin__ = "PointProcess" # plugin used in ConstanzeComponent.cpp
    correctFRT = False # without FRT. First packet comes at once

    def __init__(self, offset = 0.0, throughput = 1024, packetSize = 1024, duration = 0.0, parentLogger = None):
        throughput = float(throughput) # [bits/s]
        packetSize = float(packetSize) # [bits]
        self.offset = offset
        self.iat = openwns.distribution.Fixed(packetSize / throughput) # arg is mean interarrival time [s]
        self.packetSize = openwns.distribution.Fixed(packetSize)
        self.duration = duration;
        self.logger = openwns.logger.Logger("CONST", "TrafficGeneratorCBR", True, parentLogger)

# Poisson traffic is relatively harmless
# the coefficient of variation is exactly 1.0
# analytical results often only exist for this traffic type
# good for studying short-timescale queueing/delay issues
# in a longer timescale it behaves a bit worse than CBR
class Poisson(Traffic):
    __plugin__ = "PointProcess"
    correctFRT = False # not necessary for Poisson traffic

    def __init__(self, offset = 0.0, throughput = 1024, packetSize = 1024, duration = 0.0, parentLogger = None):
        throughput = float(throughput) # [bits/s]
        packetSize = float(packetSize) # [bits]
        self.offset = offset
        self.iat = openwns.distribution.NegExp(packetSize / throughput) # arg is mean interarrival time [s]
        self.packetSize = openwns.distribution.Fixed(packetSize)
        self.duration = duration;
        self.logger = openwns.logger.Logger("CONST", "TrafficGeneratorPoisson", True, parentLogger)
        #self.logger.level = 2

# ABR is commonly known as Available Bit Rate in ATM
# here we use is as Arbitrarily Adjustable Bit Rate
class ABR(Traffic):
    __plugin__ = "PointProcess"
    correctFRT = True

    def __init__(self, iatDistribution, packetSizeDistribution, offset = 0.0, duration = 0.0, parentLogger = None):
        self.iat = iatDistribution
        self.packetSize = packetSizeDistribution
        self.offset = offset
        self.duration = duration
        self.logger = openwns.logger.Logger("CONST", "TrafficGeneratorABR", True, parentLogger)
        #self.logger.level = 2
        
# VBR is variable bit rate, e.g. like MPEG2 or VoIP
# it uses MMPP here (MarkovModulatedPoissonProcess)
class VBR(Traffic):
    __plugin__ = "MMPP"
    numberOfChains  = None # #chains: each chain generates traffic independently
    rateScale       = None # scale the traffic rate up (>1) or down (<1)
    transitionScale = None # scale the frequency up (>1) or down (<1)
    mmppParams      = None # from MMPPparams

    def __init__(self, numberOfChains=None, rateScale=1.0, transitionScale=1.0, offset = 0.0, duration = 0.0, parentLogger = None):
        self.mmppParams = MMPPMPEG2()
        if (numberOfChains != None): # no default
            self.numberOfChains = numberOfChains # given as a parameter (overwrite spec)
        else: # default
            self.numberOfChains = self.mmppParams.numberOfChains # take from mmpp specification
        self.rateScale = rateScale
        self.transitionScale = transitionScale
        self.offset = offset # startTime
        self.duration = duration
        self.logger = openwns.logger.Logger("CONST", "TrafficGeneratorVBR", True, parentLogger)
        #self.logger.level = 2

# VoIP Traffic Model
# This is the simple ON/OFF model specified in the IMT_Advanced TD03 Annex5
# Basic characteristics:
# Coder: RTP AMR 12.2
# source rate: 12.2 kbps (during talkspurt)
# voice activity factor: 50%
# discrete time step: 20ms = encoder frame length
# probability of changing the state: 0.01
# it uses MMPP here (MarkovModulatedPoissonProcess)
# @author rs
class VoIP(Traffic):
    __plugin__ = "MMPP"
    numberOfChains  = None # #chains: each chain generates traffic independently
    rateScale       = None # scale the traffic rate up (>1) or down (<1)
    transitionScale = None # scale the frequency up (>1) or down (<1)
    mmppParams      = None # from MMPPparams
    targetRate      = None # should be unspecified. Use "numberOfChains" to scale traffic up.

    def __init__(self, numberOfChains=1, transitionScale=1.0, offset = 0.0, duration = 0.0, parentLogger = None):
        self.mmppParams = MMPPVoIP()
        self.numberOfChains = numberOfChains # models number of parallel voice streams
        self.rateScale = 1.0
        self.transitionScale = transitionScale
        self.offset = offset # startTime
        self.duration = duration
        self.logger = openwns.logger.Logger("CONST", "TrafficGeneratorVoIP", True, parentLogger)
        #self.logger.level = 2

# Self-similar traffic
# no MMPP in the strong sense
# see MMPP.py for details of model
class SelfSimilar(Traffic):
    __plugin__ = "MMPP"
    numberOfChains  = None # #chains: each chain generates traffic independently
    targetRate      = None # (mutual exclusive) alternative to rateScale. Specify the desired mean bit rate [bits/s]
    rateScale       = None # scale the traffic rate up (>1) or down (<1)
    transitionScale = None # scale the frequency up (>1) or down (<1)
    mmppParams      = None # from MMPPparams

    def __init__(self, numberOfChains=None, targetRate=None, rateScale=None, transitionScale=1.0, offset = 0.0, duration = 0.0, parentLogger = None):
        self.mmppParams = MMPPSelfSimilar() # no MMPP in the strong sense
        #self.mmppParams = MMPPSelfSimilar(alpha, hurst) # if you want to change the degree of self-similarity
        if (numberOfChains != None): # no default
            self.numberOfChains = numberOfChains # given as a parameter (overwrite spec)
        else: # default
            self.numberOfChains = self.mmppParams.numberOfChains # take from mmpp specification
        assert ((rateScale != None) and (targetRate == None)) or ((rateScale == None) and (targetRate != None))
        if (rateScale != None): # no default
            self.rateScale = rateScale
        else: # default
            self.rateScale = 1.0
        if (targetRate != None): # no default
            self.targetRate = targetRate
            self.rateScale = None
        else: # default
            self.targetRate = None
        self.transitionScale = transitionScale
        self.offset = offset # startTime
        self.duration = duration
        self.logger = openwns.logger.Logger("CONST", "SelfSimilarTrafficGenerator", True, parentLogger)
        #self.logger.level = 2

# MMPP = MarkovModulatedPoissonProcess
# @author rs,sgm
# The helper and parameter classes are in MMPP.py
class MMPP(Traffic):
    __plugin__ = "MMPP"
    numberOfChains  = None # #chains: each chain generates traffic independently
    targetRate      = None # (mutual exclusive) alternative to rateScale. Specify the desired mean bit rate [bits/s]
    rateScale       = None # scale the traffic rate up (>1) or down (<1)
    transitionScale = None # scale the frequency up (>1) or down (<1)
    mmppParams      = None # from MMPPparams

    def __init__(self, mmppParams, numberOfChains=None, targetRate=None, rateScale=None, transitionScale=1.0, offset = 0.0, duration = 0.0, parentLogger = None):
        self.mmppParams = mmppParams
        if (numberOfChains != None): # no default
            self.numberOfChains = numberOfChains # given as a parameter (overwrite spec)
            print "MMPP: default overwritten numberOfChains=",self.numberOfChains
        else: # default
            self.numberOfChains = mmppParams.numberOfChains # take from mmpp specification
        assert ((rateScale != None) and (targetRate == None)) or ((rateScale == None) and (targetRate != None))
        if (rateScale != None): # no default
            self.rateScale = rateScale
        else: # default
            self.rateScale = 1.0
        if (targetRate != None): # no default
            self.targetRate = targetRate
            self.rateScale = None
        else: # default
            self.targetRate = None
        self.transitionScale = transitionScale
        self.offset = offset # startTime
        self.duration = duration
        self.logger = openwns.logger.Logger("CONST", "TrafficGeneratorMMPP", True, parentLogger)
        #self.logger.level = 2

# DTMMPP = Discrete Time MarkovModulatedPoissonProcess
# @author rs,sgm
# The helper and parameter classes are in MMPP.py
class DTMMPP(Traffic):
    __plugin__ = "DTMMPP"
    numberOfChains  = None # #chains: each chain generates traffic independently
    targetRate      = None # (mutual exclusive) alternative to rateScale. Specify the desired mean bit rate [bits/s]
    rateScale       = None # scale the traffic rate up (>1) or down (<1)
    mmppParams      = None # from MMPPparams
    slotTime        = None # discrete time quantum

    def __init__(self, mmppParams, slotTime=None, numberOfChains=None, targetRate=None, rateScale=None, transitionScale=1.0, offset = 0.0, duration = 0.0, parentLogger = None):
        self.mmppParams = mmppParams
        if (slotTime != None): # no default
            self.slotTime = slotTime
        else: # default
            self.slotTime = mmppParams.slotTime # take from mmpp specification
        if (numberOfChains != None): # no default
            self.numberOfChains = numberOfChains # given as a parameter (overwrite spec)
            print "DTMMPP: default overwritten numberOfChains=",self.numberOfChains
        else: # default
            self.numberOfChains = mmppParams.numberOfChains # take from mmpp specification
        if (rateScale != None): # no default
            self.rateScale = rateScale
        else: # default
            self.rateScale = 1.0
        if (targetRate != None): # no default
            self.targetRate = targetRate
            self.rateScale = None
        else: # default
            self.targetRate = None
        self.offset = offset # startTime
        self.duration = duration
        self.logger = openwns.logger.Logger("CONST", "TrafficGeneratorDTMMPP", True, parentLogger)
        #self.logger.level = 2


