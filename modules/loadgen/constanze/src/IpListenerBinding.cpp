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

#include <CONSTANZE/IpListenerBinding.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/module/Base.hpp>
#include <WNS/service/nl/Address.hpp>

using namespace constanze;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	constanze::IpListenerBinding,
	constanze::Binding,
	"IpListenerBinding",
	wns::PyConfigViewCreator);

IpListenerBinding::IpListenerBinding(const wns::pyconfig::View& _pyco):
	pyco(_pyco),
	component(NULL),
	ipNotification(NULL),
	listener(NULL),
	listenDomainName(pyco.get<std::string>("listenDomainName")),
	log(pyco.get("logger"))
{
	MESSAGE_BEGIN(NORMAL, log, m, "New IpListenerBinding created for own IP=");
	m << listenDomainName; // own IP address
	MESSAGE_END();
}

void IpListenerBinding::sendData(const wns::osi::PDUPtr&)
{
	// intentionally left empty
}

void IpListenerBinding::registerComponent(wns::node::component::Component* _component)
{
	component = _component;
	ipNotification =
		component->getService<wns::service::nl::Notification*>(
			pyco.get<std::string>("ipNotification"));
	ipNotification->registerHandler(wns::service::nl::Constanze, this);
}

void IpListenerBinding::onData(wns::service::nl::Address
#ifndef WNS_NO_LOGGING
                               _address
#endif
                               , const wns::osi::PDUPtr& _data)
{
	assure(listener, "No listener set.");
	MESSAGE_BEGIN(NORMAL, log, m, "Received " << _data->getLengthInBits()/8 << " bytes from IP=" << _address);
	MESSAGE_END();
	listener->onData(_data);
}

void IpListenerBinding::registerListener(constanze::Listener* _listener)
{
	listener = _listener;
}

void IpListenerBinding::initBinding(constanze::StartTrigger* _trigger)
{
	_trigger->bindingReady();
}

void IpListenerBinding::releaseBinding(constanze::StopTrigger* _trigger)
{
	_trigger->bindingReady();
}

std::string	IpListenerBinding::printAddress() const
{
	std::ostringstream tmp;
	tmp << listenDomainName;
	return tmp.str();
}

