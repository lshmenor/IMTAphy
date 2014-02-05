/******************************************************************************
 * UDP ClientListenerBinding                                                  *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 *****************************************************************************/

#include <CONSTANZE/UdpClientListenerBinding.hpp>

#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/PortPool.hpp>
#include <WNS/module/Base.hpp>

using namespace constanze;

UdpClientListenerBinding::UdpClientListenerBinding(int _listenPort):
    listenPort(_listenPort)
{
    std::cout<<"New UdpClientListenerBinding created on my socket= "<< ownAddress << ":" << listenPort<<std::endl;
}

void
UdpClientListenerBinding::sendData(const wns::osi::PDUPtr&)
{
    // intentionally left empty
}

void
UdpClientListenerBinding::registerComponent(wns::node::component::Component* /*_component*/)
{
// intentionally left empty

//	component = _component;
//	udpService =
//		component->getService<wns::service::tl::Service*>(
//			pyco.get<std::string>("udpService"));
//	udpService->listenOnPort(listenPort, this);
}
void
UdpClientListenerBinding::registerUdpService(wns::service::tl::Service* _udpService)
{
    _udpService->listenOnPort(listenPort, this);
}

void
UdpClientListenerBinding::onConnectionEstablished(wns::service::nl::Address _address, wns::service::tl::Connection* _connection)
{
    ownAddress = _address;
    _connection->registerDataHandler(this);
}

void
UdpClientListenerBinding::onData(const wns::osi::PDUPtr& _data)
{
    assure(listener, "No listener set.");
    MESSAGE_BEGIN(NORMAL, log, m, "Received " << _data->getLengthInBits()/8 << " bytes"); // "from ?"
    m << " on my socket=" << ownAddress << ":" << listenPort;
    MESSAGE_END();
    listener->onData(_data);
}

void
UdpClientListenerBinding::registerListener(constanze::Listener* _listener)
{
    listener = _listener;
}

void
UdpClientListenerBinding::initBinding(constanze::StartTrigger* )
{
    // intentionally left empty
}

void
UdpClientListenerBinding::releaseBinding(constanze::StopTrigger*)
{
    /** @todo at this stage connections cannot be closed by the peer */
}

void
UdpClientListenerBinding::onConnectionClosed(wns::service::tl::Connection*)
{
    // UDP has no connection that can be closed
    // intentionally left empty
}

void
UdpClientListenerBinding::onConnectionClosedByPeer(wns::service::tl::Connection*)
{
    // UDP has no connection that can be closed
    // intentionally left empty
}

void
UdpClientListenerBinding::onConnectionLost(wns::service::tl::Connection*)
{
    // UDP has no connection that can be closed
    // intentionally left empty
}

std::string
UdpClientListenerBinding::printAddress() const
{
        std::ostringstream tmp;
        tmp << ownAddress << ":" << listenPort;
        return tmp.str();
}


