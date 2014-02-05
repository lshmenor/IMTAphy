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

#include <CONSTANZE/UdpBinding.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/PortPool.hpp>
#include <WNS/module/Base.hpp>

using namespace constanze;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	constanze::UdpBinding,
	constanze::Binding,
	"UdpBinding",
	wns::PyConfigViewCreator);

UdpBinding::UdpBinding(const wns::pyconfig::View& _pyco):
	pyco(_pyco),
	domainName(_pyco.get<std::string>("domainName")),
	destinationDomainName(_pyco.get<std::string>("destinationDomainName")),
	destinationPort(_pyco.get<int>("destinationPort")),
	qosClass(_pyco.get<int>("qosClass")), // aoz does not support this solution. aoz wanted the qosClass to be a parameter of the generator
	log(pyco.get("logger"))
{
	startTrigger = NULL;
	stopTrigger = NULL;
	udpService = NULL;
	connection = NULL;
	component = NULL;

	MESSAGE_BEGIN(NORMAL, log, m, "New UdpBinding created. Destination=");
	m << destinationDomainName << ":" << destinationPort;
	MESSAGE_END();
}

void UdpBinding::sendData(const wns::osi::PDUPtr& _data)
{
	assure(connection, "No connection available.");
	MESSAGE_SINGLE(NORMAL, log, "sendData to destination : " << destinationDomainName << ":" << destinationPort);
	connection->sendData(_data);

}

void UdpBinding::registerComponent(wns::node::component::Component* _component)
{
	component = _component;
		udpService =
			component->getService<wns::service::tl::Service*>(pyco.get<std::string>("udpService"));
}

void UdpBinding::onConnectionEstablished(wns::service::nl::Address, wns::service::tl::Connection* _connection)
{
	connection = _connection;
	startTrigger->bindingReady();
}

void UdpBinding::initBinding(constanze::StartTrigger* _startTrigger)
{
	startTrigger = _startTrigger;
	MESSAGE_SINGLE(NORMAL, log, "initBinding(): Opening UDP connection to " << destinationDomainName << ":" << destinationPort << " from " << domainName);
	udpService->openConnection( destinationPort, domainName, destinationDomainName, qosClass, this);
}

void UdpBinding::releaseBinding(constanze::StopTrigger* _stopTrigger)
{
	stopTrigger = _stopTrigger;
	MESSAGE_SINGLE(NORMAL, log, "releaseBinding(): Closing UDP connection.");
	udpService->closeConnection( connection );
	stopTrigger->bindingReady();
}

void UdpBinding::registerListener(constanze::Listener* )
{
	// intentionally left empty
}
void UdpBinding::onConnectionClosed(wns::service::tl::Connection*)
{
	// intentionally left empty
}

void UdpBinding::onConnectionClosedByPeer(wns::service::tl::Connection*)
{
	// intentionally left empty
}

void UdpBinding::onConnectionLost(wns::service::tl::Connection*)
{
	// intentionally left empty
}

std::string	UdpBinding::printAddress() const
{
	std::ostringstream tmp;
	tmp << destinationDomainName << ":" << destinationPort;
	return tmp.str();
}




