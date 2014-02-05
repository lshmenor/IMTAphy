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

#include <WNS/service/dll/Address.hpp>
#include <WNS/Exception.hpp>

using namespace wns::service::dll;

const long int UnicastAddress::invalid = -1;

UnicastAddress::UnicastAddress() :
	address(invalid)
{
}

UnicastAddress::UnicastAddress(long int _address) :
	address(_address)
{
	if(!(this->address > 0)) {
		wns::Exception e;
		e << "Address must be greater than 0. Was: " << address;
		throw e;
	}
}

UnicastAddress::~UnicastAddress()
{
}

bool
UnicastAddress::operator <(const UnicastAddress& other) const
{
	return this->address < other.address;
}

bool
UnicastAddress::operator ==(const UnicastAddress& other) const
{
	return this->address == other.address;
}

bool
UnicastAddress::operator !=(const UnicastAddress& other) const
{
	return this->address != other.address;
}

bool
UnicastAddress::isValid() const
{
	return this->address != invalid;
}

long int
UnicastAddress::getInteger() const
{
	return this->address;
}


