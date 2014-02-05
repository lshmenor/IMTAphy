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

#include <WNS/scheduler/queue/SimpleQueue.hpp>
#include <WNS/probe/bus/utils.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/Compound.hpp>

using namespace wns::scheduler;
using namespace wns::scheduler::queue;


STATIC_FACTORY_REGISTER_WITH_CREATOR(SimpleQueue,
                                     QueueInterface,
                                     "SimpleQueue",
                                     wns::HasReceptorConfigCreator);

SimpleQueue::SimpleQueue(wns::ldk::HasReceptorInterface*, const wns::pyconfig::View& _config)
    : probeContextProviderForCid(NULL),
      probeContextProviderForPriority(NULL),
      maxSize(0),
      logger(_config.get("logger")),
      config(_config),
      myFUN()
{
}

SimpleQueue::~SimpleQueue()
{
    if (probeContextProviderForCid) { delete probeContextProviderForCid; }
    if (probeContextProviderForPriority) { delete probeContextProviderForPriority; }
}

void SimpleQueue::setFUN(wns::ldk::fun::FUN* fun)
{
    // read the localIDs from the config
    wns::probe::bus::ContextProviderCollection localContext(&fun->getLayer()->getContextProviderCollection());
    for (int ii = 0; ii<config.len("localIDs.keys()"); ++ii)
    {
        std::string key = config.get<std::string>("localIDs.keys()",ii);
        unsigned long int value  = config.get<unsigned long int>("localIDs.values()",ii);
        localContext.addProvider( wns::probe::bus::contextprovider::Constant(key, value) );
    }
    probeContextProviderForCid  = new wns::probe::bus::contextprovider::Variable("cid", 0);
    probeContextProviderForPriority  = new wns::probe::bus::contextprovider::Variable("MAC.QoSClass", 0);
    localContext.addProvider(wns::probe::bus::contextprovider::Container(probeContextProviderForCid));
    localContext.addProvider(wns::probe::bus::contextprovider::Container(probeContextProviderForPriority));

    std::string sizeProbeName = config.get<std::string>("sizeProbeName");
    sizeProbeBus = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, sizeProbeName));
    myFUN = fun;
}

bool SimpleQueue::isAccepting(const wns::ldk::CompoundPtr&  compound ) const {
    int size = compound->getLengthInBits();

    ConnectionID cid = colleagues.registry->getCIDforPDU(compound);

    // if this is a brand new connection, return true because couldn't have
    // exceeded limit
    if (queues.find(cid) == queues.end())
    {
        MESSAGE_BEGIN(VERBOSE, logger, m, "");
        m << "Accepting PDU of size " <<  size <<  " into queue that would be newly created for CID " << cid
          << " - " << colleagues.registry->getNameForUser(colleagues.registry->getUserForCID(cid)) <<"\n";
        MESSAGE_END();
        return true;
    }

    if (size + queues.find(cid)->second.bits > maxSize)
    {
        MESSAGE_BEGIN(VERBOSE, logger, m, "");
        m << "Not accepting PDU of size "
          << size << " because queue size " << queues.find(cid)->second.bits << " for CID " << cid
          << " - " << colleagues.registry->getNameForUser(colleagues.registry->getUserForCID(cid)) <<"\n";
        MESSAGE_END();
        return  false;
    }

    MESSAGE_BEGIN(VERBOSE, logger, m, "");
    m << "Accepting PDU of size " <<  size <<  " because queue size is only " << queues.find(cid)->second.bits << " for CID " << cid
      << " - " << colleagues.registry->getNameForUser(colleagues.registry->getUserForCID(cid)) <<"\n";
    MESSAGE_END();
    return true;
}

