/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#ifndef TCP_CUMULATIVEACK_HPP
#define TCP_CUMULATIVEACK_HPP


#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>

#include <WNS/events/CanTimeout.hpp>

#include <WNS/container/Registry.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/pyconfig/View.hpp>

#include <TCP/CongestionControl.hpp>

namespace tcp {

	namespace tests {  
		class CumulativeACKTest;
	} // namespace tests

	class CongestionControl;

	class CumulativeACKCommand : 
		public wns::ldk::Command
	{
	public:
		CumulativeACKCommand()
		{
			peer.type = I;

			peer.advertisedWindowSize = 0;
			peer.sequenceNumber = 0;
			peer.ACKNumber = 0;
		}

		typedef enum {I, ACK} FrameType; //todo: add PiggyBack

		struct {} local;

		struct {
			/**
			 * @brief Determine wether the sent out segement is a
			 * data compound or a control compound
			 */
			FrameType type;

			/**
			 * @brief The receiver's capacity being able to consume TCP segments
			 */
			unsigned long int advertisedWindowSize;

			/**
			 * @brief The sender's sequence number
			 */
			unsigned long int sequenceNumber;

			/**
			 * @brief The receiver's acknowledgement number
			 * This number indicates the sender the next segment
			 * expected to received. All segments up to this
			 * number - 1 are acknowledged implicitly.
			 */
			unsigned long int ACKNumber;
		} peer;

		struct {} magic;
	};

	class CumulativeACK :
		public virtual wns::ldk::FunctionalUnit,
		public wns::ldk::CommandTypeSpecifier<CumulativeACKCommand>,
		public wns::ldk::HasReceptor<>,
		public wns::ldk::HasConnector<>,
		public wns::ldk::HasDeliverer<>,
		public wns::Cloneable<CumulativeACK>
	{
	private:
		/**
		 * @brief An element for a compound
		 */
		class Timeout :
			public wns::events::CanTimeout
		{
		public:
			Timeout();
			Timeout(CumulativeACK* _parent, unsigned long int seqNr, simTimeType timer):
				CanTimeout(),
				sequenceNumber(seqNr),
				parent(_parent),
				logger("TCP", "CumulativeACK")
			{
				assure(_parent, "Invalid CumulativeACK instance.");
				this->setTimeout(timer);
			};

			virtual ~Timeout()
			{
				parent = NULL;
			}

			void onTimeout()
			{
				parent->ccStrategy->onSegmentLoss(CongestionControl::TIMEOUT, sequenceNumber);

				MESSAGE_BEGIN(NORMAL, logger, m, parent->getFUN()->getName());
				m << "\tTimeout of compund SequenceNr: " << sequenceNumber;
				MESSAGE_END();
				parent->ccStrategy->clearDuplicateACKCounter();

				parent->retransmitData(sequenceNumber);
			}

		private:
			unsigned long int sequenceNumber;

			CumulativeACK* parent;

			wns::logger::Logger logger;
		};

		/**
		 * @brief Container for the time out events
		 */
		typedef wns::container::Registry<unsigned long int, Timeout*, wns::container::registry::DeleteOnErase> Timeouts;

		/**
		 * @brief Container for the compounds
		 */
		typedef wns::container::Registry<unsigned long int, wns::ldk::CompoundPtr, wns::container::registry::NoneOnErase> Compounds;


	public:
		CumulativeACK(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& _pyco);

		/**
		 * @brief Component's destructor
		 */
		virtual
		~CumulativeACK();

		/**
		 * @brief Implementation of the CompoundHandlerInterface
		 */
		virtual bool
		doIsAccepting(const wns::ldk::CompoundPtr& _compound) const;

		virtual void
		onFUNCreated();

		virtual void
		doOnData(const wns::ldk::CompoundPtr& _compound);

		virtual void
		doSendData(const wns::ldk::CompoundPtr& _compound);

		/**
		 * @brief Return the number of compounds being allowed to be
		 * sent
		 */
		unsigned long int
		sendCredit() const;

		simTimeType
		getRetransmissionTimeout() { return ccStrategy->getRetransmissionTimeout();}

	private:
		void
		doWakeup();

		void
		retransmitData(const unsigned long int seqNr);

		unsigned long int
		min(const unsigned long int x, const unsigned long int y) const;

        void
        updateTCPHeader(const wns::ldk::CompoundPtr& _compound);
        
		const wns::pyconfig::View& pyco;

		wns::logger::Logger logger;

		wns::ldk::fun::FUN* fun;

		/**
		 * @brief The congestion control instance which guaranties slow
		 * start behaviour and is recalculating window size from the
		 * sender's point of view
		 */
		CongestionControl* ccStrategy;

		/**
		 * @brief The receiver's buffer size being capable of processing
		 * incoming TCP segments
		 */
		unsigned long int advertisedWindowSize;

		/**
		 * @brief The sequence number of the next segment to be sent
		 */
		unsigned long int sequenceNR;

		/**
		 * @brief The sequence number of the next segment to be received/acknowledged
		 */
		unsigned long int ackNR;

		/**
		 * @brief The last successfully received acknowledgment number.
		 */
		unsigned long int lastACKInOrder;

		/**
		 * @brief Time between retransmitting a TCP segment given in seconds
		 */
		simTimeType retransmissionTimeout;

		/**
		 * @brief Container for received compounds
		 */
		Compounds receivingCompounds;
		
		/**
		 * @brief All the compounds that have been sent but not acknowledged yet
		 */
		Compounds sendingCompounds;

		/**
		 * @brief Timeout events of compounds sent out
		 */
		Timeouts timeoutSendingCompounds;

		/**
		 * @brief Acknowledgement compound of the last in order received
		 * TCP segement
		 */
		wns::ldk::CompoundPtr ackCompound;
		wns::ldk::CompoundPtr copiedACKCompound;

        /**
         * @brief Reader for the TCP Header
         */
        wns::ldk::CommandReaderInterface* tcpHeaderReader;
        
		unsigned long int probeNumber;

		unsigned long int instanceID;

		bool probingEnabled;

        wns::probe::bus::ContextCollectorPtr windowSizeContextCollector;
        wns::probe::bus::ContextCollectorPtr sendCreditContextCollector;
	};
} // namespace tcp

#endif // NOT defined TCP_CUMULATIVEACK_HPP

