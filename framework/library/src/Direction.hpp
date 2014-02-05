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


#ifndef _DIRECTION_HPP
#define _DIRECTION_HPP

#include <WNS/Position.hpp>

namespace wns {
	/**
	 * @brief Determin the direction from a PositionOffset
	 * @ingroup MISC
	 */
	class Direction
	{
	public:
		/**
		 * @brief Default Constructor
		 *
		 * Azimuth and elevation are set to 0
		 */
		Direction();

		/**
		 * @brief Constructor to set Azimuth and elevation
		 */
		Direction(double _elevation, double _azimuth);

		/**
		 * @brief Calculate angles theta and phi from PostionOffset
		 */
		Direction calcAngles(const PositionOffset& positionOffset) const;

		/**
		 * @brief Return elevation
		 */
		double getElevation() const;

		/**
		 * @brief Return azimuth
		 */
		double getAzimuth() const;

	private:
		/**
		 * @brief The elevation 0..Pi
		 */
		double elevation;

		/**
		 * @brief The azimuth 0..2Pi
		 */
		double azimuth;
	};
}
#endif //_DIRECTION_HPP

/**
 * @file
 */


