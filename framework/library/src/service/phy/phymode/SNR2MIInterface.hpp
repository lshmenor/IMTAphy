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

#ifndef WNS_SERVICE_PHY_PHYMODE_SNR2MI_HPP
#define WNS_SERVICE_PHY_PHYMODE_SNR2MI_HPP

#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <WNS/PythonicOutput.hpp>
#include <WNS/PowerRatio.hpp>

namespace wns { namespace service { namespace phy { namespace phymode {

	/**
		@brief Base Class for mapping SINR to MI (yes, there are several ways)
	*/
	class SNR2MIInterface :
		virtual public wns::PythonicOutput
	{
	public:
		virtual ~SNR2MIInterface() {};

		/** @brief returns the Mutual Information value per Bit (MIB) */
	    virtual double convertSNR2MIB(const wns::Ratio& snr, const wns::service::phy::phymode::PhyModeInterface& phyMode) const = 0;

		/** @brief returns the Mutual Information value of a modulation symbol */
	    virtual double convertSNR2MI(const wns::Ratio& snr, const wns::service::phy::phymode::PhyModeInterface& phyMode) const = 0;

        /** @brief returns the effective SINR from MIB */
        virtual wns::Ratio
        convertMIB2SNR(const double& MI, const wns::service::phy::phymode::Modulation& mod) const = 0;

		static SNR2MIInterface*
		getSNR2MImapper(const wns::pyconfig::View& config);
	};

	typedef wns::Creator<SNR2MIInterface> SNR2MICreator;

} // phymode
} // phy
} // service
} // wns

#endif // WNS_SERVICE_PHY_PHYMODE_PHYMODEINTERFACE_HPP
