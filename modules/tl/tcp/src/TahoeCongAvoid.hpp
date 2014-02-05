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

#ifndef TCP_TAHOECONGAVOID_HPP
#define TCP_TAHOECONGAVIOD_HPP

#include <WNS/logger/Logger.hpp>
#include <WNS/pyconfig/View.hpp>

#include <WNS/container/Registry.hpp>

#include <TCP/CongestionControlStrategy.hpp>


namespace tcp {

	class TahoeCongAvoid :
		virtual public CongestionAvoidanceStrategy
	{

	public:
		TahoeCongAvoid(const wns::pyconfig::View& _pyco);

		virtual
		~TahoeCongAvoid();

		/**
		 * @brief Implementation of the CongestionAvoidanceStrategy interface
		 */
		virtual void
		onSegmentLoss(segmentLoss _sl, unsigned long int _ackNR);

		virtual void
		onRTTSample();

		virtual unsigned long int
		getWindowSize();

		virtual simTimeType
		getRetransmissionTimeout();

		virtual void
		onSegmentAcknowledged();

		virtual void
		setWindowSize(unsigned long int new_cwnd);

		virtual bool
		duplicateACKThresholdReached(unsigned long int _ackNR);

		virtual void
		clearDuplicateACKCounter();

	private:
		std::string
		printReason(segmentLoss reason);

		wns::pyconfig::View pyco;

		/**
		 * @brief The size of the congestion window
		 */
		unsigned long int cwnd;

		/**
		 * @brief Counter to increase congestion window
		 * cwnd linear
		 */
		unsigned long int cwnd_cnt;

		/**
		 * @brief Tahoe's logger
		 */
		wns::logger::Logger logger;

		/**
		 * @brief Value for retransmissions in seconds
		 */
		simTimeType timeout;

		/**
		 * @brief The initial threshold of duplicate acks
		 */
		int ndup;

		/**
		 * @brief Registry for counting the number of duplicate acks of
		 * regarding the same acknowledgement number
		 */
		wns::container::Registry<unsigned long int, int> countDuplicateACKs;
	};
} // namespace tcp


#endif // NOT defined TCP_TAHOECONGAVOID_HPP
