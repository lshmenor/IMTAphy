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

#include <WNS/simulator/ISimulator.hpp>
#include <WNS/events/scheduler/Interface.hpp>

#include "ConstanzePDU.hpp"
//#include "ConstanzePCI.hpp"
#include <WNS/Assure.hpp>


using namespace constanze;

ConstanzePDU::ConstanzePDU(Bit _length)
	:
	//PDU(),
	wns::osi::PDU(new wns::osi::PCI),
	length(_length),
	creationTime(0.0)
{
	creationTime = wns::simulator::getEventScheduler()->getTime();
}

ConstanzePDU::~ConstanzePDU()
{}

void ConstanzePDU::setLengthInBits(Bit _length)
{
	length = _length;
} // setLengthInBits

void  ConstanzePDU::setTimeStamp(simTimeType time)
{
	//assure(time >= wns::simulator::getEventScheduler()->getTime(), "living in the past?");
	creationTime = time;
}

simTimeType ConstanzePDU::getTimeStamp()
{
	return creationTime;
}

/*
ConstanzePCI* ConstanzePDU::getConstanzePCI() const
{
        assureType(getPCI(), ConstanzePCI*);
        return static_cast<ConstanzePCI*>(getPCI());
}
*/

ConstanzePDU* ConstanzePDU::castPDU2ConstanzePDU(wns::osi::PDU* pdu)
{
        assureType(pdu, ConstanzePDU*);
        return static_cast<ConstanzePDU*>(pdu);
}

wns::osi::PDU* ConstanzePDU::castConstanzePDU2PDU(ConstanzePDU* pdu)
{
        return static_cast<wns::osi::PDU*>(pdu);
}

Bit ConstanzePDU::doGetLengthInBits() const
{
	//assert(getPCI());
	//return 8 * getPCI()->getPDULength();
	return length;
}
