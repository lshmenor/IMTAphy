###############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2007
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

from openwns.logger import Logger

import types
import sys
import imp
import re

class PyTree:
    maxSequence = 10
    compressLists = False
    noLists = False
    basicTypes = [
        types.IntType,
        types.FloatType,
        types.StringType,
        types.BooleanType,
        types.NoneType,
        ]

    ignorePaths = [
        '/usr/lib/python',
        ]

    ignoreTypes = [
        types.FunctionType,
        types.MethodType,
        types.BuiltinFunctionType,
        types.BuiltinMethodType
        ]

    def __init__(self, options):
        self.objs = {}
        self.options = options

        if options.ignorePaths:
            self.ignorePaths += options.ignorePaths.split(':')

    def scan(self, obj, name = ''):
        result = []

        # shortcut ignores:
        if type(obj) in self.ignoreTypes:
            return result
        if isinstance(obj, PyTree):
            return result
        if obj == PyTree:
            return result

        # we always want simple types to get printed
        if type(obj) in self.basicTypes:
            result.append("%s: %s" % (name, repr(obj)))
            return result

        # short sequences (not longer than self.maxSequence)
        # that contain basicTypes only, will get printed in
        # one line...
        if (type(obj) in [types.ListType,
                         types.TupleType]
            and len(obj) <= self.maxSequence
            and not len([it for it in obj
                         if type(it) not in self.basicTypes])):

            fmt = "%s: "
            if type(obj) == types.ListType:
                fmt += "[%s]"
            else:
                fmt += "(%s)"

            result.append(fmt % (name, ', '.join(map(repr, obj))))
            return result

        # if we already know an object, we just just print
        # a symbolic reference.
        # this helps to avoid infinite recursions and reduces
        # the output in general
        if id(obj) in self.objs:
            result.append("%s -> %s" % (name, self.objs[id(obj)]))
            return result

        self.objs[id(obj)] = name

        # here come the more comples types...
        if type(obj) in [types.ListType,
                         types.TupleType]:
            if (self.compressLists
                and not self.noLists):
                result += self.scan(obj[0], "%s[0]" % (name))
                result += [" . . ."]
                result += self.scan(obj[-1], "%s[%d]" % (name, len(obj)-1))
            elif not self.noLists:
                for n, it in enumerate(obj):
                    result += self.scan(it, "%s[%d]" % (name, n))

        elif type(obj) == types.DictType:
            for key, it in obj.items():
                result += self.scan(it, "%s[%s]" % (name, repr(key)))

        elif (type(obj) in [types.ClassType,
                            types.ModuleType,
                            types.InstanceType,
                            types.TypeType]
            or hasattr(type(obj), '__class__')):

            result.append("%s: %s" % (name, obj))

            # for modules we have some extra code to suppress
            # recursion into std python modules...
            if type(obj) == types.ModuleType:
                # builtin modules don't have a __file__ attribute, and we
                # don't want builtin modules to show up in our dump
                if (obj.__name__ != '__main__'
                    and not hasattr(obj, '__file__')):
                    return result

                for nono in self.ignorePaths:
                    if obj.__file__.startswith(nono):
                        return result
                    
            delimiter = {types.ModuleType: '::',
                        types.ClassType: ':',
                        types.TypeType: ':'}.setdefault(type(obj), '.')
    
            for it in dir(obj):
                if self.skip(it):
                    continue

                result += self.scan(getattr(obj, it), "%s%s%s" % (name, delimiter, it))

        else:
            result.append("%s: %s (fallback)" % (name, obj))
            for it in dir(obj):
                if self.skip(it):
                    continue

        return result

    def skip(self, name):
        return name.startswith('_')

# FIXME:
#
# we are doing some double bookkeeping here: nodes hold lists of their
# connections to keep things like dot file creation working and
# the fun holds a list of all connections, too.
# the latter was needed to preserve connect call order.
#

