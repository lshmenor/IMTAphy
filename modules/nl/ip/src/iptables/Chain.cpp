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

#include <IP/iptables/Chain.hpp>
#include <IP/iptables/filters/FilterInterface.hpp>
#include <IP/iptables/targets/TargetInterface.hpp>
#include <IP/IPHeader.hpp>

using namespace ip::iptables;

Chain::Chain(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _pyco) :
	wns::ldk::CommandTypeSpecifier<>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	log(_pyco.get("logger")),
	ipHeaderReader(NULL)
{
	int numRules = _pyco.len("rules");
	for (int i = 0;
		 i < numRules;
		 ++i)
	{
		wns::pyconfig::View rc = _pyco.get("rules", i);

		std::string filtername = rc.get<std::string>("filter.__plugin__");
		std::string targetname = rc.get<std::string>("target.__plugin__");
		ip::iptables::Rule::RuleTag ruleTag = rc.get<ip::iptables::Rule::RuleTag>("ruleTag");
		ip::iptables::Rule rule = Rule(
			filters::FilterInterfaceFactory::creator(filtername)->create(rc.get("filter")),
			targets::TargetInterfaceFactory::creator(targetname)->create(rc.get("target")),
			ruleTag);

		addRule(rule);
	}

}

Chain::~Chain()
{
	while (!rules.empty())
	{
		Rule rule = *rules.begin();
		rules.pop_front();
	}
}

void
Chain::onFUNCreated()
{
 	ipHeaderReader = getFUN()->getCommandReader("ip.ipHeader");
	assure(ipHeaderReader, "No reader for the IP Header available!");
} // onFUNCreated

bool
Chain::doIsAccepting(const wns::ldk::CompoundPtr& compound) const
{
	return getConnector()->hasAcceptor(compound);
}

void
Chain::doSendData(const wns::ldk::CompoundPtr& compound)
{
	//debugging stuff:
	showRules();
	//:debugging stuff

	assure(ipHeaderReader, "No reader for the IP Header available!");
    IPCommand* ipHeader = ipHeaderReader->readCommand<IPCommand>(compound->getCommandPool());
    wns::service::tl::ITCPHeader* tcpHeader = NULL;
    wns::service::tl::IUDPHeader* udpHeader = NULL;
    wns::ldk::Compound* sdu = dynamic_cast<wns::ldk::Compound*>(compound->getUserData());

    if (sdu != NULL)
    {
        if (ipHeader->peer.protocol == wns::service::nl::TCP)
        {
            tcpHeader = getFUN()->getProxy()->
                getCommand<wns::service::tl::ITCPHeader>(sdu->getCommandPool(), "tcp.tcpHeader");
        }

        if (ipHeader->peer.protocol == wns::service::nl::UDP)
        {
            udpHeader = getFUN()->getProxy()->
                getCommand<wns::service::tl::IUDPHeader>(sdu->getCommandPool(), "tcp.tcpHeader");
        }
    }

	MESSAGE_SINGLE(VERBOSE, log, "Asking for outgoing activation");

	targets::TargetResult acceptPacket = targets::ACCEPT;
	if (activateOutgoing(*ipHeader))
	{
        acceptPacket = activateChain(ipHeader, tcpHeader, udpHeader);
    }

	if (acceptPacket == targets::ACCEPT || acceptPacket == targets::CONT)
	{
		// Forward to lower FU.
		getConnector()->getAcceptor(compound)->sendData(compound);
	}
}

void
Chain::doWakeup()
{
	getReceptor()->wakeup();
}

