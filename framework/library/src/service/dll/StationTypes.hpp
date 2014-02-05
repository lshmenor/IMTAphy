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

#ifndef WNS_SERVICE_DLL_STATIONTYPE_HPP
#define WNS_SERVICE_DLL_STATIONTYPE_HPP

#include <WNS/Enum.hpp>

namespace wns { namespace service { namespace dll {

	/** @brief To distinguish between different types of stations.
         *  There is the distinction of remote terminals (RUT)
	 *  but it supposed to be removed later on
         */
	ENUM_BEGIN(StationTypes);
	ENUM(AP,  1);
	ENUM(FRS, 2);
	ENUM(UT,  3);
	ENUM(RUT, 4);
    ENUM(eNB, 5);
    ENUM(UE, 6);
	ENUM_END();
	// wns::service::dll::StationTypes::FRS()

	typedef int StationType;

} // dll
} // service
} // wns

#endif // NOT defined WNS_SERVICE_DLL_STATIONTYPE_HPP



