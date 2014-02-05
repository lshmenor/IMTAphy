/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
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

#ifndef IP_IPTABLES_FILTERS_SOURCEDESTINATIONFILTER_HPP
#define IP_IPTABLES_FILTERS_SOURCEDESTINATIONFILTER_HPP

#include <IP/iptables/filters/FilterInterface.hpp>

#include <WNS/service/nl/Address.hpp>

namespace ip { namespace iptables { namespace filters {

	class SourceDestinationFilter:
		public FilterInterface
	{
	public:
		SourceDestinationFilter(const wns::pyconfig::View& _pyco);

		SourceDestinationFilter(wns::service::nl::Address _source,
								wns::service::nl::Address _sourceMask,
								wns::service::nl::Address _destination,
								wns::service::nl::Address _destinationMask);

        virtual bool
        fires(const IPCommand*);

		virtual bool
        fires(const IPCommand*, const wns::service::tl::IUDPHeader*);

        virtual bool
        fires(const IPCommand*, const wns::service::tl::ITCPHeader*);

		bool
		operator ==(const SourceDestinationFilter& other) const;

	private:
		wns::service::nl::Address source;
		wns::service::nl::Address sourceMask;
		wns::service::nl::Address destination;
		wns::service::nl::Address destinationMask;
	};
} // filters
} // iptables
} // ip
#endif //IP_IPTABLES_FILTERS_SOURCEDESTINATIONFILTER_HPP
