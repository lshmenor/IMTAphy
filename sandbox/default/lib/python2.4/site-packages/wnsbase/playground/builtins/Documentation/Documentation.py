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

import wnsbase.playground.Core

from wnsbase.playground.builtins.PyCoDocumentation.PyCoDocumentation import PyCoDocumentationCommand
from wnsbase.playground.builtins.CreateManuals.CreateManuals import CreateManualsCommand
from wnsbase.playground.builtins.CPPDocumentation.CPPDocumentation import CPPDocuCommand

core = wnsbase.playground.Core.getCore()

class DocuCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog docu\n\n"
        rationale = "Build project documentation."

        usage += rationale
        usage += """ Build the documentation for the whole project."""
        wnsbase.playground.plugins.Command.Command.__init__(self, "docu", rationale, usage)

        self.optParser.add_option("", "--scons",
                                  dest = "scons", default = "",
                                  help="options forwarded to scons.")


        self.cppDocuCommand = CPPDocuCommand()
        self.pycoDocuCommand = PyCoDocumentationCommand()
        self.createManualsCommand = CreateManualsCommand()

    def startup(self, args):
        self.cppDocuCommand.startup(args)
        self.pycoDocuCommand.startup([])
        self.createManualsCommand.startup([])

    def run(self):
        self.cppDocuCommand.run()
        self.pycoDocuCommand.run()
        self.createManualsCommand.run()

    def shutdown(self):
        self.cppDocuCommand.shutdown()
        self.pycoDocuCommand.shutdown()
        self.createManualsCommand.shutdown()

