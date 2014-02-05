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

#ifndef IP_IPHEADER_HPP
#define IP_IPHEADER_HPP

//#include <WNS/ldk/CommandTypeSpecifier.hpp>
//#include <WNS/ldk/Forwarding.hpp>
#include <WNS/ldk/fu/Plain.hpp>

//#include <WNS/ldk/FUNConfigCreator.hpp>
//#include <WNS/ldk/HasReceptor.hpp>
//#include <WNS/ldk/HasConnector.hpp>
//#include <WNS/ldk/HasDeliverer.hpp>

#include <WNS/service/nl/Service.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/service/dll/DataTransmission.hpp>
#include <WNS/service/dll/FlowID.hpp>

namespace ip {

	class IPCommand :
		public wns::ldk::Command,
		public wns::IOutputStreamable
	{
	public:
		IPCommand()
		{
			this->local.dllName = "";
			this->local.isForwarded = false;
			this->peer.TTL = 255;
		}

		/** @brief The locally used part of the IP command */
		struct Local
		{
			wns::service::nl::Address nextHop;
			wns::service::dll::UnicastAddress destinationDLLAddress;
			std::string dllName;
			std::string arpZone;
			bool isForwarded;
			wns::service::dll::UnicastAddress macID;
			wns::service::dll::FlowID dllFlowID;
		};
		Local local;

		/** @brief The part of the IP command signalled to the peer entitity */
		struct Peer
		{
			wns::service::nl::Address source;
			wns::service::nl::Address destination;
			wns::service::nl::protocolNumber protocol;
			int TTL;
//			wns::service::dll::FlowID dllFlowID;
		};
		Peer peer;

		struct Magic
		{
		};
		Magic magic;

    private:
		virtual std::string
		doToString() const
		{
			std::stringstream str;
			str << "S: " << peer.source << " D: " << peer.destination
				<< " TTL: " << peer.TTL << " Prot: " << peer.protocol
				<< " Next: " << local.nextHop << " Via : " << local.dllName << " Forw: " << (local.isForwarded ? "True":"False");
			return str.str();
		}

	};

    /**
     * @brief Dummy Functional Unit that only provides the IP Header
     *
     * \pyco{ip.Component.IPHeader}
     */
	class IPHeader :
		public wns::ldk::fu::Plain<IPHeader, IPCommand>
	{
	public:
		IPHeader(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config);

		virtual ~IPHeader() {}

		// We are blackboard, Resistance is futile
		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr&) const { return true; }

		virtual void
		doSendData(const wns::ldk::CompoundPtr&) {}

		virtual void
		doWakeup() {}

		virtual void
		doOnData(const wns::ldk::CompoundPtr&) {}

		virtual void
		calculateSizes(const wns::ldk::CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

	private:
		/**
		 * @brief Allow for changing the IP-header size from the default
		 *        20*8b
		 */
		Bit headerSize;



	};
} // ip


#endif // IP_IPHEADER_HPP
