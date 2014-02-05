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

from openwns.pyconfig import Sealed
from openwns.pyconfig import attrsetter
import openwns.logger

class BERMeasurementReporting(Sealed):
    """BER Measurement Reporting FU"""

    __plugin__ = 'glue.BERMeasurementReporting'
    name = "BERMeasurementReporting"

    logger = None
    """Logger configuration"""

    BERProvider = None
    """Name of the BER Provider FU (friend)"""

    transmissionInterval = 0.01 # seconds
    """Interval for transmitting BER measurement reports"""

    headerSize = 32 # bit
    """Size of the BER Measurement Reporting command"""

    def __init__(self, BERProvider, **kw):
        self.logger = openwns.logger.Logger("GLUE", "BERMeasurementReporting", True)
        self.BERProvider = BERProvider
        attrsetter(self, kw)
