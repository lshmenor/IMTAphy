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

#ifndef COPPER_WIRE_HPP
#define COPPER_WIRE_HPP

#include <COPPER/Transmission.hpp>
#include <COPPER/ReceiverInterface.hpp>

#include <WNS/events/CanTimeout.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/Broker.hpp>
#include <WNS/Singleton.hpp>
#include <WNS/RoundRobin.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/events/scheduler/Interface.hpp>
#include <WNS/osi/PDU.hpp>

#include <list>
#include <map>

namespace copper
{
	/**
	 * @brief Defines the connection between 'n' Transmitters/Receivers
	 *
	 * A wire instance allows a Transmitter to send data to a Receiver
	 * connected to the same wire instance. The number of Receivers
	 * connected to a wire is unlimted. If one transmission is active the
	 * wire is blocked. If another transmitter will transmit although the
	 * wire is not free a collision will occur.
	 */
	class WireInterface
	{
	public:
		/**
		 * @brief Destructor
		 */
		virtual
		~WireInterface()
		{}

		/**
		 * @brief Send data (Unicast Transmission)
		 *
		 * @param ut Data (unicast) to be sent
		 * @param duration The duration of the data transmission
		 */
		virtual void
		sendData(const UnicastTransmissionPtr& ut, simTimeType duration) = 0;

		/**
		 * @brief Send data (Boradcast Transmission)
		 *
		 * @param bt Data (broadcast) to be sent
		 * @param duration The duration of the data transmission
		 */
		virtual void
		sendData(const BroadcastTransmissionPtr& bt, simTimeType duration) = 0;

		/**
		 * @brief Stops a transmission before the duration specified in
		 * send data is reached.
		 *
		 * @note this is only needed in case the transmission needs to
		 * be stopped BEFORE the duration that has been provided to send
		 * data is elapsed.
		 */
		virtual void
		stopTransmission(const wns::osi::PDUPtr& pdu) = 0;

		/**
		 * @brief Returns the time for which the medium has been blocked
		 *
		 * Three different groups of return values must be
		 * distinguished:
		 *
		 * - values < 0.0: The wire is free
		 *
		 * - values == 0.0: A transmission has just been started (at
		 * exactly the same time, the wire is asked)
		 *
		 * - values > 0.0: A transmission is ongoing since "value"
		 * seconds
		 *
		 */
		virtual simTimeType
		blockedSince() const = 0;

		/**
		 * @brief Receiver need to register themselves via this method
		 */
		virtual void
		addReceiver(
			ReceiverInterface* r,
			const wns::service::dll::UnicastAddress& macAddress) = 0;
	};

