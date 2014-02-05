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

#include <COPPER/Transmission.hpp>

using namespace copper;

Transmission::Transmission(
	const wns::osi::PDUPtr& _pdu,
	TransmitterDataSentInterface* _sender) :
	// initialization
	pdu(_pdu),
	collision(false),
	sender(_sender)
	// body
{
	assure(this->pdu != wns::osi::PDUPtr(), "pdu may not be NULL");
	assureNotNull(this->sender);
}


Transmission::~Transmission()
{
}


BroadcastTransmission::BroadcastTransmission(
	const wns::osi::PDUPtr& _pdu,
	TransmitterDataSentInterface* _sender) :
	// initialization
	Transmission(_pdu, _sender)
	// body
{
}


UnicastTransmission::UnicastTransmission(
	const wns::service::dll::UnicastAddress& _target,
	const wns::osi::PDUPtr& _pdu,
	TransmitterDataSentInterface* _sender) :
	// initialization
	Transmission(_pdu, _sender),
	target(_target)
	// body
{
}



