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

#include <IP/Routing.hpp>

#include <IP/Forwarding.hpp>

#include <WNS/module/Base.hpp>
#include <WNS/Assure.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/pyconfig/View.hpp>

#include <iomanip>
#include <sstream>

using namespace ip;
using namespace wns::service::nl;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	Routing,
	wns::ldk::FunctionalUnit,
	"ip.routing",
	wns::ldk::FUNConfigCreator);

wns::service::nl::Address Routing::defaultGateway = wns::service::nl::Address("0.0.0.0");

Routing::Routing(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _pyco) :
	wns::ldk::CommandTypeSpecifier<>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::ldk::Processor<Routing>(),
	wns::Cloneable<Routing>(),
	ipHeaderReader(NULL),
	pyco(_pyco),
	log(_pyco.get("logger"))
{
	for (int ii=0; ii < getConfig().len("routingTable"); ++ii)
	{
		wns::pyconfig::View re_conf = getConfig().get<wns::pyconfig::View>("routingTable", ii);
		container::RoutingTableEntry route = container::RoutingTableEntry(re_conf);

		rt.insert(rt.end(), route);
	}
}

Routing::~Routing()
{
}

void
Routing::onFUNCreated()
{
 	ipHeaderReader = getFUN()->getCommandReader("ip.ipHeader");
	assure(ipHeaderReader, "No reader for the IP Header available!");
} // onFUNCreated

void
Routing::addRoute(const wns::service::nl::Address& netAddress,
		  const wns::service::nl::Address& gateway,
		  const wns::service::nl::Address& netMask,
		  std::string dllName)
{
  container::RoutingTableEntry route;
  route.netaddress = netAddress;
  route.gateway = gateway;
  route.netmask = netMask;
  route.dllName = dllName;

  rt.insert(rt.begin(), route);
}

void
Routing::processOutgoing(const wns::ldk::CompoundPtr& compound)
{
	if (!forwardViaRoutingTable(compound))
    {
        MESSAGE_SINGLE(NORMAL, log, "No route to host!");
    }
}

void
Routing::processIncoming(const wns::ldk::CompoundPtr&)
{
}

void
Routing::setDLLs(container::DataLinkContainer _dlls)
{
	this->dlls = _dlls;
}

bool
Routing::forwardViaRoutingTable(const wns::ldk::CompoundPtr& compound)
{
	assure(ipHeaderReader, "No reader for the IP Header available!");
	IPCommand* ipHeader = ipHeaderReader->readCommand<IPCommand>(compound->getCommandPool());

	Address destAddress = ipHeader->peer.destination;

#ifndef WNS_NO_LOGGING
	printRoutingTable(destAddress);
#endif

	for(container::RoutingTable::const_iterator it=rt.begin(); it!=rt.end(); ++it)
	{
		Address netAddress = it->netaddress;
		Address netmask = it->netmask;

		if (netAddress == (destAddress&netmask))
		{
			if (it->gateway == defaultGateway)
            {
                  ipHeader->local.nextHop = destAddress;
            }
			else
            {
                  ipHeader->local.nextHop = it->gateway;
            }

			ipHeader->local.dllName = it->dllName;

			container::DataLink dll = dlls.find(ipHeader->local.dllName);

			ipHeader->local.arpZone = dll.arpZone;

			// If IP destination is directly reachable
			// local.routingDone is true
			//			rc->local.routingDone;
			return true;
		}
	}
	return false;
}

void
Routing::printRoutingTable(const wns::service::nl::Address& highlight) const
{
    MESSAGE_BEGIN(NORMAL, log, m, "");
    m << "" << std::setw(18) << std::left << "Destination"
      << std::setw(18) << std::left << "Gateway"
      << std::setw(18) << std::left << "Genmask"
      << "Iface";
    MESSAGE_END();

    bool theFirstHitIsTheDeepest = false;

    for (container::RoutingTable::const_iterator it=rt.begin(); it!=rt.end(); ++it)
    {
        MESSAGE_BEGIN(NORMAL, log, m, "");
        m << ""  << std::setw(18) << std::left << Address(it->netaddress)
          << std::setw(18) << std::left<< Address(it->gateway)
          << std::setw(18) << std::left<< Address(it->netmask)
          << it->dllName;

        if (highlight != defaultGateway && !theFirstHitIsTheDeepest)
        {
            if ( (highlight & it->netmask) == it->netaddress)
            {
                m << " ***HIT***";
                theFirstHitIsTheDeepest = true;
            }
        }
        MESSAGE_END();
    }
}

wns::pyconfig::View
Routing::getConfig()
{
	return pyco;
}

/*void
Routing::setDLLService(Address address, wns::service::dll::UnicastDataTransmission* dll)
{
	dlls[address] = dll;
}*/



