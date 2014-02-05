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

import openwns.module
import openwns.pyconfig
import openwns.node
import openwns.Buffer
import openwns.ARQ
import openwns.CRC
import openwns.Probe
import openwns.FUN
import openwns.logger
import openwns.SAR
import openwns.Tools
import openwns.Multiplexer
import openwns.ARQ

import glue.evaluation

class Logger(openwns.logger.Logger):
    """A special Logger for GLUE

    The Logger's Module name is set to GLUE"""

    def __init__(self, name, enabled, parent = None, **kw):
        super(Logger, self).__init__("GLUE", name, enabled, parent, **kw)


class Glue(openwns.module.Module):
    """GLUE Module Configuration

    Needed in order to load this Module by WNS."""
    def __init__(self):
        # The probes config might disappear with pyconfig probes ...
        super(Glue, self).__init__("Glue", "glue")

class Component(openwns.node.Component):
    """Represents a generic data link layer in a openwns.Node.Node"""

    nameInComponentFactory = 'glue.Component'
    """In C++ the node will ask the ComponentFactory with this name
    to build a Component (of this special type)"""

    logger = None
    """Logger configuration"""

    loggerEnabled = True
    """Logger enabled/disabled"""

    stationType = None
    """Station type"""

    unicastDataTransmission = None
    """FQSN to the unicastDataTransmission service I'm offering """

    broadcastDataTransmission = None
    """FQSN to the broadcastDataTransmission service I'm offering """

    unicastNotification = None
    """FQSN to the unicastNotification service I'm offering """

    broadcastNotification = None
    """FQSN to the broadcastNotification service I'm offering """

    phyDataTransmission = None
    """FQSN (Fully Qualified Service Name) of a physical layer to be
    used for communication"""

    phyNotification = None
    """FQSN (Fully Qualified Service Name) of a physical layer
    to be used for notification"""

    fun = None
    """Functional Unit Network"""

    unicastUpperConvergence = None
    """This is a special Functional Unit in the 'fun' which is used to
    communicate with higher layers."""

    broadcastUpperConvergence = None
    """This is a special Functional Unit in the 'fun' which is used to
    communicate with higher layers."""

    dispatcher = None

    lowerConvergence = None
    """ This is a special Functional Unit in the 'fun' which is used
    to communicate with the physical layer. This Functional Unit must
    fit to the physical layer. It is expected that this Functional
    Unit can be exchanged in order to use another physical layer and
    other parts of the protocol stack can remain"""

    bottleNeckDetective = None
    """ Prints messages showing where the Compounds in a FUN a
    currently located """

    layerName = None
    """(msg) Can this be removed (read name instead)?"""

    address = None

    nextAddress = 1

    def __init__(self, node, name, phyDataTransmission, phyNotification, **kw):
        """ Create Logger and FUN.

        The FUN created here is initialized with three Functional Units:
        lowerConvergence, unicastUpperConvergence and broadcastUpperConvergence. By default
        lowerConvergence is set to Lower2Copper(), unicastUpperConvergence
        is set to UnicastUpperConvergence() and broadcastUpperConvergence is set to
        BroadcastUpperConvergence(). _node is set as Loggers parent."""

        super(Component, self).__init__(node, name)
        self.logger = Logger("Glue", self.loggerEnabled, node.logger)
        self.logger.level = 2
        self.stationType = "client"
        self.address = Component.nextAddress
        Component.nextAddress += 1
        self.phyDataTransmission = phyDataTransmission
        self.phyNotification = phyNotification

        self.fun = openwns.FUN.FUN()
        self.unicastUpperConvergence = openwns.FUN.Node("unicastUpperConvergence", UnicastUpperConvergence(self.logger, self.loggerEnabled))
        self.broadcastUpperConvergence = openwns.FUN.Node("broadcastUpperConvergence", BroadcastUpperConvergence(self.logger, self.loggerEnabled))

        self.dispatcher = openwns.FUN.Node("dispatcher", openwns.Multiplexer.Dispatcher(opcodeSize = 1, parentLogger=self.logger))

        self.bottleNeckDetective = openwns.FUN.Node("bottleNeckDetective", openwns.Tools.BottleNeckDetective(0.0, 1.0, self.logger))

        self.unicastDataTransmission = name + ".dllUnicastDataTransmission"
        self.unicastNotification = name + ".dllUnicastNotification"

        self.broadcastDataTransmission = name + ".dllBroadcastDataTransmission"
        self.broadcastNotification = name + ".dllBroadcastNotification"

        self.layerName = name

        openwns.pyconfig.attrsetter(self, kw)
        # placed after the attrsetter in order to allow
        # unicastUpperConvergence, broadcastUpperConvergence and lowerConvergence to be set from
        # constructor
        self.fun.add(self.unicastUpperConvergence)
        self.fun.add(self.broadcastUpperConvergence)
        self.fun.add(self.dispatcher)
        self.fun.add(self.bottleNeckDetective)


class Component2Copper(Component):

    def __init__(self,  node, name, phyDataTransmission, phyNotification, **kw):
        super(Component2Copper, self).__init__(node, name, phyDataTransmission, phyNotification, **kw)

        self.lowerConvergence = openwns.FUN.Node(
            "lowerConvergence",
            glue.Glue.Lower2Copper(unicastRouting = self.unicastUpperConvergence.commandName,
                         broadcastRouting = self.broadcastUpperConvergence.commandName,
                         blocking = False,
                         parentLogger = self.logger,
                         enabled = self.loggerEnabled))
        self.fun.add(self.lowerConvergence)



