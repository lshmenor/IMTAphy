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

#include <TCP/Service.hpp>
#include <TCP/Connection.hpp>
#include <TCP/UpperConvergence.hpp>
#include <TCP/LowerConvergence.hpp>
#include <TCP/ConnectionHandler.hpp>
#include <TCP/HandshakeStrategyInterface.hpp>
#include <TCP/FlowHandler.hpp>
#include <TCP/FlowIDBuilder.hpp>

#include <WNS/service/tl/PortPool.hpp>
#include <WNS/service/tl/DataHandler.hpp>
#include <WNS/service/tl/FlowID.hpp>

#include <WNS/module/Base.hpp>
#include <WNS/service/dll/FlowEstablishmentAndRelease.hpp>

using namespace tcp;

Service::Service(UpperConvergence* _upperConvergence,
		 LowerConvergence* _lowerConvergence,
		 wns::ldk::FlowSeparator* _flowSeparator,
		 const wns::pyconfig::View& _pyco,
		 tcp::FlowHandler* _tcpFlowHandler,
		 wns::service::dll::FlowEstablishmentAndRelease* _fear,
		 wns::ldk::ControlServiceRegistry* _csr) :
	wns::ldk::ControlService(_csr),
	removeDelay(2),
	ipService(NULL),
	dns(NULL),
	flowIDToConnectionHandler(),
	listeners(),
	portPool(_pyco.get<simTimeType>("portUnbindDelay")),
	logger(_pyco.get("logger")),
	pyco(_pyco),
	upperConvergence(_upperConvergence),
	lowerConvergence(_lowerConvergence),
	flowSeparator(_flowSeparator),
	flowEstablishmentAndRelease(_fear),
	tcpFlowHandler(_tcpFlowHandler)
{
	upperConvergence->setTLService(this);
}

Service::~Service()
{
}

void
Service::listenOnPort(
	wns::service::tl::Port _port,
	wns::service::tl::ConnectionHandler* _ch)
{
	assure(_port >= 0 && _port <= 65535, "Valid port number are: 0-65535 (is " << _port << ")");
	assure(_ch, "No valid ConnectionHandler provided (is NULL)");
	assure(portPool.isAvailable(_port), "Port already bound (is " << _port << ")");

	portPool.bind(_port);
	listeners.insert(_port, _ch);

	MESSAGE_SINGLE(NORMAL,  logger, "Listening on port " << _port << ": " << listeners.knows(_port));
}

void
Service::stopListenOnPort(const wns::service::tl::Port _port)
{
	assure(listeners.knows(_port), "Not currently listening on port.");

	MESSAGE_SINGLE(NORMAL, logger, "Stopping listenOnPort for port " << _port);
	listeners.erase(_port);
	portPool.unbind(_port);
}

void
Service::openConnection(
	wns::service::tl::Port _dstPort,
	wns::service::nl::FQDN _source,
	wns::service::nl::FQDN _dstAddress,
	wns::service::qos::QoSClass _qosClass,
	wns::service::tl::ConnectionHandler* _ch)
{
	assure(_dstPort >= 0 && _dstPort <= 65535, "Valid port numbers are: 0-65535 (is " << _dstPort << ")");
	assure(_ch, "No valid ConnectionHandler provided (is NULL)");
	assure(dns, "No DNS service is set");

	wns::service::tl::Port srcPort = portPool.suggestPort();
	portPool.bind(srcPort);

	wns::service::nl::Address source = dns->lookup(_source);
	wns::service::nl::Address destination = dns->lookup(_dstAddress);
    wns::service::tl::FlowID flowID(source, srcPort, destination, _dstPort);

	flowIDToConnectionHandler.insert(flowID, _ch);

	assure(!flowIDToConnections.knows(flowID), "Connection already created for flowID " << flowID);
	MESSAGE_SINGLE(NORMAL, logger, "TCP OpenConnection called for: " << flowID);

	if(flowEstablishmentAndRelease == NULL)
	{
		openConnectionContinue(flowID, 0);
	}
	else
	{
		MESSAGE_SINGLE(NORMAL, logger, "Triggering FlowEstablishment at Layer2 for FlowID: " <<flowID);
		flowEstablishmentAndRelease->establishFlow(flowID,_qosClass);
	}
}

