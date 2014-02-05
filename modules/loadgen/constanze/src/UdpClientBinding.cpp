/******************************************************************************
 * UDP ClientBinding                                                          *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 *****************************************************************************/

#include <CONSTANZE/UdpClientBinding.hpp>
#include <CONSTANZE/UdpClientListenerBinding.hpp>
#include <CONSTANZE/Listener.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/PortPool.hpp>
#include <WNS/module/Base.hpp>

using namespace constanze;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    constanze::UdpClientBinding,
    constanze::Binding,
    "UdpClientBinding",
    wns::PyConfigViewCreator);

UdpClientBinding::UdpClientBinding(const wns::pyconfig::View& _pyco):
    pyco(_pyco),
    dns(NULL),
    domainName(_pyco.get<std::string>("domainName")),
    destinationDomainName(_pyco.get<std::string>("destinationDomainName")),
    destinationPort(_pyco.get<int>("destinationPort")),
    qosClass(_pyco.get<int>("qosClass")), // aoz does not support this solution. aoz wanted the qosClass to be a parameter of the generator
    log(pyco.get("logger")),
    listener(0),
    packetCounter(0),
    bitCounter(0),
    nodeID(0)
{
    startTrigger = NULL;
    stopTrigger = NULL;
    udpService = NULL;
    connection = NULL;
    component = NULL;

    listener = NULL;

    MESSAGE_BEGIN(NORMAL, log, m, "New UdpClientBinding created. Destination=");
    m << destinationDomainName << ":" << destinationPort;
    if (qosClass != 0) { // UNDEFINED == 0
        m << ", QoSClass="<<qosClass;
    }
    MESSAGE_END();
}

UdpClientBinding::~UdpClientBinding()
{
    MESSAGE_BEGIN(NORMAL, log, m, "Received a total of ");
    m << packetCounter << " packets with " << bitCounter << " bits";
    MESSAGE_END();
}


void
UdpClientBinding::sendData(const wns::osi::PDUPtr& _data)
{
    assure(connection, "No connection available.");

    //MESSAGE_SINGLE(NORMAL, log, "sendData to destination: " << destinationDomainName << ":" << destinationPort);
    MESSAGE_SINGLE(NORMAL, log, "sendData to destination: " << peerAddress << ":" << destinationPort);
    connection->sendData(_data);
}

void
UdpClientBinding::registerComponent(wns::node::component::Component* _component)
{
    component = _component;
    udpService =
        component->getService<wns::service::tl::Service*>(pyco.get<std::string>("udpService"));
    dns = component->getService<wns::service::nl::DNSService*>(pyco.get<std::string>("dnsService"));


    wns::pyconfig::View listenerView = pyco.get("listener");
    listener = new Listener(listenerView, component);

    nodeID = component->getNode()->getNodeID();
}

void
UdpClientBinding::onConnectionEstablished(wns::service::nl::Address, wns::service::tl::Connection* _connection)
{
    connection = _connection;
    startTrigger->bindingReady();
    connection->registerDataHandler(this);
}

void
UdpClientBinding::onData(const wns::osi::PDUPtr& _pdu)
{
    assure(_pdu , "data is empty.");
    //assureType(_pdu.getPtr(), ConstanzePDU*);
    int bits = _pdu->getLengthInBits();
    packetCounter ++;
    bitCounter += bits;

    MESSAGE_SINGLE(NORMAL, log, "Received " << _pdu->getLengthInBits()/8 << " bytes");
    listener->onData(_pdu);
}

void
UdpClientBinding::initBinding(constanze::StartTrigger* _startTrigger)
{
    startTrigger = _startTrigger;
    udpService->openConnection( destinationPort, domainName, destinationDomainName, qosClass, this);
    assure(dns!=NULL,"dns needed");
    wns::service::nl::Address peerAddress = dns->lookup(destinationDomainName);
}

void
UdpClientBinding::releaseBinding(constanze::StopTrigger* _stopTrigger)
{
    stopTrigger = _stopTrigger;
    udpService->closeConnection( connection );
    stopTrigger->bindingReady();
}

void
UdpClientBinding::registerListener(constanze::Listener*)
{
    // intentionally left empty
}
void
UdpClientBinding::onConnectionClosed(wns::service::tl::Connection*)
{
    // intentionally left empty
}

void
UdpClientBinding::onConnectionClosedByPeer(wns::service::tl::Connection*)
{
    // intentionally left empty
}

void
UdpClientBinding::onConnectionLost(wns::service::tl::Connection*)
{
    // intentionally left empty
}

std::string
UdpClientBinding::printAddress() const
{
    std::ostringstream tmp;
    //tmp << destinationDomainName << ":" << destinationPort;
    tmp << peerAddress << ":" << destinationPort;
    return tmp.str();
}




