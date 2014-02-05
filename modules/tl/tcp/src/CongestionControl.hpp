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

#ifndef TCP_CONGESTIONCONTROL_HPP
#define TCP_CONGESTIONCONTROL_HPP

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <TCP/CongestionControlStrategy.hpp>


namespace tcp { 

	class SlowStart;

	namespace tests {
		class CongestionControlTest;
		class CumulativeACKTest;
	} // namespace tests

	class CongestionControl :
		virtual public CongestionControlStrategy
	{	
		friend class tests::CongestionControlTest;
		friend class tests::CumulativeACKTest;

	public:
        explicit
		CongestionControl(const wns::pyconfig::View& _pyco);

		virtual
		~CongestionControl();

		/**
		 * @brief Implementation of the CongestionControlStrategy
		 * interface
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

		virtual bool
		duplicateACKThresholdReached(unsigned long int _ackNR);

		virtual void
		clearDuplicateACKCounter();

		CongestionControlStrategy*
		getCongestionControlMode();

	private:
		std::string
		printMode(Mode _mode);

		wns::pyconfig::View pyco;

		/**
		 * @brief The current mode of congestion control
		 */
		Mode mode;

		/**
		 * @brief The SlowStart instance
		 */
		SlowStartStrategy* slowStart;

		/**
		 * @brief Currently active CongestionAvoidance algorithm
		 */
		CongestionAvoidanceStrategy* ca;

		/**
		 * @brief CongestionControl's logger
		 */
		wns::logger::Logger logger;

		virtual void
		setWindowSize(unsigned long int new_cwnd);

		/**
		 * @brief Changing between SlowStart mode and
		 * CongestionAvoidance mode
		 */
		void
		setCongestionControlMode(Mode _mode);

		/**
		 * @brief Returns the SlowStart threshold value
		 */
		unsigned long int
		getSlowStartThreshold();
	};
} // namespace tcp

#endif // NOT defined TCP_CONGESTIONCONTROL_HPP
