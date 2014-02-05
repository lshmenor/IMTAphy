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

// Since Python may define some pre-processor definitions which affect the
// standard headers on some systems, you must include Python.h before any
// standard headers are included.
#include <WNS/Python.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/Exception.hpp>
#include <WNS/Assure.hpp>

#include <sstream>
#include <iostream>
#include <csignal>

using namespace wns::pyconfig;

View::View() :
	dict(NULL),
	viewExpression()
{
	++View::getCount();
	this->initializePython();
}

View::View(const View& other, const std::string &newViewExpression) :
	dict(NULL),
	viewExpression("")
{
	++View::getCount();
	this->initializePython();

	assure(!PyErr_Occurred(), "Dirty python context!");

	Object pyObject = other.getObject(newViewExpression);
	if(pyObject.isNull())
	{
		other.couldntRetrieve(newViewExpression);
		this->showdown("Couldn't resolve viewExpression.");
	}

	// not needed any longer
	pyObject.decref();

	std::stringstream ss;
	// the following python code snippet creates the context
	// for the new view.
	// View is a subclass of dict. you can find View in the
	// pyconfig::Parser module of the wns package.
	ss << "__import__('openwns.pyconfig').pyconfig.View(" << newViewExpression << ")";

	this->dict = Object(PyRun_String(ss.str().c_str(),
					 Py_eval_input,
					 other.dict.obj_,
					 other.dict.obj_));
	if(this->dict.isNull())
	{
		other.couldntRetrieve(newViewExpression);
		showdown("Couldn't resolve viewExpression.");
	}
	assert(!this->dict.isNull());

	this->viewExpression = other.viewExpression + "::" + newViewExpression;
} // View


View::View(const View& other, const std::string &newViewExpression, int at) :
	dict(NULL),
	viewExpression("")
{
	++View::getCount();
	this->initializePython();
	assure(!PyErr_Occurred(), "Dirty python context!");

	std::stringstream subscription;
	subscription << newViewExpression << "[" << at << "]";

	Object pyObject = other.getObject(subscription.str());
	if(pyObject.isNull())
	{
		other.couldntRetrieve(newViewExpression);
		showdown("Couldn't resolve viewExpression.");
	}

	// Not needed any longer
	pyObject.decref();

	{
		std::stringstream ss;
		ss << "__import__('openwns.pyconfig').pyconfig.View("
		   << newViewExpression
		   << "[" << at << "]"
		   << ")";

		dict = Object(PyRun_String(ss.str().c_str(),
					   Py_eval_input,
					   other.dict.obj_,
					   other.dict.obj_));
		assert(!dict.isNull());
	}

	{
		std::stringstream ss;

		ss << other.viewExpression
		   << "::" << newViewExpression << "[" << at << "]";
		viewExpression = ss.str();
	}
} // View


View::View(const Sequence& seq, int at) :
	dict(NULL),
	viewExpression("")
{
	++View::getCount();
	this->initializePython();
	assure(!PyErr_Occurred(), "Dirty python context!");

	assure(!seq.isSequenceAt(at), "Trying to create pyconfig::View from nested sequence!");

	Object pyDict(PyDict_New());
        assure(!pyDict.isNull(), "Creation of Python Dict Failed");

	int result = PyDict_SetItemString(pyDict.obj_, "dummy", seq.sequence.obj_);
        assure(result==0, "Inserting of element into Dict failed!");

	result = PyDict_SetItemString(pyDict.obj_, "__builtins__", PyImport_ImportModule("__builtin__"));
        assure(result==0, "Initializing __builtins__ failed!");

	{
		std::stringstream ss;
		ss << "__import__('openwns.pyconfig').pyconfig.View("
		   << "dummy[" << at << "]"
		   << ")";

		dict = Object(PyRun_String(ss.str().c_str(),
					   Py_eval_input,
					   pyDict.obj_,
					   pyDict.obj_));
		if (dict.isNull())
			showdown("couldn't create View from sequence item!", true);
	}
	pyDict.decref();

	{
		std::stringstream ss;

		ss << seq.pathName << "[" << at << "]";
		viewExpression = ss.str();
	}
}

View::~View()
{
	// can happen that dict is NULL if View() has been called but not object
	// has not been used.
	if(!dict.isNull())
	{
		dict.decref();
	}

	--View::getCount();
	this->finalizePython();
} //~View

wns::pyconfig::View
View::getView(const std::string& newViewExpression) const
{
	return View(*this, newViewExpression);
} // getView

wns::pyconfig::Sequence
View::getSequence(const std::string& sequenceExpression) const
{
	if (!this->isSequence(sequenceExpression))
	{
		Exception e;
		e << "Cannot retrieve sequence named '" << sequenceExpression
		  << "' from this view.\n\n"
		  << "Context: " << this->context() << "\n\n"
		  << "Content: " << this->asString() << "\n\n";
		throw e;
	}
	return pyconfig::Sequence(getObject(sequenceExpression),
				  this->viewExpression+"::"+sequenceExpression);
} // getSequence

wns::pyconfig::View
View::getView(const std::string& newViewExpression, int at) const
{
	return View(*this, newViewExpression, at);
} // getView

Object
View::getObject(const std::string& optionExpression) const
{
	assure(!PyErr_Occurred(), "Dirty python context!");

	return Object(PyRun_String(optionExpression.c_str(),
				   Py_eval_input,
				   dict.obj_,
				   dict.obj_));
} // getObject


