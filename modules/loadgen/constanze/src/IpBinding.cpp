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
#include <CONSTANZE/IpBinding.hpp>
#include <CONSTANZE/Generator.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/module/Base.hpp>
#include <WNS/service/nl/Address.hpp>

using namespace constanze;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	constanze::IpBinding,
	constanze::Binding,
	"IpBinding",
	wns::PyConfigViewCreator);

IpBinding::IpBinding(const wns::pyconfig::View& _pyco):
	pyco(_pyco),
	ipDataTransmission(NULL),
	dns(NULL),
	sourceDomainName(_pyco.get<wns::service::nl::FQDN>("sourceDomainName")),
	destinationDomainName(_pyco.get<wns::service::nl::FQDN>("destinationDomainName")),
	log(pyco.get("logger"))
{
	MESSAGE_BEGIN(NORMAL, log, m, "New IpBinding created. DestinationIP="<<destinationDomainName);
	MESSAGE_END();
}

IpBinding::~IpBinding()
{
}

void IpBinding::registerComponent(wns::node::component::Component* _component)
{
	component = _component;
	ipDataTransmission =
	  component->getService<wns::service::nl::Service*>(pyco.get<std::string>("ipDataTransmission"));

	dns =
	  component->getService<wns::service::nl::DNSService*>(pyco.get<std::string>("dnsService"));
}

void IpBinding::sendData(const wns::osi::PDUPtr& _data){
	assure(dns!=NULL,"dns needed");
	wns::service::nl::Address sourceIpAddress(dns->lookup(sourceDomainName));
	wns::service::nl::Address destinationIpAddress(dns->lookup(destinationDomainName));
	ipDataTransmission->sendData(sourceIpAddress, destinationIpAddress, _data, wns::service::nl::Constanze);
}

void IpBinding::registerListener(constanze::Listener*)
{
	// intentionally left empty
}

void IpBinding::initBinding(constanze::StartTrigger* _startTrigger)
{
	_startTrigger->bindingReady();
}

void IpBinding::releaseBinding(constanze::StopTrigger* _stopTrigger)
{
	_stopTrigger->bindingReady();
}

std::string IpBinding::printAddress() const
{
	std::ostringstream tmp;
	tmp << destinationDomainName;
	return tmp.str();
}
