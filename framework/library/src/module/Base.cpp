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

#include <WNS/Exception.hpp>
#include <WNS/module/Base.hpp>

#include <sstream>
#include <iostream>
#include <dlfcn.h>

using namespace wns::module;

Base::Base(const pyconfig::View& pyco) :
	convertMap(),
	//version(Release(), DepList(), DateTime(), "", ""),
	global(false),
	logger(pyco.get<pyconfig::View>("logger"))
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Constructor called");
	MESSAGE_END();
}

Base::~Base()
{
	MESSAGE_SINGLE(NORMAL, logger, "Destructor called");

	ConvertMap::iterator itr;
	for(itr=convertMap.begin(); itr!=convertMap.end(); ++itr)
	{
		removeServiceFromGlobalMap(itr->first);
	}
}

// wns::module::VersionInformation
// Base::getVersionInformation() const
// {
// 	return version;
// }

bool
Base::existsService(const std::string& s)
{
	return getGlobalMap().find(s) != getGlobalMap().end();
}

wns::module::Base::TypeMap&
Base::getGlobalMap()
{
	static TypeMap globalMap;
	return globalMap;
}

void
Base::removeServiceFromGlobalMap(const std::string& s)
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Removing service: ");
	m << s;
	MESSAGE_END();

	TypeMap::iterator itr = getGlobalMap().find(s);
	if(itr==getGlobalMap().end()) {
		std::stringstream err;
		err << "Service" << s << " not registered";
		throw(Exception(err.str()));
	} else {
		getGlobalMap().erase(itr);
	}
}

void
Base::addServiceToGlobalMap(const std::string& s)
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Adding service: ");
	m << s;
	MESSAGE_END();

	TypeMap::iterator itr = getGlobalMap().find(s);
	if(itr==getGlobalMap().end()) {
		getGlobalMap()[s]=this;
	} else {
		std::stringstream err;
		err << "Service " << s << " already registered";
		throw(Exception(err.str()));
	}
}

wns::ChamaeleonBase*
Base::create(const std::string& s)
{
	ConvertMap::iterator itr = convertMap.find(s);
	if(itr==convertMap.end()) {
		return NULL;
	} else {
		return (*(itr->second))(this);
	}
}

bool
Base::getGlobalFlag() const
{
	return global;
}

bool
Base::load(const std::string& name, bool absolutePath, bool beVerbose, bool lazyBinding)
{
	std::string str;
	if (absolutePath) {
		str = name;
	} else {
		str = "lib" + name + ".so";
	}

	if(beVerbose) {
		std::cout << std::endl << "Loading " << str;
	}

	void* handle = NULL;
	if (lazyBinding) {
		handle = dlopen(str.c_str(),RTLD_LAZY | RTLD_GLOBAL);
		if(beVerbose) {
			std::cout << " with lazy binding!" << std::endl;
		}
	} else {
		handle = dlopen(str.c_str(),RTLD_NOW | RTLD_GLOBAL);
		if(beVerbose) {
			std::cout << std::endl;
		}
	}

	return handle != NULL;
}

