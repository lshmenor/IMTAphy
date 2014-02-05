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

#include <WNS/logger/Message.hpp>

using namespace wns::logger;

Message::Message() :
	message(),
	level(0)
{}

Message::Message(const std::string& m) :
	message(),
	level(0)
{
	message << m;
}

Message::Message(const std::string& m, long int _level) :
	message(),
	level(_level)
{
	message << m;
}

Message::Message(const Message& m) :
	message(),
	level(m.level)
{
	message << m.message.str();
}

std::string
Message::getString() const
{
	return message.str();
}

long int
Message::getLevel() const
{
	return level;
}


