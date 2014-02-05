 /******************************************************************************
 * TCP ServerListener Binding                                                 *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 *****************************************************************************/

#include <CONSTANZE/TcpServerListenerBinding.hpp>
#include <CONSTANZE/TcpServerBinding.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/PortPool.hpp>
#include <WNS/module/Base.hpp>

using namespace constanze;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	constanze::TcpServerListenerBinding,
	constanze::Binding,
	"TcpServerListenerBinding",
	wns::PyConfigViewCreator);

TcpServerListenerBinding::TcpServerListenerBinding(const wns::pyconfig::View& _pyco):
	pyco(_pyco),
	log(pyco.get("logger")),
	dns(NULL),
	listenPort(_pyco.get<int>("listenPort")),
	generatorConfig(pyco.get("generator"))
{
	c = GeneratorBase::Factory::creator(generatorConfig.get<std::string>("__plugin__"));

	// who sets ownAddress??? [rs]
	MESSAGE_BEGIN(NORMAL, log, m, "New TcpServerListenerBinding created on my socket=");
	m << ownAddress << ":" << listenPort;
	MESSAGE_END();
}

TcpServerListenerBinding::~TcpServerListenerBinding()
{
  for(std::list<GeneratorBase*>::const_iterator iter = generatorList.begin(); iter != generatorList.end(); ++iter) {
    delete (*iter);
  }
}

void TcpServerListenerBinding::sendData(const wns::osi::PDUPtr&)
{
	// use registry<Generator, Connection> to send the data
}

void TcpServerListenerBinding::registerComponent(wns::node::component::Component* _component)
{
	component = _component;
	tcpService =
		component->getService<wns::service::tl::Service*>(
			pyco.get<std::string>("tcpService"));
	tcpService->listenOnPort(listenPort, this);
	dns = component->getService<wns::service::nl::DNSService*>(pyco.get<std::string>("dnsService"));
}

void
TcpServerListenerBinding::onConnectionEstablished(wns::service::nl::Address _address, wns::service::tl::Connection* _connection)
{
	// _address is not my address, but the one of the peer node;
	// my address can be found in connection's flow ID.
	// Create a Generator on top of this Binding
	// Of course handling of all connection pointers is necessary.
	// A registry with mapping of Generator to Connection#
	// Generator has to get its parameters either from here or from python
	peerAddress = _address;
	_connection->registerDataHandler(this);

	//generate the traffic generator:
	GeneratorBase* generator = c->create(generatorConfig);

	MESSAGE_BEGIN(NORMAL, log, m, "");
	m << "onConnectionEstablished() called from peer "<<peerAddress;
	MESSAGE_END();

	// save (generator, connection):
	//generatorToConnectionRegistry.insert(generator, _connection);

	// generate the TcpServerBinding for the generator:
	TcpServerBinding* serverBinding = new constanze::TcpServerBinding(_address, _connection, log);

	assure(dns!=NULL,"dns needed");
	//ownAddress = serverBinding->???();
	//ownAddress = listener->printAddress();
	ownAddress = dns->lookup(listener->printAddress());

	MESSAGE_BEGIN(NORMAL, log, m, "");
	m << "onConnectionEstablished(): me="<<ownAddress<<":"<<listenPort;
	MESSAGE_END();

	// Tell the generator which binding to use:
	generator->registerBinding(serverBinding);
	generator->bindingReady();
	// keep a list of all generated generators
	generatorList.push_back(generator);
}

void TcpServerListenerBinding::onData(const wns::osi::PDUPtr& _data)
{
	assure(listener, "No listener set.");
	MESSAGE_BEGIN(NORMAL, log, m, "Received " << _data->getLengthInBits()/8 << " bytes"); // "from ?"
	m << " on my socket=" << ownAddress << ":" << listenPort;
	MESSAGE_END();
	listener->onData(_data);
}

void TcpServerListenerBinding::registerListener(constanze::Listener* _listener)
{
	listener = _listener;
}

void TcpServerListenerBinding::initBinding(constanze::StartTrigger* )
{
	// intentionally left empty
}

void TcpServerListenerBinding::releaseBinding(constanze::StopTrigger*)
{
	/** @todo at this stage connections cannot be closed by the peer */
}

void TcpServerListenerBinding::onConnectionClosed(wns::service::tl::Connection*)
{
	// TCP has no connection that can be closed
	// intentionally left empty
}

void TcpServerListenerBinding::onConnectionClosedByPeer(wns::service::tl::Connection*)
{
	// TCP has no connection that can be closed
	// intentionally left empty
}

void TcpServerListenerBinding::onConnectionLost(wns::service::tl::Connection*)
{
	// TCP has no connection that can be closed
	// intentionally left empty
}

std::string
TcpServerListenerBinding::printAddress() const
{
	std::ostringstream tmp;
	//tmp << ownAddress << ":" << listenPort;
	tmp << peerAddress;
	return tmp.str();
}


