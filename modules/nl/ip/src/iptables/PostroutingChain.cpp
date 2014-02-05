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

#include <IP/iptables/PostroutingChain.hpp>

using namespace ip::iptables;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	PostroutingChain,
	wns::ldk::FunctionalUnit,
	"ip.iptables.postroutingchain",
	wns::ldk::FUNConfigCreator);


PostroutingChain::PostroutingChain(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _pyco):
	Chain(fun, _pyco),
	wns::Cloneable<PostroutingChain>(),
	dllFlowIDTarget(NULL)
{
}

void
PostroutingChain::onFlowRemoved(wns::service::dll::FlowID flowID)
{
	if(hasRules(flowID))
	{
		removeRules(flowID);
	    removeFromDLLFlowIDTarget(flowID);
	}
	else
	{
		//rules are created when a packet with its Ruletag (flowID) arrive.
		//if no packet has ever arrived with this flowID there will be no Rule
		//with this RUletag (flowID)!!

		MESSAGE_BEGIN(NORMAL, log, m, "");
		m << "Warning! There are no Rules with this RuleTag: " << flowID;
		MESSAGE_END();
	}
}

void
PostroutingChain::registerDLLFlowIDTarget(targets::DLLFlowIDTarget* target)
{
	dllFlowIDTarget = target;
	assure(dllFlowIDTarget, "dllFlowIDTarget not set.");
}

void
PostroutingChain::removeFromDLLFlowIDTarget(wns::service::dll::FlowID flowID)
{
	dllFlowIDTarget->ruleErased(flowID);
}
