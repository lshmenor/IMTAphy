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

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>

#include <WNS/Exception.hpp>
#include <WNS/Assure.hpp>

#include <sstream>
#include <iostream>

using namespace wns::pyconfig;

long Parser::serial = 0;

Parser::Parser() :
	View(),
	module_name()
{
	this->initPython();
} // Parser


Parser::Parser(const std::string& filename, const std::string& pathname) :
	View(),
	module_name()
{
	this->initPython();
	this->appendPath(pathname);
	this->load(filename);
} // Parser


Parser::~Parser()
{
} // ~Parser


void
Parser::load(const std::string &filename)
{
	assure(PyErr_Occurred() == false, "Dirty python context!");

	FILE* fp = fopen(filename.c_str(), "r");
	if(fp == NULL)
	{
 		throw Exception(std::string("couldn't load ")
 				+ filename
 				+ ": "
 				+ strerror(errno));
	}

	PyObject* o = PyRun_File(fp,
				 filename.c_str(),
				 Py_file_input,
				 this->dict.obj_,
				 this->dict.obj_);
	fclose(fp);
	if(o == NULL)
	{
		std::cerr << "wns::pyconfig::Parser says:\n"
			  << "Couldn't load config file '" << filename << "'.\n";

		showdown("Couldn't load config file '" + filename + "'");
	}

	Py_DECREF(o);
	this->viewExpression = filename;
} // load


void
Parser::loadString(const std::string &s)
{
	assure(PyErr_Occurred() == false, "Dirty python context!");

	PyObject* o = PyRun_String(s.c_str(),
				   Py_file_input,
				   this->dict.obj_,
				   this->dict.obj_);
	if(o == NULL)
	{
		std::cerr << "wns::pyconfig::Parser says:\n"
			  << "Couldn't load config from string:\n"
			  << s
			  << "\n";

		showdown("Couldn't load from string.");
	}
	Py_DECREF(o);

	this->viewExpression = "<string>";
} // loadString


void
Parser::initPython()
{
	std::stringstream ss;
	ss << Parser::serial++;
	ss >> this->module_name;

	PyObject* module = PyImport_AddModule(const_cast<char*>(this->module_name.c_str()));
	if(module == NULL)
	{
		throw Exception("couldn't open configfile");	// FIXME(fds)
	}

	this->dict = Object(PyModule_GetDict(module));
	this->dict.incref();
	PyDict_SetItemString(this->dict.obj_, "__builtins__", PyImport_ImportModule("__builtin__"));
} // initPython


void
Parser::appendPath(const std::string& pathname)
{
	assure(PyErr_Occurred() == false, "Dirty python context!");

	std::stringstream ss;
	ss << "import sys\n"
	   << "sys.path.append('" << pathname << "')\n";
	if(-1 == PyRun_SimpleString(ss.str().c_str()))
	{
		std::cerr << "wns::pyconfig::Parser says:\n"
			  << "Couldn't add path '" + pathname + "' to sys.path.\n";

		showdown("Couldn't add path.");
	}
} // addPath


View
Parser::fromString(const std::string& s)
{
	return helper::createViewFromString(s);
} // fromString


