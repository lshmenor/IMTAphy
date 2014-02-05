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

#include <WNS/scheduler/queue/SegmentingQueue.hpp>
#include <WNS/probe/bus/utils.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/Compound.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::queue;


STATIC_FACTORY_REGISTER_WITH_CREATOR(SegmentingQueue,
                                     QueueInterface,
                                     "SegmentingQueue",
                                     wns::HasReceptorConfigCreator);

SegmentingQueue::SegmentingQueue(wns::ldk::HasReceptorInterface*, const wns::pyconfig::View& _config)
    : segmentHeaderReader(NULL),
      probeHeaderReader(NULL),
      logger(_config.get("logger")),
      config(_config),
      myFUN(),
      maxSize(0),
      minimumSegmentSize(_config.get<unsigned long int>("minimumSegmentSize")),
      fixedHeaderSize(_config.get<Bit>("fixedHeaderSize")),
      extensionHeaderSize(_config.get<Bit>("extensionHeaderSize")),
      usePadding(_config.get<bool>("usePadding")),
      byteAlignHeader(_config.get<bool>("byteAlignHeader")),
      isDropping(_config.get<bool>("isDropping"))
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
}

bool SegmentingQueue::isAccepting(const wns::ldk::CompoundPtr&  compound ) const {
    int compoundSize = compound->getLengthInBits();
    ConnectionID cid = colleagues.registry->getCIDforPDU(compound);

    // if this is a brand new connection, return true because couldn't have
    // exceeded limit
    if (queues.find(cid) == queues.end())
    {
        MESSAGE_BEGIN(VERBOSE, logger, m, "");
        m << "Accepting PDU of size " <<  compoundSize <<  " into queue that would be newly created for CID=" << cid
          << " of user=" << colleagues.registry->getNameForUser(colleagues.registry->getUserForCID(cid)) <<"\n";
        MESSAGE_END();
        return true;
    }

    if (compoundSize + queues.find(cid)->second.queuedNettoBits() > maxSize)
    {
        MESSAGE_BEGIN(VERBOSE, logger, m, "");
        m << "Not accepting PDU of size=" << compoundSize
          << " because net queuesize=" << queues.find(cid)->second.queuedNettoBits() << " for CID=" << cid
          << " of user=" << colleagues.registry->getNameForUser(colleagues.registry->getUserForCID(cid)) <<"\n";
        MESSAGE_END();
        return  false;
    }

    MESSAGE_BEGIN(VERBOSE, logger, m, "");
    m << "Accepting PDU of size=" <<  compoundSize <<  " because net queuesize=" << queues.find(cid)->second.queuedNettoBits() << " for CID=" << cid
      << " of user=" << colleagues.registry->getNameForUser(colleagues.registry->getUserForCID(cid)) <<"\n";
    MESSAGE_END();
    return true;
} // isAccepting

void
SegmentingQueue::put(const wns::ldk::CompoundPtr& compound) {
    assure(compound, "No valid PDU");
    assure(compound != wns::ldk::CompoundPtr(), "No valid PDU" );
    assure(colleagues.registry, "Need a registry as colleague, please set first");

    bool accepting = isAccepting(compound);
    if (isDropping && !accepting)
    {
        MESSAGE_SINGLE(VERBOSE, logger, "SegmentingQueue is not accepting. Dropping compound");
        return;
    }
    else
    {
        assure(accepting, "sendData() has been called without isAccepting()");
    }

    ConnectionID cid = colleagues.registry->getCIDforPDU(compound);
    Bit compoundLength = compound->getLengthInBits();
    assure(compoundLength>0,"compoundLength="<<compoundLength);

    // saves pdu and automatically create new queue if necessary
    // needs a 'map' to do so.
    queues[cid].put(compound);

    if (fixedOverhead.find(cid) == fixedOverhead.end())
    {
        fixedOverhead[cid] = fixedHeaderSize;
    }

    MESSAGE_SINGLE(NORMAL, logger, "SegmentingQueue::put(cid="<<cid<<"): after: bits="<<queues[cid].queuedNettoBits()<<"/"<<queues[cid].queuedBruttoBits(fixedOverhead[cid],extensionHeaderSize, byteAlignHeader)<<", PDUs="<<queues[cid].queuedCompounds());

    if (sizeProbeBus) {
        int priority = colleagues.registry->getPriorityForConnection(cid); // only for probes

        sizeProbeBus->put((double)queues[cid].queuedBruttoBits(fixedOverhead[cid],extensionHeaderSize, byteAlignHeader) / (double)maxSize,
                          boost::make_tuple("cid", cid, "MAC.QoSClass", priority)); // relative (0..100%)
    } else {
        MESSAGE_SINGLE(NORMAL, logger, "SegmentingQueue::put(cid="<<cid<<"): size="<<queues[cid].queuedBruttoBits(fixedOverhead[cid],extensionHeaderSize, byteAlignHeader)<<"): undefined sizeProbeBus="<<sizeProbeBus);
    }
} // put

