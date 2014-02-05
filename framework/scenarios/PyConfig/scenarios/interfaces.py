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

import openwns.interface

class INode(openwns.interface.Interface):
    """
    Interface for nodes that are handled by scenarios
    """

    @openwns.interface.abstractmethod
    def setPosition(self, position):
        """
        Set the position of this node to position

        @type  position: openwns.geometry.position.Position
        @param position: The position to move this node to.
        """
        pass

    @openwns.interface.abstractmethod
    def getPosition(self):
        """
        Get the position of this node

        @rtype:  openwns.geometry.position.Position
        @return: The position to move this node to.
        """
        pass

    @openwns.interface.abstractmethod
    def setAntenna(self, antenna):
        """
        Set the antenna for this node

        @type antenna: rise.Antenna.Antenna
        @param antenna: The Antenna that should be installed in this node
        """
        pass

    @openwns.interface.abstractmethod
    def setChannelModel(self, channelModel):
        """
        Set the channel model for this node

        @type channelModel: ChannelModel
        @param antenna: The ChannelModel that should be installed in this node
        """
        pass

    @openwns.interface.abstractmethod
    def getProperty(self, propertyName):
        """
        Return the property value of this node.

        @rtype: all types are possible
        @return: the property value with given property name of this node
        """
        pass

    @openwns.interface.abstractmethod
    def setProperty(self, propertyName, propertyValue):
        """
        set the property value of this node.

        """
        pass


class INodePlacer(openwns.interface.Interface):
    """
    Interface for Strategies that define the placement of nodes
    """

    @openwns.interface.abstractmethod
    def setCenter(self, center):
        """
        @type center : openwns.geometry.position.Position
        @param center: The center of the coordinate system 
        Translate the center of the coordinate system to the center
        """

    @openwns.interface.abstractmethod
    def getPositions(self):
        """
        Get all positions where nodes will be placed.

        @rtype: [openwns.geometry.position.Position]
        @return: A list of all positions where nodes shall be placed
        """
        pass

    # Default for UE placers assuring same number of UEs per BS.
    # Ignored for BS placers
    perBS = True

class INodeCreator(openwns.interface.Interface):
    """
    Defines the interface for Node creation strategies
    """

    @openwns.interface.abstractmethod
    def create(self):
        """"
        Implement the strategy to build your Node here

        @rtype: scenario.interface.INode
        @return: The new node that was created
        """
        pass

class IAntennaCreator(openwns.interface.Interface):
    """
    Defines the interface for Antenna creation strategies
    """

    @openwns.interface.abstractmethod
    def create(self):
        """"
        Implement the strategy to build your Antenna here

        @rtype: rise.Antenna.Antenna
        @return: The new antenna that was created
        """
        pass

class IChannelModelCreator(openwns.interface.Interface):
    """
    Defines the interface for Channel Model creation strategies
    """

    @openwns.interface.abstractmethod
    def create(self):
        """"
        Implement the strategy to build your Channel Model here

        @rtype: rise.scenario.Propagation.Configuration
        @return: The new Channel Model that was created
        """
        pass

