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

#ifndef _TTOS_HPP
#define _TTOS_HPP

#include <string>
#include <sstream>

namespace wns
{
    /** @brief Convert values to string.
     *
     *  @param value This will be converted to a string.
     *
     *  @c Ttos converts a given value of any type to a string, if
     *  it can be added to a stringstream using "<<".
     */
    template<typename T>
    std::string Ttos(T value)
    {
	std::ostringstream temp;
	temp << value;
	return temp.str();
    }
}

#endif // _TTOS_HPP
