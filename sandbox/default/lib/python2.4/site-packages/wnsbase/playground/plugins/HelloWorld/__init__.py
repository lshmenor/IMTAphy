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
"""
A tutorial plugin for playground
"""

# HelloWorldCommand contains our command implementation
import HelloWorldCommand

# We will now register this plugin and add the HelloWorld Command

# Get the core object of playground
import wnsbase.playground.Core
core = wnsbase.playground.Core.getCore()

# We only register stuff at the core once.
if not core.hasPlugin("HelloWorld"):
    core.registerPlugin("HelloWorld")

    # We create the command
    helloworldCommand = HelloWorldCommand.HelloWorldCommand()

    # And register it at the core
    core.registerCommand(helloworldCommand)

