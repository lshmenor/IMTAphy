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
##############################################################################

from wnsbase.playground.Project import *
import wnsbase.RCS as RCS

bzrBaseURL = "https://launchpad.net/"

root = Root('./',"openwns-sdk", bzrBaseURL,
            RCS.Bazaar('.', 'sdk', 'main', '1.0'))

unitTests = Generic('./tests/unit', 
                'openwns-unittest', 
                bzrBaseURL,
                RCS.Bazaar('./tests/unit', 
                    'unittest', 'main', '1.0'))

documentation = MasterDocumentation('./documentation', 
                'openwns-documentation', 
                bzrBaseURL,
                RCS.Bazaar('./documentation', 
                    'documentation', 'main', '1.0'))

library = Library('./framework/library',
                "openwns-library", 
                bzrBaseURL,
                RCS.Bazaar('framework/library', 
                    'library', 'main', '1.0'),
                'WNS')

application = Binary('./framework/application', 
                "openwns-application", 
                bzrBaseURL,
                RCS.Bazaar('framework/application', 
                    'application', 'main', '1.0'),
                )

pywns = Python("./framework/pywns", 
                "openwns-pywns", 
                bzrBaseURL,
                RCS.Bazaar('framework/pywns', 
                    "pywns", "main", "1.0"))

scenarios = Library('./framework/scenarios',
                'openwns-scenarios', 
                bzrBaseURL,
                RCS.Bazaar('./framework/scenarios', 
                    'scenarios', 'deprecated', 'deprecated'))


libwns_test = SystemTest('./tests/system/libwns-tests', 
                'openwns-systemtest-library', 
                bzrBaseURL,
                RCS.Bazaar('./tests/system/libwns-tests',
                    'library-tests', 'main', '1.0'))

imtaphy = Library('./modules/phy/imtaphy','imtaphy',
                  bzrBaseURL,
                 RCS.Bazaar('./modules/phy/imtaphy',
                            'imtaphy', 'deprecated', 'deprecated'),
                            'IMTAPHY')

dllbase = Library('./framework/dllbase', 
                'openwns-dllbase', 
                bzrBaseURL,
                RCS.Bazaar('./framework/dllbase',
                    'dllbase', 'main', '1.0'),
                'DLL')

scenarios = Library('./framework/scenarios',
                'openwns-scenarios', 
                bzrBaseURL,
                RCS.Bazaar('./framework/scenarios', 
                    'scenarios', 'deprecated', 'deprecated'))


constanze = Library('./modules/loadgen/constanze', 
                'openwns-constanze', 
                bzrBaseURL,
                RCS.Bazaar('./modules/loadgen/constanze',
                    'constanze', 'main', '1.0'),
                'CONSTANZE')
glue = Library('./modules/dll/glue', 
                'openwns-glue', 
                bzrBaseURL,
                RCS.Bazaar('./modules/dll/glue',
                    'glue', 'main', '1.0'),
                'GLUE')

copper = Library('./modules/phy/copper', 
                'openwns-copper', 
                bzrBaseURL,
                RCS.Bazaar('./modules/phy/copper',
                    'copper', 'main', '1.0'),
                'COPPER')


ip = Library('./modules/nl/ip', 
                'openwns-ip', 
                bzrBaseURL,
                RCS.Bazaar('./modules/nl/ip',
                    'ip', 'main', '1.0'),
                'IP')


tcp = Library('./modules/tl/tcp', 
                'openwns-tcp', 
                bzrBaseURL,
                RCS.Bazaar('./modules/tl/tcp',
                    'tcp', 'main', '1.0'),
                'TCP')

all = [
    root,
    unitTests,
    documentation,
    library,
    application,
    pywns,
    scenarios,

    copper,
    glue,

    imtaphy,

    dllbase,
    constanze,
    ip,
    tcp,
    ]

# commands to prepare the sandbox for playground.py
# list of tuples, where tuples are (command, directory to execute command in)
prereqCommands = [('scons sandbox/default', '.')]
