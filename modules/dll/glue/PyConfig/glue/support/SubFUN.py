###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2009
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

import openwns.FUN
import openwns.Group
import openwns.Multiplexer
import openwns.Tools

import glue.InSequenceChecker
import glue.Routing

class Reconfiguration(openwns.Group.Group):

    def __init__(self, addressProvider, reconfigurationSchemes, masterRM = False, berProvider = None, loggerEnabled = True, parentLogger = None):
        openwns.Group.Group.__init__(self, openwns.FUN.FUN(), "inSequenceChecker", "sync")

        # create
        inSequenceChecker = openwns.FUN.Node("inSequenceChecker", glue.InSequenceChecker.InSequenceChecker(loggerEnabled, parentLogger))

        # control plane
        reconfigurationManagerFU = glue.Reconfiguration.TransmittingManager(masterRM,
                                                                            reconfigurationSchemes,
                                                                            loggerEnabled,
                                                                            parentLogger,
                                                                            reconfigurationInterval = 0.5,
                                                                            useOptimizedReconfigurationIfPossible = True)
        reconfigurationManager = openwns.FUN.Node("reconfigurationManager", reconfigurationManagerFU)
        if berProvider is not None:
            controlCRC = openwns.FUN.Node("controlCRC", openwns.CRC.CRC(berProvider))

        # user plane
        supportUpper = openwns.FUN.Node("supportUpper", glue.Reconfiguration.SupportUpper("reconfigurationManager", loggerEnabled, parentLogger))

        drain = openwns.FUN.Node("drain", glue.Reconfiguration.Drain(loggerEnabled, parentLogger))
        supportLower = openwns.FUN.Node("supportLower", glue.Reconfiguration.SupportLower("reconfigurationManager", "drain", loggerEnabled, parentLogger))

        # joined plane
        planeDispatcher = openwns.FUN.Node("planeDispatcher", openwns.Multiplexer.Dispatcher(1))
        routing = openwns.FUN.Node("routing", glue.Routing.Routing(addressProvider))
        sync = openwns.FUN.Node("sync", openwns.Tools.Synchronizer())


        # add
        self.fun.add(inSequenceChecker)

        # control plane
        self.fun.add(reconfigurationManager)
        if berProvider is not None:
            self.fun.add(controlCRC)

        # user plane
        self.fun.add(supportUpper)
        for node in reconfigurationSchemes[0].fun.functionalUnit:
            self.fun.add(node)
        self.fun.add(drain)
        self.fun.add(supportLower)

        # joined plane
        self.fun.add(planeDispatcher)
        self.fun.add(routing)
        self.fun.add(sync)


        # connect
        # control plane
        if berProvider is not None:
            reconfigurationManager.connect(controlCRC)
            controlCRC.connect(planeDispatcher)
        else:
            reconfigurationManager.connect(planeDispatcher)

        # user plane
        inSequenceChecker.connect(supportUpper)
        supportUpper.connect(reconfigurationSchemes[0].topFU)
        self.fun.connects += reconfigurationSchemes[0].fun.connects
        reconfigurationSchemes[0].bottomFU.connect(supportLower)
        supportLower.connect(planeDispatcher)

        # joined plane
        planeDispatcher.connect(routing)
        routing.connect(sync)