void
Chain::doOnData(const wns::ldk::CompoundPtr& compound)
{
    IPCommand* ipHeader = ipHeaderReader->readCommand<IPCommand>(compound->getCommandPool());
    wns::service::tl::ITCPHeader* tcpHeader = NULL;
    wns::service::tl::IUDPHeader* udpHeader = NULL;
    wns::ldk::Compound* sdu = dynamic_cast<wns::ldk::Compound*>(compound->getUserData());

    if (sdu != NULL)
    {
        if (ipHeader->peer.protocol == wns::service::nl::TCP)
        {
            tcpHeader = getFUN()->getProxy()->
                getCommand<wns::service::tl::ITCPHeader>(sdu->getCommandPool(), "tcp.tcpHeader");
        }

        if (ipHeader->peer.protocol == wns::service::nl::UDP)
        {
            udpHeader = getFUN()->getProxy()->
                getCommand<wns::service::tl::IUDPHeader>(sdu->getCommandPool(), "tcp.tcpHeader");
        }
    }

    MESSAGE_SINGLE(VERBOSE, log, "Asking for incoming activation");

	targets::TargetResult acceptPacket = targets::ACCEPT;
	if (activateIncoming(*ipHeader))
	{
        acceptPacket = activateChain(ipHeader, tcpHeader, udpHeader);
	}

	if (acceptPacket == targets::ACCEPT || acceptPacket == targets::CONT)
	{
		// Hand compound to upper FU
		if(getDeliverer()->size())
			getDeliverer()->getAcceptor(compound)->onData(compound);
	}
}

targets::TargetResult
Chain::activateChain(IPCommand* ipHeader, wns::service::tl::ITCPHeader* tcpHeader, wns::service::tl::IUDPHeader* udpHeader)
{
	for(RuleContainer::iterator it = rules.begin();
		it != rules.end();
		++it)
	{
            targets::TargetResult r;
            if (tcpHeader != NULL && it->getFilter()->fires(ipHeader, tcpHeader))
            {
                r = it->getTarget()->mangle(ipHeader, tcpHeader);
            }
            else if (udpHeader != NULL && it->getFilter()->fires(ipHeader, udpHeader))
            {
                r = it->getTarget()->mangle(ipHeader, udpHeader);
            }
            else
            {
                r = it->getTarget()->mangle(ipHeader);
            }

			switch(r)
			{
			case targets::ACCEPT:
				return targets::ACCEPT;
				break;

			case targets::DROP:
				return targets::DROP;
				break;

			case targets::CONT:
				continue;
				break;
			default:
				MESSAGE_SINGLE(NORMAL, log, "Unknown Thang!");
			};
	}
	return targets::ACCEPT;
}

void
Chain::addRule(ip::iptables::Rule rule)
{
	rules.insert(rules.end(), rule);

	MESSAGE_BEGIN(NORMAL, log, m, "");
	m << "New Rule added with RuleTag " << rule.getRuleTag();
	MESSAGE_END();
}

void
Chain::removeRules(ip::iptables::Rule::RuleTag ruleTag)
{
	int numberOfRules = 0;
	if(rules.empty())
	{
		std::cout<<"RuleContainer is empty! There are no Rules to remove with RuleTag "<<ruleTag<<std::endl;
	}
	else
	{
		for(RuleContainer::iterator it = rules.begin();
			it != rules.end();)
		{
			if((it->getRuleTag()) == ruleTag)
			{
				numberOfRules++;
				it = rules.erase(it);
				it = rules.begin();
			}
			else
			{
				++it;
			}
		}
	MESSAGE_BEGIN(NORMAL, log, m, "");
	m <<"Erased "<< numberOfRules <<" Rules for RuleTag "<<ruleTag;
	MESSAGE_END();
	}
}

bool
Chain::hasRules(ip::iptables::Rule::RuleTag ruleTag)
{
	int numberOfRules = 0;
	if(rules.empty())
	{
		return false;
	}

	for(RuleContainer::iterator it = rules.begin();
		it != rules.end();
		++it)
	{
		if((it->getRuleTag()) == ruleTag)
		{
			numberOfRules++;
		}
		else
		{
		}
	}
	if(numberOfRules>0)
		{
			return true;
		}
	else
		{
			return false;
		}
}


// use returnTarget with care!
// just for RuleTags with just one rule!
ip::iptables::Rule
Chain::getRule(ip::iptables::Rule::RuleTag ruleTag)
{
	for(RuleContainer::iterator it = rules.begin();
		it != rules.end();
		++it)
	{
		if((it->getRuleTag()) == ruleTag)
		{
			return *it;
		}
	}
}


void
Chain::showRules()
{
	for(RuleContainer::iterator it = rules.begin();
		it != rules.end();
		++it)
	{
	MESSAGE_BEGIN(NORMAL, log, m, "");
	m <<"Existing Rule for RuleTag: "<<	it->getRuleTag();
	MESSAGE_END();
	}
}
