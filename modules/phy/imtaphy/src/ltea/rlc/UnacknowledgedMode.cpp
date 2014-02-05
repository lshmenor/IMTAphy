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
 * Copyright (C) 2004-2007
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

#include <IMTAPHY/ltea/rlc/UnacknowledgedMode.hpp>
#include <DLL/UpperConvergence.hpp>
#include <DLL/StationManager.hpp>

using namespace ltea::rlc;

#define A2N(a) layer2->getStationManager()->getStationByMAC(a)->getName()

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    UnacknowledgedMode,
    wns::ldk::FunctionalUnit, "ltea.rlc.UnacknowledgedMode",
    wns::ldk::FUNConfigCreator);

UnacknowledgedMode::UnacknowledgedMode(wns::ldk::fun::FUN* fun,
                                       const wns::pyconfig::View& config):
    wns::ldk::sar::SegAndConcat(fun, config),
    layer2(NULL)
{
    layer2 = fun->getLayer<dll::ILayer2*>();
}

UnacknowledgedMode::~UnacknowledgedMode()
{
}

void
UnacknowledgedMode::processIncoming(const wns::ldk::CompoundPtr& compound)
{
    SegAndConcat::processIncoming(compound);

//    MESSAGE_BEGIN(NORMAL, logger_, m, "processIncoming");
/*    wns::ldk::CommandPool* commandPool = compound->getCommandPool();
    wns::ldk::CommandReaderInterface* rlcReader = getFUN()->getCommandReader("rlc");*/
//    lte::rlc::RLCCommand* rlcCommand = rlcReader->readCommand<lte::rlc::RLCCommand>(commandPool);
//    wns::service::dll::FlowID flowID = rlcCommand->peer.flowID;
/*    wns::ldk::sar::SegAndConcatCommand* command;

    command = SegAndConcat::getCommandReader()->readCommand<wns::ldk::sar::SegAndConcatCommand>(commandPool);
    m << "(flowID=" << flowID << ",from=" << A2N(rlcCommand->peer.source) << ",sn=" << command->peer.sn_;
    m << ",len=" << compound->getLengthInBits() << ")";
    MESSAGE_END();*/
}

void
UnacknowledgedMode::processOutgoing(const wns::ldk::CompoundPtr& sdu)
{
    SegAndConcat::processOutgoing(sdu);

/*    if(isSegmenting_)
    {
        MESSAGE_BEGIN(NORMAL, logger_, m, "processOutgoing");
        wns::ldk::CommandPool* commandPool = sdu->getCommandPool();
        wns::ldk::CommandReaderInterface* rlcReader = getFUN()->getCommandReader("rlc");
        lte::rlc::RLCCommand* rlcCommand = rlcReader->readCommand<lte::rlc::RLCCommand>(commandPool);
        wns::service::dll::FlowID flowID = rlcCommand->peer.flowID;
        wns::ldk::sar::SegAndConcatCommand* command;
	command = SegAndConcat::getCommandReader()->readCommand<wns::ldk::sar::SegAndConcatCommand>(commandPool);
        m << "(flowID=" << flowID << ",to=" << A2N(rlcCommand->peer.destination) << ",sn=" << command->peer.sn_;
        m << ",len=" << sdu->getLengthInBits() << ")";
        MESSAGE_END();
    }*/
}
