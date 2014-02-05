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

import types
import warnings

class OnlyImmutableAttributes(object):
    def __new__(cls, *args, **kw):
        for name in dir(cls):
            cls.__test(cls, name)

        oldfilters = warnings.filters[:]
        warnings.simplefilter('ignore')

        r = super(OnlyImmutableAttributes, cls).__new__(cls, *args, **kw)    

        warnings.filters[:] = oldfilters
        return r

    @staticmethod
    def __test(cls, name):
        if name.startswith('__'):
            return

        value = getattr(cls, name)
        if not isinstance(value, (int, bool, float, str, tuple, property,
                          types.ClassType, types.NoneType,
                          types.FunctionType, types.MethodType)):
            print "WARNING: class %s from module %s instantiated," \
                  % (cls.__name__, cls.__module__)
            print "         although attribute %s is of mutable type %s." \
                  % (name, type(value))

class View(dict):
    """Dictionary implementation serving as globals() for wns::pyconfig::View"""

    def __init__(self, obj):
        super(View, self).__init__()
        self.obj = obj

    def __contains__(self, key):
        return hasattr(self.obj, key)

    def __getitem__(self, key):
        try:
            return getattr(self.obj, key)
        except:
            if key == '__id__':
                return id(self.obj)
            if key == '__visible__':
                return self.asString(4)
            if key == '__stringRepresentation__':
                return self.asString(4)
            try:
                return getattr(__builtins__, key)
            except:
                return __builtins__[key]

    def asString(self, indentSize):
        result = ' '*indentSize
        result += 'Above entry is of type: ' + str(self.obj) + '\n'
        result += ' '*indentSize
        for it in dir(self.obj):
            if not it.startswith('_'):
                result += it + ' = ' + str(eval('obj.'+it, {'obj':self.obj})) + '\n'
                result += ' '*indentSize
        return result[:-indentSize].rstrip("\n")
