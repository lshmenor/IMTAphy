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
// #include <WNS/service/phy/power/OFDMAMeasurement.hpp>

#ifndef WNS_SERVICE_PHY_OFDMA_MEASUREMENT_HPP
#define WNS_SERVICE_PHY_OFDMA_MEASUREMENT_HPP

#include <WNS/node/Interface.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/PythonicOutput.hpp>
#include <WNS/RefCountable.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/SmartPtr.hpp>

namespace wns { namespace service { namespace phy { namespace power {

	/**
	 * @brief Measurement Class for vectorized values depending on the OFDMA subchannel
	 * An implementation (non-abstract) is located in OFDMAPhy
	 */
	class OFDMAMeasurement:
		public virtual wns::PythonicOutput, // operator<< defined? How?
		public virtual wns::RefCountable,
		public virtual wns::CloneableInterface
	{
	public:
		virtual ~OFDMAMeasurement() {}
		// promised interfaces
		// various ways to get the information
		/** @brief use this method if you only need the values of a few subChannels */
		virtual const wns::Ratio getPathLoss(int subChannel) const = 0;
		/** @brief use this method if you need all values for all subChannels in future */
		//virtual const wns::Ratio getPathLossInFuture(int subChannel, int samplingTimeOffset=0) const = 0;
		/** @brief use this method if you need all values for all subChannels */
		virtual const std::vector<wns::Ratio> getPathLoss() const = 0;
		/** @brief use this method if you need all values for all subChannels
		 * The integer argument can look into the future
		 * (for having a reference with optimum results) */
		//virtual const std::vector<wns::Ratio> getPathLossInFuture(int samplingTimeOffset=0) const = 0;
		/** @brief use this method if you only need the values of a few subChannels */
		virtual const wns::Power getInterferencePlusNoise(int subChannel) const = 0;
		/** @brief use this method if you need all values for all subChannels */
		virtual const std::vector<wns::Power>& getInterferencePlusNoise() const = 0;

		/** @brief something to specify the origin (source node, transmitter) */
		virtual wns::node::Interface* getSourceNode() const = 0;
		// on layer2 you can translate this to the MAC address:
		// stationManager = fun->getLayer<dll::ILayer2*>()->getStationManager();
		// wns::service::dll::UnicastAddress source = stationManager->getStationByNode(source)->getDLLAddress();

		/** @brief number of OFDM subchannels */
		virtual const int getNumberOfSubChannels() const = 0;
		/** @brief at what time has the measurement been taken */
		virtual const simTimeType getMeasurementTime() const = 0;
		/** @brief if you are unsure if this measurement is the current, ask here */
		virtual const bool isUpToDate() const = 0;
		/** @brief method to get a verbose output (string) */
		virtual std::string getString() const = 0;
	};
	/** @brief when a pointer to a OFDMAMeasurement is needed, use this one
	    (SmartPtr) which takes care of memory deallocation itself. */
	typedef wns::SmartPtr<const wns::service::phy::power::OFDMAMeasurement> OFDMAMeasurementPtr;
	/** @brief a const reference to the OFDMAMeasurement can be used anywhere. */
	typedef const wns::service::phy::power::OFDMAMeasurement& OFDMAMeasurementConstRef;

	/** @brief define stream operator for class PowerMeasurementInterface */
	inline std::ostream&
	operator<< (std::ostream& s, const wns::service::phy::power::OFDMAMeasurement& p) {
		return s << p.getString();
	}

} // power
} // phy
} // service
} // wns
#endif // WNS_SERVICE_PHY_OFDMA_MEASUREMENT_HPP



