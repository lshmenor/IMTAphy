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

#ifndef IP_FLOWIDBUILDERS_HPP
#define IP_FLOWIDBUILDERS_HPP

#include <WNS/ldk/Key.hpp>
#include <WNS/service/nl/Address.hpp>

namespace ip {

	class IPAddressKey :
		public wns::ldk::Key
	{
	public:
		IPAddressKey(wns::service::nl::Address _ip) { this->ip = _ip; }

		virtual bool operator<(const Key& other) const
		{
			assureType(&other, const IPAddressKey*);
			return this->ip < dynamic_cast<const IPAddressKey&>(other).ip;
		}

		virtual std::string str() const
		{
			std::stringstream ss;
			ss << "IP: " << this->ip;
			return ss.str();
		}

	private:
		wns::service::nl::Address ip;
	};

    /**
     * @brief Separates by the nextHopAddress of the IP Header
     *
     * This is used to separate packets and stop individual flows
     * that need to wait for ARP address resolution.
     *
     * \pyco{ip.Component.FlowIDBuilder}
     */
	class OutputBuffersFlowIDBuilder:
		public wns::ldk::KeyBuilder
	{
	public:
		OutputBuffersFlowIDBuilder(
			wns::ldk::fun::FUN* _fun,
			const wns::pyconfig::View& _config);

		virtual
		~OutputBuffersFlowIDBuilder() {}

		virtual void
		onFUNCreated();

		virtual wns::ldk::ConstKeyPtr
		operator () (const wns::ldk::CompoundPtr& compound, int direction) const;

	private:
		wns::pyconfig::View config;

		const wns::ldk::fun::FUN* fun;

		wns::ldk::CommandReaderInterface* ipHeaderReader;
	};

} // ip

#endif // IP_FLOWIDBUILDERS_HPP