class UpperConvergence(openwns.pyconfig.Sealed):
    """(msg) should may be be renamed to Service.DataLinkLayer?"""

    logger = None
    """Logger configuration"""

    def __init__(self, parentLogger = None, enabled = True):
        self.logger = Logger("UpperConvergence", enabled, parentLogger)

class UnicastUpperConvergence(UpperConvergence):
    __plugin__ = 'glue.convergence.UnicastUpper'
    """Name in FunctionalUnitFactory"""

    def __init__(self, parentLogger = None, enabled = True):
        super(UnicastUpperConvergence, self).__init__(parentLogger, enabled)

class BroadcastUpperConvergence(UpperConvergence):
    __plugin__ = 'glue.convergence.BroadcastUpper'
    """Name in FunctionalUnitFactory"""

    def __init__(self, parentLogger = None, enabled = True):
        super(BroadcastUpperConvergence, self).__init__(parentLogger, enabled)

class Lower2Copper(openwns.pyconfig.Sealed):
    """(msg) should may be be renamed to User.Copper?"""

    __plugin__ = 'glue.convergence.Lower2Copper'
    """Name in FunctionalUnitFactory"""

    unicastRouting = None
    """Functional Unit Friend: provides the peer instance to
    communicate with"""

    broadcastRouting = None
    """Functional Unit Friend: provides the peer instance to
    communicate with"""

    logger = None
    """Logger configuration"""

    blocking = None
    """ Defines whether the FU is accepting if the Copper below is
    free or not (it True, FU is not accepting if Copper is not free
    ...)

    """

    phyTraceProbeName = None

    def __init__(self, unicastRouting, broadcastRouting, blocking = True, parentLogger = None, enabled = True):
        self.unicastRouting = unicastRouting
        self.broadcastRouting = broadcastRouting
        self.logger = Logger("LowerConvergence", enabled, parentLogger)
        self.blocking = blocking
        self.phyTraceProbeName = "glue.phyTrace"

class Lower2OFDMAPhy(object):
    
    __plugin__ = 'glue.convergence.Lower2OFDMAPhy'
    
    def __init__(self, 
                unicastRouting, 
                broadcastRouting, 
                modulation, 
                symbolDuration, 
                numSubcarriers, 
                parentLogger = None):
      
        import rise
        import rise.PhyMode
        import openwns.interval
        
        self.phyModeMapper = rise.PhyMode.PhyModeMapper(symbolDuration, numSubcarriers)
        self.PhyMode = rise.PhyMode.PhyMode(modulation + "-No")
        self.phyModeMapper.addPhyMode(openwns.interval.Interval(-200.0,200.0,"(]"),self.PhyMode)
        self.phyModeMapper.setMinimumSINR(
            self.phyModeMapper.mapEntries[0].sinrInterval.lowerBound)
        
        self.unicastRouting = unicastRouting
        self.broadcastRouting = broadcastRouting
        self.logger = Logger("LowerConvergence", True, parentLogger)
        
        

class CSMACAMAC(openwns.FUN.FunctionalUnit):
    """ DON'T USE RIGHT NOW

    This FU is not tested (see CSMACATest why).
    """
    logger = None
    __plugin__ = 'glue.CSMACA'

    sifsLength = None
    slotLength = None
    ackLength = None
    stopAndWaitARQName = None
    phyNotification = None
    backoffLogger = None

    def __init__(self, commandName=None, sifsLength=16E-6, slotLength=9E-6, ackLength=44E-6, stopAndWaitARQName=None, phyNotification=None, parentLogger=None):
        super(CSMACAMAC, self).__init__(commandName=commandName)
        self.logger = Logger(name = "CSMACA", enabled = True, parent = parentLogger)
        self.backoffLogger = Logger(name = "Backoff", enabled = True, parent = self.logger)
        self.sifsLength = sifsLength
        self.slotLength = slotLength
        self.ackLength = ackLength
        self.stopAndWaitARQName = stopAndWaitARQName
        self.phyNotification = phyNotification

class StopAndWait(openwns.ARQ.StopAndWait):
    """ Special version of StopAndWait for CSMACAMAC """

    __plugin__ = 'glue.StopAndWait'
    """ Name in FU Factory """

    phyDataTransmissionFeedback = None
    """ The name of the phy service telling us when a transmission has taken place """

    phyNotification = None
    """ The ARQ needs to know when the PHY starts to receive the ACK frame """

    shortResendTimeout = None
    longResendTimeout = None
    
    def __init__(self, phyDataTransmissionFeedbackName, phyNotification, **kw):
        super(StopAndWait, self).__init__(**kw)
        self.phyDataTransmissionFeedback = phyDataTransmissionFeedbackName
        self.phyNotification = phyNotification

class Aloha(openwns.FUN.FunctionalUnit):

    logger = None
    __plugin__ = 'glue.mac.Aloha'

    maximumWaitingTime = None
    """ Maximum time (in seconds) to wait before medium access, the
    actual time will be uniformly distributed ..."""

    def __init__(self, commandName, maximumWaitingTime = 0.01, parentLogger = None):
        super(Aloha, self).__init__(commandName=commandName)
        self.maximumWaitingTime = maximumWaitingTime
        self.logger = Logger(name = "Aloha", enabled = True, parent = parentLogger)
