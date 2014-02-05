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

import sys
import string
from wnsbase.playground.Tools import *

import wnsbase.playground.Core
import wnsbase.playground.plugins.Command
import shutil

core = wnsbase.playground.Core.getCore()

class ServeCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        usage = "Prepare a directory (localRepo) to serve this SDK via 'bzr serve --directory localRepo'. WARNING: This will remove ./localRepo"

        wnsbase.playground.plugins.Command.Command.__init__(self, "serve", usage, usage)

    def run(self, arg = 'unused'):
        print "Removing 'localRepo'"
        shutil.rmtree('localRepo', True)
        print "Preparing 'localRepo'"
        os.mkdir('localRepo')
        for project in core.getProjects().all:
            # make link to repository and create any intermediate directories
            baseName = os.path.basename(project.rcsSubDir)
            dirName = os.path.dirname(project.rcsSubDir)

            if dirName != '':
                os.makedirs(os.path.join('localRepo', dirName))
    
            print "Linking: " + project.getDir() + " -> " + os.path.join('localRepo', project.rcsSubDir)
            os.symlink(project.getDir(), os.path.join('localRepo', project.rcsSubDir))

        print "localRepo is prepared."
        print "You can now say 'bzr serve --directory localRepo', to offer this directory for pulling"
        print "or 'bzr serve --allow-writes --directory localRepo', to offer this directory for pulling AND pushing"
