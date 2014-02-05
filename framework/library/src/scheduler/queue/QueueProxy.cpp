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

#include <WNS/scheduler/queue/QueueProxy.hpp>
#include <WNS/ldk/Layer.hpp>

using namespace wns::scheduler::queue;

STATIC_FACTORY_REGISTER_WITH_CREATOR(QueueProxy,
                                     wns::scheduler::queue::QueueInterface,
                                     "wns.scheduler.queue.QueueProxy",
                                     wns::HasReceptorConfigCreator);

QueueProxy::QueueProxy(wns::ldk::HasReceptorInterface*, const wns::pyconfig::View& _config) :     
    queueManagerServiceName_(_config.get<std::string>("queueManagerServiceName")),
    supportsDynamicSegmentation_(_config.get<bool>("supportsDynamicSegmentation")),
    copyQueue_(NULL),
    logger_(_config.get("logger")),
    myFUN_(NULL)
{
    MESSAGE_BEGIN(NORMAL, logger_, m, "QueueProxy");
    m << " Created ";
    m << " QueueProxy Queue using QueueManagerService ";
    m << queueManagerServiceName_;
    MESSAGE_END();

    if(supportsDynamicSegmentation_)
    {
        copyQueue_ = new detail::SegmentingInnerCopyQueue(_config.get("segmentingQueueConfig"));
    }
    else
    {
        copyQueue_ = new detail::SimpleInnerCopyQueue();
    }

}

QueueProxy::~QueueProxy()
{
    assure(copyQueue_ != NULL, "Want to delete copyQueue_ but it is NULL");
    delete copyQueue_;
}

void QueueProxy::setFUN(wns::ldk::fun::FUN* fun)
{
    myFUN_ = fun;
    colleagues.queueManager_ = fun->getLayer()->
            getManagementService<wns::scheduler::queue::IQueueManager>(
                queueManagerServiceName_);
    assure(colleagues.queueManager_, "QueueProxy needs a QueueManager");

    copyQueue_->setFUN(myFUN_);

    MESSAGE_BEGIN(NORMAL, logger_, m, myFUN_->getName());
    m << " Received valid FUN pointer and QueueManagerService ";
    m << queueManagerServiceName_;
    MESSAGE_END();
}

bool QueueProxy::isAccepting(const wns::ldk::CompoundPtr&  compound ) const 
{
    return false;
}

void
QueueProxy::put(const wns::ldk::CompoundPtr& compound) 
{
    assure(false, "Put called for readOnly QueueProxy");
}

wns::scheduler::UserSet
QueueProxy::getQueuedUsers() const 
{
    wns::scheduler::UserSet us;

    QueueContainer queues = colleagues.queueManager_->getAllQueues();    

    QueueContainer::const_iterator it;
    for(it = queues.begin(); it != queues.end(); it++)
    {
        startCollectionIfNeeded(it->first);

        wns::scheduler::ConnectionSet innerCs;

        // We have to ask our RegistryProxy to map the CID to a UserID!
        innerCs = it->second->getActiveConnections();
        wns::scheduler::ConnectionSet::iterator iit;
        
        for(iit = innerCs.begin(); iit != innerCs.end(); iit++)
            us.insert(colleagues.registry_->getUserForCID(*iit));
    }
    return us;

}

wns::scheduler::ConnectionSet
QueueProxy::getActiveConnections() const
{
    wns::scheduler::ConnectionSet cs;

    QueueContainer queues = colleagues.queueManager_->getAllQueues();

    QueueContainer::iterator it;
    for(it = queues.begin(); it != queues.end(); it++)
    {
        startCollectionIfNeeded(it->first);

        wns::scheduler::ConnectionSet innerCs;
        innerCs = it->second->getActiveConnections();
        wns::scheduler::ConnectionSet::iterator iit;
        
        for(iit = innerCs.begin(); iit != innerCs.end(); iit++)
            cs.insert(*iit);
    }
    return cs;
}

unsigned long int
QueueProxy::numCompoundsForCid(wns::scheduler::ConnectionID cid) const
{
    assure(colleagues.queueManager_->getQueue(cid) != NULL, "No queue for this CID");

    if(!copyQueue_->knowsCID(cid))
    {
        startCollectionIfNeeded(cid);
        return colleagues.queueManager_->getQueue(cid)->numCompoundsForCid(cid);
    }
    else
    {
        return copyQueue_->getSize(cid);
    }
}

