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

#include <WNS/ldk/tools/Gate.hpp>

using namespace wns::ldk;
using namespace wns::ldk::tools;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Gate, FunctionalUnit, "wns.tools.Gate", FUNConfigCreator);

Gate::Gate(fun::FUN* fuNet, const wns::pyconfig::View& config) :
		CommandTypeSpecifier<>(fuNet),
		HasReceptor<>(),
		HasConnector<>(),
		HasDeliverer<>(),
		Cloneable<Gate>(),

		logger("WNS", config.get<std::string>("name")),

		incomingState(OPEN),
		outgoingState(OPEN)
{
} // Gate


void
Gate::setIncomingState(State state)
{
	incomingState = state;
} // setIncomingState


void
Gate::setOutgoingState(State state)
{
	outgoingState = state;

	wakeup();
} // setOutgoingState


bool
Gate::doIsAccepting(const CompoundPtr& compound) const
{
	if(outgoingState == CLOSED) {
		MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
		m << " isAccepting -> false";
		MESSAGE_END();

		return false;
	}

	return getConnector()->hasAcceptor(compound);
} // isAccepting


void
Gate::doSendData(const CompoundPtr& compound)
{
	assure(isAccepting(compound), "sendData called although the FunctionalUnit is not accepting!");

	activateCommand(compound->getCommandPool());

	getConnector()->getAcceptor(compound)->sendData(compound);
} // doSendData


void
Gate::doOnData(const CompoundPtr& compound)
{
	if(incomingState == CLOSED) {
		MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
		m << " dropping incoming PDU";
		MESSAGE_END();
	} else {
		getDeliverer()->getAcceptor(compound)->onData(compound);
	}
} // doOnData


void
Gate::doWakeup()
{
	if(outgoingState == CLOSED)
		return;

	getReceptor()->wakeup();
} // wakeup



