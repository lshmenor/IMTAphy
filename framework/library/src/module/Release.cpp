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

#include <cstdlib>
#include <WNS/module/Release.hpp>

using namespace std;
using namespace wns::module;

/// One string constructor
Release::Release(const string s)
	: empty(s=="")
{
	// archive is up to the first /
	archive = s.substr(0, s.find("/"));

	// the rest is the FQRN
	string fqrn = s.substr(s.find("/") + 1, s.size() - s.find("/") - 1);

	// category is up to the first --
	category = fqrn.substr(0, fqrn.find("--"));
	// remove category from fqrn
	fqrn = fqrn.substr(fqrn.find("--") + 2, fqrn.size() - fqrn.find("--") - 2);

	// branch is up to the first -- now, as category is removed
	branch = fqrn.substr(0, fqrn.find("--"));
	// remove branch
	fqrn = fqrn.substr(fqrn.find("--") + 2, fqrn.size() - fqrn.find("--") - 2);

	// version is up to the first --
	version = fqrn.substr(0, fqrn.find("--"));
	// revision is the remaining string
	revision = fqrn.substr(fqrn.find("--") + 2, fqrn.size() - fqrn.find("--") - 2);

	//  revision = fqrn.substr(0, fqrn.find("--"));
	//  fqrn = fqrn.substr(fqrn.find("--") + 2, fqrn.size() - fqrn.find("--") - 2);
}

/// Component constructor
Release::Release(const string archive, const string category,
		 const string branch, const string version,
		 const string revision)
	: empty((archive=="")&&(category=="")&&(branch=="")&&(version=="")&&(revision==""))
{
	this->archive = archive;
	this->category = category;
	this->branch = branch;
	this->version = version;
	this->revision = revision;
}

// Operators

/// Equality operator
bool Release::operator==(const Release b) const
{
	// should we compare archive too?
	return empty || b.empty || (((category == b.category) &&
				     (branch == b.branch) &&
				     (version == b.version) &&
				     (revision == b.revision)));
}

/// Inequality operator
bool Release::operator!=(const Release b) const
{
	return empty || b.empty || (!(*this == b));
}

/// Less operator
bool Release::operator<(const Release b) const throw (CategoryMatchError, BranchMatchError)
{
	if (empty || b.empty) return true;
	else {
		// As we don't know what to do if category or branch
		// don't match, throw an exception if that's the case.
		if (category != b.category) {
			throw CategoryMatchError();
		}
		if (branch != b.branch) {
			throw BranchMatchError();
		}
    
		// Compare version and revision
		if (version != b.version) {
			return versionLowerThan(b.version);
		}
		else {
			return revisionLowerThan(b.revision);
		}
	}
}

/// Greater operator
bool Release::operator>(const Release b) const
{
	return empty || b.empty || (((*this != b) && !(*this < b)));
}

/// Less than or equal to operator
bool Release::operator<=(const Release b) const
{
	return empty || b.empty || (((*this == b) || (*this < b)));
}

/// Greater than or equal to operator
bool Release::operator>=(const Release b) const
{
	return empty || b.empty || (((*this == b) || (*this > b)));
}

// Public member functions

string Release::getString() const
{
	return empty ? "" : (archive + "/" + category + "--" + branch + "--" + version + "--" + revision);
}

string Release::getNiceString() const
{
	return empty ? "" : (category + " (" + branch + ") " + version + " " + revision);
}

// Private member functions

// Compare two versions
bool Release::versionLowerThan(string s) const
{
	// we assume that version is like "12.34"
	int ourMajorVersion = atoi(version.substr(0, version.find(".")).c_str());
	int ourMinorVersion = atoi(version.substr(version.find(".") + 1, version.size() - version.find(".") - 1).c_str());
	int hisMajorVersion = atoi(s.substr(0, s.find(".")).c_str());
	int hisMinorVersion = atoi(s.substr(s.find(".") + 1, s.size() - s.find(".") - 1).c_str());

	if (ourMajorVersion == hisMajorVersion)
	{
		return (ourMinorVersion < hisMinorVersion);
	}
	else
	{
		return (ourMajorVersion < hisMajorVersion);
	}
}

// Compare two revisions
bool Release::revisionLowerThan(string s) const
{
	// we assume that the revision contains the numbers after a "-" up to the end
	int ourPatchLevel = atoi(revision.substr(revision.find("-")).c_str());
	int hisPatchLevel = atoi(s.substr(s.find("-")).c_str());

	return (ourPatchLevel < hisPatchLevel);
}


