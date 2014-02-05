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

import unittest

import openwns.node
import copper.Copper
import glue.support.Configuration

class ConfigurationTest(unittest.TestCase):

    def setUp(self):
        self.node = openwns.node.Node("testNode")
        self.wire = copper.Copper.Wire("testWire")
        self.phy = copper.Copper.Transceiver(self.node, "testPhy", self.wire, ber = 0.0, dataRate = 1e+3)


    def test_ShortCutComponent(self):
        glue.support.Configuration.ShortCutComponent(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_AcknowledgedModeShortCutComponent(self):
        glue.support.Configuration.AcknowledgedModeShortCutComponent(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_SelectiveRepeatARQ(self):
        glue.support.Configuration.SelectiveRepeatARQ(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_SARSelectiveRepeatARQ(self):
        glue.support.Configuration.SARSelectiveRepeatARQ(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_GoBackNARQ(self):
        glue.support.Configuration.GoBackNARQ(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_SARGoBackNARQ(self):
        glue.support.Configuration.SARGoBackNARQ(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_StopAndWaitARQ(self):
        glue.support.Configuration.StopAndWaitARQ(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_SARStopAndWaitARQ(self):
        glue.support.Configuration.SARStopAndWaitARQ(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)

    def test_ShortCut(self):
        glue.support.Configuration.ShortCut(self.node, "test", self.phy.dataTransmission, self.phy.notification, 1)
