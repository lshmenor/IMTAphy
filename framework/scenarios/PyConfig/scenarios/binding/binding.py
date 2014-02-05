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
import constanze.node
import openwns.simulator



class binding:
    """
    Binding
    """

    def __init__(self, qosClass):
        self.qosClass = qosClass



    def create(self, ue):
        rang = openwns.simulator.getSimulator().simulationModel.getNodesByType("RANG")[0]

        return constanze.node.TCPClientBinding(
                    ue.nl.domainName,
                    rang.nl.domainName,
                    1024,
                    parentLogger = ue.logger,
                    qosClass = self.qosClass)



    def createUL(self, ue):
        rang = openwns.simulator.getSimulator().simulationModel.getNodesByType("RANG")[0]

        return constanze.node.TCPClientBinding(
                    ue.nl.domainName,
                    rang.nl.domainName,
                    1024,
                    parentLogger = ue.logger,
                    qosClass = self.qosClass)

    def createDL(self, ue):
        rang = openwns.simulator.getSimulator().simulationModel.getNodesByType("RANG")[0]

        return constanze.node.TCPClientBinding(
                    rang.nl.domainName,
                    ue.nl.domainName,
                    1024,
                    parentLogger = rang.logger,
                    qosClass = self.qosClass)

    def createNormal(self, source, destination):

        return constanze.node.TCPClientBinding(
                    source.nl.domainName,
                    destination.nl.domainName,
                    1024,
                    parentLogger = source.logger,
                    qosClass = self.qosClass)


