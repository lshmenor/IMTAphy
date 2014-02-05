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
from wnsbase.playground.Tools import *
import shutil

import SimpleHTTPServer
import SocketServer
import socket

core = wnsbase.playground.Core.getCore()

class ServeDocuCommand(wnsbase.playground.plugins.Command.Command):

    def __init__(self):
        #super(ServeDocuCommand, self).__init__()
        usage = "\n%prog servedocu\n\n"
        rationale = "Start the builtin web server to serve some documentation"

        usage += rationale
        usage += """ Serve the documentation from some directory (default
 sandbox/default/doc) on some port (default 8000) on the current machine.
 """
        wnsbase.playground.plugins.Command.Command.__init__(self, "servedocu", rationale, usage)

        self.optParser.add_option("", "--port",
                                  dest = "port", default = 8000,
                                  type = int,
                                  help="port for the websever")

        self.optParser.add_option("", "--path",
                                  dest = "path", default = "sandbox/default/doc",
                                  help="the document root for the webserver")
    def run(self):

        assert(self.options.port <= 65535)
        Handler = SimpleHTTPServer.SimpleHTTPRequestHandler

        cwd = os.getcwd()
        # change to document root
        os.chdir(self.options.path)
        httpd = SocketServer.TCPServer(("", self.options.port), Handler)

        print "Serving docu from: " + self.options.path
        print "Serving docu at:   http://" + socket.getfqdn() + ":" + str(self.options.port)
        httpd.serve_forever()
        # go back to old dir
        os.chdir(cwd)
