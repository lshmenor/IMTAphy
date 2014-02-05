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

import openwns.module
import openwns.node
import openwns.logger
import openwns.FUN
import openwns.Group
import openwns.FlowSeparator
import openwns.Buffer

class TCP(openwns.module.Module):
    def __init__(self):
        super(TCP, self).__init__("tcp", "tcp")

class Logger(openwns.logger.Logger):
    """A special Logger for TCP

    The Logger's Module name is set to TCP"""

    def __init__(self, name, enabled, parent = None, **kw):
        super(Logger, self).__init__("TCP", name, enabled, parent, **kw)


class TCPComponent(openwns.node.Component):
    """Represents TCP entity"""

    nameInComponentFactory = "tcp.Component"
    logger = None

    service = None
    serviceConfig = None
    protocolNumber = 'TCP'
    dataLinkLayer = None

    fun = None
    """ Functional Unit Network """
    tcpFlowHandler = None
    """ Used for FlowEstablishment at Layer2 """
    flowEstablishmentAndRelease = None

    dllNotification = None

    subFUN = None
    """ Prototype of function unit network for flow separator """

    group = None
    """ The flow separator's prototype """

    ipDataTransmission = None
    """ Name of the service to use for data transmission via IP """

    ipNotification = None
    """ Name of the service to use for notification from IP """

    dnsService = "ip.dns"

    tcpHeader = None

    upperConvergence = None
    """ Upper FU in the flow separator  """

    subFUNBuffer = None
    """ Buffer FU in the flow separator """

    subFUNSAR = None

    subFUNARQ = None
    """ Lower FU in the flow separator """
    subFUNHandshakeStrategy = None
    """ TCP's finite state machine for 3-way-handshake connection establishement and termination """
    subFUNDispatcher = None
    """ Responsible for dispatching TCP's user data plane and control plane """

    flowSeparator = None
    """ Flow separation for different TCP connections """

    lowerConvergence = None
    """ The bottom FU in TCP's FUN  """

    portUnbindDelay = None
    """ Source Ports will be reused after this delay """

    def __init__(self, node, name, ipDataTransmission, ipNotification, _portUnbindDelay = 60.0):
        super(TCPComponent, self).__init__(node, name)
        self.logger = Logger("TCP", True, node.logger)
        self.service = "tcp.connectionService"
        self.serviceConfig = Service(self.logger, _portUnbindDelay, "subFUN.3WH")
        
        self.ipDataTransmission = ipDataTransmission
        self.ipNotification = ipNotification
        self.fun = openwns.FUN.FUN()
        self.tcpHeader = openwns.FUN.Node("tcp.tcpHeader", TCPHeader(parentLogger=self.logger))

        self.upperConvergence = openwns.FUN.Node("TCP.upperConvergence", UpperConvergence(self.logger))
        self.subFUN = openwns.FUN.FUN()
        #self.subFUNARQ = openwns.FUN.Node('subFUN.ARQ', openwns.ARQ.CumulativeACK(self.logger))
        self.subFUNARQ = openwns.FUN.Node('subFUN.ARQ', CumulativeACK(self.logger))
        self.subFUNSAR = openwns.FUN.Node('subFUN.SAR', openwns.SAR.Fixed(segmentSize=512, parentLogger = self.logger))
        self.subFUNBuffer = openwns.FUN.Node('subFUN.Buffer', openwns.Buffer.Dropping(size=512))
        self.subFUNHandshakeStrategy = openwns.FUN.Node('subFUN.3WH', ThreeWayHandshake(self.logger, 'TCP.upperConvergence'))
        self.subFUNDispatcher = openwns.FUN.Node('subFUN.Dispatcher', openwns.Multiplexer.Dispatcher(opcodeSize = 1, parentLogger=self.logger))
        self.subFUN.add(self.subFUNARQ)
        self.subFUN.add(self.subFUNSAR)
        self.subFUN.add(self.subFUNBuffer)
        self.subFUN.add(self.subFUNDispatcher)
        self.subFUN.add(self.subFUNHandshakeStrategy)
        
        #self.subFUNSAR.connect(self.subFUNBuffer)
        #self.subFUNBuffer.connect(self.subFUNARQ)
        #self.subFUNARQ.connect(self.subFUNDispatcher)
        self.subFUNBuffer.connect(self.subFUNDispatcher)
        self.subFUNHandshakeStrategy.connect(self.subFUNDispatcher)
        
        #self.group = openwns.Group.Group(self.subFUN, 'subFUN.SAR', 'subFUN.Dispatcher')
        self.group = openwns.Group.Group(self.subFUN, 'subFUN.Buffer', 'subFUN.Dispatcher')
        
        #ifNotFoundStrategy = openwns.FlowSeparator.PrototypeCreator('flowSeparatorPrototype', self.group)
        #creator = openwns.FlowSeparator.Prototype('flowSeparatorPrototype', self.group)
        creator = openwns.FlowSeparator.Config('flowSeparatorPrototype', self.group)
        ifNotFoundStrategy = openwns.FlowSeparator.CreateOnValidFlow(creator, 'fip')
        self.flowSeparator = openwns.FUN.Node("flowSeparator",
                                          openwns.FlowSeparator.FlowSeparator(FlowIDBuilder("TCP.upperConvergence"),
                                                                          ifNotFoundStrategy))
        self.lowerConvergence = openwns.FUN.Node("TCP.lowerConvergence", LowerConvergence('TCP.upperConvergence', self.logger))
        self.portUnbindDelay = _portUnbindDelay

        self.fun.add(self.tcpHeader)
        self.fun.add(self.upperConvergence)
        self.fun.add(self.flowSeparator)
        self.fun.add(self.lowerConvergence)
        self.upperConvergence.connect(self.flowSeparator)
        self.flowSeparator.connect(self.lowerConvergence)

    def addFlowHandling(self, _dllNotification, _flowEstablishmentAndRelease):
        self.dllNotification = _dllNotification
        self.flowEstablishmentAndRelease = _flowEstablishmentAndRelease


