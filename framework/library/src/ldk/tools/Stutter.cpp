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

#include <WNS/ldk/tools/Stutter.hpp>

#include <WNS/ldk/fun/FUN.hpp>

using namespace wns::ldk::tools;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Stutter, wns::ldk::FunctionalUnit, "wns.tools.MMMyGGGeneration", wns::ldk::FUNConfigCreator);
Stutter::Stutter(fun::FUN* fuNet, const pyconfig::View& _config) :
		CommandTypeSpecifier<>(fuNet),
		HasReceptor<>(),
		HasConnector<>(),
		HasDeliverer<>(),
		Forwarding<Stutter>(),
		Cloneable<Stutter>(),
		events::PeriodicTimeout(),

		close(true),
		logger("WNS", "Stutter"),
		config(_config)
{
	double interval = config.get<double>("interval");

	if(interval != 0.0)
		startPeriodicTimeout(interval, interval);
} // Stutter


void
Stutter::onFUNCreated()
{
	friends.flow = getFUN()->findFriend<command::FlowControl*>(config.get<std::string>("flowControl"));
} // onFUNCreated


void
Stutter::periodically()
{
	if(close) {
		MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
		m << " closing";
		MESSAGE_END();

		friends.flow->close();
	} else {
		MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
		m << " opening";
		MESSAGE_END();

		friends.flow->open();
	}

	close = !close;
} // periodically



