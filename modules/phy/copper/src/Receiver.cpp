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

#include <COPPER/Receiver.hpp>
#include <COPPER/Wire.hpp>
#include <COPPER/Transmission.hpp>
#include <WNS/events/MemberFunction.hpp>
#include <string>

using namespace copper;

Receiver::Receiver(const wns::pyconfig::View& _pyco, WireInterface* _wire) :
	macAddress(),
	wire(_wire),
	berDist(NULL),
	sensingTime(_pyco.get<simTimeType>("sensingTime")),
	logger(_pyco.get("logger"))
{
	wns::pyconfig::View distConfig = _pyco.get("ber");
	wns::distribution::DistributionCreator* dc =
		wns::distribution::DistributionFactory::creator(distConfig.get<std::string>("__plugin__"));
	this->berDist = dc->create(distConfig);
}

Receiver::~Receiver()
{
}

bool
Receiver::onData(const UnicastTransmissionPtr& transmission)
{
	if (this->macAddress == transmission->target)
	{
		double ber = (*berDist)();
		MESSAGE_SINGLE(NORMAL, this->logger, "Received unicast data with BER: " << ber);
		this->wns::Subject<Handler>::forEachObserver(
			OnData(transmission->pdu, ber, transmission->collision));
		return true;
	}
	else
	{
		return false;
	}
}

bool
Receiver::onData(const BroadcastTransmissionPtr& transmission)
{
	double ber = (*berDist)();
	MESSAGE_SINGLE(NORMAL, this->logger, "Received broadcast data with BER: " << ber);
	this->wns::Subject<Handler>::forEachObserver(
		OnData(transmission->pdu, ber, transmission->collision));
	return true;
}

void
Receiver::onCopperFree()
{
	this->wns::Subject<CarrierSensing>::forEachObserver(
		wns::events::DelayedMemberFunction<CarrierSensing>(
			&CarrierSensing::onCarrierIdle,
			this->sensingTime));
}

void
Receiver::onCopperBusy()
{
	this->wns::Subject<CarrierSensing>::forEachObserver(
		wns::events::DelayedMemberFunction<CarrierSensing>(
			&CarrierSensing::onCarrierBusy,
			this->sensingTime));
}

void
Receiver::onCollision()
{
	this->wns::Subject<CarrierSensing>::forEachObserver(
		wns::events::DelayedMemberFunction<CarrierSensing>(
			&CarrierSensing::onCollision,
			this->sensingTime));
}

void
Receiver::setDLLUnicastAddress(const wns::service::dll::UnicastAddress& _macAddress)
{
	assure(this->macAddress.isValid() == false, "MAC address may only be set once!");
	assure(_macAddress.isValid() == true, "Provided invalid MAC address!");

	this->macAddress = _macAddress;
	this->wire->addReceiver(this, this->macAddress);
}


