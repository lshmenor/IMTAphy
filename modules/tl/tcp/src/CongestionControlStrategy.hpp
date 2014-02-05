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

#ifndef TCP_CONGESTIONCONTROLSTRATEGY_HPP
#define TCP_CONGESTIONCONTROLSTRATEGY_HPP


#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>

namespace tcp { 

	namespace tests {
		class CongestionControlTest;
		class CumulativeACKTest;
	} // namespace tests

	class CongestionControlStrategy
	{	
		friend class CongestionControl;
		friend class tests::CongestionControlTest;
		friend class tests::CumulativeACKTest;

	public:
		/**
		 * @brief Reasons for a segement loss
		 */
		enum segmentLoss {TIMEOUT, DUPLICATE_ACK};

		virtual
		~CongestionControlStrategy() {}

		virtual void
		onSegmentLoss(segmentLoss _sl, unsigned long int _ackNR) = 0;

		virtual void
		onRTTSample() = 0;

		virtual unsigned long int
		getWindowSize() = 0;

		virtual simTimeType
		getRetransmissionTimeout() = 0;

		virtual void
		onSegmentAcknowledged() = 0;

		virtual bool
		duplicateACKThresholdReached(unsigned long int _ackNR) = 0;

		virtual void
		clearDuplicateACKCounter() = 0;

	protected:
		virtual void
		setWindowSize(unsigned long int new_cwnd) = 0;

		/**
		 * @brief CongestionControl is either in SlowStart mode
		 * or CongestionAvoidance mode
		 */
		enum Mode {SLOWSTART, CONGESTION_AVOIDANCE};
	};

	typedef CongestionControlStrategy SlowStartStrategy;
	typedef CongestionControlStrategy CongestionAvoidanceStrategy;

	typedef wns::PyConfigViewCreator<SlowStartStrategy> SlowStartCreator;
	typedef wns::StaticFactory<SlowStartCreator> SlowStartFactory;

	typedef wns::PyConfigViewCreator<CongestionAvoidanceStrategy> CongestionAvoidanceCreator;
	typedef wns::StaticFactory<CongestionAvoidanceCreator> CongestionAvoidanceFactory;

} // namespace tcp

#endif // NOT defined TCP_CONGESTIONCONTROLSTRATEGY_HPP
