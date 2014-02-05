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

#ifndef IP_ARPRESOLVER_HPP
#define IP_ARPRESOLVER_HPP

#include <IP/IPHeader.hpp>
#include <IP/VirtualARP.hpp>

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/service/dll/Address.hpp>
#include <WNS/Observer.hpp>

namespace ip {
    /**
     * @brief Functional unit that resolves IP to MAC addresses
     *
     * To resolve the MAC addresses this Functional Unit collaborates
     * with the VirtualARP
     *
     * \pyco{ip.Component.ARPResolver}
     */
	class ARPResolver :
		public virtual wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<ARPResolver>,
                public ResolveCallback
	{
	public:
		ARPResolver(wns::ldk::fun::FUN* fuNet, const wns::pyconfig::View& config);

		virtual void
		onFUNCreated();

		virtual void
		doSendData(const wns::ldk::CompoundPtr& compound);

		virtual void
		doOnData(const wns::ldk::CompoundPtr& compound);

		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr& compound) const;

		virtual void
		doWakeup();

        virtual void
		onAddressResolved(wns::service::dll::UnicastAddress dest);

	private:
		wns::logger::Logger log;

		wns::ldk::CommandReaderInterface* ipHeaderReader;

		wns::service::dll::UnicastAddress macID;

		wns::ldk::CompoundPtr buffer;
	};
} // ip

#endif // IP_ARPRESOLVER_HPP
