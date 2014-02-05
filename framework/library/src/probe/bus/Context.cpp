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

#include <WNS/probe/bus/Context.hpp>
#include <WNS/Assure.hpp>


using namespace wns::probe::bus;


PyContext::PyContext():
    pyDict_(NULL)
{
    assure(Py_IsInitialized(), "Python interpreter is not initialized!");
    pyDict_ = wns::pyconfig::Object(PyDict_New());
    assure(!pyDict_.isNull(), "Creation of Python Dict Failed");
}

PyContext::~PyContext()
{
    assure(Py_IsInitialized(), "Python interpreter is not initialized!");
    assure(!pyDict_.isNull(), "No Python Object!");
    PyDict_Clear(pyDict_.obj_);
    pyDict_.decref();
}

void
PyContext::insert(const std::string& key, int value)
{
    insertInt(key, value);
}

void
PyContext::insert(const std::string& key, const std::string& value)
{
    insertString(key,value);
}

void
PyContext::insertInt(const std::string& key, int value)
{
    assure(Py_IsInitialized(), "Python interpreter is not initialized!");
    assure(!pyDict_.isNull(), "Creation of Python Dict Failed");

    // Check if entry with "key" already exists
    if ( this->knows(key) == true )
    {
        throw context::DuplicateKey();
    }

    PyObject* pyValue = PyInt_FromLong(value);

#ifndef NDEBUG
    int result =
#endif
        PyDict_SetItemString(pyDict_.obj_, key.c_str(), pyValue);

    assure(result==0, "Inserting of element into Dict failed!");
    Py_DECREF(pyValue);
}

void
PyContext::insertString(const std::string& key, const std::string& value)
{
    assure(Py_IsInitialized(), "Python interpreter is not initialized!");
    assure(!pyDict_.isNull(), "Creation of Python Dict Failed");

    // Check if entry with "key" already exists
    if ( this->knows(key) == true )
    {
        throw context::DuplicateKey();
    }

    PyObject* pyValue = PyString_FromString(value.c_str());

#ifndef NDEBUG
    int result =
#endif
        PyDict_SetItemString(pyDict_.obj_, key.c_str(), pyValue);

    assure(result==0, "Inserting of element into Dict failed!");
    Py_DECREF(pyValue);
}

bool
PyContext::knows(const std::string& key) const
{
    assure(Py_IsInitialized(), "Python interpreter is not initialized!");
    assure(!pyDict_.isNull(), "Creation of Python Dict Failed");

    PyObject* pyKey = PyString_FromString(key.c_str());
    int result = PyDict_Contains(pyDict_.obj_, pyKey);

    assure(result!=-1, "Error in PyDict_Contains occurred");

    Py_DECREF(pyKey);

    return result==1;
}

bool
PyContext::isInt(const std::string& key) const
{
    assure(Py_IsInitialized(), "Python interpreter is not initialized!");
    assure(!pyDict_.isNull(), "Creation of Python Dict Failed");

    PyObject* pyKey = PyString_FromString(key.c_str());

    // PyDict_GetItem returns a "borrowed" reference, which means we are not
    // responsible of DECREFING it.
    PyObject* pyValue = PyDict_GetItem(pyDict_.obj_, pyKey);
    Py_DECREF(pyKey);

    if (pyValue == NULL)
        throw context::NotFound();

    return PyInt_Check(pyValue);
}

int
PyContext::getInt(const std::string& key) const
{
    assure(Py_IsInitialized(), "Python interpreter is not initialized!");
    assure(!pyDict_.isNull(), "Creation of Python Dict Failed");

    PyObject* pyKey = PyString_FromString(key.c_str());

    // PyDict_GetItem returns a "borrowed" reference, which means we are not
    // responsible of DECREFING it.
    PyObject* pyValue = PyDict_GetItem(pyDict_.obj_, pyKey);

    if (pyValue == NULL)
        throw context::NotFound();

    if (PyInt_Check(pyValue) != true)
    {
        context::TypeError up;
        up << "Type mismatch. Object with key=" << key << " is not of type int";
        throw up;
    }

    int32_t value = PyInt_AsLong(pyValue);

    Py_DECREF(pyKey);

    // Python 2.6: manually clear cached list of integer objects
    //PyInt_ClearFreeList();

    return value;
}