void
SimpleQueue::put(const wns::ldk::CompoundPtr& compound) {
    assure(compound, "No valid PDU");
    assure(compound != wns::ldk::CompoundPtr(), "No valid PDU" );
    assure(isAccepting(compound),"sendData() has been called without isAccepting()");
    assure(colleagues.registry, "Need a registry as colleague, please set first");

    ConnectionID cid = colleagues.registry->getCIDforPDU(compound);
    int     priority = colleagues.registry->getPriorityForConnection(cid); // only for probes

    // saves pdu and automatically create new queue if necessary
    // needs a 'map' to do so.
    (queues[cid].pduQueue).push(compound);
    queues[cid].bits += compound->getLengthInBits();

    if (probeContextProviderForCid && probeContextProviderForPriority && sizeProbeBus) {
        probeContextProviderForCid->set(cid /*int context*/);
        probeContextProviderForPriority->set(priority);
        sizeProbeBus->put((double)queues[cid].bits / (double)maxSize); // relative (0..100%)
    } else {
        MESSAGE_SINGLE(NORMAL, logger, "SimpleQueue::put(cid="<<cid<<"): size="<<queues[cid].bits<<"): undefined sizeProbeBus="<<sizeProbeBus);
    }

}

// [rs]: obsolete? Better use cid-related questions
UserSet
SimpleQueue::getQueuedUsers() const {
    UserSet users;
    for (QueueContainer::const_iterator iter = queues.begin(); iter != queues.end(); ++iter)
    {
        if ((*iter).second.pduQueue.size() != 0)
        {
            ConnectionID cid = iter->first;
            UserID user = colleagues.registry->getUserForCID(cid);
            users.insert(user);
        }
    }
    return users;
}

ConnectionSet
SimpleQueue::getActiveConnections() const
{
    ConnectionSet result;

    for (QueueContainer::const_iterator iter = queues.begin(); iter != queues.end(); ++iter)
        if ((*iter).second.pduQueue.size() != 0)
            result.insert((*iter).first);

    return result;
}

unsigned long int
SimpleQueue::numCompoundsForCid(ConnectionID cid) const
{
    QueueContainer::const_iterator iter = queues.find(cid);
    assure(iter != queues.end(),"cannot find queue for cid="<<cid);
    return iter->second.pduQueue.size();
}

unsigned long int
SimpleQueue::numBitsForCid(ConnectionID cid) const
{
    QueueContainer::const_iterator iter = queues.find(cid);
    assure(iter != queues.end(),"cannot find queue for cid="<<cid);
    return iter->second.bits;
}

// result is sorted per-cid
QueueStatusContainer
SimpleQueue::getQueueStatus(bool forFuture) const
{
    wns::scheduler::QueueStatusContainer result;

    // Find all queues that belong to this user (obsolete)
    // Find all queues
    for (QueueContainer::const_iterator iter = queues.begin(); iter != queues.end(); ++iter)
    {
        ConnectionID cid = iter->first;
        QueueStatus queueStatus;
        queueStatus.numOfBits      = iter->second.bits;
        queueStatus.numOfCompounds = iter->second.pduQueue.size();
        result.insert(cid,queueStatus);
        MESSAGE_SINGLE(NORMAL, logger, "SimpleQueue::getQueueStatus():"
                       << " for cid=" << cid
                       << ": bits=" << iter->second.bits
                       << ", PDUs=" << iter->second.pduQueue.size());
    }
    return result;
}

wns::ldk::CompoundPtr
SimpleQueue::getHeadOfLinePDU(ConnectionID cid) {
    assure(queueHasPDUs(cid), "getHeadOfLinePDU called for CID without PDUs or non-existent CID");

    wns::ldk::CompoundPtr pdu = queues[cid].pduQueue.front();
    queues[cid].pduQueue.pop();
    queues[cid].bits -= pdu->getLengthInBits();

    if (probeContextProviderForCid && probeContextProviderForPriority && sizeProbeBus) {
        probeContextProviderForCid->set(cid /*int context*/);
        int priority = colleagues.registry->getPriorityForConnection(cid);
        probeContextProviderForPriority->set(priority);
        sizeProbeBus->put((double)queues[cid].bits / (double)maxSize); // relative (0..100%)
    }

    return pdu;
}

int
SimpleQueue::getHeadOfLinePDUbits(ConnectionID cid)
{
    assure(queueHasPDUs(cid), "getHeadOfLinePDUbits called for CID without PDUs or non-existent CID");
    return queues[cid].pduQueue.front()->getLengthInBits();
}

