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

#include <IMTAPHY/ltea/rlc/SegmentingQueue.hpp>

#include <DLL/UpperConvergence.hpp>
#include <DLL/StationManager.hpp>

#include <WNS/probe/bus/utils.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/scheduler/queue/ISegmentationCommand.hpp>

using namespace ltea::rlc;

STATIC_FACTORY_REGISTER_WITH_CREATOR(SegmentingQueue,
                                     IQueue,
                                     "SegmentingQueue",
                                     wns::PyConfigViewCreator);

SegmentingQueue::SegmentingQueue(const wns::pyconfig::View& _config):
      probeHeaderReader(NULL), 
      segmentHeaderReader(NULL),
      logger(_config.get("logger")),
      config(_config),
      myFUN(),
      maxSize(_config.get<unsigned int>("queueSizeLimitBits")),
      minimumSegmentSize(_config.get<unsigned long int>("minimumSegmentSize")),
      fixedHeaderSize(_config.get<Bit>("fixedHeaderSize")),
      extensionHeaderSize(_config.get<Bit>("extensionHeaderSize")),
      usePadding(_config.get<bool>("usePadding")),
      byteAlignHeader(_config.get<bool>("byteAlignHeader")),
      isDropping(_config.get<bool>("isDropping")),
      pdcpCommandReader(NULL),
      layer(NULL)
{
}

SegmentingQueue::~SegmentingQueue()
{
    if (segmentHeaderReader) { segmentHeaderReader = NULL;}
}

void SegmentingQueue::setFUN(wns::ldk::fun::FUN* fun)
{
    myFUN = fun;
    // read the localIDs from the config
    wns::probe::bus::ContextProviderCollection localContext(&fun->getLayer()->getContextProviderCollection());
    for (int ii = 0; ii<config.len("localIDs.keys()"); ++ii)
    {
        std::string key = config.get<std::string>("localIDs.keys()",ii);
        unsigned long int value  = config.get<unsigned long int>("localIDs.values()",ii);
        localContext.addProvider( wns::probe::bus::contextprovider::Constant(key, value) );
    }

    std::string sizeProbeName = config.get<std::string>("sizeProbeName");
    sizeProbeBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, sizeProbeName));

    std::string overheadProbeName = config.get<std::string>("overheadProbeName");
    overheadProbeBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, overheadProbeName));

    if(!config.isNone("delayProbeName"))
    {
        std::string delayProbeName = config.get<std::string>("delayProbeName");
        delayProbeBus = wns::probe::bus::ContextCollectorPtr(
            new wns::probe::bus::ContextCollector(localContext, 
                delayProbeName + ".delay"));
    
        // Same name as the probe prefix
        probeHeaderReader = myFUN->getCommandReader(delayProbeName);
    }

    std::string segmentHeaderCommandName = config.get<std::string>("segmentHeaderCommandName");
    segmentHeaderReader = myFUN->getCommandReader(segmentHeaderCommandName);
    assure(segmentHeaderReader, "No reader for the Segment Header ("<<segmentHeaderCommandName<<") available!");
    MESSAGE_SINGLE(NORMAL, logger, "SegmentingQueue::setFUN(): segmentHeaderCommandName="<<segmentHeaderCommandName);
    
    pdcpCommandReader = fun->getCommandReader("pdcp");
    assure(pdcpCommandReader, "Could not set pdcp command reader");
    
    layer = dynamic_cast<ltea::Layer2*>(myFUN->getLayer());
    assure(layer, "Could not get layer or not a ltea::Layer2");
}

UserID
SegmentingQueue::getUserId(const wns::ldk::CompoundPtr&  compound ) const 
{    
     // unfortunately we have to use the uppercommand - check if our own one could be used
    assure(pdcpCommandReader->commandIsActivated(compound->getCommandPool()), "Could not get pdcp command for address information");
    dll::UpperCommand* pdcpCmd = pdcpCommandReader->readCommand<dll::UpperCommand>(compound->getCommandPool());
    
    return layer->getNodeByMACAddress(pdcpCmd->peer.targetMACAddress);
}
    
bool ltea::rlc::SegmentingQueue::isAccepting(const wns::ldk::CompoundPtr& compound) const
{
    // we always return true here to allow probe above us to record offered traffic
    // if queues are full, put will drop extra packets
    
    return true;
}
    
    
bool SegmentingQueue::hasCapacity(const wns::ldk::CompoundPtr&  compound ) const {
    int compoundSize = compound->getLengthInBits();

    UserID user = getUserId(compound);
    
    // if this is a brand new connection, return true because couldn't have
    // exceeded limit
    if (queues.find(user) == queues.end())
    {
        MESSAGE_BEGIN(NORMAL, logger, m, "");
        m << "Accepting PDU of size " <<  compoundSize <<  " into queue that will be newly created for"
          << " user=" << user->getName();
        MESSAGE_END();
        return true;
    }

    if (compoundSize + queues.find(user)->second.queuedNettoBits() > maxSize)
    {
        MESSAGE_BEGIN(NORMAL, logger, m, "");
        m << "Not accepting PDU of size=" << compoundSize
          << " because net queuesize=" << queues.find(user)->second.queuedNettoBits() << " for "
          << "user=" << user->getName();
        MESSAGE_END();
        return  false;
    }

    return true;
}



