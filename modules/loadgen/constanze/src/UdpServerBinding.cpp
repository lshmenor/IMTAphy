/******************************************************************************
 * UDP ServerBinding                                                          *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 *****************************************************************************/

#include <CONSTANZE/UdpServerBinding.hpp>

using namespace constanze;

UdpServerBinding::UdpServerBinding(wns::service::nl::Address _address, wns::service::tl::Connection* _connection, wns::logger::Logger& logger):
  peerAddress(_address),
  connection(_connection),
  log(logger)
{
	startTrigger = NULL;
	stopTrigger = NULL;
	//udpService = NULL;
	component = NULL;
	MESSAGE_SINGLE(NORMAL, log, "New UdpServerBinding created (peer="<<peerAddress<<")");
}

void UdpServerBinding::sendData(const wns::osi::PDUPtr& _data)
{
	assure(connection, "No connection available.");
	MESSAGE_SINGLE(NORMAL, log, "UdpServerBinding ->sendData ");
	connection->sendData(_data);
}

void UdpServerBinding::registerComponent(wns::node::component::Component* /*_component*/)
{
/*	component = _component;
	udpService =
		component->getService<wns::service::tl::Service*>(pyco.get<std::string>("udpService"));
*/
}

void UdpServerBinding::onConnectionEstablished(wns::service::nl::Address, wns::service::tl::Connection* _connection)
{
	connection = _connection;
	startTrigger->bindingReady();
}

void UdpServerBinding::initBinding(constanze::StartTrigger* _startTrigger)
{
	startTrigger = _startTrigger;
	//  openConnection not needed at UdpServerBinding, because connection
	//	already established; Servers don't start connections
	//	udpService->openConnection( destinationPort, domainName, destinationDomainName, this);
}

void UdpServerBinding::releaseBinding(constanze::StopTrigger* _stopTrigger)
{
//  closeConnection not needed,
//	udpService->closeConnection( connection );
	stopTrigger = _stopTrigger;
	stopTrigger->bindingReady();
}

void UdpServerBinding::registerListener(constanze::Listener* )
{
	// intentionally left empty
}
void UdpServerBinding::onConnectionClosed(wns::service::tl::Connection*)
{
	// intentionally left empty
}

void UdpServerBinding::onConnectionClosedByPeer(wns::service::tl::Connection*)
{
	// intentionally left empty
}

void UdpServerBinding::onConnectionLost(wns::service::tl::Connection*)
{
	// intentionally left empty
}

std::string
UdpServerBinding::printAddress() const
{
	std::ostringstream tmp;
	//tmp << ownAddress << ":" << listenPort;
	tmp << peerAddress;
	return tmp.str();
}




