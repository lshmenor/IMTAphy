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

#include <COPPER/Wire.hpp>
#include <COPPER/Transceiver.hpp>
#include <COPPER/Receiver.hpp>
#include <COPPER/Transmission.hpp>
#include <COPPER/Transmitter.hpp>

#include <WNS/service/phy/copper/Handler.hpp>
#include <WNS/service/dll/Address.hpp>

#include <WNS/Exception.hpp>
#include <WNS/rng/RNGen.hpp>

using namespace copper;

Wire::Wire(const wns::pyconfig::View& config) :
	name(config.get<std::string>("name")),
	roundRobin(),
	receivers(),
	transmissionEndEvents(),
	transmissions(),
	addressMapping(),
	logger(config.get<wns::pyconfig::View>("logger"))
{
	MESSAGE_BEGIN(NORMAL, logger, m, "Created wire: ");
	m << this->name;
	MESSAGE_END();
}

void
Wire::sendData(const UnicastTransmissionPtr& ut, simTimeType duration)
{
	assure(
		this->addressMapping.find(ut->target) != this->addressMapping.end(),
		"Target receiver not at this wire");

	simTimeType arrivalTime = sendDataGeneric(ut, duration);

 	MESSAGE_SINGLE(
		       NORMAL, this->logger,
		       "Sending to MAC address: " << ut->target
		       << ". Arrival time: " << arrivalTime);

}

void
Wire::sendData(const BroadcastTransmissionPtr& bt, simTimeType duration)
{
	simTimeType arrivalTime = sendDataGeneric(bt, duration);

	MESSAGE_SINGLE(
		NORMAL, this->logger,
		"Sending to: BROADCAST. Arrival time: " << arrivalTime);
}

simTimeType
Wire::blockedSince() const
{
	if (this->isFree())
	{
		// return a number < 0 (doesn't matter)
		return -1.0;
	}
	else
	{
		// the wire is blocked, return the time for which the wire
		// has been blocked
		return wns::simulator::getEventScheduler()->getTime() - this->timeWireBlocked;
	}
}


bool
Wire::isFree() const
{
	return this->transmissions.empty();
}

void
Wire::addReceiver(
	ReceiverInterface* r,
	const wns::service::dll::UnicastAddress& macAddress)
{
	assure(r, "wo must be non-NULL");
	MESSAGE_SINGLE(
		NORMAL, this->logger,
		"adding receiver with MAC address" << macAddress);

	this->roundRobin.add(r);
	this->receivers.push_back(r);
	if (this->addressMapping.find(macAddress) != this->addressMapping.end())
	{
		wns::Exception e;
		e << "Receiver with this MAC address ("
		  << macAddress
		  <<") is already registered";
		throw e;
	}
	else
	{
		this->addressMapping[macAddress] = r;
	}
}

void
Wire::stopTransmission(const wns::osi::PDUPtr& pdu)
{
	assure(
		this->transmissions.find(pdu) != this->transmissions.end(),
		"Transmission not active");

	TransmissionEndEventContainer::iterator itr =
		this->transmissionEndEvents.find(pdu);

	// delete event from EventScheduler
	wns::simulator::getEventScheduler()->cancelEvent(itr->second);
	// manually execute the event to stop transmission
	this->transmissionEndEvents.erase(itr);

	this->transmissions.erase(pdu);

	if (this->isFree())
	{
		this->signalCopperFreeAgainToReceivers();
	}

}

void
Wire::removeTransmissionEndEvent(const TransmissionPtr& transmission)
{
	assure(
		this->transmissions.find(transmission->pdu) != this->transmissions.end(),
		"No such transmission active");

	this->transmissionEndEvents.erase(transmission->pdu);
	this->transmissions.erase(transmission->pdu);
}

void
Wire::stopTransmission(const UnicastTransmissionPtr& ut)
{
	assure(ut, "must be non-NULL");

	Address2ReceiverContainer::iterator itr =
		this->addressMapping.find(ut->target);

	assure(itr != this->addressMapping.end(), "Target receiver not at this wire");

	this->removeTransmissionEndEvent(ut);

	// inform sender, that the data has been sent
	ut->sender->onDataSent(ut->pdu);

	// inform the receiver, that there is data available
	itr->second->onData(ut);

	MESSAGE_SINGLE(NORMAL, this->logger, "UnicastTransmission finished");

	if (this->isFree())
	{
		this->signalCopperFreeAgainToReceivers();
	}
}

void
Wire::stopTransmission(const BroadcastTransmissionPtr& bt)
{
	assure(bt, "must be non-NULL");

	this->removeTransmissionEndEvent(bt);

	// inform sender, that the data has been sent
	bt->sender->onDataSent(bt->pdu);

	// inform the receivers, that there is data available
	for (
		std::list<ReceiverInterface*>::iterator itr = this->receivers.begin();
		itr != this->receivers.end();
		++itr)
	{
		(*itr)->onData(bt);
	}

	MESSAGE_SINGLE(NORMAL, this->logger, "BroadcastTransmission finished");

	if(this->isFree())
	{
		this->signalCopperFreeAgainToReceivers();
	}
}

void
Wire::checkForCollision(const TransmissionPtr& t)
{
	if(!this->isFree())
	{
		for(
			Transmissions::iterator itr = this->transmissions.begin();
			itr != this->transmissions.end();
			++itr)
		{
			itr->second->collision = true;
		}
		t->collision = true;
		MESSAGE_SINGLE(NORMAL, logger, "Collision occured!!");

		for(
			std::list<ReceiverInterface*>::iterator itr = this->receivers.begin();
			itr != this->receivers.end();
			++itr)
		{
			(*itr)->onCollision();
		}
	}

}


void
Wire::signalCopperFreeAgainToReceivers()
{
	MESSAGE_SINGLE(NORMAL, this->logger, "Wire is free again");

	this->roundRobin.startRound();
	MESSAGE_SINGLE(NORMAL, this->logger, "Starting round robin wakeup");

	while(this->roundRobin.hasNext() && this->isFree())
	{
		this->roundRobin.next()->onCopperFree();
	}

	this->roundRobin.endRound();
	MESSAGE_SINGLE(NORMAL, this->logger, "Round robin stopped");
}


