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
import unittest
from backend.pyconfig import View, OnlyImmutableAttributes

def attrsetter(it, attrs):
    for key, value in attrs.items():
        setattr(it, key, value)


class Frozen(OnlyImmutableAttributes):
    """ Deriving from this class will prevent write access to members of the
    instance"""

    class Error(Exception):
        def __init__(self, obj, name):
            Exception.__init__(self, "No write access to '%s' in frozen object %s" % (name, obj))

    def __setattr__(self, name, value):
        raise Frozen.Error(self, name)

class Sealed(OnlyImmutableAttributes):
    def __setattr__(self, attr, value):
        getattr(self, attr) # raises an exception in case attr is not known
        object.__setattr__(self, attr, value)

def Plugin(name):
    class Strategy(Frozen):
        __plugin__ = name

    return Strategy


def getParentComponent(component):
    return component.getParentComponent()

def getComponentPath(component):
    # if we get an empty string, this is a root component
    path = component.getComponentPath()
    if path == "":
        path = "/"
    return path

def getComponentName(component):
    return component.getComponentName()


class IAttributeHandler(property):
    # we are a property ourselfs :)
    def __init__(self, getter=None, setter=None, deleter=None, doc=None):
        super(IAttributeHandler, self).__init__(getter, setter, deleter, doc)

    """ MetaComponent uses these to inject the necessary information """
    def setAttributeName(self, attrName):
        self._attributeName = attrName

    def setClassName(self, className):
        self._className = className


class MetaComponent(type):

    def __new__(cls, className, bases, dct):
        if not dct.has_key("__attributes__"):
            dct["__attributes__"] = []
        for attributeName, attribute in dct.items():
            if isinstance(attribute, IAttributeHandler):
                # inject the name of the attribute and the name of the class
                # into the IAttributeHandler
                attribute.setAttributeName(attributeName)
                attribute.setClassName(className)
                dct["__attributes__"].append(attributeName)
        return type.__new__(cls, className, bases, dct)

    def __init__(cls, className, bases, dct):
        super(MetaComponent, cls).__init__(className, bases, dct)


class Component(OnlyImmutableAttributes):

    class NoSuchAttribute(Exception):
        """ This exception is thrown if an attribute is accessed that is
        not available """

        def __init__(self, obj, name):
            """ The constructor takes the object where the attribute was
            missing and the name of the attribute. It prints all allowed attributes"""

            cls = obj.__class__
            error = "\nAttribute '" + name + "' not allowed in '" + cls.__name__ + "'"
            error += "\nAllowed attributes are:"
            for allowedAttribute in obj.__attributes__:
                error += "\n  " + allowedAttribute
            Exception.__init__(self, error)

    __metaclass__ = MetaComponent

    # this is used by the params to store the actual instances
    __attributeValues = None
    __parentComponent = None
    __componentName = None

    def __setattr__(self, attr, value):
        if hasattr(self, attr) or hasattr(self.__class__, attr):
            object.__setattr__(self, attr, value)
            return
        # we didn't find it in the allowed attributes list
        raise Component.NoSuchAttribute(self, attr)

    def __new__(cls, *args, **kws):
        obj = super(OnlyImmutableAttributes, cls).__new__(cls)
        obj.__attributeValues = {}
        obj.__parentComponent = None
        for attr in obj.__attributes__:
            if hasattr(getattr(cls, attr), "onComponentCreation"):
                getattr(cls, attr).onComponentCreation(obj)
        return obj

    @classmethod
    def getAttributeHandler(cls, name):
        return cls.__dict__[name]

    def isAttributeSet(self, name):
        return self.__attributeValues.has_key(name)

    def getAttribute(self, name):
        return self.__attributeValues[name]

    def setAttribute(self, name, value):
        self.__attributeValues[name] = value

    def setParentComponent(self, parent):
        assert isinstance(parent, Component)
        self.__parentComponent = parent

    def getParentComponent(self):
        return self.__parentComponent

    def setComponentName(self, name):
        assert isinstance(name, str)
        self.__componentName = name

    def getComponentName(self):
        return self.__componentName

    def getComponentPath(self):
        if self.getParentComponent() == None:
            # this is a root component
            return ""
        return self.getParentComponent().getComponentPath() + "/" + self.getComponentName()

