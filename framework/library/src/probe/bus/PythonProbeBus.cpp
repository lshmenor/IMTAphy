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
#include <WNS/probe/bus/PythonProbeBus.hpp>

#include <sstream>

using namespace wns::probe::bus;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    PythonProbeBus,
    wns::probe::bus::ProbeBus,
    "PythonProbeBus",
    wns::PyConfigViewCreator);

PythonProbeBus::PythonProbeBus(const wns::pyconfig::View& pyco):
    pyco_(pyco),
    reportErrors_(pyco_.get<bool>("reportErrors"))
{
    assure(pyco_.knows("accepts"), "Cannot retrieve method accept from PyConfig");
    assure(pyco_.knows("onMeasurement"), "Cannot retrieve method on Measurement from PyConfig");
    pyAcceptsMethod_ = pyco_.getObject("accepts");
    pyOnMeasurementMethod_ = pyco_.getObject("onMeasurement");
    pyOutputMethod_ = pyco_.getObject("output");
}

PythonProbeBus::~PythonProbeBus()
{
}

void
PythonProbeBus::onMeasurement(const wns::simulator::Time& timestamp,
                              const double& value,
                              const IContext& reg)
{
    PyObject* pyTimestamp = Py_BuildValue("d", timestamp);
    PyObject* pyValue = Py_BuildValue("d", value);

    assure(pyValue != NULL, "Could not create Python Object");

    const PyContext* c = dynamic_cast<const PyContext*>(&reg);
    assure(c != NULL, "PythonProbeBus can only collaborate with 'PyContext' implementation of IContext");

    PyObject* result = PyObject_CallFunctionObjArgs(pyOnMeasurementMethod_.obj_,
                                                    pyTimestamp,
                                                    pyValue,
                                                    c->pyDict_.obj_,
                                                    NULL);
    if (result == NULL)
        this->showdown("Error when calling Python accepts method.");

    Py_DECREF(result);
    Py_DECREF(pyTimestamp);
    Py_DECREF(pyValue);

    // Post Condition: Python must be clean before we leave
    if (PyErr_Occurred())
    {
        this->showdown("Error Python Context ist dirty");
    }
}

bool
PythonProbeBus::accepts(const wns::simulator::Time& timestamp,
                        const IContext& reg)
{
    PyObject* pyTimestamp = Py_BuildValue("d", timestamp);

    const PyContext* c = dynamic_cast<const PyContext*>(&reg);
    assure(c != NULL, "PythonProbeBus can only collaborate with 'PyContext' implementation of IContext");

    PyObject* result = PyObject_CallFunctionObjArgs(pyAcceptsMethod_.obj_,
                                                    pyTimestamp,
                                                    c->pyDict_.obj_,
                                                    NULL);
    if (result == NULL)
    {
        this->showdown("Error when calling Python accepts method.");
    }

    bool returnValue = (result==Py_True);

    Py_DECREF(result);
    Py_DECREF(pyTimestamp);

    // Post Condition: Python must be clean before we leave
    if (PyErr_Occurred())
    {
        this->showdown("Error Python Context ist dirty");
    }

    return returnValue;
}

void
PythonProbeBus::output()
{
    PyObject* result = PyObject_CallFunctionObjArgs(pyOutputMethod_.obj_,
                                                    NULL);
    if (result == NULL)
        this->showdown("Error when calling Python output method.");

    Py_DECREF(result);

    // Post Condition: Python must be clean before we leave
    if (PyErr_Occurred())
    {
        this->showdown("Error Python Context ist dirty");
    }
}

void
PythonProbeBus::showdown(const std::string& reason) const
{
    if (reportErrors_)
    {
        std::cerr << "\n" << reason << "\n";
        std::cerr << "\nPython says:\n";
        PyErr_Print();
        std::cerr << "\n";

    }

    PyErr_Clear();

    throw Exception(reason);
} // showdown
