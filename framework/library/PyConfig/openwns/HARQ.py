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

import openwns
from openwns.logger import Logger

class UniformRandomDecoder(openwns.StaticFactoryClass):

    def __init__(self, initialPER = 0.1, rolloffFactor=1, parentLogger = None):
        """ Simple statistical decoder. No SINR values used. The probability
        that after the k-th transmission the PDU cannot be decoded is given by

        PER(k) = initialPER^(k*rollofFactor)

        k=1 Initial transmission
        k=2 First retransmission
        etc..

        UniformRandomDecoder ignores the redundancy version field (RV)
        """

        openwns.StaticFactoryClass.__init__(self, "UniformRandomDecoder")
        self.initialPER = initialPER
        self.rolloffFactor = rolloffFactor
        self.logger = Logger('WNS', 'UniformRandomDecoder', True, parentLogger)

class ReceiverProcess:

    def __init__(self, numRVs = 3, parentLogger = None):
        self.numRVs = numRVs
        self.decoder = UniformRandomDecoder(parentLogger = parentLogger)
        self.logger = Logger('WNS', 'HARQReceiverProcess', True, parentLogger)

class HARQ:
    __plugin__ = 'wns.harq.HARQ'
    name = "HARQ"

    def __init__(self, numSenderProcesses, numReceiverProcesses, parentLogger = None):
        self.numSenderProcesses = numSenderProcesses

        self.logger = Logger('WNS', 'HARQ', True, parentLogger)

        self.receiverProcesses = []

        for i in xrange(numReceiverProcesses):
            self.receiverProcesses.append(ReceiverProcess(numRVs = 3, parentLogger = self.logger))

        self.numRVs = 3