// [rs]: obsolete? Better use cid-related questions. Used frequently in OLD scheduler strategies
UserSet
SegmentingQueue::getQueuedUsers() const {
    UserSet users;

    for (QueueContainer::const_iterator iter = queues.begin(); iter != queues.end(); ++iter)
    {
        if ( !( (*iter).second.empty()) )
        {
            ConnectionID cid = iter->first;
            UserID user = colleagues.registry->getUserForCID(cid);
            users.insert(user);
        }
    }
    return users;
}

ConnectionSet
SegmentingQueue::getActiveConnections() const
{
    ConnectionSet result;

    for (QueueContainer::const_iterator iter = queues.begin(); iter != queues.end(); ++iter) {
        ConnectionID cid = iter->first;
        if ( !( (*iter).second.empty() ) ) {
            result.insert(cid);
        } else {
            assure(iter->second.queuedNettoBits()==0,"Zero packets but "<<iter->second.queuedNettoBits()<<" bits. How can this be?");
        }
    }
    return result;
}

unsigned long int
SegmentingQueue::numCompoundsForCid(ConnectionID cid) const
{
    QueueContainer::const_iterator iter = queues.find(cid);
    assure(iter != queues.end(),"cannot find queue for cid="<<cid);
    return iter->second.queuedCompounds();
}

unsigned long int
SegmentingQueue::numBitsForCid(ConnectionID cid) const
{

    QueueContainer::const_iterator iter = queues.find(cid);
    assure(iter != queues.end(),"cannot find queue for cid="<<cid);

    /**
     * @todo dbn: Header Sizes depend on CID! User plane and control plane must be handled
     * properly. Currently fixedHeaderSize also applies for the ResourceMaps!
     * This must be fixed!
     */
    assure(fixedOverhead.find(cid)!=fixedOverhead.end(), "Cannot find overhead entry for cid " << cid);
    int overhead = fixedOverhead.find(cid)->second;
    return iter->second.queuedBruttoBits(overhead, extensionHeaderSize, byteAlignHeader);
} // numBitsForCid()

// result is sorted per-cid
QueueStatusContainer
SegmentingQueue::getQueueStatus(bool forFuture) const
{
    wns::scheduler::QueueStatusContainer result;

    // Find all queues
    for (QueueContainer::const_iterator iter = queues.begin(); iter != queues.end(); ++iter)
    {
        ConnectionID cid = iter->first;
        QueueStatus queueStatus;
        int overhead;

        /* The header will be reset in the next frame */
        if(forFuture)
        {
            overhead = fixedHeaderSize;
        }
        else
        {    
            assure(fixedOverhead.find(cid)!=fixedOverhead.end(), "Cannot find overhead entry for cid " << cid);
            overhead = fixedOverhead.find(cid)->second;
        }
        queueStatus.numOfBits      = iter->second.queuedBruttoBits(overhead, extensionHeaderSize, byteAlignHeader);
        queueStatus.numOfCompounds = iter->second.queuedCompounds();
        result.insert(cid,queueStatus);
        MESSAGE_SINGLE(NORMAL, logger, "SegmentingQueue::getQueueStatus():"
                       << " for cid=" << cid
                       << ": bits=" << queueStatus.numOfBits
                       << ", PDUs=" << queueStatus.numOfCompounds);
        // if we have bits we also must have a pdu:
        assure((queueStatus.numOfBits==0)
               || (queueStatus.numOfCompounds>0),
               "numOfBits="<<queueStatus.numOfBits<<" but numOfCompounds="<<queueStatus.numOfCompounds<<" for cid="<<cid);
    }
    return result;
} // getQueueStatus()

wns::ldk::CompoundPtr
SegmentingQueue::getHeadOfLinePDU(ConnectionID cid) {
    assure(false, "The SegmentingQueue does not support getHeadOfLinePDU");
}

// return only those bits which belong to one PDU
int
SegmentingQueue::getHeadOfLinePDUbits(ConnectionID cid)
{
    QueueContainer::const_iterator iter = queues.find(cid);
    assure(iter != queues.end(),"cannot find queue for cid="<<cid);
    assure(queueHasPDUs(cid), "getHeadOfLinePDUbits called for CID without PDUs or non-existent CID="<<cid);

    return numBitsForCid(cid);
}

