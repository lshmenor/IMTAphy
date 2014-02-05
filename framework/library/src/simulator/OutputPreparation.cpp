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

#include <WNS/simulator/OutputPreparation.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>

#include <iomanip>

using namespace wns::simulator;

STATIC_FACTORY_REGISTER(Move, OutputPreparationStrategy, "Move");
STATIC_FACTORY_REGISTER(Delete, OutputPreparationStrategy, "Delete");

void
Move::prepare(const std::string& path)
{
	struct stat buf;

	if(-1 == lstat(path.c_str(), &buf)) {
			// we encountered problems accessing the output directory.
			// the only acceptable error here is ENOENT (the directory
			// does not exist.)
			// all other errors should probably terminate the simulation.

		if(errno != ENOENT) {
			throw(wns::Exception("Couldn't access output directory."));
		}
	} else {
			// now we know the output path already exists.
			// let's just move it out of our way.

		boost::posix_time::ptime date(boost::posix_time::second_clock::local_time());

		std::stringstream ss;
		ss << path << "." << boost::posix_time::to_simple_string(date);

		rename(path.c_str(), ss.str().c_str());
	}

	system(("mkdir -p " + path).c_str());
} // Move::prepare


void
Delete::prepare(const std::string& path)
{
	system(("rm -fr " + path).c_str());
	system(("mkdir " + path).c_str());
} // Delete::prepare




