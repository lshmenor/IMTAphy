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

#ifndef COPPER_RECEIVER_HPP
#define COPPER_RECEIVER_HPP

#include <COPPER/Transmission.hpp>
#include <COPPER/ReceiverInterface.hpp>

#include <WNS/service/phy/copper/Handler.hpp>
#include <WNS/service/phy/copper/CarrierSensing.hpp>
#include <WNS/service/phy/copper/Notification.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/distribution/Distribution.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/Subject.hpp>

namespace copper {

	class WireInterface;

	/**
	 * @brief A simple Receiver with configurable BER patterns
	 *
	 * The BER patterns can be any distribution available in WNS (or a
	 * combination of any distribution).
	 */
	class Receiver :
		virtual public wns::service::phy::copper::Notification,
		virtual public ReceiverInterface
	{
		typedef wns::service::phy::copper::Handler
		Handler;

		typedef wns::service::phy::copper::CarrierSensing
		CarrierSensing;

		/**
		 * @brief Used as functor to Handler::onDataCalls
		 */
		struct OnData
		{
			OnData(const wns::osi::PDUPtr& _pdu, double _ber, bool _collision) :
				pdu(_pdu),
				ber(_ber),
				collision(_collision)
			{
			}

			void
			operator()(Handler* handler)
			{
				handler->onData(this->pdu, this->ber, this->collision);
			}

		private:
			wns::osi::PDUPtr pdu;
			double ber;
			bool collision;
		};

	public:
		/**
		 * @brief Constructor
		 */
		Receiver(const wns::pyconfig::View& _pyco, WireInterface* _wire);

		/**
		 * @brief Destructor
		 */
		virtual
		~Receiver();

		/**
		 * @name ReceiverInterface
		 */
		//@{
		virtual bool
		onData(const UnicastTransmissionPtr& transmission);

		virtual bool
		onData(const BroadcastTransmissionPtr& transmission);

		virtual void
		onCopperFree();

		virtual void
		onCopperBusy();

		virtual void
		onCollision();
		//@}

		/**
		 * @name wns::service::phy::copper::Notification
		 */
		//@{
		virtual void
		setDLLUnicastAddress(const wns::service::dll::UnicastAddress& _macAddress);
		//@}

	private:
		/**
		 * @brief MAC Address of the higher layerx
		 */
		wns::service::dll::UnicastAddress macAddress;

		/**
		 * @brief Wire, this receiver is listening to
		 */
		WireInterface* wire;

		/**
		 * @brief Distribution of BER
		 */
		wns::distribution::Distribution* berDist;

		/**
		 * @brief Sensing time
		 *
		 * This delays the onCarrierIdle and onCarrierBusy calls to the
		 * CarrierSensing instance
		 */
		simTimeType sensingTime;

		/**
		 * @brief Logger
		 */
		wns::logger::Logger logger;
	};
}

#endif // NOT defined COPPER_RECEIVER_HPP