class UDPComponent(openwns.node.Component):
    """Represents TCP entity only with UDP functionality"""

    nameInComponentFactory = "tcp.Component"

    logger = None

    service = None

    serviceConfig = None

    protocolNumber = 'UDP'

    fun = None
    """ Functional Unit Network """

    subFUN = None
    """ Prototype of function unit network for flow separator """

    group = None
    """ The flow separator's prototype """

    ipDataTransmission = None
    """ Name of the service to use for data transmission via IP """

    ipNotification = None
    """ Name of the service to use for notification from IP """

    flowEstablishmentAndRelease = None

    dllNotification = None

    dnsService = "ip.dns"

    udpHeader = None

    upperConvergence = None
    """ Upper FU in the flow separator  """

    subFUNBuffer = None
    """ Buffer FU in the flow separator """

    subFUNHandshakeStrategy = None
    """
    FU for UDP's connection establishement and termination.
    Since the UDP's flow separator's prototype only conists of a buffer,
    this FU is not connected to any other FU in the flow separator, but
    only correspond with the service
    """

    lowerConvergence = None
    """ The bottom FU of UDP's FUN """

    flowSeparator = None
    """ Flow separation for different UDP connections """

    portUnbindDelay = None
    """ Source Ports will be reused after this delay """

    def __init__(self, node, name, ipDataTransmission, ipNotification, _portUnbindDelay = 60.0):
        super(UDPComponent, self).__init__(node, name)
        self.logger = Logger("UDPComponent", True, node.logger)
        self.service = "tcp.connectionService"
        self.serviceConfig = Service(self.logger, _portUnbindDelay, "subFUN.1WH")

        self.ipDataTransmission = ipDataTransmission
        self.ipNotification = ipNotification

        self.fun = openwns.FUN.FUN()

        self.udpHeader = openwns.FUN.Node("tcp.tcpHeader", UDPHeader(parentLogger=self.logger))

        self.upperConvergence = openwns.FUN.Node("UDP.upperConvergence", UpperConvergence(self.logger))

        self.subFUN = openwns.FUN.FUN()

        self.subFUNBuffer = openwns.FUN.Node("subFUN.Buffer", openwns.Buffer.Dropping())
        self.subFUNHandshakeStrategy = openwns.FUN.Node("subFUN.1WH", OneWayHandshake(self.logger))

        self.subFUN.add(self.subFUNBuffer)
        self.subFUN.add(self.subFUNHandshakeStrategy)

        self.group = openwns.Group.Group(self.subFUN, 'subFUN.Buffer', 'subFUN.Buffer')

        #ifNotFoundStrategy = openwns.FlowSeparator.PrototypeCreator('flowSeparatorPrototype', self.group)
        creator = openwns.FlowSeparator.Prototype('flowSeparatorPrototype', self.group)
        #creator = openwns.FlowSeparator.Config('flowSeparatorPrototype', self.group)
        ifNotFoundStrategy = openwns.FlowSeparator.CreateOnValidFlow(creator, 'fip')

        self.flowSeparator = openwns.FUN.Node("flowSeparator",
                                          openwns.FlowSeparator.FlowSeparator(FlowIDBuilder("UDP.upperConvergence"),
                                                                          ifNotFoundStrategy))

        self.lowerConvergence = openwns.FUN.Node("UDP.lowerConvergence", LowerConvergence("UDP.upperConvergence", self.logger))

        self.portUnbindDelay = _portUnbindDelay

        self.fun.add(self.udpHeader)
        self.fun.add(self.upperConvergence)
        self.fun.add(self.flowSeparator)
        self.fun.add(self.lowerConvergence)

        self.upperConvergence.connect(self.flowSeparator)
        self.flowSeparator.connect(self.lowerConvergence)

    def addFlowHandling(self, _dllNotification, _flowEstablishmentAndRelease):
        self.dllNotification = _dllNotification
        self.flowEstablishmentAndRelease = _flowEstablishmentAndRelease