bool
PyContext::isString(const std::string& key) const
{
    assure(Py_IsInitialized(), "Python interpreter is not initialized!");
    assure(!pyDict_.isNull(), "Creation of Python Dict Failed");

    PyObject* pyKey = PyString_FromString(key.c_str());

    // PyDict_GetItem returns a "borrowed" reference, which means we are not
    // responsible of DECREFING it.
    PyObject* pyValue = PyDict_GetItem(pyDict_.obj_, pyKey);
    Py_DECREF(pyKey);

    if (pyValue == NULL)
        throw context::NotFound();

    return PyString_Check(pyValue);
}

std::string
PyContext::getString(const std::string& key) const
{
    assure(Py_IsInitialized(), "Python interpreter is not initialized!");
    assure(!pyDict_.isNull(), "Creation of Python Dict Failed");

    PyObject* pyKey = PyString_FromString(key.c_str());

    // PyDict_GetItem returns a "borrowed" reference, which means we are not
    // responsible of DECREFING it.
    PyObject* pyValue = PyDict_GetItem(pyDict_.obj_, pyKey);

    if (pyValue == NULL)
        throw context::NotFound();

    if (PyString_Check(pyValue) != true)
    {
        context::TypeError up;
        up << "Type mismatch. Object with key=" << key << " is not of type string";
        throw up;
    }

    std::string value(PyString_AsString(pyValue));

    Py_DECREF(pyKey);

    return value;
}

std::string
PyContext::doToString() const
{
    assure(!pyDict_.isNull(), "NULL Python Object!");

    PyObject *key, *value;

    Py_ssize_t pos = 0;

    std::stringstream str;
    str << "{";
    while (PyDict_Next(pyDict_.obj_, &pos, &key, &value))
    {
        str << PyString_AsString(key) << " : ";
        if (PyInt_Check(value) == true)
        {
            str << PyInt_AsLong(value);
        }
        else if (PyString_Check(value) == true)
        {
            str << "'" << PyString_AsString(value) << "'";
        }
        else
        {
            str << "Unknown Object Type";
        }
        str << ",";
    }
    str << "}";
    return str.str();
}





Context::Context()
{}

Context::~Context()
{
    stringContextMap.clear();
    intContextMap.clear();
}

void
Context::insert(const std::string& key, int value)
{
    insertInt(key, value);
}

void
Context::insert(const std::string& key, const std::string& value)
{
    insertString(key,value);
}

void
Context::insertInt(const std::string& key, int value)
{
    // check in both maps if key already exists
    bool knownKey = this->knows(key);

    if (knownKey)
        throw context::DuplicateKey();

    intContextMap[key] = value;
}

void
Context::insertString(const std::string& key, const std::string& value)
{
    // check in both maps if key already exists
    bool knownKey = this->knows(key);

    if (knownKey)
        throw context::DuplicateKey();

    stringContextMap[key] = value;
}

bool
Context::stringMapknows(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator iterString = stringContextMap.find(key);
    return (iterString != stringContextMap.end());
}

bool
Context::intMapknows(const std::string& key) const
{
    std::map<std::string, int>::const_iterator iterInt = intContextMap.find(key);
    return (iterInt != intContextMap.end());
}

bool
Context::knows(const std::string& key) const
{
    return this->stringMapknows(key) || this->intMapknows(key);
}

bool
Context::isInt(const std::string& key) const
{
    return this->intMapknows(key);
}

int
Context::getInt(const std::string& key) const
{
    if (isString(key))
    {
        context::TypeError up;
        up << "Type mismatch. Object with key=" << key << " is not of type int";
        throw up;
    }

    if (!isInt(key))
        throw context::NotFound();

    int32_t value = intContextMap.find(key)->second;

    return value;
}

bool
Context::isString(const std::string& key) const
{
    return this->stringMapknows(key);
}

std::string
Context::getString(const std::string& key) const
{
    if (isInt(key))
    {
        context::TypeError up;
        up << "Type mismatch. Object with key=" << key << " is not of type string";
        throw up;
    }

    if (!isString(key))
        throw context::NotFound();

    std::string value = stringContextMap.find(key)->second;

    return value;
}

std::string
Context::doToString() const
{
    std::stringstream str;
    str << "{";    

    std::map<std::string, int>::const_iterator iterInt = intContextMap.begin();
    for (iterInt; iterInt != intContextMap.end(); ++iterInt)
    {
        str << iterInt->first << " : "
            << iterInt->second << ",";
    }

    std::map<std::string, std::string>::const_iterator iterString = stringContextMap.begin();
    for (iterString; iterString != stringContextMap.end(); ++iterString)
    {
        str << iterString->first << " : "
            << "'" << iterString->second << "',";
    }

    str << "}";
    return str.str();
}
