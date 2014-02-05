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

#ifndef WIMAC_SCHEDULER_CLASSIFIERPOLICYDROPIN_HPP
#define WIMAC_SCHEDULER_CLASSIFIERPOLICYDROPIN_HPP

#include <WNS/ldk/Classifier.hpp>
#include <WNS/ldk/ldk.hpp>

namespace wns { namespace scheduler { namespace tests {

	class ClassifierPolicyDropIn
	{
	public:
		//ClassifierPolicyDropIn( wns::ldk::FUN* fun ); / Classify the compound
		//with a pre-defined sequence of CIDs or with an individual CID

		ClassifierPolicyDropIn(wns::ldk::fun::FUN*) 
			: counter(0),
			  nextCID(wns::ldk::ClassificationID(0)) {};

		wns::ldk::ClassificationID classify( wns::ldk::CompoundPtr compound );
		void setCIDsVector(std::vector<wns::ldk::ClassificationID> _ids);
		void setNextCID(wns::ldk::ClassificationID cid);
	private:
		//friend class ConnectionClassifier; // what for?
		std::vector<wns::ldk::ClassificationID> ids;
		int counter;
		wns::ldk::ClassificationID nextCID;
	};


}}} // namespace wimac::scheduler::tests
#endif // WIMAC_SCHEDULER_CLASSIFIERPOLICYDROPIN_HPP