	/**
	 * @brief An implementation of WireInterface
	 */
	class Wire :
		public virtual WireInterface
	{
	public:
		/**
		 * @brief Constructor
		 */
		explicit
		Wire(const wns::pyconfig::View& config);

		/**
		 * @name WireInterface
		 */
		//@{
		void
		sendData(const UnicastTransmissionPtr& ut, simTimeType duration);

		void
		sendData(const BroadcastTransmissionPtr& bt, simTimeType duration);

		/**
		 * @brief Cancel a transmission before it is finished
		 */
		void
		stopTransmission(const wns::osi::PDUPtr& pdu);

		simTimeType
		blockedSince() const;

		void
		addReceiver(
			ReceiverInterface* r,
			const wns::service::dll::UnicastAddress& macAddress);
		//@}

	private:
		/**
		 * @brief Event to be scheduled at start of a transmission to
		 * signal the end of a transmission
		 */
		class TransmissionEndEventBase
		{
		public:
			/**
			 * @brief Destructor
			 */
			virtual
			~TransmissionEndEventBase()
			{}

			/**
			 * @brief Returns the transmission the event was
			 * scheduled for
			 */
			virtual TransmissionPtr
			getTransmission() const = 0;
		};

		/**
		 * @brief Signale the end of transmission for the different
		 * transmission types (broadcast, unicast)
		 */
		template<typename TRANSMISSIONTYPE>
		class TransmissionEndEvent :
			public TransmissionEndEventBase
		{
		public:
			/**
			 * @brief Constructor
			 *
			 * @param Wire
			 */
			TransmissionEndEvent(Wire* w, const TRANSMISSIONTYPE& t) :
				wire(w),
				transmission(t)
			{
				assure(this->wire, "must be non-NULL");
				assure(this->transmission, "must be non-NULL");
			}

			/**
			 * @brief Called by EventScheduler on execution of this
			 * event
			 */
			void
			operator()()
			{
				this->wire->stopTransmission(transmission);
			}

			/**
			 * @brief Retuns the according transmission
			 */
			virtual TransmissionPtr
			getTransmission() const
			{
				return this->transmission;
			}

		private:
			/**
			 * @brief The wire on which the transmission took place
			 */
			Wire* wire;

			/**
			 * @brief The transmission itself
			 *
			 * @note The transmission is stored with its real type
			 * (this is why this class is a template) not its base
			 * class TransmissionPtr. This way the method
			 * wire->stopTransmission(transmission) can call two
			 * different methods according to the type of
			 * "transmission" because they are overload in
			 * "Wire". This makes the processing, which is different
			 * for Broadcast and Unicast transmission much easier.
			 */
			TRANSMISSIONTYPE transmission;
		};

		typedef std::map< wns::osi::PDUPtr, wns::events::scheduler::IEventPtr >
		TransmissionEndEventContainer;

		typedef std::map< wns::osi::PDUPtr, TransmissionPtr >
		Transmissions;

		typedef std::map<wns::service::dll::UnicastAddress, ReceiverInterface*>
		Address2ReceiverContainer;

		/**
		 * @brief Stops a unicast transmission (called on finished transmission)
		 *
		 * This one is called by TransmissionEndEvent
		 */
		void
		stopTransmission(const UnicastTransmissionPtr& ut);

		/**
		 * @brief Stops a broadcast transmission (called on finished transmission)
		 *
		 * This one is called by TransmissionEndEvent
		 */
		void
		stopTransmission(const BroadcastTransmissionPtr& bt);

		/**
		 * @brief Used to check if the wire is free
		 */
		bool
		isFree() const;

		/**
		 * @brief Checks if a transmission collided with another
		 * transmission
		 *
		 * The result will be stored IN the TransmissionPtr t
		 */
		void
		checkForCollision(const TransmissionPtr& t);

		/**
		 * @brief Calls onCopperFreeAgain of each connected receiver
		 *
		 * This is called at the end of a transmission. The
		 * implementation is round robin style, which means it will
		 * guarantee that each receiver will be the first one in the
		 * round to be called.
		 */
		void
		signalCopperFreeAgainToReceivers();

		/**
		 * @brief Stores and sends the event together with the PDU of
		 * the Transmission
		 */
		template <typename TRANSMISSIONTYPE>
		void
		addTransmissionEndEvent(const TRANSMISSIONTYPE& t, simTimeType arrivalTime)
		{
			assure(t, "must be non-NULL");
			assure(
				this->transmissions.find(t->pdu) == this->transmissions.end(),
				"already got this event");

			TransmissionEndEvent<TRANSMISSIONTYPE> te (this, t);

			
			this->transmissions[t->pdu] = te.getTransmission();
			this->transmissionEndEvents[t->pdu] =
				wns::simulator::getEventScheduler()->schedule(te, arrivalTime);
			
		}

		template <typename TRANSMISSIONTYPE>
		wns::simulator::Time
		sendDataGeneric(const TRANSMISSIONTYPE& transmission, simTimeType duration)
		{
			assure(transmission, "must be non-NULL");
			// if wire is not already blocked set time to now and
			// tell every Receiver the wire is busy
			if (this->isFree())
			{
				this->timeWireBlocked = wns::simulator::getEventScheduler()->getTime();
				std::for_each(
					receivers.begin(),
					receivers.end(),
					std::mem_fun(&ReceiverInterface::onCopperBusy));
			}

			wns::simulator::Time arrivalTime =
				wns::simulator::getEventScheduler()->getTime() + duration;
			this->checkForCollision(transmission);

			this->addTransmissionEndEvent(transmission, arrivalTime);
			return arrivalTime;
		}

		/**
		 * @brief Remove event according to its PDU
		 */
		void
		removeTransmissionEndEvent(const TransmissionPtr& transmission);

		/**
		 * @brief Name of the Wire
		 */
		std::string name;

		/**
		 * @brief Round robin container to realize round robin
		 * signalling in signalCopperFreeAgainToReceivers
		 */
		wns::RoundRobin<ReceiverInterface*> roundRobin;

		/**
		 * @brief Stores connected receives (used for non-round-robin
		 * tasks)
		 */
		std::list<ReceiverInterface*> receivers;

		/**
		 * @brief Associative container: key=PDU,
		 * value=TransmissionEndEvent
		 */
		TransmissionEndEventContainer transmissionEndEvents;

		/**
		 * @brief Keep all active transmissions.
		 */
		Transmissions transmissions;

		/**
		 * @brief Associative container: Allows mapping of Layer 2
		 * addresses to a copper::Receiver
		 */
		Address2ReceiverContainer addressMapping;

		/**
		 * @brief Logger instance
		 */
		wns::logger::Logger logger;

		/**
		 * @brief the time when the wire was blocked by a transmission
		 */
		simTimeType timeWireBlocked;
	};

	/**
	 * @brief The wire broker, see the wns::Broker documentation on how this
	 * works
	 */
	typedef wns::Broker<Wire> WireBroker;
}

#endif // NOT defined COPPER_WIRE_HPP


