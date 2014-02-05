/******************************************************************************
 * TCP Client Listener Binding                                                *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 *****************************************************************************/

#include <CONSTANZE/TcpClientListenerBinding.hpp>

#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/PortPool.hpp>
#include <WNS/module/Base.hpp>

using namespace constanze;

TcpClientListenerBinding::TcpClientListenerBinding(int _listenPort):
	listenPort(_listenPort)
{
	std::cout<<"New TcpClientListenerBinding created on my socket= "<< ownAddress << ":" << listenPort<<std::endl;
}

void TcpClientListenerBinding::sendData(const wns::osi::PDUPtr&)
{
	// intentionally left empty
}

void TcpClientListenerBinding::registerComponent(wns::node::component::Component* /*_component*/)
{
// intentionally left empty

// 	component = _component;
// 	tcpService =
// 		component->getService<wns::service::tl::Service*>(
// 			pyco.get<std::string>("tcpService"));
// 	tcpService->listenOnPort(listenPort, this);
}
void
TcpClientListenerBinding::registerTcpService(wns::service::tl::Service* _tcpService)
{
	_tcpService->listenOnPort(listenPort, this);
}

void TcpClientListenerBinding::onConnectionEstablished(wns::service::nl::Address _address, wns::service::tl::Connection* _connection)
{
	ownAddress = _address;
	_connection->registerDataHandler(this);
}

void TcpClientListenerBinding::onData(const wns::osi::PDUPtr& _data)
{
	assure(listener, "No listener set.");
	MESSAGE_BEGIN(NORMAL, log, m, "Received " << _data->getLengthInBits()/8 << " bytes"); // "from ?"
	m << " on my socket=" << ownAddress << ":" << listenPort;
	MESSAGE_END();
	listener->onData(_data);
}

void TcpClientListenerBinding::registerListener(constanze::Listener* _listener)
{
	listener = _listener;
}

void TcpClientListenerBinding::initBinding(constanze::StartTrigger* )
{
	// intentionally left empty
}

void TcpClientListenerBinding::releaseBinding(constanze::StopTrigger*)
{
	/** @todo at this stage connections cannot be closed by the peer */
}

void TcpClientListenerBinding::onConnectionClosed(wns::service::tl::Connection*)
{
	// TCP has no connection that can be closed
	// intentionally left empty
}

void TcpClientListenerBinding::onConnectionClosedByPeer(wns::service::tl::Connection*)
{
	// TCP has no connection that can be closed
	// intentionally left empty
}

void TcpClientListenerBinding::onConnectionLost(wns::service::tl::Connection*)
{
	// TCP has no connection that can be closed
	// intentionally left empty
}

std::string
TcpClientListenerBinding::printAddress() const
{
        std::ostringstream tmp;
        tmp << ownAddress << ":" << listenPort;
        return tmp.str();
}


