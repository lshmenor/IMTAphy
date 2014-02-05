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

import string
import openwns.pyconfig

showFunction = False

class LoggerRegistry(object):
    def __init__(self):
        self.dict = {}

    def addLogger(self, category, logger):
        assert isinstance(category, str)
        if not self.dict.has_key(category):
            self.dict[category] = []
        self.dict[category].append(logger)

    def setAttribute(self, category, attribute, value):
        assert isinstance(category, str)
        for logger in self.dict[category]:
            assert isinstance(value, type(getattr(logger, attribute)))
            setattr(logger, attribute, value)

    def getCategories(self):
        return self.dict.keys()

    def getLoggers(self, category):
        assert isinstance(category, str)
        return self.dict[category]

    def dump(self):
        keys = sorted(self.dict)
        for k in keys:
            print "%s :" % k
            for l in self.dict[k]:
                print "\t%s\tL=%d \tE=%s" % (str(l), l.level, l.enabled)

globalRegistry = LoggerRegistry()

class ModuleColor(object):
    __slots__ = ["name", "modifier"]

    def __init__(self, _name, _modifiers):
        super(ModuleColor, self).__init__()
        self.name = _name
        modifiers = ';'.join(['00'] + _modifiers)
        self.modifier = "\033[" + modifiers + "m"

class Effects(openwns.pyconfig.Frozen):
    Bold="01"
    Italic="03"
    Underline="04"
    Blink="05"
    RapidBlink="06"
    ReverseVideo="07"
    Invisble="08"

class Foreground(openwns.pyconfig.Frozen):
    Black="30"
    Red="31"
    Green="32"
    Yellow="33"
    Blue="34"
    Magenta="35"
    Cyan="36"
    White="37"

class Background(openwns.pyconfig.Frozen):
    Black="40"
    Red="41"
    Green="42"
    Yellow="43"
    Blue="44"
    Magenta="45"
    Cyan="46"
    White="47"

class Logger(object):
    __slots__ = ["moduleName", "__enabled", "__parent", "__name", "showFunction", "__level"]

    """The Levels shall have the following meaning:
    0: disabled - no logging
    1: quiet - only startup/shutdown messages
    2: normal - all messages (default)
    3: verbose - extra stuff for tracing and debugging"""
    __defaultLevel = 2

    def __init__(self, moduleName, name, enabled, parent = None, **kw):
        self.moduleName = moduleName
        self.__enabled = enabled
        self.__parent = parent
        self.__name = name
        self.__level = None
        self.showFunction = False
        openwns.pyconfig.attrsetter(self, kw)
        globalRegistry.addLogger("all", self)
        globalRegistry.addLogger(moduleName, self)
        globalRegistry.addLogger("%s.%s" % (moduleName, self.name), self)

    def __levelGetter(self):
        if self.__level is not None:
            return self.__level

        if self.__parent is not None:
            return self.__parent.level

        return self.__defaultLevel

    def __levelSetter(self, value):
        self.__level = value

    def __enabledGetter(self):
        if self.__parent is not None:
            return (self.__parent.enabled and
                    self.__enabled and
                    self.level > 0)

        return (self.__enabled and
            self.level > 0)

    def __enabledSetter(self, value):
        self.__enabled = value

    def __nameSetter(self, value):
        self.__name = value

    def __nameGetter(self):
        if(self.__parent):
            return self.__parent.name + "." + self.__name
        else:
            return self.__name

    name = property(__nameGetter, __nameSetter)
    level = property(__levelGetter, __levelSetter)
    enabled = property(__enabledGetter, __enabledSetter)

