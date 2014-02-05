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

#include <IP/iptables/PreroutingChain.hpp>

using namespace ip::iptables;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	PreroutingChain,
	wns::ldk::FunctionalUnit,
	"ip.iptables.preroutingchain",
	wns::ldk::FUNConfigCreator);


PreroutingChain::PreroutingChain(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _pyco):
	Chain(fun, _pyco),
	wns::Cloneable<PreroutingChain>(),
	fun(fun),
	log(_pyco.get("logger"))
{
}

void
PreroutingChain::onFUNCreated()
{
	assure(fun, "FUN not set!" );
	postroutingChain = fun->findFriend<ip::iptables::IRuleControl*>("postroutingChain");
	assure(postroutingChain, "PostroutingChain not found!");

	if(hasRules(99999))
	{
		dllFlowIDTarget = dynamic_cast<ip::iptables::targets::DLLFlowIDTarget*>(getRule(99999).getTarget());
		assure(dllFlowIDTarget, "Target not set");
		dllFlowIDTarget->setRuleController(postroutingChain);
	}
	Chain::onFUNCreated();
}
