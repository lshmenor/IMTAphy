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

#include <CONSTANZE/TcpListenerBinding.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/PortPool.hpp>
#include <WNS/module/Base.hpp>

using namespace constanze;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	constanze::TcpListenerBinding,
	constanze::Binding,
	"TcpListenerBinding",
	wns::PyConfigViewCreator);

TcpListenerBinding::TcpListenerBinding(const wns::pyconfig::View& _pyco):
	pyco(_pyco),
	listenPort(_pyco.get<int>("listenPort")),
	//log("CONST","TcpListenerBinding",wns::module::Base::getService<wns::logger::Master>("W-NS-MSG"))
	log(pyco.get("logger"))
{
	MESSAGE_BEGIN(NORMAL, log, m, "New TcpListenerBinding created on my socket=");
	m << ownAddress << ":" << listenPort;
	MESSAGE_END();
}

void TcpListenerBinding::sendData(const wns::osi::PDUPtr&)
{
	// intentionally left empty
}

void TcpListenerBinding::registerComponent(wns::node::component::Component* _component)
{
	component = _component;
	tcpService =
		component->getService<wns::service::tl::Service*>(
			pyco.get<std::string>("tcpService"));
	tcpService->listenOnPort(listenPort, this);
}

void TcpListenerBinding::onConnectionEstablished(wns::service::nl::Address _address, wns::service::tl::Connection* _connection)
{
	ownAddress = _address;
	_connection->registerDataHandler(this);
}

void TcpListenerBinding::onData(const wns::osi::PDUPtr& _data)
{
	assure(listener, "No listener set.");
	MESSAGE_BEGIN(NORMAL, log, m, "Received " << _data->getLengthInBits()/8 << " bytes"); // "from ?"
	m << " on my socket=" << ownAddress << ":" << listenPort;
	MESSAGE_END();
	listener->onData(_data);
}

void TcpListenerBinding::registerListener(constanze::Listener* _listener)
{
	listener = _listener;
}

void TcpListenerBinding::initBinding(constanze::StartTrigger* )
{
	// intentionally left empty
}

void TcpListenerBinding::releaseBinding(constanze::StopTrigger*)
{
	/** @todo at this stage connections cannot be closed by the peer */
}

void TcpListenerBinding::onConnectionClosed(wns::service::tl::Connection*)
{
	// TCP has no connection that can be closed
	// intentionally left empty
}

void TcpListenerBinding::onConnectionClosedByPeer(wns::service::tl::Connection*)
{
	// TCP has no connection that can be closed
	// intentionally left empty
}

void TcpListenerBinding::onConnectionLost(wns::service::tl::Connection*)
{
	// TCP has no connection that can be closed
	// intentionally left empty
}

std::string
TcpListenerBinding::printAddress() const
{
	std::ostringstream tmp;
	tmp << ownAddress << ":" << listenPort;
	return tmp.str();
}


