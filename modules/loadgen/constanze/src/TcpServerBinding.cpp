/******************************************************************************
 * TCP Binding                                                                *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 *****************************************************************************/

#include <CONSTANZE/TcpServerBinding.hpp>

using namespace constanze;

TcpServerBinding::TcpServerBinding(wns::service::nl::Address _address, wns::service::tl::Connection* _connection, wns::logger::Logger& logger):
  peerAddress(_address),
  connection(_connection),
  log(logger)
{
	startTrigger = NULL;
	stopTrigger = NULL;
	//tcpService = NULL;
	component = NULL;
	MESSAGE_SINGLE(NORMAL, log, "New TcpServerBinding created (peer="<<peerAddress<<")");
}

void TcpServerBinding::sendData(const wns::osi::PDUPtr& _data)
{
	assure(connection, "No connection available.");
	MESSAGE_SINGLE(NORMAL, log, "TcpServerBinding ->sendData ");
	connection->sendData(_data);
}

void TcpServerBinding::registerComponent(wns::node::component::Component* /*_component*/)
{
/*	component = _component;
	tcpService =
		component->getService<wns::service::tl::Service*>(pyco.get<std::string>("tcpService"));
*/
}

void TcpServerBinding::onConnectionEstablished(wns::service::nl::Address, wns::service::tl::Connection* _connection)
{
	connection = _connection;
	startTrigger->bindingReady();
}

void TcpServerBinding::initBinding(constanze::StartTrigger* _startTrigger)
{
	startTrigger = _startTrigger;
	//  openConnection not needed at TcpServerBinding, because connection
	//	already established; Servers don't start connections
	//	tcpService->openConnection( destinationPort, domainName, destinationDomainName, this);
}

void TcpServerBinding::releaseBinding(constanze::StopTrigger* _stopTrigger)
{
//  closeConnection not needed,
//	tcpService->closeConnection( connection );
	stopTrigger = _stopTrigger;
	stopTrigger->bindingReady();
}

void TcpServerBinding::registerListener(constanze::Listener* )
{
	// intentionally left empty
}
void TcpServerBinding::onConnectionClosed(wns::service::tl::Connection*)
{
	// intentionally left empty
}

void TcpServerBinding::onConnectionClosedByPeer(wns::service::tl::Connection*)
{
	// intentionally left empty
}

void TcpServerBinding::onConnectionLost(wns::service::tl::Connection*)
{
	// intentionally left empty
}

std::string
TcpServerBinding::printAddress() const
{
	std::ostringstream tmp;
	//tmp << ownAddress << ":" << listenPort;
	tmp << peerAddress;
	return tmp.str();
}




