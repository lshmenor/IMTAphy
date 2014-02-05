/******************************************************************************
 * TCP ClientBinding                                                          *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 *****************************************************************************/

#include <CONSTANZE/TcpClientBinding.hpp>
#include <CONSTANZE/TcpClientListenerBinding.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/PortPool.hpp>
#include <WNS/module/Base.hpp>

using namespace constanze;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	constanze::TcpClientBinding,
	constanze::Binding,
	"TcpClientBinding",
	wns::PyConfigViewCreator);

TcpClientBinding::TcpClientBinding(const wns::pyconfig::View& _pyco):
	pyco(_pyco),
	dns(NULL),
	domainName(_pyco.get<std::string>("domainName")),
	destinationDomainName(_pyco.get<std::string>("destinationDomainName")),
	destinationPort(_pyco.get<int>("destinationPort")),
	qosClass(_pyco.get<int>("qosClass")), // aoz does not support this solution. aoz wanted the qosClass to be a parameter of the generator
	log(pyco.get("logger")),
	packetCounter(0),
	bitCounter(0)
{
	startTrigger = NULL;
	stopTrigger = NULL;
	tcpService = NULL;
	connection = NULL;
	component = NULL;

	MESSAGE_BEGIN(NORMAL, log, m, "New TcpClientBinding created. Destination=");
	m << destinationDomainName << ":" << destinationPort;
	m << ", QoSClass="<<qosClass;
	MESSAGE_END();
}

TcpClientBinding::~TcpClientBinding()
{
	MESSAGE_BEGIN(NORMAL, log, m, "Received a total of ");
	m << packetCounter << " packets with " << bitCounter << " bits";
	MESSAGE_END();
}


void TcpClientBinding::sendData(const wns::osi::PDUPtr& _data)
{
	assure(connection, "No connection available.");
	//MESSAGE_SINGLE(NORMAL, log, "sendData to destination: " << destinationDomainName << ":" << destinationPort);
	MESSAGE_SINGLE(NORMAL, log, "sendData to destination: " << peerAddress << ":" << destinationPort);
	connection->sendData(_data);
}

void TcpClientBinding::registerComponent(wns::node::component::Component* _component)
{
	component = _component;
	tcpService =
		component->getService<wns::service::tl::Service*>(pyco.get<std::string>("tcpService"));
	dns = component->getService<wns::service::nl::DNSService*>(pyco.get<std::string>("dnsService"));
}

void TcpClientBinding::onConnectionEstablished(wns::service::nl::Address, wns::service::tl::Connection* _connection)
{
	connection = _connection;
	startTrigger->bindingReady();
	connection->registerDataHandler(this);
}

void TcpClientBinding::onData(const wns::osi::PDUPtr& _pdu)
{
	assure(_pdu , "data is empty.");
	//assureType(_pdu.getPtr(), ConstanzePDU*);
	int bits = _pdu->getLengthInBits();
	packetCounter ++;
	bitCounter += bits;
}

void TcpClientBinding::initBinding(constanze::StartTrigger* _startTrigger)
{
	startTrigger = _startTrigger;
	tcpService->openConnection( destinationPort, domainName, destinationDomainName, qosClass, this);
	assure(dns!=NULL,"dns needed");
	wns::service::nl::Address peerAddress = dns->lookup(destinationDomainName);
}

void TcpClientBinding::releaseBinding(constanze::StopTrigger* _stopTrigger)
{
	stopTrigger = _stopTrigger;
	tcpService->closeConnection( connection );
	stopTrigger->bindingReady();
}

void TcpClientBinding::registerListener(constanze::Listener*)
{
	// intentionally left empty
}
void TcpClientBinding::onConnectionClosed(wns::service::tl::Connection*)
{
	// intentionally left empty
}

void TcpClientBinding::onConnectionClosedByPeer(wns::service::tl::Connection*)
{
	// intentionally left empty
}

void TcpClientBinding::onConnectionLost(wns::service::tl::Connection*)
{
	// intentionally left empty
}

std::string
TcpClientBinding::printAddress() const
{
	std::ostringstream tmp;
	//tmp << destinationDomainName << ":" << destinationPort;
	tmp << peerAddress << ":" << destinationPort;
	return tmp.str();
}




