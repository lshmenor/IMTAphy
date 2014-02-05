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

#ifndef WNS_SERVICE_PHY_POWER_MEASUREMENT_HPP
#define WNS_SERVICE_PHY_POWER_MEASUREMENT_HPP

#include <WNS/node/Interface.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/PythonicOutput.hpp>
#include <WNS/RefCountable.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/SmartPtr.hpp>

#include <vector>

namespace wns { namespace service { namespace phy { namespace power {

    /**
	 * @brief Measurement class for power values (S,I,N,SINR)
	 * These are only basic, scalar measurements without much calculation effort.
	 * Some of the methods calculate 'on demand', so no effort if they are not used
	 * An implementation (non-abstract) is located in RISE or OFDMAPhy.
	 */
    class PowerMeasurementInterface:
        public virtual wns::PythonicOutput,
        public virtual wns::RefCountable,
        public virtual wns::CloneableInterface
    {
    public:
        virtual
        ~PowerMeasurementInterface() {};

        /** @brief get (averaged) received power for the received PDU */
        virtual const wns::Power
        getRxPower() const = 0;

        /** @brief get (averaged) received interference+noise power for the received PDU */
        virtual const wns::Power
        getInterferencePower() const = 0; // interference contains noise here

        /** @brief get (averaged) received interference+noise power for an assumed omnidirectional antenna */
        virtual const wns::Power
        getOmniInterferencePower() const = 0;

        /**
         * @brief get (averaged) Interference of Thermal Noise Ratio
         */
        virtual const wns::Ratio
        getIoT() const = 0;

        /** @brief get SINR for the received PDU */
        virtual const wns::Ratio
        getSINR() const = 0;

        /** @brief Factors for every stream after the MIMO-processing */
        virtual const std::vector<wns::Ratio>
        getPostProcessingSINRFactor() const = 0;

        /** @brief get  mutual information (MI) for the received PDU */
        virtual const double
        getMI()  const = 0;

        /** @brief get  mutual information per bit (MIB) [0..1] for the received PDU */
        virtual const double
        getMIB() const = 0;

        /** @brief get transmit power with which PDU has been sent */
        virtual const wns::Power
        getTxPower() const = 0;

        /** @brief get path loss from transmitter to receiver (does not include fading) */
        virtual const wns::Ratio
        getPathLoss() const = 0;

        /** @brief get total loss from transmitter to receiver (including fading) */
        virtual const wns::Ratio
        getLoss() const = 0;

        /** @brief dynamic fading for example FTFading (pathloss - fading = loss) */
        virtual const wns::Ratio
        getFading() const = 0;

        /** @brief get distance between sender and receiver */
        virtual const double
        getDistance() const = 0;

        /** @brief get received signal strength (S+I) */
        virtual const wns::Power
        getRSS() const = 0;

        /** @brief get PhyMode with which PDU has been sent */
        virtual const wns::service::phy::phymode::PhyModeInterfacePtr
        getPhyMode() const = 0;

        /** @brief get transmitter node */
        virtual wns::node::Interface*
        getSourceNode() const = 0;

        /** @brief method to get a verbose output (string) */
        virtual std::string
        getString() const = 0;
    };

    /** @brief when a pointer to a PowerMeasurement is needed, use this one
	    (SmartPtr) which takes care of memory deallocation itself. */
    typedef wns::SmartPtr<const wns::service::phy::power::PowerMeasurementInterface> PowerMeasurementPtr;

    /** @brief a const reference to the PowerMeasurement can be used anywhere. */
    typedef const wns::service::phy::power::PowerMeasurementInterface& PowerMeasurementConstRef;

    /** @brief define stream operator for class PowerMeasurementInterface */
    inline std::ostream&
    operator<< (std::ostream& s, const wns::service::phy::power::PowerMeasurementInterface& p) {
      return s << p.getString();
    }

} // power
} // phy
} // service
} // wns
#endif // WNS_SERVICE_PHY_POWER_MEASUREMENT_HPP