# this is how the level thing is supposed to work...
#
# 0>fd@fante:[spikes]python
# Python 2.3.5 (#2, Feb  9 2005, 00:38:15)
# >>> import openwns.Logger
# >>> base = openwns.Logger.Logger()
# >>> child1 = openwns.Logger.Logger(base)
# >>> child2 = openwns.Logger.Logger(child1)
# >>>
# >>> [it.level for it in base, child1, child2]
# [3, 3, 3]
# >>> base.level = 2
# >>> [it.level for it in base, child1, child2]
# [2, 2, 2]
# >>> base.level = None
# >>> [it.level for it in base, child1, child2]
# [3, 3, 3]
# >>> child1.level = 2
# >>> [it.level for it in base, child1, child2]
# [3, 2, 2]
# >>> child2.level = 1
# >>> [it.level for it in base, child1, child2]
# [3, 2, 1]
# >>> child2.level = None
# >>> [it.level for it in base, child1, child2]
# [3, 2, 2]
# >>> child1.level = None
# >>> [it.level for it in base, child1, child2]
# [3, 3, 3]
# >>>

class Output(object):
    __slots__ = ["__plugin__"]

class Cout(Output):
    __slots__ = []

    def __init__(self):
        self.__plugin__ = "Cout"

class Cerr(Output):
    __slots__ = []
    def __init__(self):
        self.__plugin__ = "Cerr"

class File(Output):
    __slots__ = []

    def __init__(self):
        self.__plugin__ = "File"

class Format(object):
    __slots__ = ["__plugin__"]

class ColorMode(object):
    Off  = 0
    Auto = 1
    On   = 2

class Console(Format):
    __slots__ = ["timePrecision", "timeWidth", "maxLocationLength", "colors", "colorMap"]

    # 'WNS' uses default color of the terminal
    colorMap = []
    ''' Global color map for the Console output format

    Colors need to be registered by the modules individually. In
    __init__.py of the modules root package you may say:

    openwns.logger.Console.colorMap.append(
        ModuleColor(
            "YourModule",
            [openwns.logger.Foreground.Red]
        )
    )
    '''

    def __init__(self):
        super(Console, self).__init__()
        self.__plugin__ = "Console"
        self.timePrecision = 7
        self.timeWidth = 11
        self.maxLocationLength = 50
        self.colors = ColorMode.Auto

class XML(Format):
    __slots__ = []

    def __init__(self):
        self.__plugin__ = "wns.logger.XMLFormat"

class SQLite(Format):
    __slots__ = []

    def __init__(self):
        self.__plugin__ = "wns.logger.SQLiteFormat"

class Delimiter(Format):
    __slots__ = ["delimiter"]

    def __init__(self):
        self.__plugin__ = "wns.logger.DelimiterFormat"

    def __init__(self, delimiter = '|'):
        self.delimiter = delimiter

class FormatOutputPair(object):
    __slots__ = ["format", "output"]

    def __init__(self, _format, _output):
        self.format = _format
        self.output = _output

class Backtrace(object):
    __slots__ = ["enabled", "length"]

    def __init__(self):
        super(Backtrace, self).__init__()
        self.enabled = False
        self.length = 10000

class Master(object):
    """ Style and destination of logging

    The FormatOutputPair defines the Style (e.g., Console) and the
    destination (e.g., Cout, Cerr) of logging.

    For self.enabled and self.backtrace.enabled the following combinations
    are possible:

    1) self.backtrace.enabled = False und self.enabled = True
    normal output

    2) self.backtrace.enabled = False and self.enabled = False
    No messages at all.

    3) self.backtrace.enabled = True and self.enabled = False
    No messages at all.

    4) self.backtrace.enabled = True und self.enabled = True
    Backtrace (configurable lenght) at uncaught exceptions, segfaults and
    interrupts

    Default is 1). 4) is useful to get quick to the point where the
    exception/segfault happens (message output is time consuming) without
    messages and to get a backtrace of what happened just before the
    segfault/exception.
    """

    __slots__ = ["enabled", "backtrace", "loggerChain"]

    def __init__(self):
        super(Master, self).__init__()
        self.enabled = True
        self.backtrace = Backtrace()
        self.loggerChain = [ FormatOutputPair(Console(), Cout()) ]
