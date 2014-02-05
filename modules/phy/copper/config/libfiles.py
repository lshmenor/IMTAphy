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

libname = 'copper'
srcFiles = dict()

srcFiles = [
    'src/Copper.cpp',
    'src/Transceiver.cpp',
    'src/Transmitter.cpp',
    'src/Receiver.cpp',
    'src/Wire.cpp',
    'src/Transmission.cpp',

    'src/tests/WireTest.cpp',
    'src/tests/ReceiverTest.cpp',
    'src/tests/TransmitterTest.cpp',
    ]

hppFiles = [
    'src/Receiver.hpp',
    'src/ReceiverInterface.hpp',
    'src/tests/TransmitterDataSentMock.hpp',
    'src/tests/ReceiverMock.hpp',
    'src/Transceiver.hpp',
    'src/Transmitter.hpp',
    'src/Transmission.hpp',
    'src/Copper.hpp',
    'src/Wire.hpp',

    ]

pyconfigs = [
'copper/TimeDependentDistBER.py',
'copper/Copper.py',
'copper/__init__.py',
]
dependencies = []
Return('libname srcFiles hppFiles pyconfigs dependencies')
