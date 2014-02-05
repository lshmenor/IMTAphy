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

#include <WNS/ldk/SingleLink.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>

using namespace wns::ldk;

SingleLink::SingleLink() :
	fu(NULL)
{
}

SingleLink::~SingleLink()
{
	fu = NULL;
}

void
SingleLink::add(FunctionalUnit* it)
{
	assure(
		this->fu == NULL,
		"Can't add " + it->toString() + ", already got " + this->fu->toString());

	assureNotNull(it);

	this->fu = it;
}


void
SingleLink::clear()
{
	this->fu = NULL;
}


unsigned long int
SingleLink::size() const
{
	return this->fu == NULL ? 0 : 1;
}


const Link::ExchangeContainer
SingleLink::get() const
{
	Link::ExchangeContainer container;
	if(this->fu != NULL)
	{
		container.push_back(this->fu);
	}
	return container;
}


void
SingleLink::set(const Link::ExchangeContainer& src)
{
	assure(src.size() < 2, "SingleLink takes one FU at most");

	this->fu = NULL;
	if (!src.empty())
	{
		this->fu = src.front();
		assureNotNull(this->fu);
	}
}