Object
View::getObject(const std::string& optionExpression, int at) const
{
	Object seq = getObject(optionExpression);
	if(seq.isNull())
	{
		return Object();
	}

	if(!PySequence_Check(seq.obj_))
	{
		seq.decref();
		return Object();
	}

	Object o(PySequence_GetItem(seq.obj_, at));
	seq.decref();

	return o;
} // getObject


bool
View::len(int& result, const std::string& optionExpression) const
{
	assure(!PyErr_Occurred(), "Dirty python context!");

	Object o = getObject(optionExpression);
	if(o.isNull())
	{
		PyErr_Clear();
		return false;
	}

	if(!PySequence_Check(o.obj_))
	{
		o.decref();
		return false;
	}

	result = PySequence_Length(o.obj_);
	o.decref();
	return true;
} // len


int
View::len(const std::string& optionExpression) const
{
	assure(!PyErr_Occurred(), "Dirty python context!");

	int result;

	if(!len(result, optionExpression))
	{
		couldntRetrieve(optionExpression);

		showdown("Unknown thang in View::len.");
	}

	return result;
} // len

bool
View::knows(const std::string& optionExpression) const
{
	assure(!PyErr_Occurred(), "Dirty python context!");

	Object o = getObject(optionExpression);
	if(o.isNull())
	{
		PyErr_Clear();
		return false;
	}

	o.decref();
	return true;
} // knows

long long int
View::getId() const
{
	// id(object) returns long long since python2.5
	// Using PY_LONG_LONG here yields a compile error
	// if PY_LONG_LONG cannot be converted to long long int,
	// which moves the problem from runtime to compile time
	// fixes bug lp:174730
	return get<PY_LONG_LONG>("__id__");
} // getId

std::string
View::context() const
{
	return viewExpression;
} // context


void
View::patch(const std::string& expression)
{
	assure(!PyErr_Occurred(), "Dirty python context!");

	Object o(PyRun_String(expression.c_str(), Py_file_input, dict.obj_, dict.obj_));
	if(o.isNull())
	{
		showdown("couldn't patch View.", true);
	}

	o.decref();
} // patch

bool
View::isNone(const std::string& expression) const
{
	Object o = this->getObject(expression);
	bool isNone = o.isNone();
	o.decref();
	return isNone;
} // isNone

bool
View::isSequence(const std::string& expression) const
{
	Object o = this->getObject(expression);

        if(o.isNull())
        {
                this->couldntRetrieve(expression);
                this->showdown("Couldn't resolve viewExpression.");
        }

	if (PySequence_Check(o.obj_) == 1)
	{
		o.decref();
		return true;
	}
	else
	{
		o.decref();
		return false;
	}
}

View::View(const View& other)
{
	++View::getCount();
	this->initializePython();
	viewExpression = other.viewExpression;
	dict = other.dict;
	dict.incref();
} // copy constructor


View&
View::operator=(const View& other)
{
	viewExpression = other.viewExpression;
	dict = other.dict;
	dict.incref();

	return *this;
} // assignment operator


bool
View::operator==(const View& other) const
{
	return getId() == other.getId();
} // equality operator

bool
View::operator!=(const View& other) const
{
	return !(*this == other);
} // inequality operator

bool
View::operator<(const View& other) const
{
	return getId() < other.getId();
} // less than operator

bool
View::doConvert(bool& value, Object o) const
{
	switch(PyObject_IsTrue(o.obj_))
	{
		case 1:
			value = true;
			break;
		case 0:
			value = false;
			break;
		default:
			return false;
	}

	return true;
} // convert

bool
View::doConvert(std::string& value, Object o) const
{
	if(!o.isConvertibleToString())
	{
		return false;
	}

	value = o.toString();

	return true;
} // convert

void
View::couldntRetrieve(const std::string& optionExpression) const
{
	std::cerr << "PyConfig says:\n"
		  << "The configuration entry "
		  << "'" << optionExpression << "' "
		  << "could not be retrieved from:\n    "
		  << context() << "\n"
		  << "Following configuration entries are available:\n";
	{
		PyObject *ptype, *pvalue, *ptraceback;
		PyErr_Fetch(&ptype, &pvalue, &ptraceback);

		PyRun_String(
			"__import__('sys').stderr.write( __visible__ + '\\n')\n",
			Py_file_input,
			dict.obj_,
			dict.obj_);

		PyErr_Restore(ptype, pvalue, ptraceback);
	}
} // couldntRetrieve


void
View::showdown(const std::string& reason, bool raise) const
{
	std::cerr << "PyConfig says:\n";
	PyErr_Print();
	std::cerr << "\n";

	PyErr_Clear();

	if(raise)
		throw Exception(reason);
} // showdown

std::string
View::asString() const
{
	return this->get<std::string>("__stringRepresentation__");
} // asString

void
View::initializePython()
{
	if(View::getCount() == 1)
	{
		assure(Py_IsInitialized() == false, "called initializePython() although interpreter is already initialized!");
		void (*handler)(int);
		handler = std::signal(2, SIG_DFL);
		Py_Initialize();
		std::signal(2, handler);
	}
}

void
View::finalizePython()
{
	if(View::getCount() == 0)
	{
		assure(Py_IsInitialized() == true, "called finalizePython although interpreter has not been initialized!");
		void (*handler)(int);
		handler = std::signal(2, SIG_DFL);
		Py_Finalize();
		std::signal(2, handler);
	}
}

int&
View::getCount()
{
	static int count = 0;
	assure(count >=0, "Internal reference counting is broken!");
	return count;
}



