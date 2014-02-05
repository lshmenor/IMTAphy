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

#include <CONSTANZE/UdpListenerBinding.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/PortPool.hpp>
#include <WNS/module/Base.hpp>

using namespace constanze;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	constanze::UdpListenerBinding,
	constanze::Binding,
	"UdpListenerBinding",
	wns::PyConfigViewCreator);

UdpListenerBinding::UdpListenerBinding(const wns::pyconfig::View& _pyco):
	pyco(_pyco),
	listenPort(_pyco.get<int>("listenPort")),
	//log("CONST","UdpListenerBinding",wns::module::Base::getService<wns::logger::Master>("W-NS-MSG"))
	log(pyco.get("logger"))
{
	MESSAGE_BEGIN(NORMAL, log, m, "New UdpListenerBinding created on my socket=");
	m << ownAddress << ":" << listenPort;
	MESSAGE_END();
}

void UdpListenerBinding::sendData(const wns::osi::PDUPtr&)
{
	// intentionally left empty
}

void UdpListenerBinding::registerComponent(wns::node::component::Component* _component)
{
	component = _component;
		udpService =
			component->getService<wns::service::tl::Service*>(
				pyco.get<std::string>("udpService"));
		udpService->listenOnPort(listenPort, this);
}

void UdpListenerBinding::onConnectionEstablished(wns::service::nl::Address _address, wns::service::tl::Connection* _connection)
{
	ownAddress = _address;
	_connection->registerDataHandler(this);
}

void UdpListenerBinding::onData(const wns::osi::PDUPtr& _data)
{
	assure(listener, "No listener set.");
	MESSAGE_BEGIN(NORMAL, log, m, "Received " << _data->getLengthInBits()/8 << " bytes"); // "from ?"
	m << " on my socket=" << ownAddress << ":" << listenPort;
	MESSAGE_END();
	listener->onData(_data);
}

void UdpListenerBinding::registerListener(constanze::Listener* _listener)
{
	listener = _listener;
}

void UdpListenerBinding::initBinding(constanze::StartTrigger* )
{
	// intentionally left empty
}

void UdpListenerBinding::releaseBinding(constanze::StopTrigger*)
{
	/** @todo at this stage connections cannot be closed by the peer */
}

void UdpListenerBinding::onConnectionClosed(wns::service::tl::Connection*)
{
	// UDP has no connection that can be closed
	// intentionally left empty
}

void UdpListenerBinding::onConnectionClosedByPeer(wns::service::tl::Connection*)
{
	// UDP has no connection that can be closed
	// intentionally left empty
}

void UdpListenerBinding::onConnectionLost(wns::service::tl::Connection*)
{
	// UDP has no connection that can be closed
	// intentionally left empty
}

std::string	UdpListenerBinding::printAddress() const
{
        std::ostringstream tmp;
        tmp << ownAddress << ":" << listenPort;
        return tmp.str();
}


