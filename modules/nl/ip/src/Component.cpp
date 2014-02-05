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
#include <IP/UpperConvergence.hpp>
#include <IP/LowerConvergence.hpp>
#include <IP/Forwarding.hpp>

#include <IP/Routing.hpp>
#include <IP/Resolver.hpp>
#include <IP/VirtualARP.hpp>
#include <IP/VirtualDNS.hpp>

#include <WNS/module/Base.hpp>

#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/utils.hpp>

using namespace ip;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	Component,
	wns::node::component::Interface,
	"ip.IPv4",
	wns::node::component::ConfigCreator);

Component::Component(wns::node::Interface* _node, const wns::pyconfig::View& _pyco) :
	wns::node::component::Component(_node, _pyco),
	log(_pyco.get("logger")),
	domainName(_pyco.get<wns::service::nl::FQDN>("domainName")),
	dns(NULL),
	upperConvergence(NULL),
	lowerConvergence(NULL)
{
}

void
Component::doStartup()
{
	// build FUN
	fun = new wns::ldk::fun::Main(this, getConfig());
	wns::ldk::configureFUN(fun, getConfig().get<wns::pyconfig::View>("fun"));
	fun->onFUNCreated();


 	// create IPFlowHandlerService and register the RuleController:
 	iRuleControl = fun->findFriend<wns::service::dll::IRuleControl*>("postroutingChain");
 	assure(iRuleControl, "RuleController not set.");

	// configure upper convergence (services, MAC address)
	upperConvergence =
		dynamic_cast<UpperConvergence*>(
			fun->getFunctionalUnit(
				getConfig().get<std::string>("upperConvergence.commandName")));

	lowerConvergence =
		dynamic_cast<LowerConvergence*>(
			fun->getFunctionalUnit(
				getConfig().get<std::string>("lowerConvergence.commandName")));

	addService(getConfig().get<std::string>("dataTransmission"), upperConvergence);
	addService(getConfig().get<std::string>("notification"), upperConvergence);
	addService(getConfig().get<std::string>("dns"), this);
}

Component::~Component()
{
	while (!resolvers.empty())
	{
		delete *resolvers.begin();
		resolvers.pop_front();
	}
	delete fun;
}

wns::service::nl::Address
Component::lookup(wns::service::nl::FQDN name)
{
	assure(dns, "No Dynamic Domain Name Service found!");

	return dns->request(name);
}

trace::TraceCollector*
Component::getTraceCollector()
{
	static trace::TraceCollector tc = trace::TraceCollector("iptrace.cap.junk");
	return &tc;
}

void
Component::onNodeCreated()
{
	MESSAGE_BEGIN(NORMAL, log, m, "Connecting to DLL components : ");
	MESSAGE_END();

	for (int ii=0; ii < getConfig().len("dataLinkLayers"); ++ii)
	{

		// We build the container::DataLink Structure here

		wns::pyconfig::View entry = getConfig().get<wns::pyconfig::View>("dataLinkLayers", ii);

		MESSAGE_BEGIN(NORMAL, log, m, "   Attaching to DLL : ");
		m << entry.get<std::string>("dllDataTransmission");
		MESSAGE_END();

		wns::service::dll::UnicastDataTransmission* dts = NULL;
		std::string dtsname = entry.get<std::string>("dllDataTransmission");
		dts = getService<wns::service::dll::UnicastDataTransmission*>(dtsname);

		wns::service::dll::Notification* notify = NULL;
		std::string notname = entry.get<std::string>("dllNotification");
		notify = getService<wns::service::dll::Notification*>(notname);
		notify->registerHandler(wns::service::dll::IP, lowerConvergence);

 		notify->registerIRuleControl(iRuleControl);

		wns::pyconfig::View arconf = entry.get("addressResolver");
		std::string plugin = arconf.get<std::string>("nameInResolverFactory");

		resolver::Creator* cc = resolver::Factory::creator(plugin);

		// create and add the Component to this Node
		resolver::ResolverInterface* ri = cc->create(arconf);
		resolvers.push_back(ri);

		std::string arpZone = entry.get<std::string>("arpZone");

		// Is this link pointToPoint
		// If this is true all traffic goes to the gateway, no local
		// delivery

		bool pointToPoint = entry.get<bool>("pointToPoint");

		// Do we trace this DLL

		bool traceEnabled = entry.get<bool>("traceEnabled");

		dlls.insert(entry.get<std::string>("name"), container::DataLink(entry.get<std::string>("name"),
										dts,
										notify,
										arpZone,
										ri,
										pointToPoint,
										traceEnabled,
										getTraceCollector()));
	}

	if(!getConfig().isNone("tunnelExitNotification"))
	{
		MESSAGE_BEGIN(NORMAL, log, m, "Connecting to TunnelExitPoint");
		MESSAGE_END();

		std::string tunExitName = getConfig().get<std::string>("tunnelExitNotification");
		getService<wns::service::dll::Notification*>(tunExitName)->registerHandler(wns::service::dll::IP, this->lowerConvergence);
	}
}

