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

#include <WNS/Birthmark.hpp>
#include <WNS/Assure.hpp>

using namespace wns;


long long int Birthmark::globalId = 0;


Birthmark::Birthmark() :
	id(++globalId)
{
	// This will flip after 9.223.372.036.854.775.807 birthmarks
	assure(id > 0, "If you can see this message please send an eCard to: [msg|pab]@comnets.rwth-aachen.de");
}


Birthmark::~Birthmark()
{
}


std::string
Birthmark::doToString() const
{
	std::stringstream tmp;
	tmp << PythonicOutput::doToString();
	tmp << " with id: ";
	tmp << id;
	return tmp.str();
}


bool
Birthmark::operator ==(const Birthmark& other) const
{
	return id == other.id;
}


bool
Birthmark::operator !=(const Birthmark& other) const
{
	return !(*this == other);
}

bool
Birthmark::operator <(const Birthmark& other) const
{
	return (this->id < other.id);
}


HasBirthmark::HasBirthmark() :
	birthmark()
{
}


HasBirthmark::~HasBirthmark()
{
}


const Birthmark&
HasBirthmark::getBirthmark() const
{
	return birthmark;
}


void
HasBirthmark::setBirthmark(const Birthmark& other)
{
	birthmark = other;
}