class Parameter(IAttributeHandler):
    class NoValueSet(Exception):
        def __init__(self):
            Exception.__init__(self, "No value is set!")

    def __init__(self, doc, *hooks):
        super(Parameter, self).__init__(self.getter, self.setter)

        self.hooks = hooks
        # hooks can implement , if they implement it, they will be
        # asked to provide something. they all get: the
        # attributeValue, the AtrributeHandler and the object where to
        # set the attribute. Available hooks are:
        # onComponentCreation()
        # beforeAttributeSet()
        # afterAttributeGet()
        # getDocString()

        # convert to string, __doc__ MUST be a string
        self.__doc__ = str(doc)

        # append additional documentation contributed by hooks
        for hook in self.hooks:
            if hasattr(hook, "getDocString"):
                self.__doc__ += "\n\n" + str(hook.getDocString())

        for hook in self.hooks:
            if hasattr(hook, "getDocField"):
                self.__doc__ += "\n\n" + str(hook.getDocField())

    def setter(self, obj, attributeValue):
        """ The setter takes exactly two parameters: the object for
        which the parameter should be set and the value to be set """

        assert isinstance(obj, Component)
        for hook in self.hooks:
            if hasattr(hook, "beforeAttributeSet"):
                attributeValue = hook.beforeAttributeSet(attributeValue, self, obj)
        obj.setAttribute(self._attributeName, attributeValue)
        attribute = getattr(obj, self._attributeName)
        if isinstance(attribute, Component):
            attribute.setParentComponent(obj)
            attribute.setComponentName(self._attributeName)

    def getter(self, obj):
        """ The getter takes exactly one parameters: the object from
        which the parameter should be fetched"""

        assert isinstance(obj, Component)
        if obj.isAttributeSet(self._attributeName):
            attributeValue = obj.getAttribute(self._attributeName)
            for hook in self.hooks:
                if hasattr(hook, "afterAttributeGet"):
                    attributeValue = hook.afterAttributeGet(attributeValue, self, obj)
            return attributeValue
        raise Parameter.NoValueSet()

    def onComponentCreation(self, obj):
        assert isinstance(obj, Component)
        for hook in self.hooks:
            if hasattr(hook, "onComponentCreation"):
                hook.onComponentCreation(obj, self._attributeName)


class RangeCheck(object):
    class Error(Exception):
        def __init__(self, cls, name, start, stop, value, doc):
            error =  "\nRangeCheck.Error for for '%s.%s'" % (cls, name)
            error += "\n  range: [%f, %f]" % (start, stop)
            error += "\n  value: '%f'" % (value)
            error += "\n    doc: %s" % (doc)
            Exception.__init__(self, error)

    def __init__(self, start, stop):
        self.start = start
        self.stop = stop
        self.afterAttributeGet = self.beforeAttributeSet

    def getDocString(self):
        return "B{Range}: [%s .. %s]" % (str(self.start), str(self.stop))

    def beforeAttributeSet(self, attributeValue, attributeHandler, obj):
        if not (attributeValue >= self.start and attributeValue <= self.stop):
            raise RangeCheck.Error(
                attributeHandler._className,
                attributeHandler._attributeName,
                self.start,
                self.stop,
                attributeValue,
                attributeHandler.__doc__)
        return attributeValue


class TypeCheck(object):
    class Error(Exception):
        def __init__(self, cls, name, paramType, value, doc):
            error =  "\nTypeCheck.Error for '%s.%s'" % (cls, name)
            error += "\n  expected: '%s'" % (paramType.__name__)
            error += "\n       got: '%s'" % (type(value).__name__)
            error += "\n       doc: %s" % (doc)
            Exception.__init__(self, error)

    def __init__(self, theType):
        self.theType = theType
        self.afterAttributeGet = self.beforeAttributeSet

    def getDocField(self):
        return "@type: L{%s}" % self.theType.__name__

    def beforeAttributeSet(self, attributeValue, attributeHandler, obj):
        if not isinstance(attributeValue, self.theType):
            raise TypeCheck.Error(
                attributeHandler._className,
                attributeHandler._attributeName,
                self.theType,
                attributeValue,
                attributeHandler.__doc__)
        return attributeValue

class Default(object):
    def __init__(self, cls, *args, **kwds):
        self.cls = cls
        self.args = args
        self.kwds = kwds

    def getDocString(self):
        return "B{Default:} %s(%s, %s)" % (self.cls.__name__, self.args, self.kwds)

    def onComponentCreation(self, obj, attributeName):
        return setattr(obj, attributeName, self.cls(*self.args, **self.kwds))

class TypedParameter(Parameter):

    def __init__(self, doc, type, *args, **kwds):
        super(TypedParameter, self).__init__(doc, TypeCheck(type), Default(type, *args, **kwds))

class Int(TypedParameter):
    def __init__(self, doc, default):
        super(Int, self).__init__(doc, int, default)

class String(TypedParameter):
    def __init__(self, doc, default):
        super(String, self).__init__(doc, str, default)

