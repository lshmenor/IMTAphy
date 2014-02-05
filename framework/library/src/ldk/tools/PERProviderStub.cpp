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

#include <WNS/ldk/tools/PERProviderStub.hpp>

#include <cstdlib>
#include <functional>

using namespace wns::ldk;
using namespace wns::ldk::tools;

STATIC_FACTORY_REGISTER_WITH_CREATOR(PERProviderStub, FunctionalUnit, "wns.tools.PERProviderStub", FUNConfigCreator);


PERProviderStub::PERProviderStub(fun::FUN* fuNet, const pyconfig::View& config) :
	StubBase(),
	CommandTypeSpecifier<PERProviderPCI>(fuNet),
	Cloneable<PERProviderStub>(),

	fixedPER(config.get<double>("fixedPER"))
{
	this->setName("perProviderStub");
} // PERProviderStub


void
PERProviderStub::doOnData(const CompoundPtr& compound)
{
	assure(compound != CompoundPtr(), "Not a valid PDU.");

	received.push_back(compound);

	getCommand(compound->getCommandPool())->setPER(fixedPER);


	if(getReceptor()->size()) {
		getDeliverer()->getAcceptor(compound)->onData(compound);
	}

	assure(integrityCheck(), "Integrity check failed.");
} // doOnData


