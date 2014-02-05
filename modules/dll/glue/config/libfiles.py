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

libname = 'glue'
srcFiles = dict()

srcFiles = [
    'src/Glue.cpp',
    'src/Component.cpp',

    # convergence
    'src/convergence/Upper.cpp',
    'src/convergence/Lower2Copper.cpp',
    'src/convergence/Lower2OFDMAPhy.cpp',

    'src/Routing.cpp',
    'src/BERProvider.cpp',
    'src/BERConsumer.cpp',
    'src/BERMeasurementReporting.cpp',
    'src/Pilot.cpp',
    'src/KeyBuilder.cpp',

    # macs
    'src/mac/Aloha.cpp',
    'src/mac/CSMACA.cpp',
    'src/mac/Backoff.cpp',

    'src/trigger/Trigger.cpp',

    'src/arqfsm/stopandwait/FSMFU.cpp',
    'src/arqfsm/stopandwait/BaseState.cpp',
    'src/arqfsm/stopandwait/ReadyForTransmission.cpp',
    'src/arqfsm/stopandwait/WaitingForACK.cpp',

    'src/arqfsm/selectiverepeat/FSMFU.cpp',
    'src/arqfsm/selectiverepeat/BaseState.cpp',
    'src/arqfsm/selectiverepeat/ReadyForTransmissionBufferEmpty.cpp',
    'src/arqfsm/selectiverepeat/ReadyForTransmissionBufferPartlyFilled.cpp',
    'src/arqfsm/selectiverepeat/WaitingForACKsBufferFull.cpp',

    'src/tests/RoutingTest.cpp',
    'src/tests/BERProviderConsumerTest.cpp',
    'src/tests/BERMeasurementReportingTest.cpp',
    'src/tests/PilotTest.cpp',
    'src/tests/StamperTest.cpp',

    'src/trigger/tests/TriggerTest.cpp',

    'src/convergence/tests/UnicastBroadcastTest_Copper.cpp',

    'src/mac/tests/AlohaTest.cpp',
    'src/mac/tests/BackoffTest.cpp',
    'src/mac/tests/CSMACATest.cpp',

 #   'src/arqfsm/stopandwait/tests/StopAndWaitTest.cpp',
 #   'src/arqfsm/selectiverepeat/tests/SelectiveRepeatFSMTest.cpp',
    ]

hppFiles = [
'src/arqfsm/ARQBaseState.hpp',
'src/arqfsm/InSignals.hpp',
'src/arqfsm/OutSignals.hpp',
'src/arqfsm/selectiverepeat/BaseState.hpp',
'src/arqfsm/selectiverepeat/FSMFU.hpp',
'src/arqfsm/selectiverepeat/ReadyForTransmissionBufferEmpty.hpp',
'src/arqfsm/selectiverepeat/ReadyForTransmissionBufferPartlyFilled.hpp',
'src/arqfsm/selectiverepeat/WaitingForACKsBufferFull.hpp',
'src/arqfsm/stopandwait/BaseState.hpp',
'src/arqfsm/stopandwait/FSMFU.hpp',
'src/arqfsm/stopandwait/InSignals.hpp',
'src/arqfsm/stopandwait/OutSignals.hpp',
'src/arqfsm/stopandwait/ReadyForTransmission.hpp',
'src/arqfsm/stopandwait/tests/StopAndWaitTest.hpp',
'src/arqfsm/stopandwait/WaitingForACK.hpp',
'src/BERConsumer.hpp',
'src/BERMeasurementReporting.hpp',
'src/BERProvider.hpp',
'src/Component.hpp',
'src/convergence/Lower2Copper.hpp',
'src/convergence/Lower2OFDMAPhy.hpp',
'src/convergence/Lower.hpp',
'src/convergence/Upper.hpp',
'src/Glue.hpp',
'src/KeyBuilder.hpp',
'src/mac/Aloha.hpp',
'src/mac/Backoff.hpp',
'src/mac/CSMACA.hpp',
'src/Pilot.hpp',
'src/Routing.hpp',
'src/Stamper.hpp',
'src/trigger/FunctionalUnitLight.hpp',
'src/trigger/Trigger.hpp',
]

pyconfigs = [
'glue/Routing.py',
'glue/BERMeasurementReporting.py',
#   'glue/InSequenceChecker.py',
'glue/Stamper.py',
'glue/Trigger.py',
'glue/ARQFSM.py',
'glue/Pilot.py',
'glue/Glue.py',
'glue/KeyBuilder.py',
'glue/__init__.py',
'glue/support/SubFUN.py',
'glue/support/Aloha.py',
'glue/support/tests/ConfigurationTest.py',
'glue/support/tests/__init__.py',
'glue/support/StopAndWait.py',
'glue/support/GoBackN.py',
'glue/support/CSMACA.py',
'glue/support/Configuration.py',
'glue/support/SelectiveRepeat.py',
'glue/support/__init__.py',
'glue/support/ShortCut.py',
'glue/CompoundBacktracker.py',
'glue/evaluation/__init__.py',
'glue/evaluation/default.py',
'glue/evaluation/csma.py',
'glue/evaluation/acknowledgedModeShortCut.py'
]
dependencies = []
Return('libname srcFiles hppFiles pyconfigs dependencies')