unsigned long int
QueueProxy::numBitsForCid(wns::scheduler::ConnectionID cid) const
{
    assure(colleagues.queueManager_->getQueue(cid) != NULL, "No queue for this CID");

    if(!copyQueue_->knowsCID(cid))
    {
        startCollectionIfNeeded(cid);
        return colleagues.queueManager_->getQueue(cid)->numBitsForCid(cid);
    }
    else
    {
        return copyQueue_->getSizeInBit(cid);
    }
}

wns::scheduler::QueueStatusContainer
QueueProxy::getQueueStatus(bool forFuture) const
{
    wns::scheduler::QueueStatusContainer csc;

    QueueContainer queues = colleagues.queueManager_->getAllQueues();    

    QueueContainer::iterator it;
    for(it = queues.begin(); it != queues.end(); it++)
    {
        startCollectionIfNeeded(it->first);

        wns::scheduler::QueueStatusContainer innerCsc;
        innerCsc = it->second->getQueueStatus(forFuture);
        wns::scheduler::QueueStatusContainer::const_iterator iit;

        for(iit = innerCsc.begin(); iit != innerCsc.end(); iit++)
            csc.insert(iit->first, iit->second);
    }
    return csc;
}

wns::ldk::CompoundPtr
QueueProxy::getHeadOfLinePDU(wns::scheduler::ConnectionID cid) 
{        
    assure(!copyQueue_->isEmpty(cid), "Requested PDU from emty queue");
    
    wns::ldk::CompoundPtr pdu = copyQueue_->getPDU(cid);        
    return pdu;
}

int
QueueProxy::getHeadOfLinePDUbits(wns::scheduler::ConnectionID cid)
{
    assure(hasQueue(cid), "No queue for this CID");

    if(!copyQueue_->knowsCID(cid))
    {
        startCollectionIfNeeded(cid);
        return colleagues.queueManager_->getQueue(cid)->getHeadOfLinePDUbits(cid);
    }
    else
    {
        return copyQueue_->getHeadofLinePDUBit(cid);
    }
}

bool
QueueProxy::isEmpty() const
{
    QueueContainer queues = colleagues.queueManager_->getAllQueues();    
    QueueContainer::iterator it;
    for(it = queues.begin(); it != queues.end(); it++)
    {
        if(queueHasPDUs(it->first))
            return false;
    }
    return true;
}

bool
QueueProxy::hasQueue(wns::scheduler::ConnectionID cid)
{
    wns::scheduler::queue::QueueInterface* queue;
    queue = colleagues.queueManager_->getQueue(cid);

    return (queue == NULL)?false:true;
}

