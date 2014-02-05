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

#include <IP/LowerConvergence.hpp>
#include <IP/Routing.hpp>
#include <IP/trace/PacketTrace.hpp>

#include <WNS/module/Base.hpp>


using namespace ip;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	LowerConvergence,
	wns::ldk::FunctionalUnit,
	"ip.lowerConvergence",
	wns::ldk::FUNConfigCreator);

LowerConvergence::LowerConvergence(wns::ldk::fun::FUN* fun, const wns::pyconfig::View& _pyco) :
	wns::ldk::CommandTypeSpecifier<>(fun),
	wns::ldk::HasReceptor<>(),
	wns::ldk::HasConnector<>(),
	wns::ldk::HasDeliverer<>(),
	wns::Cloneable<LowerConvergence>(),
	ipHeaderReader(NULL),
	pyco(_pyco),
	//log("IP", "IP.LowerConvergence", wns::module::Base::getService<wns::logger::Master>("W-NS-MSG"))
	log(_pyco.get("logger"))
{
}

LowerConvergence::~LowerConvergence()
{
}

void
LowerConvergence::onFUNCreated()
{
	ipHeaderReader = getFUN()->getCommandReader("ip.ipHeader");
	assure(ipHeaderReader, "No reader for the IP Header available!");
}

void
LowerConvergence::setDLLs(container::DataLinkContainer _dlls)
{
	this->dlls = _dlls;
}

void
LowerConvergence::doSendData(const wns::ldk::CompoundPtr& compound)
{
	assure(compound, "sendData called with an invalid compound.");

	assure(ipHeaderReader, "No reader for the IP Header available!");
	IPCommand* ipHeader = ipHeaderReader->readCommand<IPCommand>(compound->getCommandPool());

	// 1. Retrieve NextHop Address from compound
	// 2. Retrieve DLLName from compound
	// 3. See if ARP on this DLL knows the NextHop
	// 4a. Positve dll->sendData to the corresponding MAC Id
	// 4b. Negative Destination Unreachable


	// 3. See if ARP on this DLL knows the NextHop
	try{
		container::DataLink dll = dlls.find(ipHeader->local.dllName);

		MESSAGE_BEGIN(NORMAL, log, m, "");
		m << "nextHop : " << ipHeader->local.nextHop
		  << " -> macId : " << ipHeader->local.macID
		  << " DLL FlowID : " << ipHeader->local.dllFlowID;
		MESSAGE_END();

		dll.dts->sendData(ipHeader->local.macID, compound, wns::service::dll::IP, ipHeader->local.dllFlowID);

		if (dll.traceEnabled == true)
		{
			trace::PacketTrace pt(wns::simulator::getEventScheduler()->getTime(),
								  ipHeader->local.macID,
								  dll.dts->getMACAddress(),
								  ipHeader->peer.source,
								  ipHeader->peer.destination,
								  ipHeader->peer.TTL,
								  ipHeader->peer.protocol,
								  compound->getData()->getLengthInBits());

			assure(dll.traceCollector, "No TraceCollector given in DataLink structure");
			dll.traceCollector->addPacketTrace(pt);
		}
	}
	catch(container::DataLinkContainer::UnknownKeyValue)
	  {
		  MESSAGE_BEGIN(NORMAL, log, m, "Destination unreachable : ");
		  m << ipHeader->local.nextHop;
		  m << " via iface : " << ipHeader->local.dllName;
		  MESSAGE_END()
	  }
}

bool
LowerConvergence::doIsAccepting(const wns::ldk::CompoundPtr& /* compound */) const
{
	return true;
}

void
LowerConvergence::doOnData(const wns::ldk::CompoundPtr& compound)
{
	// test:
	assure(compound, "onData called with an invalid compound.");

	// end test;

	assure(compound != wns::ldk::CompoundPtr(), "onData called with an invalid compound.");

	MESSAGE_BEGIN(NORMAL, log, m, getFUN()->getName());
	m << ": doOnData(), forwading to higher FU";
	MESSAGE_END();

	getDeliverer()->getAcceptor(compound)->onData(compound);

	// We now activate upper FUs to try to send their pending data
	getReceptor()->wakeup();
}

void
LowerConvergence::doWakeup()
{
}

void
LowerConvergence::onData(const wns::osi::PDUPtr& pdu, wns::service::dll::FlowID dllFlowID)
{
	MESSAGE_BEGIN(NORMAL, log, m,"" );
	m << "Incoming PDU with DLLFlowID: "<< dllFlowID;
	MESSAGE_END();

	assure(wns::dynamicCast<wns::ldk::Compound>(pdu), "not a CompoundPtr");
	wns::ldk::CompoundPtr compound = wns::staticCast<wns::ldk::Compound>(pdu)->copy();

	assure(ipHeaderReader, "No reader for the IP Header available!");
	IPCommand* ipHeader = ipHeaderReader->readCommand<IPCommand>(compound->getCommandPool());
	assure(ipHeader, "IP Header not set");
	ipHeader->local.dllFlowID = dllFlowID;

	this->wns::ldk::FunctionalUnit::onData(compound);
}
