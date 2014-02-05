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

#ifndef WNS_SCHEDULER_GROUPER_RELAYMETAGROUPER_HPP
#define WNS_SCHEDULER_GROUPER_RELAYMETAGROUPER_HPP

#include <WNS/scheduler/grouper/SpatialGrouper.hpp>

namespace wns { namespace scheduler { namespace grouper {
	class RelayMetaGrouper :
		public SpatialGrouper
	{
	public:
		RelayMetaGrouper(const wns::pyconfig::View& config);

		virtual
		~RelayMetaGrouper();

		virtual Grouping
		getTxGrouping(const UserSet activeUsers, int);
		virtual Grouping
		getRxGrouping(const UserSet activeUsers, int);

		virtual void setColleagues(RegistryProxyInterface* _registry);
		virtual void setFriends(wns::service::phy::ofdma::BFInterface* _ofdmaProvider);

	private:
		GroupingProviderInterface* internalStrategy;

		Grouping
		joinGroupings(const Grouping& a, const Grouping& b) const;
	};
}}}
#endif //WNS_SCHEDULER_GROUPER_RELAYMETAGROUPER_HPP