bool
QueueProxy::queueHasPDUs(wns::scheduler::ConnectionID cid) const 
{
    wns::scheduler::queue::QueueInterface* queue;
    queue = colleagues.queueManager_->getQueue(cid);

    if(queue != NULL)
    {
        createQueueCopyIfNeeded(cid);

        if(copyQueue_->knowsCID(cid))
        {
            if(copyQueue_->isEmpty(cid)) 
            {
                MESSAGE_BEGIN(NORMAL, logger_, m, myFUN_->getName());
                m << " queueHasPDUs: CopyQueue for CID " << cid << " is empty.";
                MESSAGE_END();

                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            MESSAGE_BEGIN(NORMAL, logger_, m, myFUN_->getName());
            m << " queueHasPDUs: Passing call for  CID " << cid << " to real queue.";
            MESSAGE_END();

            return queue->queueHasPDUs(cid);
        }
    }
    else
    {
        return false;
    }
}

wns::scheduler::ConnectionSet
QueueProxy::filterQueuedCids(wns::scheduler::ConnectionSet connections) 
{
    colleagues.queueManager_->getAllQueues();    
}

void
QueueProxy::setColleagues(wns::scheduler::RegistryProxyInterface* registry) 
{
    colleagues.registry_ = registry;
}

wns::scheduler::queue::QueueInterface::ProbeOutput
QueueProxy::resetAllQueues()
{
    wns::scheduler::queue::QueueInterface::ProbeOutput po;

    QueueContainer queues = colleagues.queueManager_->getAllQueues();    

    QueueContainer::iterator it;
    for(it = queues.begin(); it != queues.end(); it++)
    {
        wns::scheduler::queue::QueueInterface::ProbeOutput innerPo;
        innerPo = it->second->resetAllQueues();
        po.bits += innerPo.bits;
        po.compounds += innerPo.compounds;

        // Empty the copyQueue but do not count for statistics
        if(copyQueue_->knowsCID(it->first))
            copyQueue_->reset(it->first);
    }
    return po;   
}

wns::scheduler::queue::QueueInterface::ProbeOutput
QueueProxy::resetQueues(wns::scheduler::UserID _user)
{
    assure(false, "Not implemeted, use request with CID instead");
}

wns::scheduler::queue::QueueInterface::ProbeOutput
QueueProxy::resetQueue(wns::scheduler::ConnectionID cid)
{
    assure(hasQueue(cid), "No queue for this CID");

    // Empty the copyQueue but do not count for statistics
    if(copyQueue_->knowsCID(cid))
        copyQueue_->reset(cid);

    return colleagues.queueManager_->getQueue(cid)->resetQueue(cid);    
}

void
QueueProxy::frameStarts()
{
}

std::string
QueueProxy::printAllQueues()
{
    std::stringstream s;
    
    QueueContainer queues = colleagues.queueManager_->getAllQueues();    

    QueueContainer::iterator it;
    for(it = queues.begin(); it != queues.end(); it++)
    {    
        startCollectionIfNeeded(it->first);
        s << it->second->printAllQueues() << "\n";
    }
    return s.str();
}

bool
QueueProxy::supportsDynamicSegmentation() const
{
    return supportsDynamicSegmentation_;   
}

wns::ldk::CompoundPtr 
QueueProxy::getHeadOfLinePDUSegment(wns::scheduler::ConnectionID cid, int bits)
{
    assure(supportsDynamicSegmentation_, "Dynamic segmentation not supported");
    assure(!copyQueue_->isEmpty(cid), "Requested PDU from emty queue");
    
    wns::ldk::CompoundPtr pdu = copyQueue_->getPDU(cid, bits);        
    return pdu;
}

int 
QueueProxy::getMinimumSegmentSize() const
{
    assure(supportsDynamicSegmentation_, "Dynamic segmentation not supported");
    detail::SegmentingInnerCopyQueue* q;
    q = dynamic_cast<detail::SegmentingInnerCopyQueue*>(copyQueue_);

    return q->getMinimumSegmentSize();
}

void
QueueProxy::createQueueCopyIfNeeded(wns::scheduler::ConnectionID cid) const
{
    wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();

    wns::scheduler::queue::QueueInterface* queue;
    queue = colleagues.queueManager_->getQueue(cid);

    // New round, create new PDUs in copyQueue
    if(queue != NULL &&  
        (lastChecked_.find(cid) == lastChecked_.end() || lastChecked_[cid] != now))
    {
        lastChecked_[cid] = now;

        // Empty the old copy queue
        if(copyQueue_->knowsCID(cid))
        {
            MESSAGE_BEGIN(NORMAL, logger_, m, myFUN_->getName());
            m << " Removing " << copyQueue_->getSize(cid) << " PDUs form old copyQueue ";
            m << " for CID " << cid;
            MESSAGE_END();

            copyQueue_->reset(cid);
        }
        
        startCollectionIfNeeded(cid);

        if(!queue->queueHasPDUs(cid))
        {
            MESSAGE_BEGIN(NORMAL, logger_, m, myFUN_->getName());
            m << " Real queue is empty for CID: ";
            m << cid;
            MESSAGE_END();
            return;
        }

        copyQueue_->setQueue(cid, queue->getQueueCopy(cid));

        MESSAGE_BEGIN(NORMAL, logger_, m, myFUN_->getName());
        m << " Created a copy of " << copyQueue_->getSize(cid) << " PDUs for CID ";
        m << cid;
        MESSAGE_END();

    }
}

std::queue<wns::ldk::CompoundPtr> 
QueueProxy::getQueueCopy(ConnectionID cid)
{ 
    wns::Exception("You should not call getQueueCopy of the QueueProxy."); 
}

void
QueueProxy::startCollectionIfNeeded(wns::scheduler::ConnectionID cid) const
{
    wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();

    if(lastCollected_.find(cid) == lastCollected_.end() || lastCollected_[cid] != now)
    {
        lastCollected_[cid] = now;
        colleagues.queueManager_->startCollection(cid);
    }    
}

