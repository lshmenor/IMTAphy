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

from openwns.pyconfig import attrsetter
import openwns.Multiplexer


class TimingControl(object):
    __plugin__ = "wns.ldk.fcf.TimingControl"
    name = "wns.ldk.fcf.TimingControl"

class DurationPolicy(object):
    Fixed = 1
    Expanding = 2
    
class PhaseDescriptor(object):
    compoundCollector = None
    
class BasicPhaseDescriptor(PhaseDescriptor):
    __plugin__ = "wns.ldk.fcf.BasicPhaseDescriptor"

    def __init__(self, compoundCollector ):
        super(BasicPhaseDescriptor, self).__init__()
        self.compoundCollector = compoundCollector

class FrameBuilderNode(object):
    name = None
    config = None
    frameBuilder = None

    def __init__(self, name, config):
        self.name = name
        self.config = config
        
class FrameBuilder(openwns.Multiplexer.FrameDispatcher):
    __plugin__ = "wns.ldk.fcf.FrameBuilder"
    phaseDescriptor = None
    frameDuration = None
    symbolDuration = None
    timingControl = None
    
    def __init__(self, opcodeSize, timingControl = TimingControl(), **kw):
        super(FrameBuilder, self).__init__(opcodeSize)
        self.phaseDescriptor = []
        self.timingControl = timingControl
        attrsetter(self, kw)

    def add(self, descriptor):
    #    descriptor.frameBuilder = self
    
        self.phaseDescriptor.append( descriptor )
    #    return descriptor
    

#class FrameBuilderPutter(Sealed):
#    __plugin__ = "wns.ldk.fcf.FrameBuilderPutter"
#    frameBuilderName = None

#    def __init__(self, frameBuilderName):
#        self.frameBuilderName = frameBuilderName


class CompoundCollector(object):
    frameBuilderName = None

    def __init__(self, frameBuilderName):
        self.frameBuilderName = frameBuilderName