class Connection(openwns.pyconfig.Sealed):
    """Represents a connection"""

    __plugin__ = 'tcp.connection'
    """Name in FunctionalUnitFactory"""

    logger = None
    """Logger configuration"""

    def __init__(self, parentLogger):
        self.logger = Logger("Connection", True, parentLogger)

class TCPHeader(openwns.pyconfig.Sealed):

    __plugin__ = "tcp.tcpHeader"

    headerSize = 20*8

    logger = None

    def __init__(self, parentLogger=None):
        self.logger = Logger("TCPHeader", True, parentLogger)

class UDPHeader(openwns.pyconfig.Sealed):

    __plugin__ = "tcp.tcpHeader"

    headerSize = 8*8

    logger = None

    def __init__(self, parentLogger=None):
        self.logger = Logger("UDPHeader", True, parentLogger)

class LowerConvergence(openwns.pyconfig.Sealed):
    """Responsible for forwarding IP packets to the flow separator"""

    __plugin__ = 'tcp.lowerConvergence'
    """Name in FunctionalUnitFactory"""

    logger = None
    """Logger configuration"""

    upperConvergence = None
    """Friend FU of the lower convergence"""

    flowSeparator = None

    def __init__(self, upperConvergenceName, parentLogger):
        self.logger = Logger("LowerConvergence", True, parentLogger)
        self.upperConvergence = upperConvergenceName
        self.flowSeparator = "flowSeparator"

class FlowHandler(openwns.pyconfig.Sealed):

    __plugin__ = "tcp.FlowHandler"
    logger = None

    def __init__(self, parentLogger):
        self.logger = Logger("FlowHandler", True, parentLogger)

