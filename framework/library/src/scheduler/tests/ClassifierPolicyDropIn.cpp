/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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
#include <WNS/scheduler/tests/ClassifierPolicyDropIn.hpp>


using namespace wns::scheduler::tests;

//ClassifierPolicyDropIn::ClassifierPolciyDropIn( wns::ldk::FUN* fun );

/// Classify the compound with a pre-defined sequence of CIDs.
wns::ldk::ClassificationID
ClassifierPolicyDropIn::classify( wns::ldk::CompoundPtr /* compound */) {
	assure(nextCID || (ids.size() != 0), "You have to tell me first which ID(s) I should assign");

	if (nextCID != wns::ldk::ClassificationID(0))
	{
		wns::ldk::ClassificationID cid = nextCID;
		nextCID = wns::ldk::ClassificationID(0);
		return cid;
	}
	else
	{
		wns::ldk::ClassificationID retVal = ids[counter];
		counter = (++counter) % ids.size();

		return retVal;
	}
}


void ClassifierPolicyDropIn::setCIDsVector(std::vector<wns::ldk::ClassificationID> _ids) {
	ids = _ids;
}

void ClassifierPolicyDropIn::setNextCID(wns::ldk::ClassificationID cid)
{
	nextCID = cid;
}




