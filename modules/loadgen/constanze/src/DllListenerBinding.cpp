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

#include <CONSTANZE/DllListenerBinding.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/module/Base.hpp>
#include <WNS/service/dll/Address.hpp>

using namespace constanze;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	constanze::DllListenerBinding,
	constanze::Binding,
	"DllListenerBinding",
	wns::PyConfigViewCreator);

DllListenerBinding::DllListenerBinding(const wns::pyconfig::View& _pyco):
	pyco(_pyco),
	listenDll(pyco.get<long int>("listenDll")),
	log(pyco.get("logger"))
{
	MESSAGE_BEGIN(NORMAL, log, m, "New DllListenerBinding created for own MACAddress=");
	m << listenDll; // own MAC address
	MESSAGE_END();
}

void DllListenerBinding::sendData(const wns::osi::PDUPtr&)
{
	// intentionally left empty
}

void DllListenerBinding::registerComponent(wns::node::component::Component* _component)
{
	component = _component;
	dllNotification =
		component->getService<wns::service::dll::Notification*>(
			pyco.get<std::string>("dllNotification"));
	dllNotification->registerHandler(wns::service::dll::CONSTANZE, this);
}

void DllListenerBinding::onData(const wns::osi::PDUPtr& _data, wns::service::dll::FlowID /*dllFlowID*/)
{
	assure(listener, "No listener set.");
	MESSAGE_BEGIN(NORMAL, log, m, "Received " << _data->getLengthInBits()/8 << " bytes"); // "from ?"
	MESSAGE_END();
	listener->onData(_data);
}

void DllListenerBinding::registerListener(constanze::Listener* _listener)
{
	listener = _listener;
}

void DllListenerBinding::initBinding(constanze::StartTrigger* _trigger)
{
	_trigger->bindingReady();
}

void DllListenerBinding::releaseBinding(constanze::StopTrigger* _trigger)
{
	_trigger->bindingReady();
}

std::string	DllListenerBinding::printAddress() const
{
        std::ostringstream tmp;
        tmp << listenDll;
        return tmp.str();
}

