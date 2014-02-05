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

import openwns.node
import openwns.logger
import openwns.qos
from openwns.pyconfig import attrsetter

class Logger(openwns.logger.Logger):
    def __init__(self, name, enabled, parent = None, **kw):
        super(Logger, self).__init__("CONSTANZE", name, enabled, parent, **kw)
        openwns.logger.globalRegistry.addLogger("CONSTANZE", self)

# TODO: pack this into Measurement.py:
class Measurement(object):
    MMPPestimationResultFileName = None
    probeWindow = None
    estimatorNumberOfStates = None # for MMPP parameter estimation
    meanRateList = None
    logger = None

    def __init__(self, **kw):
        attrsetter(self, kw)

# the data sink
class Listener(object):
    domainName = None
    logger = None
    probeWindow = 1.0 # [s], 1.0 is quite slow, 0.01s=10ms is often reasonable, but may generate a lot of samples
    doMMPPestimation = False
    measurement = None # Measurement()

    def __init__(self, _domainName, parentLogger = None, **kw):
        self.domainName = _domainName
        self.logger = Logger("Listener", True, parentLogger)
        measurement = Measurement()
        attrsetter(self, kw)

class UDPBinding(object):
    nameInBindingFactory = "UdpBinding"
    # the UDP service
    udpService = "tcp.connectionService"
    # UDP specific parameters
    domainName = None
    destinationDomainName = None
    destinationPort = None
    qosClass = None
    logger = None

    def __init__(self, _domainName, _destinationDomainName, _destionationPort, qosClass = openwns.qos.undefinedQosClass, parentLogger = None):
        self.domainName = _domainName;
        self.destinationDomainName = _destinationDomainName
        self.destinationPort = _destionationPort
        self.qosClass = qosClass.number
        self.logger = Logger("UDPBinding", True, parentLogger)

class UDPListenerBinding(object):
    nameInBindingFactory = "UdpListenerBinding"
    # the UDP service
    udpService = "tcp.connectionService"
    # UDP specific parameters
    listenPort = None
    logger = None

    def __init__(self, _listenPort, parentLogger = None):
        self.listenPort = _listenPort
        self.logger = Logger("UDPListenerBinding", True, parentLogger)

class TCPBinding(object):
    nameInBindingFactory = "TcpBinding"
    # the TCP service
    tcpService = "tcp.connectionService"
    # TCP specific parameters
    domainName = None
    destinationDomainName = None
    destinationPort = None
    qosClass = None
    logger = None

    def __init__(self, _domainName, _destinationDomainName, _destionationPort, qosClass = openwns.qos.undefinedQosClass, parentLogger = None):
        self.domainName = _domainName
        self.destinationDomainName = _destinationDomainName
        self.destinationPort = _destionationPort
        self.qosClass = qosClass.number
        self.logger = Logger("TCPBinding", True, parentLogger)

class TCPListenerBinding(object):
    nameInBindingFactory = "TcpListenerBinding"
    # the TCP service
    tcpService = "tcp.connectionService"
    # TCP specific parameters
    listenPort = None
    logger = None

    def __init__(self, _listenPort, parentLogger = None):
        self.listenPort = _listenPort
        self.logger = Logger("TCPListenerBinding", True, parentLogger)

class BindingStub(object):
    nameInBindingFactory = "BindingStub"
    logger = None
    
    def __init__(self):
        self.logger = Logger("BindingStub", False, None)

class ConstanzeComponent(openwns.node.Component):

    nameInComponentFactory = "Constanze"
    logger = None
    generatorBindings = None
    generators = None # list of generators. A generator is a derived "class Traffic" object, see Constanze.py
    listenerBindings = None
    listeners = None
    __countGenerators__ = 0

    def __init__(self, _node, _name, parentLogger = None):
        super(ConstanzeComponent, self).__init__(_node, _name)
        self.logger = Logger("Constanze", True, parentLogger)
        self.generators = []
        self.generatorBindings = []
        self.listeners = []
        self.listenerBindings = []

    def addTraffic(self, _binding, _generator) :
        self.generatorBindings.append(_binding)
        self.generators.append(_generator)
        indexString = "[" +("%d" % self.__countGenerators__) + "]"
        loggerName = _binding.logger.name.split(".")[-1]
        _binding.logger.name = loggerName + indexString
        loggerName = _generator.logger.name.split(".")[-1]
        _generator.logger.name = loggerName + indexString
        self.__countGenerators__ += 1

    def addListener(self, _binding, _listener) :
        self.listenerBindings.append(_binding)
        self.listeners.append(_listener)

class DllBinding(object):
    nameInBindingFactory = "DllBinding"
    # the DLL service
    dllDataTransmission = None
    # DLL specific parameters
    destinationDllAddress = None
    logger = None

    def __init__(self, _destinationDll, _stationName, parentLogger = None):
        self.destinationDllAddress = _destinationDll
        self.dllDataTransmission = _stationName + ".dllUnicastDataTransmission"
        self.logger = Logger("DllBinding", True, parentLogger)

