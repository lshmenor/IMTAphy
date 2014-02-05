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


#include <WNS/Positionable.hpp>
#include <WNS/PositionObserver.hpp>
#include <WNS/Assure.hpp>

#include <functional>

using namespace wns;

Positionable::Positionable() :
	position()
{
}

Positionable::Positionable(const Positionable& other) :
	SubjectType::SubjectType(other),
	PositionableInterface(other),
	SubjectType(other),
	position(other.position)
{
}

Positionable::Positionable(const Position& p) :
	position(p)
{
}

Positionable::Positionable(const wns::pyconfig::View& positionView) :
	position()
{
	position = Position(positionView.get<double>("x"),
			    positionView.get<double>("y"),
			    positionView.get<double>("z"));
}

Positionable::~Positionable()
{
}

const wns::Position&
Positionable::getPosition() const
{
	return position;
}

double
Positionable::getDistance(PositionableInterface* p)
{
	assure(p, "No valid object provided!");
	return (p->getPosition() - this->getPosition()).abs();
}

double
Positionable::getDistance(const PositionableInterface& p)
{
	return (p.getPosition() - this->getPosition()).abs();
}

double
Positionable::getAngle(const PositionableInterface &p)
{
    return(p.getPosition() - this->getPosition()).getAzimuth();
}


void
Positionable::setPosition(const wns::Position& p)
{
	this->sendNotifies(&PositionObserver::positionWillChange);
	position = p;
	this->sendNotifies(&PositionObserver::positionChanged);
}



