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

#ifndef IP_TESTS_ADDRESSINGPROVIDERSTUB_HPP
#define IP_TESTS_ADDRESSINGPROVIDERSTUB_HPP

#include <WNS/service/nl/Address.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/ErrorRateProviderInterface.hpp> 

#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/View.hpp>

#include <deque>

using namespace wns::ldk;

namespace ip { namespace tests { 

	/**
	 * @brief Provide a fixed PER for use by other layers
	 *
	 */
	class AddressingProviderCommand :
		public EmptyCommand
	{
	public:
		AddressingProviderCommand()
			: source(), destination()
			{};
		void setSourceAddress(wns::service::nl::Address s) { source = s;} 
		void setDestinationAddress(wns::service::nl::Address d) { destination = d;} 

		virtual wns::service::nl::Address
		getSourceAddress() { return source;}

		virtual wns::service::nl::Address
		getDestinationAddress() { return destination;}

	private:
		wns::service::nl::Address source;
		wns::service::nl::Address destination;
	};

	class AddressingProviderStub :
		public virtual FunctionalUnit,
		public CommandTypeSpecifier<AddressingProviderCommand>,
		public HasReceptor<>,
		public HasConnector<>,
		public HasDeliverer<>,
		public wns::Cloneable<AddressingProviderStub>
	{
		typedef std::deque<CompoundPtr> ContainerType;

	public:
		/**
		 * Set stepping to true, if you whish to have a AddressingProviderStub with
		 * debugger like stepping of execution.
		 */
		AddressingProviderStub(fun::FUN* fuNet, const wns::pyconfig::View& config);

		virtual ~AddressingProviderStub();

		void setStepping(bool _stepping);

		void setSizes(int addToPCISize, int addToPDUSize);

		void setSourceAddress(wns::service::nl::Address s) { source = s;} 

		void setDestinationAddress(wns::service::nl::Address d) { destination = d;}

		/**
		 * @brief Empty the receive and sent buffer
		 */
		void flush();

		/**
		 * @brief Allow packets to be received.
		 *
		 */
		void open(bool wakeup = true);

		/**
		 * @brief Disallow packets to be received.
		 *
		 */
		void close();

		/**
		 * @brief Allow one packet to be received when in stepping mode.
		 *
		 */
		void step();

		virtual void doSendData(const CompoundPtr& sdu);
		virtual void doOnData(const CompoundPtr& compound);

		virtual void calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

		virtual void onFUNCreated();

		bool integrityCheck();

		/**
		 * @brief Container storing the compounds received via doOnData
		 */
		ContainerType received;

		/**
		 * @brief Container storing the compounds received via doSendData
		 */
		ContainerType sent;

		/**
		 * @brief Incremented each time wakeup gets called
		 */
		long wakeupCalled;

		/**
		 * @brief Incremented each time onFUNCreated gets called.
		 */
		long onFUNCreatedCalled;

	private:
		// CompoundHandlerInterface
		virtual bool doIsAccepting(const CompoundPtr& compound) const;
		virtual void doWakeup();

		bool stepping;
		bool accepting;

		int addToPCISize;
		int addToPDUSize;

		double fixedPER;

		wns::service::nl::Address source;
		wns::service::nl::Address destination;
	};
} // tests
} // ip


#endif // NOT defined IP_TESTS_ADDRESSINGPROVIDERSTUB_HPP


