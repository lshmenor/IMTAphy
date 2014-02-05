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

import openwns.logger
import openwns.distribution

class SimpleMM1Step1(object):
    __slots__ = ['nameInFactory', 'logger', 'jobInterArrivalTimeDistribution', 'jobProcessingTimeDistribution']


    def __init__(self, meanJobInterArrivalTime, meanJobProcessingTime):
        self.nameInFactory = 'openwns.queuingsystem.SimpleMM1Step1'
        self.logger = openwns.logger.Logger("WNS", "SimpleMM1Step1", True)
        self.jobInterArrivalTimeDistribution = openwns.distribution.NegExp(meanJobInterArrivalTime)
        self.jobProcessingTimeDistribution = openwns.distribution.NegExp(meanJobProcessingTime)

# a simple alias
SimpleMM1 = SimpleMM1Step1

class SimpleMM1Step2(object):
    __slots__ = ['nameInFactory', 'logger', 'jobInterArrivalTimeDistribution', 'jobProcessingTimeDistribution']

    def __init__(self, meanJobInterArrivalTime, meanJobProcessingTime):
        self.nameInFactory = 'openwns.queuingsystem.SimpleMM1Step2'
        self.logger = openwns.logger.Logger("WNS", "SimpleMM1Step2", True)
        self.jobInterArrivalTimeDistribution = openwns.distribution.NegExp(meanJobInterArrivalTime)
        self.jobProcessingTimeDistribution = openwns.distribution.NegExp(meanJobProcessingTime)

class SimpleMM1Step3(object):
    __slots__ = ['nameInFactory', 'logger', 
        'jobInterArrivalTimeDistribution', 'jobProcessingTimeDistribution', 'probeBus']

    def __init__(self, meanJobInterArrivalTime, meanJobProcessingTime):
        self.nameInFactory = 'openwns.queuingsystem.SimpleMM1Step3'
        self.logger = openwns.logger.Logger("WNS", "SimpleMM1Step3", True)
        self.jobInterArrivalTimeDistribution = openwns.distribution.NegExp(meanJobInterArrivalTime)
        self.jobProcessingTimeDistribution = openwns.distribution.NegExp(meanJobProcessingTime)

        self.probeBus = openwns.probebus.LoggingProbeBus()

class SimpleMM1Step5(object):
    __slots__ = ['nameInFactory', 'logger', 
    'jobInterArrivalTimeDistribution', 'jobProcessingTimeDistribution', 'probeBusName']

    def __init__(self, meanJobInterArrivalTime, meanJobProcessingTime):
        self.nameInFactory = 'openwns.queuingsystem.SimpleMM1Step5'
        self.logger = openwns.logger.Logger("WNS", "SimpleMM1Step5", True)
        self.jobInterArrivalTimeDistribution = openwns.distribution.NegExp(meanJobInterArrivalTime)
        self.jobProcessingTimeDistribution = openwns.distribution.NegExp(meanJobProcessingTime)

        self.probeBusName = "openwns.queuingsystem.MM1.sojournTime"


class SimpleMM1Step6(object):
    __slots__ = ['nameInFactory', 'logger', 
    'jobInterArrivalTimeDistribution', 'jobProcessingTimeDistribution', 'probeBusName']

    def __init__(self, meanJobInterArrivalTime, meanJobProcessingTime):
        self.nameInFactory = 'openwns.queuingsystem.SimpleMM1Step6'
        self.logger = openwns.logger.Logger("WNS", "SimpleMM1Step6", True)
        self.jobInterArrivalTimeDistribution = openwns.distribution.NegExp(meanJobInterArrivalTime)
        self.jobProcessingTimeDistribution = openwns.distribution.NegExp(meanJobProcessingTime)

        self.probeBusName = "openwns.queuingsystem.MM1.sojournTime"
        
class GGn(object):

    def __init__(self, iatDist, procDist, servers):
        self.nameInFactory = 'openwns.queuingsystem.ggn'
        self.iatDist = iatDist
        self.procDist = procDist
        self.serverCount = servers
        self.logger = openwns.logger.Logger("WNS", "GGn", True)

