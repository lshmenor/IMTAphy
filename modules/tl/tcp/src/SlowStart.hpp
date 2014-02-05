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

#ifndef TCP_SLOWSTART_HPP
#define TCP_SLOWSTART_HPP


#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <TCP/CongestionControlStrategy.hpp>


namespace tcp {

	class SlowStart :
		virtual public SlowStartStrategy
	{	

	public:
        explicit
		SlowStart(const wns::pyconfig::View& _pyco);

		virtual
		~SlowStart();

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

		/**
		 * @brief Reset counter for duplicate acks if timeout occurs
		 */
		virtual void
		clearDuplicateACKCounter();

		/**
		 * @brief Return the SlowStart threshold value
		 */
		unsigned long int
		getSlowStartThreshold();

		/**
		 * @brief Recalculation of SlowStart threshold
		 */
		void
		reCalcThreshold();

	private:
		std::string
		printReason(segmentLoss reason);

		const wns::pyconfig::View pyco;

		unsigned long int cwnd;

		/**
		 * @brief Value of SlowStart's threshold
		 */
		unsigned long int ssthresh;

		/**
		 * @brief SlowStart's logger
		 */
		wns::logger::Logger logger;

		/**
		 * @brief Value for timeout of retransmissions in seconds
		 */
		simTimeType timeout;

		/**
		 * @brief Return the maximum of two unsigned integers
		 */
		unsigned long int
		max(unsigned long int x, unsigned long int y);
	};
} // namespace 

#endif // NOT defined TCP_SLOWSTART_HPP