class Service(openwns.pyconfig.Sealed):

    logger = None
    """Logger configuration"""

    connection = None

    portUnbindDelay = None

    handshakeStrategy = None

    def __init__(self, parentLogger, _portUnbindDelay, _handshakeStrategy):
        self.logger = Logger("Service", True, parentLogger)
        self.portUnbindDelay = _portUnbindDelay
        self.connection = Connection(self.logger)
        self.handshakeStrategy = _handshakeStrategy



class UpperConvergence( openwns.pyconfig.Sealed):
    __plugin__ = "tcp.upperConvergence"
    """Name in FunctionUnitFactory"""

    logger = None

    def __init__(self, parentLogger):
        self.logger = Logger("UpperConvergence", True, parentLogger)

class FlowIDBuilder( openwns.pyconfig.Sealed):
    __plugin__ = 'tcp.FlowIDBuilder'

    upperConvergence = None
    """The friend of the flow-id builder"""

    def __init__(self, upperConvergenceName):
        self.upperConvergence = upperConvergenceName


class ThreeWayHandshake( openwns.pyconfig.Sealed):
    __plugin__ = 'tcp.ThreeWayHandshake'

    logger = None

    tcpService = None

    upperConvergence = None
    """ The friend FU """

    def __init__(self, parentLogger, upperConvergenceName):
        self.logger = Logger("ThreeWayHandshake", True, parentLogger)
        self.tcpService = 'tcp.connectionService'
        self.upperConvergence = upperConvergenceName


class OneWayHandshake( openwns.pyconfig.Sealed):
    __plugin__ = 'udp.OneWayHandshake'

    logger = None

    udpService = None

    def __init__(self, parentLogger):
        self.logger = Logger("OneWayHandshake", True, parentLogger)
        self.udpService = 'tcp.connectionService'


class CongestionControl( openwns.pyconfig.Sealed):
    __plugin__ = 'tcp.CongestionControl'

    logger = None

    slowStart = None
    tahoeCA = None

    def __init__(self, parentLogger=None):
        self.logger = Logger("CongestionControl", True, parentLogger)

        self.slowStart = SlowStart(parentLogger)
        self.tahoeCA = TahoeCongAvoid(parentLogger)


class SlowStart( openwns.pyconfig.Sealed):
    __plugin__ = 'tcp.SlowStart'

    logger = None
    ssthresh = None
    congestionWindow = None
    retransmissionTimeout = None
    """ Time between retransmitting a TCP segment given in seconds  """

    def __init__(self, parentLogger=None):
        self.logger = Logger('SlowStart', True, parentLogger)
        self.ssthresh = 96#256
        self.congestionWindow = 1
        self.retransmissionTimeout = 2


class TahoeCongAvoid( openwns.pyconfig.Sealed):
    __plugin__ = 'tcp.TahoeCongAvoid'

    logger = None
    congestionWindow = None
    congestionWindowCounter = None
    retransmissionTimeout = None
    """ Time between retransmitting a TCP segment given in seconds  """

    numberDupACKs = None
    """ Initial threshold to enter fast retransmit  """

    def __init__(self, parentLogger=None):
        self.logger = Logger('TahoeCongAvoid', True, parentLogger)
        self.congestionWindow = 1
        self.congestionWindowCounter = 0
        self.retransmissionTimeout = 2
        self.numberDupACKs = 3


class CumulativeACK( openwns.pyconfig.Sealed):
    __plugin__ = 'tcp.CumulativeACK'

    logger = None

    congestionControl = None
    """ TCP congestion control instance  """

    advertisedWindow = None
    """ The receiver's of incoming TCP segments """

    def __init__(self, parentLogger=None, advWin=32):
        self.logger = Logger('CumulativeACK', True, parentLogger)
        self.congestionControl = CongestionControl(parentLogger)
        self.advertisedWindow = advWin
