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

#include <WNS/Direction.hpp>

using namespace wns;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Direction::Direction()
	: elevation(0),
	  azimuth(0)
{}

Direction::Direction(double _elevation, double _azimuth)
	: elevation(_elevation),
	  azimuth(_azimuth)
{}

double Direction::getElevation() const
{
	return elevation;
}

double Direction::getAzimuth() const
{
	return azimuth;
}

Direction Direction::calcAngles(const PositionOffset& positionOffset) const
{
 	Direction d(fabs(positionOffset.getElevation() - elevation),
		    positionOffset.getAzimuth() - azimuth);
	while(d.azimuth < 0) {
		d.azimuth += 2*M_PI;
	}
	return d;
}


