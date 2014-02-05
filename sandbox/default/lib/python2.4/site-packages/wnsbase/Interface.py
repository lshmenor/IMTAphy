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

import inspect

def abstractmethod(functionObject):
    """ This is the abstractmethod Decorator. Use it to declare an abstract
    method. Example:

    @abstractmethod
    def youMustImplementMe(self, aParameter):
        pass
    """
    functionObject.__isAbstractMethod = True
    functionObject.__doc__ = "NOTE: This is an abstract method.\n"
    functionObject.__doc__ += (functionObject.__doc__ or "")
    return functionObject

class Interface(object):
    """ Derive from this class if you want to define an interface by using the
    @abstractmethod decorator. The Interface class will check if all abstract
    methods are implemented during construction of your object.

    Make sure to call the constructor of Interface!"""
    def __new__(cls, *args, **kwargs):
        # Find all abstract methods for this object and check if they
        # are implemented. Otherwise raise a type error.
        obj = object.__new__(cls)

        for methodname in dir(obj):
            method = getattr(obj, methodname, None)
            if not callable(method):
                continue

            if hasattr(method, "__isAbstractMethod"):
                classObjects = type(obj).mro()
                mro = type(obj).mro()
                mro.reverse()
                abstractInModule = ""
                abstractInClass = ""
                for classObject in mro:
                    if classObject.__dict__.has_key(method.__name__):
                        abstractInModule = classObject.__module__
                        abstractInClass =  classObject.__name__

                methodArguments = inspect.getargspec(method)[0]
                methodSignature = abstractInClass + "."
                methodSignature += method.__name__
                methodSignature += "(" + ", ".join(methodArguments) + ")"

                raise TypeError, ("Abstract method '%s.%s.%s' not implemented in '%s':\n\n%s :\n%s"
                                  % (str(abstractInModule),
                                     str(abstractInClass),
                                     str(method.__name__),
                                     str(method.im_class.__name__),
                                     str(methodSignature),
                                     str(method.__doc__)))

        return obj
