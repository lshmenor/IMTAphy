/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <WNS/Python.hpp>
#include <WNS/pyconfig/Object.hpp>
#include <sstream>
#include <stddef.h>

using namespace wns::pyconfig;

Object::Object() :
    obj_(NULL)
{
}

Object::Object(PyObject* obj) :
    obj_(obj)
{
}

std::string
Object::toString() const
{
    PyObject* s = PyObject_Str(obj_);
    std::string result(PyString_AS_STRING(s));
    // Don't care if null
    Py_XDECREF(s);
    return result;
}

void
Object::decref() const
{
    Py_DECREF(obj_);
}

void
Object::incref() const
{
    Py_INCREF(obj_);
}

bool
Object::isNone() const
{
    return obj_ == Py_None;
}

bool
Object::isNull() const
{
    return obj_ == NULL;
}

bool
Object::isConvertibleToString() const
{
    PyObject* s = PyObject_Str(obj_);
    bool result = s != NULL;
    // Don't care if null
    Py_XDECREF(s);
    return result;
}

Object
Object::getItem(int n) const
{
    return Object(PySequence_GetItem(obj_, n));
}

bool
Object::isSequence() const
{
    return PySequence_Check(obj_) == 1;
}

int
Object::isTrue() const
{
    return PyObject_IsTrue(obj_);
}

bool
Object::operator==(const Object& other)
{
    return obj_ == other.obj_;
}

bool
Object::operator!=(const Object& other)
{
    return obj_ != other.obj_;
}
