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

#include <IP/Component.hpp>
#include <IP/Forwarding.hpp>

#include <WNS/module/Base.hpp>

using namespace ip;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	Forwarding,
	wns::ldk::FunctionalUnit,
	"ip.forwarding",
	wns::ldk::FUNConfigCreator);

Forwarding::Forwarding(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _pyco) :
	wns::ldk::CommandTypeSpecifier<ForwardingCommand>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<Forwarding>(),
	ipHeaderReader(NULL),
	pyco(_pyco),
	log(_pyco.get("logger")),
	isForwarding(_pyco.get<bool>("isForwarding")),
	ttl(_pyco.get<int>("ttl"))
{

}

void
Forwarding::addListeningAddress(wns::service::nl::Address _la)
{
	MESSAGE_SINGLE(NORMAL, log, "Adding " << _la << " to listening addresses");

	assure(find(this->listeningAddresses.begin(), this->listeningAddresses.end(), _la)
		   == this->listeningAddresses.end(), "Address already registered!");

	this->listeningAddresses.insert(this->listeningAddresses.end(), _la);
}

void
Forwarding::removeListeningAddress(wns::service::nl::Address _la)
{
	MESSAGE_SINGLE(NORMAL, log, "Adding " << _la << " to listening addresses");

	assure(find(this->listeningAddresses.begin(), this->listeningAddresses.end(), _la)
		   != this->listeningAddresses.end(), "Address not registered!");

	this->listeningAddresses.erase(find(this->listeningAddresses.begin(), this->listeningAddresses.end(), _la));
}


void
Forwarding::onFUNCreated()
{
 	ipHeaderReader = getFUN()->getCommandReader("ip.ipHeader");
	assure(ipHeaderReader, "No reader for the IP Header available!");
}

void
Forwarding::processOutgoing(const wns::ldk::CompoundPtr& compound)
{
	activateCommand(compound->getCommandPool());

	assure(ipHeaderReader, "No reader for the IP Header available!");
	IPCommand* ipHeader = ipHeaderReader->readCommand<IPCommand>(compound->getCommandPool());

	// Set initial TimeToLive
	ipHeader->peer.TTL = this->ttl;

	// This packet originates here, we do not mark it as a forwarded packet
	ipHeader->local.isForwarded = false;

	MESSAGE_BEGIN(NORMAL, log, m, "Initial TTL value");
	m << " (TTL:" << ipHeader->peer.TTL << ")";
	MESSAGE_END();

	// Forward to lower FU.
	assure(hasCapacity(), "received PDU although not accepting");

	this->buffer = compound;

//	getConnector()->getAcceptor(compound)->sendData(compound);
}

void
Forwarding::processIncoming(const wns::ldk::CompoundPtr& compound)
{
	assure(ipHeaderReader, "No reader for the IP Header available!");
	IPCommand* ipHeader = ipHeaderReader->readCommand<IPCommand>(compound->getCommandPool());

	// Test wether compound is for us
	if (reachedDestination(compound))
	{
	  MESSAGE_BEGIN(NORMAL, log, m, "Delivering compound from : " << ipHeader->peer.source);
	  m << " (TTL:" << ipHeader->peer.TTL << ")";
	  MESSAGE_END();

	  MESSAGE_BEGIN(VERBOSE, log, m, getFUN()->getName());
	  m << ": Compound backtrace"
	    << compound->dumpJourney(); // JOURNEY
	  MESSAGE_END();

	  // Hand compound to upper FU
	  if(getDeliverer()->size())
	    getDeliverer()->getAcceptor(compound)->onData(compound);
	}
	else
	{
		if(putOnNextHop(compound))
		{
			MESSAGE_BEGIN(NORMAL, log, m, "Forwarding compound from : " << ipHeader->peer.source);
			m << " (TTL:" << ipHeader->peer.TTL << ")";
			MESSAGE_END();

			MESSAGE_BEGIN(VERBOSE, log, m, getFUN()->getName());
			m << ": Compound backtrace"
			  << compound->dumpJourney(); // JOURNEY
			MESSAGE_END();

			// We now create a partial copy of the CommandPool we received which we can
			// then pass down the stack again. All commands from FUs above this FU stay
			// activated (cmp. 2.8. Relaying in fds diploma thesis).
			wns::ldk::CommandPool* newCP = getFUN()->getProxy()->createCommandPool();
			getFUN()->getProxy()->partialCopy(this, newCP, compound->getCommandPool());

			assure(ipHeaderReader, "No reader for the IP Header available!");
			IPCommand* newIPHeader = ipHeaderReader->readCommand<IPCommand>(newCP);

			// Decrease the TTL
			newIPHeader->peer.TTL = ipHeader->peer.TTL - 1;

			// We put this packet on the next hop, thus we mark it as a forwarded packet
			newIPHeader->local.isForwarded = true;

			// New compound....
			wns::ldk::CompoundPtr newcompound(new wns::ldk::Compound(newCP, compound->getData()));

			activateCommand(newcompound->getCommandPool());

			// and off we go.

			// Forward to lower FU.
			assure(hasCapacity(), "received PDU although not accepting");

			this->buffer = newcompound;
		}
		else
		{
			MESSAGE_BEGIN(NORMAL, log, m, "Dropping compound from : " << ipHeader->peer.source);
			m << " (TTL:" << ipHeader->peer.TTL << ")";
			MESSAGE_END();

			MESSAGE_BEGIN(VERBOSE, log, m, getFUN()->getName());
			m << ": Compound backtrace"
			  << compound->dumpJourney(); // JOURNEY
			MESSAGE_END();
		}
	}
}

//bool
//Forwarding::doIsAccepting(const wns::ldk::CompoundPtr& compound) const
//{
//	return getConnector()->hasAcceptor(compound);
//}

bool
Forwarding::hasCapacity() const
{
	return this->buffer == wns::ldk::CompoundPtr();
}

//void
//Forwarding::doWakeup()
//{
//	getReceptor()->wakeup();
//}

const wns::ldk::CompoundPtr
Forwarding::hasSomethingToSend() const
{
	return this->buffer;
}

wns::ldk::CompoundPtr
Forwarding::getSomethingToSend()
{
	wns::ldk::CompoundPtr it;
	it = this->buffer;
	this->buffer = wns::ldk::CompoundPtr();
	return it;
}

bool
Forwarding::reachedDestination(const wns::ldk::CompoundPtr& compound)
{
	assure(ipHeaderReader, "No reader for the IP Header available!");
	IPCommand* ipHeader = ipHeaderReader->readCommand<IPCommand>(compound->getCommandPool());

	// Is this compound for us?
	if ( (find(this->listeningAddresses.begin(), this->listeningAddresses.end(), ipHeader->peer.destination) 
		  != this->listeningAddresses.end()))
	{
		if (ipHeader->peer.TTL >= 0)
		{
			// TTL ok
			return true;
		}
		else
		{
			// TTL exceeded
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool
Forwarding::putOnNextHop(const wns::ldk::CompoundPtr& compound)
{
	assure(ipHeaderReader, "No reader for the IP Header available!");
	IPCommand* ipHeader = ipHeaderReader->readCommand<IPCommand>(compound->getCommandPool());

	if (ipHeader->peer.TTL > 0)
	{ // TTL available
		if(isForwarding)
		{
			// Routing
			return true;
		}
		else
		{
			// No Routing
			return false;
		}
	}
	else
	{
		// TTL exceeded for another hop
		return false;
	}
}