class FunctionalUnit(object):
    functionalUnitName = None
    commandName = None
    upConnects = None
    downConnects = None
    fun = None

    def __init__(self, functionalUnitName = None, commandName = None):
        self.functionalUnitName = functionalUnitName
        if commandName == None:
            self.commandName = self.functionalUnitName
        else:
            self.commandName = commandName
            
        self.upConnects = []
        self.downConnects = []

    def connect(self, other):
        self.fun.connect(self, other)
#        self.__connect(other)

    def downConnect(self, other):
        self.fun.downConnect(self, other)
#        self.__downConnect(other)

    def upConnect(self, other):
        self.fun.upConnect(self, other)
#        self.__upConnect(other)

    def __connect(self, other):
        self.__downConnect(other)
        self.__upConnect(other)

    def __downConnect(self, other):
        self.downConnects.append(other.functionalUnitName)

    def __upConnect(self, other):
        self.upConnects.append(other.functionalUnitName)

class Node(FunctionalUnit):
    config = None

    def __init__(self, funame, config, commandname = None):
        super(Node,self).__init__(funame, commandname)
        self.config = config


class Port(object):
    __slots__ = ('name')
    name = None


class SinglePort(Port):
    name = 'SinglePort'


class Connection(object):
    __slots__ = ("type", "src", "srcPort", "dst", "dstPort")

    def __init__(self, type, src, srcPort, dst, dstPort):
        self.type = type
        self.src = src
        self.srcPort = srcPort
        self.dst = dst
        self.dstPort = dstPort


class CommandProxy(object):
    logger = None

    def __init__(self, parentLogger):
        self.logger = Logger('WNS','CommandProxy',True, parentLogger)

dotFlowSepCounter = 0

