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

#include <WNS/ldk/MultiLink.hpp>



using namespace wns::ldk;



void
MultiLink::add(FunctionalUnit* fu)
{
	fus_.push_back(fu);
} // add



void
MultiLink::clear()
{
	fus_.clear();
} // clear



unsigned long int
MultiLink::size() const
{
	return fus_.size();
} // size



const Link::ExchangeContainer
MultiLink::get() const
{
	Link::ExchangeContainer result;

	for(Link::ExchangeContainer::const_iterator it = fus_.begin();
		it != fus_.end();
		++it)
	{
		result.push_back(*it);
	}

	return result;
} // get



void
MultiLink::set(const Link::ExchangeContainer& src)
{
	fus_.clear();

	for(Link::ExchangeContainer::const_iterator it = src.begin();
		it != src.end();
		++it)
	{
		fus_.push_back(*it);
	}
} // set