std::queue<wns::ldk::CompoundPtr> 
SimpleQueue::getQueueCopy(ConnectionID cid)
{
    assure(queues.find(cid) != queues.end(), "getQueueCopy called for non-existent CID");
    return queues[cid].pduQueue;
}

bool
SimpleQueue::isEmpty() const
{
    for (QueueContainer::const_iterator iter = queues.begin(); iter != queues.end(); ++iter)
    {
        if ((*iter).second.pduQueue.size() != 0)
            return false;
    }
    return true;
}
bool
SimpleQueue::hasQueue(ConnectionID cid)
{
    return queues.find(cid) != queues.end();
}

bool
SimpleQueue::queueHasPDUs(ConnectionID cid) const {
    if (queues.find(cid) == queues.end())
        return false;
    return (queues.find(cid)->second.pduQueue.size() != 0);
}

ConnectionSet
SimpleQueue::filterQueuedCids(ConnectionSet connections) {
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
SimpleQueue::setColleagues(RegistryProxyInterface* _registry) {
    colleagues.registry = _registry;
    maxSize = colleagues.registry->getQueueSizeLimitPerConnection();
}

QueueInterface::ProbeOutput
SimpleQueue::resetAllQueues()
{
    // Store number of bits and compounds for Probe which will be deleted
    ProbeOutput probeOutput;
    for (QueueContainer::iterator iter = queues.begin();
         iter != queues.end(); ++iter)
    {
        ConnectionID cid = iter->first;
        probeOutput.bits += iter->second.bits;
        probeOutput.compounds += iter->second.pduQueue.size();
        if (probeContextProviderForCid && probeContextProviderForPriority && sizeProbeBus) {
            probeContextProviderForCid->set(cid);
            int priority = colleagues.registry->getPriorityForConnection(cid);
            probeContextProviderForCid->set(priority);
            sizeProbeBus->put(0.0 /*double wert*/);
        }
    }

    // queues is a std::map that stores the std::queues that store the
    // CompoundPtrs. So by doing a queues.clear(), the destructors are called
    // and the refCounting mechanism of the CompoundPtr takes care of actually
    // deleting the compounds.
    queues.clear();

    return probeOutput;
}

// [rs]: obsolete? Better use cid-related questions
QueueInterface::ProbeOutput
SimpleQueue::resetQueues(UserID _user)
{
    // MESSAGE_SINGLE(NORMAL, logger, "SimpleQueue::resetQueues(): obsolete"); // TODO [rs]; not supported by [aoz]
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
            probeOutput.bits += iter->second.bits;
            probeOutput.compounds += iter->second.pduQueue.size();
            if (probeContextProviderForCid && probeContextProviderForPriority && sizeProbeBus) {
                probeContextProviderForCid->set(cid);
                int priority = colleagues.registry->getPriorityForConnection(cid);
                probeContextProviderForCid->set(priority);
                sizeProbeBus->put(0.0 /*double wert*/);
            }
            queues.erase(iter++);
        }
        else
            ++iter;
    }
    return probeOutput;
}

QueueInterface::ProbeOutput
SimpleQueue::resetQueue(ConnectionID cid)
{
    // Store number of bits and compounds for Probe which will be deleted
    ProbeOutput probeOutput;
    probeOutput.bits += queues[cid].bits;
    probeOutput.compounds += queues[cid].pduQueue.size();
    if (probeContextProviderForCid && probeContextProviderForPriority && sizeProbeBus) {
        probeContextProviderForCid->set(cid /*int context*/);
        int priority = colleagues.registry->getPriorityForConnection(cid);
        probeContextProviderForCid->set(priority);
        sizeProbeBus->put(0.0 /*double wert*/);
    }

#ifndef NDEBUG
    int numRemoved =
#endif
        queues.erase(cid);
    assure(numRemoved == 1, "Non-existing or too many queues with that CID");

    return probeOutput;
}

std::string
SimpleQueue::printAllQueues()
{
    std::stringstream s;
    for (QueueContainer::iterator iter = queues.begin();
         iter != queues.end(); ++iter)
    {
        ConnectionID cid = iter->first;
        int bits      = iter->second.bits;
        int compounds = iter->second.pduQueue.size();
        s << cid << ":" << bits << "," << compounds << " ";
    }
    return s.str();
}


