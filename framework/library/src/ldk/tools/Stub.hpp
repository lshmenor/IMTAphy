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

#ifndef WNS_LDK_TOOLS_STUB_HPP
#define WNS_LDK_TOOLS_STUB_HPP

#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/Command.hpp>

#include <WNS/Cloneable.hpp>
#include <WNS/pyconfig/View.hpp>

#include <deque>


namespace wns { namespace ldk { namespace tools {

	/**
	 * @brief Provides arrival and depature time
	 *
	 * The arrival and depature time may be used to check your expectations
	 * in unit tests
	 */
	class StubCommand :
		public wns::ldk::Command
	{
	public:
		StubCommand()
		{
			// initialize to something invalid
			this->magic.sendDataTime = -1.0;
			// initialize to something invalid
			this->magic.onDataTime = -1.0;
                        this->magic.sequenceNumber = 0;
		}

		struct {
		} local;

		struct {
		} peer;

		struct {
			// when has sendData been called?
			simTimeType sendDataTime;
			// when has onData been called?
			simTimeType onDataTime;
                    int sequenceNumber;
		} magic;
	};

	/**
	 * @brief Base class for Stub and PERProviderStub
	 */
	class StubBase :
		public virtual FunctionalUnit,
		public HasReceptor<>,
		public HasConnector<>,
		public HasDeliverer<>
	{
	public:
		typedef std::deque<CompoundPtr> ContainerType;

		virtual
		~StubBase();

		/**
		 * @brief Enable or disable stepping.
		 */
		void
		setStepping(bool _stepping);

		/**
		 * @brief Set PCI and PDU size offsets.
		 */
		void
		setSizes(Bit addToPCISize, Bit addToPDUSize);

		/**
		 * @brief Accept only compounds with total size smaller or equal compoundSize
		 */
		void
		setAcceptanceSize(Bit compoundSize);

		/**
		 * @brief Empty the receive and sent buffer
		 */
		void
		flush();

		/**
		 * @brief Allow packets to be received.
		 *
		 */
		void
		open(bool wakeup = true);

		/**
		 * @brief Disallow packets to be received.
		 *
		 */
		void
		close();

		/**
		 * @brief Allow one packet to be received when in stepping mode.
		 *
		 */
		void
		step();

		virtual void
		calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const;

		virtual void
		onFUNCreated();

		bool
		integrityCheck();

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

	protected:
		StubBase();

	protected:
		// CompoundHandlerInterface
		virtual void
		doSendData(const CompoundPtr& sdu);

		virtual void
		doOnData(const CompoundPtr& compound);

		virtual bool
		doIsAccepting(const CompoundPtr& compound) const;

		virtual void
		doWakeup();

		bool stepping;
		bool accepting;

		int addToPCISize;
		int addToPDUSize;
            int compoundAcceptanceSize;
	}; // StubBase


	/**
	 * @brief Forward and record traffic.
	 *
	 * Use the Stub for testing, when you want to inspect the
	 * PDU flow in detail.
	 * Beside the recording of compounds, you may as well step through
	 * the execution like in a debugger using the step() method.
	 *
	 */
	class Stub :
		public StubBase,
		public CommandTypeSpecifier<StubCommand>,
		public Cloneable<Stub>
	{
	public:
		/**
		 * Set stepping to true, if you whish to have a Stub with
		 * debugger like stepping of execution.
		 */
		Stub(fun::FUN* fuNet, const pyconfig::View& config);

		// we need a unique overrider
		virtual void
		calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
		{
			StubBase::calculateSizes(commandPool, commandPoolSize, dataSize);
		} // calculateSizes

	private:
		virtual void
		doSendData(const CompoundPtr& sdu);

		virtual void
		doOnData(const CompoundPtr& sdu);

            int sequenceNumber;
	}; // Stub

}}}


#endif // NOT defined WNS_LDK_TOOLS_STUB_HPP


