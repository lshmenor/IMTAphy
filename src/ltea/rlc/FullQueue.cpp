/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2011
 * Institute of Communication Networks (LKN)
 * Department of Electrical Engineering and Information Technology (EE & IT)
 * Technische Universitaet Muenchen
 * Arcisstr. 21
 * 80333 Muenchen - Germany
 * http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
 * 
 * _____________________________________________________________________________
 *
 *   IMTAphy is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   IMTAphy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

// based on code from openWNS with the following license:

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

#include <IMTAPHY/ltea/rlc/FullQueue.hpp>
#include <IMTAPHY/ltea/pdcp/PDCPCommand.hpp>

#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/service/dll/Address.hpp>
#include <WNS/scheduler/queue/ISegmentationCommand.hpp>

#include <DLL/UpperConvergence.hpp>

using namespace ltea::rlc;

STATIC_FACTORY_REGISTER_WITH_CREATOR(FullQueue,
                                     IQueue,
                                     "FullQueue",
                                     wns::PyConfigViewCreator);

FullQueue::FullQueue(const wns::pyconfig::View& _config): 
      logger(_config.get("logger")),
      config(_config),
      myFUN(),
      sequenceNumber(0),
      headerSize(_config.get<Bit>("headerSize")),
      pdcpCommandReader(NULL),
      windowProbeCommandReader(NULL),
      packetProbeCommandReader(NULL),
      windowProbe(NULL),
      layer(NULL)
{
}

FullQueue::~FullQueue()
{
}

void 
FullQueue::setFUN(wns::ldk::fun::FUN* fun)
{
    myFUN = fun;

    std::string pdcpCommandName = config.get<std::string>("pdcpCommandName");
    pdcpCommandReader = myFUN->getCommandReader(pdcpCommandName);
    assure(pdcpCommandReader, "No reader for the PDCP Header (" << pdcpCommandName << ") available!");   

    std::string windowProbeCommandName = config.get<std::string>("windowProbeCommandName");
    windowProbeCommandReader = myFUN->getCommandReader(windowProbeCommandName);
    assure(windowProbeCommandReader, "No reader for the Window Probe (" << windowProbeCommandName << ") available!");

    windowProbe = myFUN->findFriend<ltea::helper::Window*>(windowProbeCommandName);
    assure(windowProbe != NULL, "Could not get Window probe"); 

    std::string packetProbeCommandName = config.get<std::string>("packetProbeCommandName");
    packetProbeCommandReader = myFUN->getCommandReader(packetProbeCommandName);
    assure(packetProbeCommandReader, "No reader for the Packet Probe (" << packetProbeCommandName << ") available!");

    packetProbe = myFUN->findFriend<wns::ldk::probe::Packet*>(packetProbeCommandName);
    assure(packetProbe != NULL, "Could not get Packet probe"); 
    
    layer = dynamic_cast<ltea::Layer2*>(myFUN->getLayer());
    assure(layer, "Could not get layer or not a ltea::Layer2");
}

bool 
FullQueue::isAccepting(const wns::ldk::CompoundPtr&  compound ) const 
{
    assure(false, "There should be no compound above this FU!");

    return true;
} 

void
FullQueue::put(const wns::ldk::CompoundPtr& compound) 
{
    assure(false, "There should be no compound above this FU!");
}

unsigned long int
FullQueue::numCompoundsForUser(UserID user) const
{
    return 1;
}

Bit
FullQueue::numBitsForUser(UserID user) const
{
    return std::numeric_limits<Bit>::max();
} 

wns::ldk::CompoundPtr
FullQueue::getHeadOfLinePDUSegment(UserID user, int requestedBits)
{
    assure(requestedBits > headerSize, "Must request at least " << headerSize << " bit.");

    wns::ldk::helper::FakePDUPtr pdu(wns::ldk::helper::FakePDUPtr(
        new wns::ldk::helper::FakePDU(requestedBits - headerSize)));

    wns::ldk::CompoundPtr compound(new wns::ldk::Compound(myFUN->createCommandPool(), pdu));

    pdcpCommandReader->activateCommand(compound->getCommandPool());
    dll::UpperCommand* pdcpCommand = pdcpCommandReader->readCommand<dll::UpperCommand>(compound->getCommandPool());
    pdcpCommand->peer.sourceMACAddress = wns::service::dll::UnicastAddress(layer->getNode()->getNodeID());
    pdcpCommand->peer.targetMACAddress = wns::service::dll::UnicastAddress(user->getNodeID());

    windowProbeCommandReader->activateCommand(compound->getCommandPool());
    ltea::helper::WindowCommand* wCommand = windowProbeCommandReader->readCommand<ltea::helper::WindowCommand>(compound->getCommandPool());
    wCommand->magic.probingFU = windowProbe;

    packetProbeCommandReader->activateCommand(compound->getCommandPool());
    wns::ldk::probe::PacketCommand* pCommand = packetProbeCommandReader->readCommand<wns::ldk::probe::PacketCommand>(compound->getCommandPool());
    pCommand->magic.probingFU = packetProbe;
    pCommand->magic.t = wns::simulator::getEventScheduler()->getTime();

    return compound;
}

bool
FullQueue::queueHasPDUs(UserID user) const 
{
    return true;
}