void
Component::onWorldCreated()
{
	leaseAddresses();
	setupARP();
	setupDNS();
	setupForwarding();
	setupRoutingTable();
	setupLowerConvergence();
}


void
Component::leaseAddresses()
{
	for(container::DataLinkContainer::const_iterator it = dlls.begin(); it != dlls.end(); ++it)
	{
		it->second.resolver->resolve();
	}

}

void
Component::setupARP()
{
	for(container::DataLinkContainer::const_iterator it = dlls.begin(); it != dlls.end(); ++it)
	{
		VirtualARP* arp = TheARPService::Instance().getZoneManager(it->second.arpZone);

		arp->bind(it->second.dts->getMACAddress(), it->second.resolver->getAddress());
	}
}

void
Component::setupDNS()
{
	// For now we set our source Address to the first DLL that was added
	wns::service::nl::Address sourceAddress = dlls.begin()->second.resolver->getAddress();

	dns = TheDNSService::Instance().getZoneManager(getConfig().get<std::string>("dnsZone"));

	assure(dns, "No Dynamic Domain Name Service found!");

	dns->bind(domainName, sourceAddress);
}

void
Component::setupForwarding()
{
	Forwarding* forwarding =
		dynamic_cast<Forwarding*>(
		 fun->getFunctionalUnit(
		  getConfig().get<std::string>("forwarding.commandName")));


	for(container::DataLinkContainer::const_iterator it = dlls.begin(); it != dlls.end(); ++it)
	{
		forwarding->addListeningAddress(it->second.resolver->getAddress());
	}
}

void
Component::setupRoutingTable()
{
	Routing* routing =
		dynamic_cast<Routing*>(
			fun->getFunctionalUnit(
				getConfig().get<std::string>("routing.commandName")));

	for(container::DataLinkContainer::const_iterator it = dlls.begin(); it != dlls.end(); ++it)
	{
		if (!it->second.pointToPoint)
		{
			routing->addRoute(
				// Address on this dll
				it->second.resolver->getAddress() & it->second.resolver->getSubnetMask(),
				// No gateway - local delivery route
				wns::service::nl::Address("0.0.0.0"),
				// Subnet Mask for this dll
				it->second.resolver->getSubnetMask(),
				// Dll name
				it->first);
		}
	}
	routing->printRoutingTable();

	routing->setDLLs(dlls);
}

void
Component::setupLowerConvergence()
{
	LowerConvergence* lowerConvergence =
	  dynamic_cast<LowerConvergence*>(
	    fun->getFunctionalUnit(
	      getConfig().get<std::string>("lowerConvergence.commandName")));

	lowerConvergence->setDLLs(dlls);
}

void
Component::onShutdown()
{
	if (getTraceCollector()->hasSomethingToWrite())
	{
		getTraceCollector()->write();
	}
}