void
Service::openConnectionContinue(wns::service::tl::FlowID flowID, wns::service::dll::FlowID dllFlowID)
{
	Connection* connection = new Connection(
		flowID, upperConvergence, this->pyco.get<wns::pyconfig::View>("connection"));

	lowerConvergence->mapFlowID(flowID, dllFlowID);

	assure(!flowIDToConnections.knows(flowID), "There is already a Connection for flowID "<< flowID);
	flowIDToConnections.insert(flowID, connection);

    MESSAGE_SINGLE(NORMAL, logger, "Opening connection for FlowID " << flowID);
    assure(!listeners.knows(flowID.srcPort), "Unable to open source port " << flowID  << " actively. Application is listening.");
	addFlowSeparatorInstance(flowID);

	// get handshake fu from instance of flow separator according to flowID
    wns::ldk::ConstKeyPtr key(new FlowIDKey(flowID));
    assure(flowSeparator->getInstance(key)!=NULL,"No flow created according to flowID " << flowID);
	HandshakeStrategyInterface* handshakeStrategy = getHandshakeStrategyFU(flowID);

	handshakeStrategy->registerStrategyHandler(this);
	handshakeStrategy->activeOpen(flowID);
}

void
Service::dllFlowChanged(wns::service::tl::FlowID flowID, wns::service::dll::FlowID dllFlowID)
{
       // delete old Flow, save new one
	lowerConvergence->unmapFlowID(flowID);
	lowerConvergence->mapFlowID(flowID, dllFlowID);
}

void
Service::passiveOpenConnection(const wns::service::tl::FlowID& _flowID)
{
	wns::service::tl::ConnectionHandler* ch = listeners.find(_flowID.srcPort);

    assure(!flowIDToConnectionHandler.knows(_flowID), "Connection Handler already registered for flowID " << _flowID);
	flowIDToConnectionHandler.insert(_flowID, ch);

    assure(!flowIDToConnections.knows(_flowID), "Connection already created for flowID " << _flowID);
	Connection* connection = new Connection(
		_flowID, upperConvergence, this->pyco.get<wns::pyconfig::View>("connection"));

	flowIDToConnections.insert(_flowID, connection);

    wns::ldk::ConstKeyPtr key(new FlowIDKey(_flowID));
    assure(flowSeparator->getInstance(key)!=NULL,"No flow created according to flowID " << _flowID);
	HandshakeStrategyInterface* handshakeStrategy = getHandshakeStrategyFU(_flowID);
	handshakeStrategy->registerStrategyHandler(this);
	handshakeStrategy->passiveOpen(_flowID);
}

void
Service::addFlowSeparatorInstance(const wns::service::tl::FlowID& _flowID)
{
	MESSAGE_SINGLE(VERBOSE, this->logger, "Asking FlowSeparator to create instance for flowID: " << _flowID);
	// The instance must not be available!
    wns::ldk::ConstKeyPtr key(new FlowIDKey(_flowID));
	assure(flowSeparator->getInstance(key) == NULL, "Instance of prototype flow separator already generated for flow id!");
	flowSeparator->addInstance(key);
}

HandshakeStrategyInterface*
Service::getHandshakeStrategyFU(const wns::service::tl::FlowID& _flowID)
{
	// get handshake fu from instance of flow separator according to flowID
    wns::ldk::ConstKeyPtr key(new FlowIDKey(_flowID));
    assure(flowSeparator->getInstance(key)!=NULL,
           "No instance of flow separator created for flowID " << _flowID);
	HandshakeStrategyInterface* hs = dynamic_cast<HandshakeStrategyInterface*>(
		dynamic_cast<wns::ldk::Group*>(
			flowSeparator->getInstance(key))->getSubFUN()->getFunctionalUnit(pyco.get<std::string>("handshakeStrategy")));

	return hs;
}

void
Service::connectionEstablished(const wns::service::tl::FlowID& _flowID)
{
	Connection* connection = flowIDToConnections.find(_flowID);

	flowIDToConnectionHandler.find(_flowID)->onConnectionEstablished(_flowID.dstAddress, connection);
    MESSAGE_SINGLE(NORMAL, logger, "Connection established for flow id " << _flowID);
}

void
Service::closeConnection(wns::service::tl::Connection* _connection)
{
	assure(_connection, "No valid Connection provided (is NULL)");
	assureType(_connection, tcp::Connection*);

	wns::service::tl::FlowID flowID = dynamic_cast<tcp::Connection*>(_connection)->getFlowID();

	HandshakeStrategyInterface* hs = getHandshakeStrategyFU(flowID);
	hs->closeConnection(flowID);
}

void
Service::setDataTransmissionService(wns::service::nl::Service* _ipService)
{
	assure(!ipService, "ipService already set!");
	assure(_ipService, "invalid ip service (NULL)");

	ipService = _ipService;

	MESSAGE_SINGLE(NORMAL, logger, "IP services registered.");
}

void
Service::setTcpFlowHandlerService(tcp::FlowHandler* _flowHandler)
{
	assure(_flowHandler, "invalid TcpFlowHandler service (NULL)!");
	tcpFlowHandler = _flowHandler;
	MESSAGE_SINGLE(NORMAL, logger, "TcpFlowHandler service registered.");
}