wns::ldk::CompoundPtr
SegmentingQueue::getHeadOfLinePDUSegment(ConnectionID cid, int requestedBits)
{
    assure(queueHasPDUs(cid), "getHeadOfLinePDUSegments(cid="<<cid<<",bits="<<requestedBits<<") called for CID without PDUs or non-existent CID");

    assure(segmentHeaderReader != NULL, "No valid segmentHeaderReader set! You need to call setFUN() first.");
    assure(fixedOverhead.find(cid)!=fixedOverhead.end(), "Cannot find overhead entry for cid " << cid);

    wns::ldk::CompoundPtr segment;

    int ov = fixedOverhead[cid];

    if (requestedBits <= ov)
    {
        ov = requestedBits - 1;
    }

    segment = queues[cid].retrieve(requestedBits, ov, extensionHeaderSize, 
        usePadding, byteAlignHeader, segmentHeaderReader, delayProbeBus, probeHeaderReader);

    assure(segment != wns::ldk::CompoundPtr(), "Inner queue did not return a PDU");

    // Clear this. The next request will not include a fixed header
    // Will be reset in frameStarts()
    fixedOverhead[cid] -= ov;
    if (fixedOverhead[cid] < 0)
    {
        fixedOverhead[cid] = 0;
    }

    segmentHeaderReader->commitSizes(segment->getCommandPool());

    ISegmentationCommand* header = segmentHeaderReader->readCommand<ISegmentationCommand>(segment->getCommandPool());

    if (sizeProbeBus) {
        int priority = colleagues.registry->getPriorityForConnection(cid);
        sizeProbeBus->put((double)queues[cid].queuedBruttoBits(fixedOverhead[cid],extensionHeaderSize, byteAlignHeader) / (double)maxSize,
                          boost::make_tuple("cid", cid, "MAC.QoSClass", priority)); // relative (0..100%)
    }

    if (overheadProbeBus) {
        int priority = colleagues.registry->getPriorityForConnection(cid);
        overheadProbeBus->put( ( (double) header->headerSize())/((double) header->totalSize()),
                          boost::make_tuple("cid", cid, "MAC.QoSClass", priority)); // relative (0..100%)
    }
    
    MESSAGE_SINGLE(NORMAL, logger, "getHeadOfLinePDUSegment(cid="<<cid<<",to="<<colleagues.registry->getNameForUser(colleagues.registry->getUserForCID(cid))
                   <<",bits="<<requestedBits<<"): totalSize="<<header->totalSize()<<" bits, sn="<< header->getSequenceNumber() );

    MESSAGE_SINGLE(NORMAL, logger, "getHeadOfLinePDUSegment(cid="<<cid<<"): after: bits="<<queues[cid].queuedNettoBits()<<"/"<<queues[cid].queuedBruttoBits(fixedOverhead[cid], extensionHeaderSize, byteAlignHeader)<<", PDUs="<<queues[cid].queuedCompounds() << ", fh: " << fixedOverhead[cid] << ", eh: " << extensionHeaderSize);

    assure(header->totalSize()<=requestedBits,"pdulength="<<header->totalSize()<<" > bits="<<requestedBits);
    return segment;
}

std::queue<wns::ldk::CompoundPtr> 
SegmentingQueue::getQueueCopy(ConnectionID cid)
{
    assure(queues.find(cid) != queues.end(), "getQueueCopy called for non-existent CID");
    return queues[cid].getQueueCopy();
}

bool
SegmentingQueue::isEmpty() const
{
    for (QueueContainer::const_iterator iter = queues.begin(); iter != queues.end(); ++iter)
    {
        if ( !( (*iter).second.empty() ))
            return false;
    }
    return true;
}

bool
SegmentingQueue::hasQueue(ConnectionID cid)
{
    return queues.find(cid) != queues.end();
}

bool
SegmentingQueue::queueHasPDUs(ConnectionID cid) const {
    if (queues.find(cid) == queues.end())
        return false;
    return ( !(queues.find(cid)->second.empty()) );
}

ConnectionSet
SegmentingQueue::filterQueuedCids(ConnectionSet connections) {
    ConnectionSet activeConnections;
    for ( wns::scheduler::ConnectionSet::iterator iter = connections.begin(); iter != connections.end(); ++iter )
    {
        ConnectionID cid = *iter;
        if ( queueHasPDUs(cid) )
            activeConnections.insert(cid);
    }
    return activeConnections;
}

void
SegmentingQueue::setColleagues(RegistryProxyInterface* _registry) {
    colleagues.registry = _registry;
    maxSize = colleagues.registry->getQueueSizeLimitPerConnection();
}

