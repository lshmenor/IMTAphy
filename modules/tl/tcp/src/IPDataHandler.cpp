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

#include <WNS/ldk/Compound.hpp>

#include <TCP/IPDataHandler.hpp>
#include <TCP/UpperConvergence.hpp>

using namespace tcp;

IPDataHandler::IPDataHandler() :
	lowerConvergence(NULL)
{
	friends.uc = NULL;
}

void
IPDataHandler::onData(
	wns::service::nl::Address /*_sourceIP*/,
	const wns::osi::PDUPtr& _pdu)
{
	assure(_pdu, "Invalid PDU (NULL)");


	lowerConvergence->onData(wns::dynamicCast<wns::ldk::Compound>(_pdu));
}

void
IPDataHandler::setLowerConvergence(wns::ldk::FunctionalUnit* _lowerConvergence)
{
	assure(!lowerConvergence, "LowerConvergence already set!");
	lowerConvergence = _lowerConvergence;
}

wns::ldk::FunctionalUnit*
IPDataHandler::getLowerConvergence() const
{
	assure(lowerConvergence, "LowerConvergence not set!");
	return lowerConvergence;
}
