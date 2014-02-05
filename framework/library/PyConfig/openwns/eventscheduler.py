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
###############################################################################

import openwns.logger

class EventScheduler(object):
    """Configures the behaviour of EventScheduler
       Attributes of this class are:
       - type = string
    """
    __slots__ = ["type"]

    def __init__(self):
        super(EventScheduler, self).__init__()

class Map(EventScheduler):
    """Implementation using a std::map, where each entry with 'Key k' in the
    std::map is a list of events at time 'k'

    # no further arguments
    __slots__ = []


    Complexities (N represents the nummber of different points in time
    for which events exists)
      - sendNow(...):   O(1)
      - cancel(...):    O(1)
      - sendDelay(...): O(log(N))
      - sendAt(...):    O(log(N))
    """
    __slots__ = []

    def __init__(self):
        super(Map, self).__init__()
        self.type = "wns.events.scheduler.Map"

class RealTime(EventScheduler):
    """Tries to schedule the events in real time"""

    __slots__ = []

    def __init__(self):
        super(RealTime, self).__init__()
        self.type = "wns.events.scheduler.RealTime"

class Monitor(object):
    """ Collects some basic statistics about the event scheduler

    Additionally it provides some logging.
    """

    def __init__(self):
        super(Monitor, self).__init__()
        # the logger is set to normal logging per default. all log
        # messages are defined verbose in C++. thus with this setting
        # the logger remains effectively silent during simulation. If
        # you want logging switch to level 3
        # (WNS.eventSchedulerMonitor.level = 3)
        self.logger = openwns.logger.Logger(moduleName = "WNS",
                                            name = "EventSchedulerMonitor",
                                            enabled = True)