void
Service::setDNSService(wns::service::nl::DNSService* _dns)
{
	assure(!dns, "DNSService already set!");
	assure(_dns, "invalid DNS service (NULL)");

	dns = _dns;

	MESSAGE_SINGLE(NORMAL, logger, "DNS services registered.");
}

bool
Service::isListening(wns::service::tl::Port _port)
{
	return listeners.knows(_port);
}

wns::service::nl::Service*
Service::getIPService()
{
	return ipService;
}

Connection*
Service::getConnection(const wns::service::tl::FlowID& _flowID)
{
	/**
	 * This member function is called from the UpperConvergence. Compounds
	 * which arrive at the UpperConvergence belong to a valid flow.
	 * In case of the user datagram protocol a new connection is created for a
	 * server if the first incoming compound is processed by the upper convergence.
	 */
	if(!flowIDToConnections.knows(_flowID)){ // server: first compound arrived
		/**
		 * Valid flow. On server side a new connection has to be create
		 * if firs compound arrived.
		 */
		passiveOpenConnection(_flowID);
	}

    assure(flowIDToConnections.knows(_flowID), "No connection created for flowID " << _flowID);
	return flowIDToConnections.find(_flowID);
}

bool
Service::isValidFlow(const wns::ldk::ConstKeyPtr& _key) const
{
    ConstFlowIDKeyPtr key = wns::dynamicCast<const FlowIDKey>(_key);
    wns::service::tl::FlowID flowID = key->flowID_;

	MESSAGE_SINGLE(VERBOSE, logger, "isValidFlow(): flowID: " << flowID);

    if(listeners.knows(flowID.srcPort))
	{ // server: incoming compound
		//const_cast<Service*>(this)->passiveOpenConnection(*flowID);
		return true;
	}
	else if(flowIDToConnections.knows(flowID))
	{
		// no connection open on client side
		return true;
	}
	else
	{
		return false;
	}
}

void
Service::connectionClosed(const wns::service::tl::FlowID& _flowID)
{
    assure(flowIDToConnections.knows(_flowID), "No connection created for flow id " << _flowID);
	Connection* _connection = flowIDToConnections.find(_flowID);
    assure(flowIDToConnectionHandler.knows(_flowID), "No connection handler registered for flow id " << _flowID);
	wns::service::tl::ConnectionHandler* ch = flowIDToConnectionHandler.find(_flowID);

	if(listeners.knows(_flowID.srcPort)){ // server
		ch->onConnectionClosed(_connection);
		flowIDToConnectionHandler.erase(_flowID);
		flowIDToConnections.erase(_flowID);
	}
	else { // client
		ch->onConnectionClosed(_connection);
		flowIDToConnectionHandler.erase(_flowID);
		flowIDToConnections.erase(_flowID);
		portPool.unbind(_flowID.srcPort);
	}

	wns::simulator::getEventScheduler()->scheduleNow(RemoveFlowSeparatorInstance(this, _flowID));
	// release the flow in layer 2
	lowerConvergence->unmapFlowID(_flowID);

	if(!(flowEstablishmentAndRelease == NULL))
	{
		MESSAGE_SINGLE(NORMAL, logger, "Deleting DLLFlow for TlFlowID: " <<_flowID);
		flowEstablishmentAndRelease->releaseFlow(_flowID);
	}

	// release flow in layer 2 only if there is a layer 2 at this peer
	delete _connection;
}

void
Service::passiveClosed(const wns::service::tl::FlowID& _flowID)
{
    assure(flowIDToConnections.knows(_flowID), "No connection created for flow id " << _flowID);
	Connection* _connection = flowIDToConnections.find(_flowID);
    assure(flowIDToConnectionHandler.knows(_flowID), "No connection handler registered for flow id " << _flowID);
	wns::service::tl::ConnectionHandler* ch = flowIDToConnectionHandler.find(_flowID);

	ch->onConnectionClosedByPeer(_connection);
	flowIDToConnectionHandler.erase(_flowID);
	flowIDToConnections.erase(_flowID);

	wns::simulator::getEventScheduler()->scheduleNow(RemoveFlowSeparatorInstance(this, _flowID));
	// the right place to release the flow in layer 2
	// release flow in layer 2 only if there is a layer 2 at this peer
	delete _connection;
}
void
Service::deleteFlowSeparatorInstance(const wns::service::tl::FlowID& _flowID)
{
    wns::ldk::ConstKeyPtr key(new FlowIDKey(_flowID));
    assure(flowSeparator->getInstance(key)!=NULL, "No instance with flowID " << _flowID);
	flowSeparator->removeInstance(key);
}
