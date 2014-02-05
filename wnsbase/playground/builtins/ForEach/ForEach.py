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

from wnsbase.playground.Tools import *

import wnsbase.playground.Core
import wnsbase.playground.plugins.Command
core = wnsbase.playground.Core.getCore()

class ForEachCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "\n%prog foreach [switches] COMMAND\n\n"
        rationale = "Execute a command on multiple projects."
        usage += rationale
        super(ForEachCommand, self).__init__("foreach", rationale, usage)
        # the command to execute
        self.numberOfArgs = 1

    def run(self):

        command = " ".join(self.args)

        def run(project):
            print "Running '%s' in %s ..." % (command, project.getDir())
            runCommand(command)

        core.foreachProject(run)

