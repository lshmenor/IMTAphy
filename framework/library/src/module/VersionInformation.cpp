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

// Dependencies
#include <WNS/module/VersionInformation.hpp>

using namespace std;
using namespace wns::module;

// public member functions

// return version information as machine readable string
string VersionInformation::getString() const
{
	return version.getString() + "::" + dependencies.getString();
}

// return version information as human readable string
string VersionInformation::getNiceString() const
{
	return version.getNiceString(true, "") + dependencies.getNiceString();
}

// private member functions

// get the version part of a version information string
string VersionInformation::Syntax::extractVersion(const string s)
{
	return s.substr(0, s.find("::"));
}

// get the dependencies part of a version information string
string VersionInformation::Syntax::extractDependencies(const string s)
{
	return s.substr(s.find("::") + 2);
}