class FUN(object):
    commandProxy = None
    functionalUnit = None
    connects = None
    logger = None

    def __init__(self, parentLogger = None):
        self.functionalUnit = []
        self.connects = []
        self.logger = Logger('WNS', 'FUN', True, parentLogger)
        self.commandProxy = CommandProxy(parentLogger)

    def __resolveParams(self, param1, param2, param3, param4):
        if isinstance(param1, FunctionalUnit) and \
                isinstance(param2, FunctionalUnit) and \
                param3 is None and \
                param4 is None:
            return param1, SinglePort, param2, SinglePort
        elif isinstance(param1, FunctionalUnit) and \
                issubclass(param2, Port) and \
                isinstance(param3, FunctionalUnit) and \
                param4 is None:
            return param1, param2, param3, SinglePort
        elif isinstance(param1, FunctionalUnit) and \
                isinstance(param2, FunctionalUnit) and \
                issubclass(param3, Port) and \
                param4 is None:
            return param1, SinglePort, param2, param3
        elif isinstance(param1, FunctionalUnit) and \
                issubclass(param2, Port) and \
                isinstance(param3, FunctionalUnit) and \
                issubclass(param4, Port):
            return param1, param2, param3, param4
        else:
            raise 'Connection of FUs failed!'

    def connect(self, param1, param2, param3 = None, param4 = None):
        self.connects.append(Connection(0, *self.__resolveParams(param1, param2, param3, param4)))

    def downConnect(self, param1, param2, param3 = None, param4 = None):
        self.connects.append(Connection(1, *self.__resolveParams(param1, param2, param3, param4)))

    def upConnect(self, param1, param2, param3 = None, param4 = None):
        self.connects.append(Connection(2, *self.__resolveParams(param1, param2, param3, param4)))

    def setFunctionalUnits(self, *functionalUnit):
        assert len(self.functionalUnit) == 0
        self.functionalUnit = list(functionalUnit)
        for it in functionalUnit:
            it.fun = self

    def add(self, node):
        node.fun = self
        self.functionalUnit.append(node)
        return node

    def dot(self, name, showParameters=True, FUcompressLists = [], FUnoLists = []):
        out = file(name, 'w')

        out.write('digraph lala {')
        out.write('node [shape=record]')
        out.write(self.dotNodes(self, showParameters, FUcompressLists, FUnoLists))
        out.write(self.dotConns(self))
        out.write('}')

    def dotNodes(self, fun, showParameters, FUcompressLists, FUnoLists):
        if fun is None:
            return ""

        result = ""
        for it in fun.functionalUnit:
            class options:
                ignorePaths = None
            pytree = PyTree(options)
            if it.functionalUnitName in FUcompressLists:
                    pytree.compressLists = True
            if it.functionalUnitName in FUnoLists:
                    pytree.noLists = True
            aConfig = None
            if 'config' in dir(it):
                aConfig = it.config
            else:
                aConfig = it

            if aConfig.__class__.__name__ == "FlowSeparator":# and aConfig.notFound.creator.prototypeConfig.fun is not None:
                global dotFlowSepCounter
                dotFlowSepCounter += 1
            
                result += "subgraph cluster%d {\n" % dotFlowSepCounter
                result += "color=blue;\n"
                result += "label = \"%s\";\n" % str(it.functionalUnitName).replace(".","_")

                # if we have a subfun
                if "fun" in dir(aConfig.notFound.creator.prototypeConfig):
                    sub = aConfig.notFound.creator.prototypeConfig.fun

                    result += self.dotNodes(sub, showParameters, FUcompressLists, FUnoLists)
                    result += self.dotConns(sub)

                else:
                    # It is a single functional unit
                    sub = aConfig.notFound.creator.prototypeConfig
                    
                    if "functionalUnitName" in dir(aConfig.notFound.creator.prototypeConfig):
                        s = str(aConfig.notFound.creator.prototypeConfig.functionalUnitName)
                    else:
                        s = str(aConfig.notFound.creator.prototypeConfig)

                    result += s.replace(".","_")

                result += "}"
            else:

                if showParameters:
                    parameters = pytree.scan(aConfig)[1:]

                    def cleanup(par):
                        par = par.replace('<', '')
                        par = par.replace('>', '')
                        while 'object at' in par:
                            m = re.match('(.*) object at 0x[0-9a-f]+(.*)', par)
                            par = m.group(1) + m.group(2)
                        return par.lstrip('.')
                    parameters = [cleanup(par) for par in parameters]


                    label = ('\\N \\n %s|' % (aConfig.__plugin__)
                             + '\\l'.join(parameters) + '\\l')
                else:
                    label = ('\\N \\n %s' % (aConfig.__plugin__))

                result += '  %s [label="{%s}"];\n' % (str(it.functionalUnitName).replace(".","_"), label)

        return result

    def dotGetNodeName(self, o, dir="down"):
        try:
            # Check if we have a nest flow separator
            if o.__class__.__name__ == "Node" and o.config.__class__.__name__ == "FlowSeparator":
                o = o.config

            if o.__class__.__name__ == "FlowSeparator":
                creatorConfig = o.notFound.creator.prototypeConfig
                if creatorConfig.__class__.__name__ == "Group":
                    if dir=="down":
                        return str(creatorConfig.top)
                    else:
                        return str(creatorConfig.bottom)
                else:
                    return str(o.notFound.creator.prototypeConfig.functionalUnitName)    

            if o.__class__.__name__ == "Node":
                r = str(o.functionalUnitName)
                return r
    
            return str(o.functionalUnitName)

        except:
            pass

        return str(o)

    def dotConns(self, fun):
        if fun is None:
            return ""

        result = ""

        for conn in fun.connects:
            src = self.dotGetNodeName(conn.src, "up")
            dst = self.dotGetNodeName(conn.dst, "down")
            
            if src == "None" : 
                src = str(src)

            if dst == "None" : 
                dst = str(dst)

            result += '  %s -> %s [dir=%s];\n' % (src.replace(".","_"),
                                                  dst.replace(".","_"),
                                                  'forward')
        return result