void
SegmentingQueue::put(const wns::ldk::CompoundPtr& compound) {
    assure(compound, "No valid PDU");
    assure(compound != wns::ldk::CompoundPtr(), "No valid PDU" );

    bool accepting = hasCapacity(compound);
    if (isDropping && !accepting)
    {
        MESSAGE_SINGLE(NORMAL, logger, "SegmentingQueue is not accepting. Dropping compound");
        return;
    }

    UserID user = getUserId(compound);
    Bit compoundLength = compound->getLengthInBits();
    assure(compoundLength>0,"compoundLength="<<compoundLength);

    // saves pdu and automatically create new queue if necessary
    // needs a 'map' to do so.
    queues[user].put(compound);

    MESSAGE_SINGLE(NORMAL, logger, "SegmentingQueue::put(user=" << user->getName() << "): after: bits="
                                    << queues[user].queuedNettoBits() << "/" 
                                    << queues[user].queuedBruttoBits(fixedHeaderSize,extensionHeaderSize, 
                                        byteAlignHeader)
                                    << ", PDUs=" << queues[user].queuedCompounds());

    if (sizeProbeBus) {
        sizeProbeBus->put((double)queues[user].queuedBruttoBits(fixedHeaderSize,extensionHeaderSize, byteAlignHeader) / (double)maxSize);
//                          boost::make_tuple("cid", cid, "MAC.QoSClass", 0)); // relative (0..100%)
    } else {
        MESSAGE_SINGLE(NORMAL, logger, "SegmentingQueue::put(user=" << user << "): size=" 
                                        << queues[user].queuedBruttoBits(fixedHeaderSize, extensionHeaderSize, 
                                        byteAlignHeader)
                                        << "): undefined sizeProbeBus=" << sizeProbeBus);
    }
} // put

unsigned long int
SegmentingQueue::numCompoundsForUser(UserID user) const
{
    QueueContainer::const_iterator iter = queues.find(user);
    assure(iter != queues.end(), "cannot find queue for user=" << user);
    return iter->second.queuedCompounds();
}

Bit
SegmentingQueue::numBitsForUser(UserID user) const
{

    QueueContainer::const_iterator iter = queues.find(user);
    assure(iter != queues.end(), "cannot find queue for cid=" << user);

    return iter->second.queuedBruttoBits(fixedHeaderSize, extensionHeaderSize, byteAlignHeader);
} // numBitsForUser()

wns::ldk::CompoundPtr
SegmentingQueue::getHeadOfLinePDUSegment(UserID user, int requestedBits)
{
    assure(queueHasPDUs(user), "getHeadOfLinePDUSegments(user=" << user->getName() << ",bits=" << requestedBits<<") called for CID without PDUs or non-existent CID");

    assure(segmentHeaderReader != NULL, "No valid segmentHeaderReader set! You need to call setFUN() first.");

    wns::ldk::CompoundPtr segment;

    assure(requestedBits > fixedHeaderSize, "Must request more than the fixed header size.");

    segment = queues[user].retrieve(requestedBits, fixedHeaderSize, extensionHeaderSize, 
        usePadding, byteAlignHeader, 
        segmentHeaderReader, delayProbeBus, probeHeaderReader);

    assure(segment != wns::ldk::CompoundPtr(), "Inner queue did not return a PDU");

    segmentHeaderReader->commitSizes(segment->getCommandPool());

    wns::scheduler::queue::ISegmentationCommand* header = segmentHeaderReader->readCommand<wns::scheduler::queue::ISegmentationCommand>(segment->getCommandPool());

    assure(!byteAlignHeader || header->headerSize() % 8 == 0, "Header not byte aligned.");

    if (sizeProbeBus) {
        sizeProbeBus->put((double)queues[user].queuedBruttoBits(fixedHeaderSize, extensionHeaderSize, byteAlignHeader) / (double)maxSize);
    }

    if (overheadProbeBus) {
        overheadProbeBus->put( ( (double) header->headerSize())/((double) header->totalSize()));
    }
    
    MESSAGE_SINGLE(NORMAL, logger, "getHeadOfLinePDUSegment(user=" << user->getName() <<",to=" << user->getName()
                   << ",bits="<<requestedBits<<"): totalSize="<<header->totalSize()<<" bits, sn="<< header->getSequenceNumber() );

    MESSAGE_SINGLE(NORMAL, logger, "getHeadOfLinePDUSegment(user="<< user->getName() <<"): after: bits="<<queues[user].queuedNettoBits()
                                   << "/" << queues[user].queuedBruttoBits(fixedHeaderSize, extensionHeaderSize, 
                                            byteAlignHeader) 
                                   << ", PDUs=" << queues[user].queuedCompounds() << ", fh: " << fixedHeaderSize << ", eh: " << extensionHeaderSize);

    assure(header->totalSize()<=requestedBits,"pdulength="<<header->totalSize()<<" > bits="<<requestedBits);
    return segment;
}

bool
SegmentingQueue::queueHasPDUs(UserID user) const {
    if (queues.find(user) == queues.end())
    {
        MESSAGE_SINGLE(NORMAL, logger, "User " << user->getName() << " not found.");
        
        for (QueueContainer::const_iterator iter = queues.begin(); iter != queues.end(); iter++)
        {
            MESSAGE_SINGLE(NORMAL, logger, "I do know user " << iter->first->getName());
        }
        
        return false;
    }
    return ( !(queues.find(user)->second.empty()) );
}

