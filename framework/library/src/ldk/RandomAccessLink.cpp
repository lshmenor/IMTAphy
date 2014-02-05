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

#include <WNS/ldk/RandomAccessLink.hpp>

using namespace wns::ldk;

RandomAccessLink::RandomAccessLink() :
	fus()
{
}

void
RandomAccessLink::add(FunctionalUnit* fu)
{
	assureNotNull(fu);
	fus.push_back(fu);
}


void
RandomAccessLink::clear()
{
	fus.clear();
}


unsigned long int
RandomAccessLink::size() const
{
	return fus.size();
}


const Link::ExchangeContainer
RandomAccessLink::get() const
{
	Link::ExchangeContainer result;

	for(Link::ExchangeContainer::const_iterator it = fus.begin();
		it != fus.end();
		++it) {
		assureNotNull(*it);
		result.push_back(*it);
	}

	return result;
}


void
RandomAccessLink::set(const Link::ExchangeContainer& src)
{
	fus.clear();

	for(Link::ExchangeContainer::const_iterator it = src.begin();
		it != src.end();
		++it) {
		assureNotNull(*it);
		fus.push_back(*it);
	}
}


