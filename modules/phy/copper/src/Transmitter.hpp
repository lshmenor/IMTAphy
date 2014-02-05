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

#ifndef COPPER_TRANSMITTER_HPP
#define COPPER_TRANSMITTER_HPP

#include <WNS/service/phy/copper/DataTransmissionFeedback.hpp>
#include <WNS/service/phy/copper/DataTransmission.hpp>
#include <WNS/service/phy/copper/Handler.hpp>
#include <WNS/logger/Logger.hpp>

namespace copper
{
	class WireInterface;

	class TransmitterDataSentInterface
	{
	public:
		virtual
		~TransmitterDataSentInterface()
		{
		}

		virtual void
		onDataSent(
			wns::osi::PDUPtr pdu) = 0;
	};

	/**
	 * @brief Transmitter with configurable sensing time (detect wire free
	 * or busy).
	 *
	 * This transmitter can detect ongoing transmissions after a
	 * configurable amount time (sensing time). Setting this time to 0.0
	 * (the default) allows collision free operation.
	 */
	class Transmitter :
		virtual public wns::service::phy::copper::DataTransmission,
		virtual public wns::service::phy::copper::DataTransmissionFeedback,
		virtual public TransmitterDataSentInterface
	{
		typedef wns::service::phy::copper::DataTransmission
		Super;

		typedef wns::service::phy::copper::Handler
		Handler;
	public:
		/**
		 * @brief Constructor
		 */
		Transmitter(
			const wns::pyconfig::View& pyco,
			WireInterface* wire);

		/**
		 * @brief Destructor
		 */
		virtual
		~Transmitter(
			);

		/**
		 * @name wns::service::phy::copper::DataTransmission Interface
		 */
		//@{
		virtual void
		sendData(
			const wns::service::dll::BroadcastAddress& /*peerAddress*/,
			const wns::osi::PDUPtr& data);

		virtual void
		sendData(
			const wns::service::dll::UnicastAddress& peerAddress,
			const wns::osi::PDUPtr& data);

		virtual void
		cancelData(
			const wns::osi::PDUPtr& data);

		virtual bool
		isFree(
			) const;
		//@}

		virtual void
		onDataSent(
			wns::osi::PDUPtr pdu);

	private:
		/**
		 * @brief Calculate how long it takes to transmit "len" Bit
		 */
		simTimeType
		getDuration(
			Bit len);

		/**
		 * @brief Wire to be used for transmission
		 */
		WireInterface* wire;

		/**
		 * @brief The data rate the transmitter is able to use for tranmission
		 */
		double dataRate;

		/**
		 * @brief Time needed to detect the state of the wire (free or
		 * busy)
		 */
		simTimeType sensingTime;

		/**
		 * @brief Logger
		 */
		wns::logger::Logger logger;
	};
}

#endif // NOT defined COPPER_TRANSMITTER_HPP


