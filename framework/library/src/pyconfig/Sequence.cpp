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

#include <WNS/pyconfig/Sequence.hpp>
#include <sstream>

using namespace wns::pyconfig;

Sequence::Sequence(Object _sequence) :
	sequence(_sequence),
	pathName("")

{
	assure(_sequence.isSequence(), "Object is no sequence!");
	sequence.incref();
} // Sequence

Sequence::Sequence(Object _sequence,
		   const std::string& _pathName) :
	sequence(_sequence),
	pathName(_pathName)

{
	assure(_sequence.isSequence(), "Object is no sequence!");
	sequence.incref();
} // Sequence

Sequence::Sequence(const Sequence& other) :
	sequence(other.sequence),
	pathName(other.pathName)
{
	sequence.incref();
} // copy constructor


Sequence::~Sequence()
{
	sequence.decref();
} // ~Sequence


Sequence&
Sequence::operator=(const Sequence& other)
{
	sequence = other.sequence;
	sequence.incref();

	return *this;
} // =


bool
Sequence::empty() const
{
	return !PySequence_Length(sequence.obj_);
} // empty


int
Sequence::size() const
{
	return PySequence_Length(sequence.obj_);
} // size


bool
Sequence::isSequenceAt(int n) const
{
	Object obj = sequence.getItem(n);
	assure(!obj.isNull(), "This is the end, my friend.");
	if (obj.isSequence())
	{
		obj.decref();
		return true;
	}
	else
	{
		obj.decref();
		return false;
	}
}

Sequence
Sequence::getSequenceAt(int n) const
{
	Object obj = sequence.getItem(n);
	assure(isSequenceAt(n), "No sequence at pos:"<<n);

	if (!obj.isSequence())
	{
		obj.decref();
		Exception e;
		e << "Sequence: Item at pos:'" << n
		  << "' is no nested sequence.\n\n";
		throw e;
	}

	std::stringstream ss;
	ss << pathName << "[" << n << "]";
	Sequence s(obj, ss.str());
	obj.decref();
	return s;
} // getSequenceAt


//
// IterPolicy
//

Sequence::IterPolicy::IterPolicy() :
		iter(),
		nextObject()
{
} // end() constructor


Sequence::IterPolicy::IterPolicy(Object sequence) :
		iter(),
		nextObject()
{
	iter = Object(PyObject_GetIter(sequence.obj_));
	assure(!iter.isNull(), "Sequence::iterator called without sequence.");

	next();
} // valid constructor


Sequence::IterPolicy::IterPolicy(const IterPolicy& other) :
		iter(other.iter),
		nextObject(other.nextObject)
{
	Py_XINCREF(iter.obj_);
	Py_XINCREF(nextObject.obj_);
} // copy constructor


Sequence::IterPolicy::~IterPolicy()
{
	Py_XDECREF(iter.obj_);
	Py_XDECREF(nextObject.obj_);
} // ~IterPolicy


Sequence::IterPolicy&
Sequence::IterPolicy::operator=(const IterPolicy& other)
{
	iter = other.iter;
	nextObject = other.nextObject;

	Py_XINCREF(iter.obj_);
	Py_XINCREF(nextObject.obj_);

	return *this;
} // =


void
Sequence::IterPolicy::next()
{
	Py_XDECREF(nextObject.obj_);
	nextObject = Object(PyIter_Next(iter.obj_));
} // next


Object
Sequence::IterPolicy::obj() const
{
	return nextObject;
} // obj


Sequence
Sequence::fromString(const std::string& s)
{
	assure(!PyErr_Occurred(), "Dirty python context!");

	PyObject* pyDict = PyDict_New();

#ifndef NDEBUG
	int result =
#endif
		PyDict_SetItemString(pyDict, "__builtins__", PyImport_ImportModule("__builtin__"));
        assure(result==0, "Initializing __builtins__ failed!");

	Object o(PyRun_String(s.c_str(),
			      Py_eval_input,
			      pyDict,
			      pyDict));

	Py_DECREF(pyDict);

	if (o.isNull())
	{
		Exception e;
		e << "Erroneous Python Expression: \n\n" << s;
		throw e;
	}

	assure(o.isSequence(), "Python expression '" << s << "' is not a sequence!");

	Sequence seq(o, "<string>");
	o.decref();
	return seq;
}


