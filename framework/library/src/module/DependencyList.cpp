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

#include <string>
#include <iostream>

#include <WNS/module/DependencyList.hpp>
#include <WNS/module/Release.hpp>

using namespace std;
using namespace wns::module;

DepListElem::DepListElem(const depModeType mode, const Version dep)
	: depMode(mode), dependency(dep)
{
}

DepListElem::DepListElem(const string s) throw (DepListElemInvalidInitString)
	: dependency(Version(s.substr(1)))
{
	switch (s[0]) {
	case '<':
		depMode = LessThan;
		break;
	case '=':
		depMode = EqualTo;
		break;
	case '>':
		depMode = GreaterThan;
		break;
	default:
		throw DepListElemInvalidInitString();
	}
}

DepListElem::depModeType DepListElem::getDepMode() const
{
	return depMode;
}


Version DepListElem::getDependency() const
{
	return dependency;
}

void DepListElem::setDepMode(const depModeType mode)
{
	depMode = mode;
}

void DepListElem::setDependency(const Version dep)
{
	dependency = dep;
}

bool DepListElem::dependencyMetBy(const Version ver) const
{
	switch (depMode) {
	case LessThan:
		return (ver < dependency);
	case EqualTo:
		return (ver == dependency);
	case GreaterThan:
		return (ver > dependency);
	default:
		cout << "No valid comparison method for dependency checking defined!" << endl;
		return false;
	}
}

string DepListElem::getString() const
{
	string mode;
	switch (depMode) {
	case LessThan:
		mode = '<';
		break;
	case EqualTo:
		mode = '=';
		break;
	case GreaterThan:
		mode = '>';
		break;
	}
	return mode + dependency.getString();
}

string DepListElem::getNiceString() const
{
	string s = "    +--- ";
	switch (depMode) {
	case LessThan:
		s += "at least ";
		break;
	case EqualTo:
		s += "";
		break;
	case GreaterThan:
		s += "at most ";
		break;
	}
	return s + dependency.getNiceString(false, "    ");
}

DepList::DepList(const string& s)
{
	string deps = s;
	while (deps.find("(") != deps.npos) {
		depListElements.push_back(DepListElem(deps.substr(deps.find("(") + 1, deps.find(")") - 1)));
		deps = deps.substr(deps.find(")") + 1);
	}
}

string DepList::getString() const
{
	string s = "";
	for (size_t i = 0; i < depListElements.size(); i++) {
		s += "(" + depListElements[i].getString() + ")";
	}
	return s;
}

string DepList::getNiceString() const
{
	string s = "  ";
	if(!depListElements.empty()) {
		s += "+--- depends on:\n";
		for(size_t i = 0; i < depListElements.size(); i++) {
			s += depListElements[i].getNiceString();
		}
	}
	return s;
}

