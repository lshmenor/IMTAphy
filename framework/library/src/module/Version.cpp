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

#include <WNS/module/Version.hpp>

using namespace std;
using namespace wns::module;

// Component constructor
Version::Version(const Release bRelease, const DateTime bTime,
		 const string bUser, const string bHost,
		 const string bFlags, const DateTime mTime,
		 const string mFile)
	: buildTime(bTime),
	  buildUser(bUser),
	  buildHost(bHost),
	  buildFlags(bFlags),
	  buildRelease(bRelease),
	  modificationTime(mTime),
	  modifiedFile(mFile)
{
}

// One string contructor
Version::Version(const string s)
	: buildTime((long int)(atof(Syntax::extractTime(s).c_str()))),
	  buildUser(Syntax::extractUser(s)),
	  buildHost(Syntax::extractHost(s)),
	  buildFlags(Syntax::extractFlags(s)),
	  buildRelease(Syntax::extractRelease(s)),
	  modificationTime(atol(Syntax::extractModificationTime(s).c_str())),
	  modifiedFile(Syntax::extractModifiedFile(s))
{
}

// Operators

// Equality operator
bool Version::operator==(const Version b) const
{
	return ((buildRelease == b.buildRelease) &&
		((modificationTime == b.modificationTime) ||
		 modificationTime.isEmpty() || b.modificationTime.isEmpty()));
}

// Inequality operator
bool Version::operator!=(const Version b) const
{
	return !(*this == b);
}

// Less operator
bool Version::operator<(const Version b) const
{
	if (buildRelease != b.buildRelease)
	{
		return (buildRelease < b.buildRelease);
	}
	else
	{
		return (modificationTime < b.modificationTime);
	}
}

// Greater operator
bool Version::operator>(const Version b) const
{
	return ((*this != b) && !(*this < b));
}

// Less than or equal operator
bool Version::operator<=(const Version b) const
{
	return ((*this == b) || (*this < b));
}

// Greater than or equal operator
bool Version::operator>=(const Version b) const
{
	return ((*this == b) || (*this > b));
}

// Public member functions
//  Selectors / Accessors

wns::module::DateTime
Version::getBuildTime() const
{
	return buildTime;
}

string Version::getBuildUser() const
{
	return buildUser;
}

string Version::getBuildHost() const
{
	return buildHost;
}

wns::module::Release
Version::getBuildRelease() const
{
	return buildRelease;
}

string Version::getBuildFlags() const
{
	return buildFlags;
}

wns::module::DateTime
Version::getModificationTime() const
{
	return modificationTime;
}

string Version::getModifiedFile() const
{
	return modifiedFile;
}

void Version::setBuildTime(const DateTime bTime)
{
	buildTime = bTime;
}

void Version::setBuildUser(const string bUser)
{
	buildUser = bUser;
}

void Version::setBuildHost(const string bHost)
{
	buildHost = bHost;
}

void Version::setBuildRelease(const Release bRelease)
{
	buildRelease = bRelease;
}

void Version::setBuildFlags(const string bFlags)
{
	buildFlags = bFlags;
}

void Version::setModificationTime(const DateTime mTime)
{
	modificationTime = mTime;
}

void Version::setModifiedFile(const string mFile)
{
	modifiedFile = mFile;
}

//  other public member functions

// return Version as a machine readable string
string Version::getString() const
{
	return buildRelease.getString() + "," + buildTime.getString() + "," +
		buildUser + "," + buildHost + "," + buildFlags + "," +
		modificationTime.getString() + "," + modifiedFile;
}

// return Version as a human readable string
string Version::getNiceString(bool verbose, string offset) const
{
	// release
	string s = "\033[00;1;32m" + buildRelease.getNiceString() + "\033[00m\n";

	// latest modification
	if ((modifiedFile.size() > 0) ||
	    (modificationTime.getString().size() > 0))
	{
		s += offset+"  +---\033[00;31m latest modification: " + modificationTime.getString() + "\n";
		s += offset+"  \033[00m|\033[00;31m                         " + modifiedFile + "\033[00m\n";
	}
	if (verbose)
	{
		// build ...
		if ((buildTime.getString().size() > 0) ||
		    (buildUser.size() > 0) ||
		    (buildHost.size() > 0)) s += offset+"  +---\033[00;33m built:              ";
		// ... time
		if (buildTime.getString().size() > 0) s += " " + buildTime.getString();
                s += '\n';
		// ... user
		if (buildUser.size() > 0) s += offset+"  \033[00m|\033[00;33m                         by " + buildUser;
		// ... host
		if (buildHost.size() > 0) s += " on  " + buildHost;
		if ((buildTime.getString().size() > 0) ||
		    (buildUser.size() > 0) ||
		    (buildHost.size() > 0)) s += "\n";

		// flags
		if (buildFlags.size() > 0)
		{
			s += offset+"  \033[00m+---\033[00;35m with flags:          " + buildFlags + "\033[00m\n";
		}
	}
	return s;
}

// private member functions

/// Returns the part of s that's after the n-th occurance of pattern.
string Version::cutAt(const string s, const string pattern, const int n)
{
	string s2 = s.substr(s.find(pattern) + pattern.length());
	for(int i=1; i<n; i++) {
		s2 = s2.substr(s2.find(pattern) + pattern.length());
	}
	return s2;
}

//  string syntax functions

string Version::Syntax::extractTime(const string s)
{
	// the build time is after the first comma
	string time = cutAt(s, ",", 1);
	// remove all after the time
	return time.substr(0, time.find(","));
}

string Version::Syntax::extractUser(const string s)
{
	// the build user is after the second comma
	string user = cutAt(s, ",", 2);
	// remove all after the user
	return user.substr(0, user.find(","));
}

string Version::Syntax::extractHost(const string s)
{
	// the build host is after the third comma
	string host = cutAt(s, ",", 3);
	return host.substr(0, host.find(","));
}

string Version::Syntax::extractFlags(const string s)
{
	// the build flags are after the fourth comma
	string flags = cutAt(s, ",", 4);
	return flags.substr(0, flags.find(","));
}

string Version::Syntax::extractModificationTime(const string s)
{
	// the modification time is are after the fifth comma
	string mtime = cutAt(s, ",", 5);
	return mtime.substr(0, mtime.find(","));
}

string Version::Syntax::extractModifiedFile(const string s)
{
	// the modification time is are after the fifth comma
	return cutAt(s, ",", 6);
}

string Version::Syntax::extractRelease(const string s)
{
	// the release is before the first comma
	return s.substr(0, s.find(","));
}


