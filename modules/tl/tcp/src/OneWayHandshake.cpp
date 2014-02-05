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

#include <TCP/OneWayHandshake.hpp>
#include <TCP/Service.hpp>
#include <TCP/UpperConvergence.hpp>
#include <TCP/Component.hpp>
#include <TCP/HandshakeStrategyHandlerInterface.hpp>

#include <WNS/module/Base.hpp>

using namespace tcp;


STATIC_FACTORY_REGISTER_WITH_CREATOR(
	OneWayHandshake,
	wns::ldk::FunctionalUnit,
	"udp.OneWayHandshake",
	wns::ldk::FUNConfigCreator
	);

OneWayHandshake::OneWayHandshake(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& _config) :
	wns::ldk::CommandTypeSpecifier<>(_fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<OneWayHandshake>(),
	logger(_config.get("logger")),
	strategyHandler(NULL),
	config(_config),
	fun(_fun)
{
	MESSAGE_SINGLE(NORMAL, logger, "FU for 1-way-handshake created!");
}

OneWayHandshake::~OneWayHandshake()
{
}

void
OneWayHandshake::onFUNCreated()
{
}

void
OneWayHandshake::doOnData(const wns::ldk::CompoundPtr&)
{
}

void
OneWayHandshake::doSendData(const wns::ldk::CompoundPtr&)
{
}

bool
OneWayHandshake::doIsAccepting(const wns::ldk::CompoundPtr&)
{
	return true;
}

void
OneWayHandshake::doWakeup()
{
}

bool
OneWayHandshake::doIsAccepting(const wns::ldk::CompoundPtr&) const
{
	return true;
}

void
OneWayHandshake::activeOpen(const wns::service::tl::FlowID& _flowID)
{
	MESSAGE_SINGLE(NORMAL, logger, "OneWayHandshake::activeOpen() called for flowID " << _flowID);

	strategyHandler->connectionEstablished(_flowID);
}

void
OneWayHandshake::passiveOpen(const wns::service::tl::FlowID& _flowID)
{
	MESSAGE_SINGLE(NORMAL, logger, "OneWayHandshake::passiveOpen() called for flowID " << _flowID);

	strategyHandler->connectionEstablished(_flowID);
}

void
OneWayHandshake::registerStrategyHandler(HandshakeStrategyHandlerInterface* _strategyHandler)
{
	strategyHandler = _strategyHandler;
}

void
OneWayHandshake::closeConnection(const wns::service::tl::FlowID& _flowID)
{
	strategyHandler->connectionClosed(_flowID);
}
