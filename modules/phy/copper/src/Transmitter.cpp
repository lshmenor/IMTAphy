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

#include <COPPER/Transmitter.hpp>
#include <COPPER/Transmission.hpp>
#include <COPPER/Wire.hpp>

using namespace copper;

Transmitter::Transmitter(
	const wns::pyconfig::View& pyco,
	WireInterface* w) :
	// init
	wire(w),
	dataRate(pyco.get<double>("dataRate")),
	sensingTime(pyco.get<double>("sensingTime")),
	logger(pyco.get("logger"))
	// body
{
}


Transmitter::~Transmitter()
{
}


void
Transmitter::sendData(
	const wns::service::dll::BroadcastAddress& /*peerAddress*/,
	const wns::osi::PDUPtr& data)
{
	BroadcastTransmissionPtr bt(new BroadcastTransmission(data, this));

	MESSAGE_SINGLE(NORMAL, this->logger, "sendData, broadcast");

	this->wire->sendData(bt, this->getDuration(data->getLengthInBits()));
}


void
Transmitter::sendData(
	const wns::service::dll::UnicastAddress& peerAddress,
	const wns::osi::PDUPtr& data)
{
	UnicastTransmissionPtr ut(new UnicastTransmission(peerAddress, data, this));

	MESSAGE_SINGLE(NORMAL, this->logger, "sendData, target's MAC address: " << peerAddress);

	wire->sendData(ut, this->getDuration(data->getLengthInBits()));
}


void
Transmitter::cancelData(
	const wns::osi::PDUPtr& pdu)
{
	MESSAGE_SINGLE(NORMAL, this->logger, "stopping transmission");
	this->wire->stopTransmission(pdu);
}


bool
Transmitter::isFree(
	) const
{
	return wire->blockedSince() < this->sensingTime;
}

void
Transmitter::onDataSent(wns::osi::PDUPtr pdu)
{
	this->sendNotifies(
		&wns::service::phy::copper::DataTransmissionFeedbackInterface::onDataSent,
		pdu);
}

simTimeType
Transmitter::getDuration(
	Bit len)
{
	return len/this->dataRate;
}



