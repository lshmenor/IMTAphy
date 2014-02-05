/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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

#include <COPPER/Transceiver.hpp>
#include <COPPER/Wire.hpp>
#include <COPPER/Transmission.hpp>
#include <COPPER/Transmitter.hpp>
#include <COPPER/Receiver.hpp>

#include <WNS/node/Node.hpp>


using namespace copper;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	Transceiver,
	wns::node::component::Interface,
	"copper.Transceiver",
	wns::node::component::ConfigCreator
	);


Transceiver::Transceiver(
	wns::node::Interface* node,
	const wns::pyconfig::View& pyco) :

	wns::node::component::Component(node, pyco),
	transmitter(NULL),
	receiver(NULL),
	logger(pyco.get<wns::pyconfig::View>("logger"))
{
}

void
Transceiver::doStartup()
{
	wns::pyconfig::View pyco = this->getConfig();
	// No need to store this here, the Broker keeps the instances ...
	WireInterface* wire = Transceiver::getWireBroker().procure(pyco.get("wire"));

	this->transmitter = new Transmitter(pyco.get("transmitter"), wire);

	this->receiver = new Receiver(pyco.get("receiver"), wire);
	this->addService(pyco.get<std::string>("dataTransmission"), transmitter);
	this->addService(pyco.get<std::string>("dataTransmissionFeedback"), transmitter);
	this->addService(pyco.get<std::string>("notification"), receiver);
}

Transceiver::~Transceiver()
{
	if (this->transmitter != NULL)
	{
		delete this->transmitter;
	}
	if (this->receiver != NULL)
	{
		delete this->receiver;
	}
}


void
Transceiver::onNodeCreated()
{
}


void
Transceiver::onWorldCreated()
{
}


void
Transceiver::onShutdown()
{
}


copper::WireBroker&
Transceiver::getWireBroker()
{
	static WireBroker b;
	return b;
}