QueueInterface::ProbeOutput
SegmentingQueue::resetAllQueues()
{
    // Store number of bits and compounds for Probe which will be deleted
    ProbeOutput probeOutput;
    for (QueueContainer::iterator iter = queues.begin();
         iter != queues.end(); ++iter)
    {
        ConnectionID cid = iter->first;
        probeOutput.bits += iter->second.queuedBruttoBits(fixedOverhead[cid],extensionHeaderSize, byteAlignHeader);
        probeOutput.compounds += iter->second.queuedCompounds();
        if (sizeProbeBus) {
            int priority = colleagues.registry->getPriorityForConnection(cid);
            sizeProbeBus->put(0.0, boost::make_tuple("cid", cid, "MAC.QoSClass", priority)); // relative (0..100%)
        }
    }

    // queues is a std::map that stores the std::queues that store the
    // CompoundPtrs. So by doing a queues.clear(), the destructors are called
    // and the refCounting mechanism of the CompoundPtr takes care of actually
    // deleting the compounds.
    queues.clear();
    fixedOverhead.clear();

    return probeOutput;
}

// [rs]: obsolete? Better use cid-related questions
QueueInterface::ProbeOutput
SegmentingQueue::resetQueues(UserID _user)
{
    // MESSAGE_SINGLE(NORMAL, logger, "SegmentingQueue::resetQueues(): obsolete"); // TODO [rs]; not supported by [aoz]
    // Store number of bits and compounds for Probe which will be deleted
    ProbeOutput probeOutput;

    // Find all queues that belong to this user and delete them.  This one is a
    // little bit tricky, see section 6.6.2 of Josutti's STL book: we have to be
    // careful when deleting the current iterator position
    for (QueueContainer::iterator iter = queues.begin(); iter != queues.end(); )
    {
        ConnectionID cid = iter->first;
        UserID user = colleagues.registry->getUserForCID(cid);
        if (user == _user)
        {
            ConnectionID cid = iter->first;
            probeOutput.bits += iter->second.queuedBruttoBits(fixedOverhead[cid],extensionHeaderSize, byteAlignHeader);
            probeOutput.compounds += iter->second.queuedCompounds();
            if (sizeProbeBus) {
                int priority = colleagues.registry->getPriorityForConnection(cid);
                sizeProbeBus->put(0.0, boost::make_tuple("cid", cid, "MAC.QoSClass", priority)); // relative (0..100%)
            }
            queues.erase(iter++);
        }
        else
            ++iter;
    }
    for (FixedOverheadContainer::iterator iter = fixedOverhead.begin(); iter != fixedOverhead.end(); )
    {
        ConnectionID cid = iter->first;
        UserID user = colleagues.registry->getUserForCID(cid);
        if (user == _user)
        {
            ConnectionID cid = iter->first;
            fixedOverhead.erase(iter++);
        }
        else
            ++iter;
    }

    return probeOutput;
}

QueueInterface::ProbeOutput
SegmentingQueue::resetQueue(ConnectionID cid)
{
    // Store number of bits and compounds for Probe which will be deleted
    ProbeOutput probeOutput;
    probeOutput.bits += queues[cid].queuedBruttoBits(fixedOverhead[cid],extensionHeaderSize, byteAlignHeader);
    probeOutput.compounds += queues[cid].queuedCompounds();
    if (sizeProbeBus) {
        int priority = colleagues.registry->getPriorityForConnection(cid);
        sizeProbeBus->put(0.0, boost::make_tuple("cid", cid, "MAC.QoSClass", priority)); // relative (0..100%)
    }

#ifndef NDEBUG
    int numRemoved =
#endif
        queues.erase(cid);
    fixedOverhead.erase(cid);
    assure(numRemoved == 1, "Non-existing or too many queues with that CID");

    return probeOutput;
}

void
SegmentingQueue::frameStarts()
{
    MESSAGE_SINGLE(NORMAL, logger, "frameStarts(): resetting fixed header flags");

    for (FixedOverheadContainer::iterator it=fixedOverhead.begin(); it!=fixedOverhead.end(); ++it)
    {
        it->second = fixedHeaderSize;
    }
}

std::string
SegmentingQueue::printAllQueues()
{
    std::stringstream s;
    for (QueueContainer::iterator iter = queues.begin();
         iter != queues.end(); ++iter)
    {
        ConnectionID cid = iter->first;
        int bits      = iter->second.queuedBruttoBits(fixedOverhead[cid],extensionHeaderSize, byteAlignHeader);
        int compounds = iter->second.queuedCompounds();
        s << cid << ":" << bits << "," << compounds << " ";
    }
    return s.str();
}


