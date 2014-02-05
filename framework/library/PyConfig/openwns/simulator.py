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

import openwns.eventscheduler
import openwns.logger
import openwns.rng
import openwns.pyconfig
import openwns.probebus
import openwns.pyconfig

class OutputStrategy(object):
    DELETE = openwns.pyconfig.Plugin('Delete')
    MOVE = openwns.pyconfig.Plugin('Move')

class Modules(object):
    def __len__(self):
        return len(self.__getAllModules())

    def __iter__(self):
        for it in self.__getAllModules():
                yield it

    def __getitem__(self, key):
        return self.__getAllModules()[key]

    def __getattr__(self, key):
        # show available attributes
        if self.__dict__.has_key(key):
            return self.__dict__[key]
        else:
            raise Exception(key + " not available.\nAvailable modules are: "+", ".join(self.__dict__.keys()))

    def __setattr__(self, key, value):
        self.__dict__[key] = value

    def __getAllModules(self):
        return self.__dict__.values()


# global simulator config -> will be read by wns::simulator::Application
# needs to be set by the user via openwns.setSimulator(...)
# example:
#
# import openwns
# simulator = openwns.Simulator()
# simulator.environment.masterLogger.enabled = False
# openwns.setSimulator(simulator)

config = None

def getSimulator():
    global config
    return config

def setSimulator(simulator):
    global config
    config = simulator

class OpenWNS(object):
    """ Master configuration

    This class is the root of the configuration.
    """

    __slots__ = ["environment", "__postProcessingFuncs", "logger", "maxSimTime", "eventSchedulerMonitor",
    "simulationModel", "outputDir", "statusWriteInterval", "probesWriteInterval", "statusFileName",
    "outputStrategy", "memConsumptionProbeBusName", "simTimeProbeBusName", "cpuCyclesProbeBusName"]

    modules = Modules()

    def __init__(self, **kw):
        # set defaults
        self.environment = Environment()
        self.logger = openwns.logger.Logger("WNS", "Application", True)
        self.maxSimTime = 0.0
        self.eventSchedulerMonitor = openwns.eventscheduler.Monitor()
        self.simulationModel = None
        self.outputDir = "output"
        self.statusWriteInterval = 10 # in seconds
        self.probesWriteInterval = 10 * 60 # in seconds
        self.statusFileName = "WNSStatus.dat"
        self.outputStrategy = OutputStrategy.MOVE
        # make available for easy access in wns-core
        self.memConsumptionProbeBusName = "wns.Memory"
        self.simTimeProbeBusName = "wns.SimTimePerRealTime"
        self.cpuCyclesProbeBusName = "wns.cpuCycles"
        

        openwns.pyconfig.attrsetter(self, kw)

        # private: keeps a list of tasks for post processing
        self.__postProcessingFuncs = []

    def addPostProcessing(self, func):
        """ Add tasks to the post processing hook

        - func must be callable
        - func must return bool (true on successful operation)
        """
        self.__postProcessingFuncs.append(func)

    def postProcessing(self):
        """ This will be called from the simulator

        Tasks registered for post processing will be executed until a
        task returns False or no tasks are left
        """
        for func in self.__postProcessingFuncs:
            if not func(self):
                return False
        return True

    # for backward compatibility
    def __getEventScheduler(self):
        return self.environment.eventScheduler

    def __setEventScheduler(self, eventScheduler):
        self.environment.eventScheduler = eventScheduler

    def __getProbeBusRegistry(self):
        return self.environment.probeBusRegistry

    def __setProbeBusRegistry(self, probeBusRegistry):
        self.environment.probeBusRegistry = probeBusRegistry

    def __getMasterLogger(self):
        return self.environment.masterLogger

    def __setMasterLogger(self, masterLogger):
        self.environment.masterLogger = masterLogger

    def __getRNG(self):
        return self.environment.rng

    def __setRNG(self, rng):
        self.environment.rng = rng

    eventScheduler = property(__getEventScheduler, __setEventScheduler)
    masterLogger = property(__getMasterLogger, __setMasterLogger)
    rng = property(__getRNG, __setRNG)
    probeBusRegistry = property(__getProbeBusRegistry, __setProbeBusRegistry)

class Environment(object):

    __slots__ = ["eventScheduler", "masterLogger", "rng", "probeBusRegistry"]

    def __init__(self, **kw):
        self.eventScheduler = openwns.eventscheduler.Map()
        self.masterLogger = openwns.logger.Master()
        self.rng = openwns.rng.RNG(useRandomSeed = False)
        self.probeBusRegistry = openwns.probebus.ProbeBusRegistry()
        openwns.pyconfig.attrsetter(self, kw)