class Float(TypedParameter):
    def __init__(self, doc, default):
        super(Float, self).__init__(doc, float, default)


# For testing

class PyConfigTest:
    foo = 42
    def __init__(self):
        """test wns::pyconfig::Parser::fromClass"""
        pass

class PyConfigTestWithArg:
    foo = 42
    def __init__(self, n):
        """test wns::pyconfig::Parser::fromClass"""
        self.foo = n

class Tests(unittest.TestCase):

    class A(Frozen):
        x = 3

    class B(A):
        y = 42

    class C:
        def __init__(self, z):
            self.z = z

    class TestComponent(Component):
        foo = Int("The parameter foo", 3)
        bar = Float("The parameter bar", 3.3)
        baz = String("The parameter baz", "3.3")

    def testFrozen(self):
        a = Tests.A()
        self.assertRaises(Frozen.Error, setattr, a, "x", 23)

    def testFrozenArgNotAvailable(self):
        b = Tests.B()
        self.assertRaises(Frozen.Error, setattr, b, "z", 23)

    def testIntParam(self):
        t = Tests.TestComponent()
        self.assertEqual(3, t.foo)
        t.foo = 4
        self.assertEqual(4, t.foo)
        self.assertRaises(TypeCheck.Error, setattr, t, "foo", 3.3)

    def testFloatParam(self):
        t = Tests.TestComponent()
        self.assertEqual(3.3, t.bar)
        t.bar = 4.4
        self.assertEqual(4.4, t.bar)
        self.assertRaises(TypeCheck.Error, setattr, t, "bar", "4.4")

    def testStringParam(self):
        t = Tests.TestComponent()
        self.assertEqual("3.3", t.baz)
        t.baz = "4.4"
        self.assertEqual("4.4", t.baz)
        self.assertRaises(TypeCheck.Error, setattr, t, "baz", 3.3)

    def testTypedParam(self):
        class TestTypedParameters(Component):
            bak = TypedParameter("The parameter bak", Tests.C, 3)

        t = TestTypedParameters()
        self.assertEqual(3, t.bak.z)
        t.bak = Tests.C(4)
        self.assertEqual(4, t.bak.z)
        self.assertRaises(TypeCheck.Error, setattr, t, "bak", 3)

    def testRangeCheck(self):
        class TestRangeCheck(Component):
            bak = Parameter("The parameter bak", RangeCheck(0,3))

        t = TestRangeCheck()
        t.bak = 3
        self.assertEqual(3, t.bak)
        t.bak = 2
        self.assertEqual(2, t.bak)
        t.bak = 0
        self.assertEqual(0, t.bak)
        self.assertRaises(RangeCheck.Error, setattr, t, "bak", 4)

    def testComplexObjects(self):
        class Bar(Component):
            baz = Int("The baz", 3)

        class Foo(Component):
            bar = TypedParameter("Instance of Bar", Bar)

        f = Foo()
        f2 = Foo()
        self.assertEqual(f.bar.baz, 3)
        self.assertEqual(f2.bar.baz, 3)
        f.bar.baz = 4
        self.assertEqual(f.bar.baz, 4)
        self.assertEqual(f2.bar.baz, 3)

    def testComponentPath(self):
        class Bar(Component):
            baz = Int("The baz", 3)

        class Foo(Component):
            bar = TypedParameter("Instance of Bar", Bar)

        class Baz(Component):
            foo = TypedParameter("Instance of Foo", Foo)

        b = Baz()
        self.assertEqual(getComponentPath(b.foo.bar), "/foo/bar")
        self.assertEqual(getComponentPath(b.foo), "/foo")
        self.assertEqual(getComponentPath(b), "/")

    def testParentComponent(self):
        class Bar(Component):
            baz = Int("The baz", 3)

        class Foo(Component):
            bar = TypedParameter("Instance of Bar", Bar)

        class Baz(Component):
            foo = TypedParameter("Instance of Foo", Foo)

        b = Baz()
        self.assertEqual(getParentComponent(b.foo.bar), b.foo)
        self.assertEqual(getParentComponent(b.foo), b)

    def testComponentName(self):
        class Bar(Component):
            baz = Int("The baz", 3)

        class Foo(Component):
            bar = TypedParameter("Instance of Bar", Bar)

        class Baz(Component):
            foo = TypedParameter("Instance of Foo", Foo)

        b = Baz()
        self.assertEqual(getComponentName(b.foo.bar), "bar")
        self.assertEqual(getComponentName(b.foo), "foo")
        self.assertEqual(getComponentName(b), None)


if __name__ == '__main__':
    unittest.main()
