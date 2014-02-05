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

#include <CONSTANZE/DllBinding.hpp>
#include <CONSTANZE/Generator.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/module/Base.hpp>
#include <WNS/service/dll/Address.hpp>

using namespace constanze;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	constanze::DllBinding,
	constanze::Binding,
	"DllBinding",
	wns::PyConfigViewCreator);

DllBinding::DllBinding(const wns::pyconfig::View& _pyco):
	pyco(_pyco),
	destinationDllAddress(pyco.get<long int>("destinationDllAddress")),
	log(pyco.get("logger"))
{
	MESSAGE_BEGIN(NORMAL, log, m, "New DllBinding created. DestinationMACAddress="<<destinationDllAddress);
	MESSAGE_END();
}

DllBinding::~DllBinding()
{
}

void DllBinding::registerComponent(wns::node::component::Component* _component)
{
	component = _component;
	dllDataTransmission =
			component->getService<wns::service::dll::UnicastDataTransmission*>(
				pyco.get<std::string>("dllDataTransmission"));

}
void DllBinding::sendData(const wns::osi::PDUPtr& _data){
	dllDataTransmission->sendData(destinationDllAddress, _data, wns::service::dll::CONSTANZE);
}

void DllBinding::registerListener(constanze::Listener*)
{
	// intentionally left empty
}

void DllBinding::initBinding(constanze::StartTrigger* _startTrigger)
{
	_startTrigger->bindingReady();
}

void DllBinding::releaseBinding(constanze::StopTrigger* _stopTrigger)
{
	_stopTrigger->bindingReady();
}

std::string	DllBinding::printAddress() const
{
        std::ostringstream tmp;
        tmp << destinationDllAddress;
        return tmp.str();
}