class DllListenerBinding(object):
    nameInBindingFactory = "DllListenerBinding"
    # the Dll service
    dllNotification = ".dllNotification"
    # Dll specific parameters
    listenDll = None
    logger = None

    def __init__(self, _listenDll, _stationName, parentLogger = None):
        self.listenDll = _listenDll
        self.dllNotification = _stationName + ".dllUnicastNotification"
        self.logger = Logger("DllListenerBinding", True, parentLogger)

class UDPBinding(object):
    nameInBindingFactory = "UdpBinding"
    # the UDP service
    udpService = "tcp.connectionService"
    # UDP specific parameters
    domainName = None
    destinationDomainName = None
    destinationPort = None
    qosClass = None
    logger = None

    def __init__(self, _domainName, _destinationDomainName, _destionationPort, qosClass = openwns.qos.undefinedQosClass, parentLogger = None):
        self.domainName = _domainName;
        self.destinationDomainName = _destinationDomainName
        self.destinationPort = _destionationPort
        self.qosClass = qosClass.number
        self.logger = Logger("UDPBinding", True, parentLogger)

class UDPClientBinding(object):
    nameInBindingFactory = "UdpClientBinding"
    # the UDP service
    udpService = "tcp.connectionService"
    dnsService = "ip.dns"
    # UDP specific parameters
    domainName = None
    destinationDomainName = None
    destinationPort = None
    qosClass = None
    logger = None
    listener = None

    def __init__(self, _domainName, _destinationDomainName, _destinationPort, qosClass = openwns.qos.undefinedQosClass,parentLogger = None, **kw):
        self.domainName = _domainName
        self.destinationDomainName = _destinationDomainName
        self.destinationPort = _destinationPort
        self.qosClass = qosClass.number
        self.logger = Logger("UDPClientBinding", True, parentLogger)
        self.listener = Listener(self.domainName, self.logger, **kw)

class UDPListenerBinding(object):
    nameInBindingFactory = "UdpListenerBinding"
    # the UDP service
    udpService = "tcp.connectionService"
    # UDP specific parameters
    listenPort = None
    logger = None

    def __init__(self, _listenPort, parentLogger = None):
        self.listenPort = _listenPort
        self.logger = Logger("UDPListenerBinding", True, parentLogger)

class UDPServerListenerBinding(object):
    nameInBindingFactory = "UdpServerListenerBinding"
    # the UDP service
    udpService = "tcp.connectionService"
    dnsService = "ip.dns"
    # UDP specific parameters
    listenPort = None
    logger = None
    generator = None

    def __init__(self, _listenPort, _generator, parentLogger = None):
        self.listenPort = _listenPort
        self.generator = _generator
        self.logger = Logger("UDPServerListenerBinding", True, parentLogger)

class TCPClientBinding(object):
    nameInBindingFactory = "TcpClientBinding"
    # the TCP service
    tcpService = "tcp.connectionService"
    dnsService = "ip.dns"
    # TCP specific parameters
    domainName = None
    destinationDomainName = None
    destinationPort = None
    qosClass = None
    logger = None

    def __init__(self, _domainName, _destinationDomainName, _destinationPort, qosClass = openwns.qos.undefinedQosClass, parentLogger = None):
        self.domainName = _domainName
        self.destinationDomainName = _destinationDomainName
        self.destinationPort = _destinationPort
        self.qosClass = qosClass.number
        self.logger = Logger("TCPClientBinding", True, parentLogger)

class TCPServerListenerBinding(object):
    nameInBindingFactory = "TcpServerListenerBinding"
    # the TCP service
    tcpService = "tcp.connectionService"
    dnsService = "ip.dns"
    # TCP specific parameters
    listenPort = None
    logger = None
    generator = None

    def __init__(self, _listenPort, _generator, parentLogger = None):
        self.listenPort = _listenPort
        self.generator = _generator
        self.logger = Logger("TCPServerListenerBinding", True, parentLogger)

class IPBinding(object):
    nameInBindingFactory = "IpBinding"
    # the IP service
    ipDataTransmission = "ip.dataTransmission"
    dnsService = "ip.dns"
    # IP specific parameters
    sourceDomainName = None
    destinationDomainName = None
    logger = None

    def __init__(self, _sourceDomainName, _destinationDomainName, parentLogger = None):
        self.sourceDomainName = _sourceDomainName
        self.destinationDomainName = _destinationDomainName
        self.logger = Logger("IPBinding", True, parentLogger)

class IPListenerBinding(object):
    nameInBindingFactory = "IpListenerBinding"
    # the IP service
    ipNotification = "ip.notification"
    # IP specific parameters
    listenDomainName = None
    logger = None

    def __init__(self, _listenDomainName, parentLogger = None):
        self.listenDomainName = _listenDomainName
        self.logger = Logger("IPListenerBinding", True, parentLogger)


