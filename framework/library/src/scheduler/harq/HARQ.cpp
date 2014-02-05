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

#include <WNS/scheduler/harq/HARQ.hpp>
#include <WNS/Exception.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

using namespace wns::scheduler::harq;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    HARQ,
    HARQInterface,
    "harq",
    wns::PyConfigViewCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    UniformRandomDecoder,
    IDecoder,
    "UniformRandomDecoder",
    wns::PyConfigViewCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    ChaseCombiningDecoder,
    IDecoder,
    "ChaseCombiningDecoder",
    wns::PyConfigViewCreator);

UniformRandomDecoder::UniformRandomDecoder(const wns::pyconfig::View& config):
    dis_(new wns::distribution::StandardUniform()),
    initialPER_(config.get<double>("initialPER")),
    rolloffFactor_(config.get<double>("rolloffFactor")),
    logger_(config.get("logger"))
{
}

UniformRandomDecoder::UniformRandomDecoder(const UniformRandomDecoder& other):
    dis_(new wns::distribution::StandardUniform()),
    initialPER_(other.initialPER_),
    rolloffFactor_(other.rolloffFactor_),
    logger_(other.logger_)
{
}

bool
UniformRandomDecoder::canDecode(const SoftCombiningContainer& input)
{
    int numTransmissions = 0;

    std::list<int> positions = input.getAvailablePosInTB();

    assure(positions.size() > 0, "Nothing to decode");

    for (int ii=0; ii < input.getNumRVs(); ++ii)
    {
        numTransmissions += input.getEntriesForRV(positions.front(), ii).size();
    }

    double threshold = pow(initialPER_, numTransmissions * rolloffFactor_);

    //MESSAGE_BEGIN(NORMAL, logger_, m, "");
    //m << "Effective PER is " << threshold;
    //MESSAGE_END();

    if ((*dis_)() > threshold)
    {
        return true;
    }
    return false;
}

ChaseCombiningDecoder::ChaseCombiningDecoder(const wns::pyconfig::View& config):
    dis_(new wns::distribution::StandardUniform()),
    logger_(config.get("logger")),
    effSINRCC_("scheduler.harq.effSINR")
{
}

ChaseCombiningDecoder::ChaseCombiningDecoder(const ChaseCombiningDecoder& other):
    dis_(new wns::distribution::StandardUniform()),
    logger_(other.logger_),
    effSINRCC_("scheduler.harq.effSINR")
{
}

bool
ChaseCombiningDecoder::canDecode(const SoftCombiningContainer& input)
{
    // input contains softcombining containers including multiple subchannels

    // Calculate effective MIB

    // According to IEEE 802.16m-08/004r5 (PMD) pp. 91 Chase Combinig can be modeled as
    // follows. Sum up all SINR values in the linear domain (power, not dB) for one symbol. From that value
    // determine the Sum Mutual Information per bit (MIB). Calculate the average MIB of all symbols.
    // From this value the PER can be detected according to the coding scheme
    // Here we do not work on symbols but on subchannels.

    double mib = 0;
    int totalSize = 0;
    int numSC = 0;


    std::list<int> positions = input.getAvailablePosInTB();

    assure(positions.size() > 0, "Nothing to decode");

    assure(input.getEntriesForRV(positions.front(),0).size() > 0, "Nothing to decode");

    wns::service::phy::phymode::PhyModeInterfacePtr pm = input.getEntriesForRV(positions.front(),0).front().measurement_->getPhyMode();

    MESSAGE_SINGLE(NORMAL, logger_, "Decoding a TB spanning " << positions.size() << " subchannels");

    std::list<int>::iterator it;

    for (it=positions.begin(); it!=positions.end(); ++it)
    {
        numSC++;

        for (int ii=1; ii < input.getNumRVs(); ++ii)
        {
            assure(input.getEntriesForRV(*it, ii).size() == 0, "ChaseCombining expects only RV 0 to be used, but RV=" << ii << " is used, too.");
        }

        std::list<SchedulingTimeSlotInfo> sis = input.getEntriesForRV(*it, 0);
        assure(sis.size() > 0, "Chase combining has no receptions for RV 0.");

        std::list<SchedulingTimeSlotInfo>::iterator it;

        assure( (*pm)==(*sis.front().measurement_->getPhyMode()), "Must have the same phy modes on all SCs");

        double sumSINR = 0.0;
        // For all retransmissions:
        for(it = sis.begin(); it != sis.end(); ++it)
        {
            sumSINR += it->measurement_->getSINR().get_factor();
        }

        totalSize += sis.front().timeSlot_->getNetBlockSizeInBits(); // (net,netto, i.e. without code/redundancy bits)
        mib += sis.front().measurement_->getPhyMode()->getSINR2MIB(wns::Ratio::from_factor(sumSINR));
    }

    // Mean MIB for whole TB
    mib = mib/numSC;

    double per = pm->getMI2PER(mib, totalSize); // MIB not MI is expected here

    MESSAGE_BEGIN(NORMAL, logger_, m, "canDecode: PhyMode=" << *pm);
    m << ", Average MIB=" << mib;
    m << ", bits=" << totalSize;
    m << " => per=" << per;
    MESSAGE_END();

    wns::scheduler::UserID userID = input.getEntriesForRV(positions.front(),0).front().timeSlot_->physicalResources[0].getSourceUserIDOfScheduledCompounds();
    unsigned int nodeID = userID.getNodeID();

    if ((*dis_)() > per)
    {
        MESSAGE_SINGLE(NORMAL, logger_, "Decode success");

        effSINRCC_.put(pm->getMIB2SINR(mib).get_dB(), boost::make_tuple("nodeID",nodeID, "decoded", 1));
        return true;
    }
    MESSAGE_SINGLE(NORMAL, logger_, "Decode failed");
    effSINRCC_.put(pm->getMIB2SINR(mib).get_dB(), boost::make_tuple("nodeID",nodeID, "decoded", 0));
    return false;
}

HARQ::HARQ(const wns::pyconfig::View& config):
    numSenderProcesses_(config.get<int>("numSenderProcesses")),
    numReceiverProcesses_(config.get<int>("numReceiverProcesses")),
    numRVs_(config.get<int>("numRVs")),
    logger_(config.get("logger")),
    retransmissionLimit_(config.get<int>("retransmissionLimit")),
    harqEntityPrototype_(new HARQEntity(config.get("harqEntity"), numSenderProcesses_, numReceiverProcesses_, numRVs_, retransmissionLimit_, logger_)),
    numRetransmissionsProbeCC("scheduler.harq.retransmissions")
{
}

HARQ::~HARQ()
{
}

HARQEntity*
HARQ::findEntity(wns::scheduler::UserID userID)
{
    if (!harqEntities_.knows(userID))
    {
        MESSAGE_SINGLE(NORMAL, logger_, "Creating new HARQEntity for peer UserID=" << userID.getName() );
        HARQEntity* he = wns::clone(harqEntityPrototype_);
        harqEntities_.insert(userID, he);

        return he;
    }

    return harqEntities_.find(userID);
}

void
HARQ::storeSchedulingTimeSlot(long int transportBlockID, const wns::scheduler::SchedulingTimeSlotPtr& resourceBlock)
{
    assure(resourceBlock->physicalResources[0].hasScheduledCompounds(), "No resources in RB");

    int subChannelIndex = resourceBlock->subChannelIndex;
    int timeSlotIndex   = resourceBlock->timeSlotIndex;

    if (!resourceBlock->isHARQEnabled())
    {
        MESSAGE_SINGLE(NORMAL, logger_, "storeSchedulingTimeSlot("<<subChannelIndex<<"."<<timeSlotIndex<<"): HARQ is disabled. Skipping.");
        MESSAGE_SINGLE(NORMAL, logger_, resourceBlock->toString());
        return;
    }
    else if (resourceBlock->isEmpty())
    {
        MESSAGE_SINGLE(NORMAL, logger_, "storeSchedulingTimeSlot("<<subChannelIndex<<"."<<timeSlotIndex<<"): time slot is empty");
        MESSAGE_SINGLE(NORMAL, logger_, resourceBlock->toString());
        return;
    }
    else if (!resourceBlock->harq.NDI)
    {
        MESSAGE_SINGLE(NORMAL, logger_, "storeSchedulingTimeSlot("<<subChannelIndex<<"."<<timeSlotIndex<<"): Found a retransmission. Skipping.");
        MESSAGE_SINGLE(NORMAL, logger_, resourceBlock->toString());
        return;
    }
    else
    {
        // The destination user is in the scheduled compound
        // The scheduling time slot does not have the right user,
        // because of the way how the wns::scheduler works.
        // Certainly, this could be done better, works for now

        /**
         * @todo dbn: Improve fetching of correct UserID in HARQ
         */
        wns::scheduler::UserID destination = resourceBlock->physicalResources[0].getUserIDOfScheduledCompounds();

        //MESSAGE_SINGLE(NORMAL, logger_, "storeSchedulingTimeSlot("<<subChannelIndex<<"."<<timeSlotIndex<<")");
        MESSAGE_SINGLE(NORMAL, logger_, "storeSchedulingTimeSlot("<<subChannelIndex<<"."<<timeSlotIndex<<")"
                       <<" for "<< destination.getName());
        MESSAGE_SINGLE(NORMAL, logger_, resourceBlock->toString());
    }

    // at this point we have a brand new transmission which we must take care of...
    // this is the userID seen from here (the sending peer):
    wns::scheduler::UserID userID = resourceBlock->physicalResources[0].getUserIDOfScheduledCompounds();

    resourceBlock->harq.firstTxTime = wns::simulator::getEventScheduler()->getTime();

    HARQEntity* entity = this->findEntity(userID);

    assure(entity->hasCapacity(transportBlockID), "The HARQ Entity for peer UserID=" << userID.getName() << " does not have anymore capacity!");

    // now store the SchedulingTimeSlotPtr
    /**
     * @todo dbn: Need to ask for permission. Use entity->hasCapacity
     */
    entity->newTransmission(transportBlockID, resourceBlock);
}

void
HARQ::onTimeSlotReceived(const wns::scheduler::SchedulingTimeSlotPtr& resourceBlock,
                         HARQInterface::TimeSlotInfo info)
{
    wns::scheduler::UserID userID = resourceBlock->physicalResources[0].getSourceUserIDOfScheduledCompounds();


    MESSAGE_SINGLE(NORMAL, logger_, "onTimeSlotReceived(resource="
                   << resourceBlock->subChannelIndex << "."
                   << resourceBlock->timeSlotIndex <<")"
                   << " for "<< userID.getName());

    resourceBlock->consistencyCheck();

    HARQEntity* entity = this->findEntity(userID);
    /**
     * @todo dbn: Need to ask for permission. Use entity->hasCapacity
     */
    entity->onTimeSlotReceived(resourceBlock, info);

    MESSAGE_SINGLE(NORMAL, logger_, "onTimeSlotReceived(resource="<<resourceBlock->subChannelIndex<<"."<<resourceBlock->timeSlotIndex<<")"
                   <<" for "<<userID.getName()<< " TBID " << resourceBlock->harq.transportBlockID << ")");
}

HARQInterface::DecodeStatusContainer
HARQ::decode()
{
    HARQInterface::DecodeStatusContainer tmp;


    // Join all others
    for(HARQEntityContainer::const_iterator it=harqEntities_.begin(); it!=harqEntities_.end(); ++it)
    {
        HARQInterface::DecodeStatusContainer perEntity;
        perEntity = it->second->decode();

        for (HARQInterface::DecodeStatusContainer::iterator it2=perEntity.begin(); it2!=perEntity.end(); ++it2)
        {
            if (it2->first->harq.successfullyDecoded || it2->first->harq.retryCounter >= retransmissionLimit_ ) {
                wns::scheduler::UserID userID = it2->first->physicalResources[0].getSourceUserIDOfScheduledCompounds();
                unsigned int nodeID = userID.getNodeID();
                numRetransmissionsProbeCC.put(it2->first->harq.retryCounter, boost::make_tuple("nodeID",nodeID));
            }
            tmp.push_back(*it2);
        }
    }

    return tmp;
}

wns::scheduler::UserSet
HARQ::getUsersWithRetransmissions() const
{
  wns::scheduler::UserSet users;

  for (HARQEntityContainer::const_iterator it = harqEntities_.begin();
       it != harqEntities_.end();
       ++it)
    { // foreach user
      if (it->second->hasRetransmissions())
	{
	  users.insert(it->first);
	}
    }
  return users;
}

std::list<int>
HARQ::getProcessesWithRetransmissions(wns::scheduler::UserID peer) const
{
    if(harqEntities_.knows(peer))
    {
        return harqEntities_.find(peer)->getProcessesWithRetransmissions();
    }
    else
    {
        std::list<int> empty;
        return empty;
    }
}

int
HARQ::getNumberOfRetransmissions(wns::scheduler::UserID user, int processID)
{
  HARQEntity* he = findEntity(user);
  return he->getNumberOfRetransmissions(processID);
}


wns::scheduler::SchedulingTimeSlotPtr
HARQ::getNextRetransmission(wns::scheduler::UserID user, int processID)
{
  if (harqEntities_.knows(user))
  {
    HARQEntity* he = findEntity(user);

    wns::scheduler::SchedulingTimeSlotPtr r;
    r = he->getNextRetransmission(processID);

    if (r != NULL)
      {
	r->consistencyCheck();
	return r;
      }
  }
  else
  {
    throw wns::Exception("No user by that name in getNextRetransmission " + user.getName());
  }

  return wns::scheduler::SchedulingTimeSlotPtr(); // empty
}

wns::scheduler::SchedulingTimeSlotPtr
HARQ::peekNextRetransmission(wns::scheduler::UserID user, int processID) const
{
  if (harqEntities_.knows(user))
  {
    wns::scheduler::SchedulingTimeSlotPtr r;
    r = harqEntities_.find(user)->peekNextRetransmission(processID);

    if (r != NULL)
    {
      r->consistencyCheck();
      return r;
    }
  }
  return wns::scheduler::SchedulingTimeSlotPtr(); // empty
}

void
HARQ::setDownlinkHARQ(HARQInterface* downlinkHARQ)
{
    assure(false, "You tried to set a downlinkHARQ in the downlink HARQ dude.");
}

wns::scheduler::UserSet
HARQ::getPeersWithPendingRetransmissions() const
{
    wns::scheduler::UserSet u;

    for (HARQEntityContainer::const_iterator it = harqEntities_.begin();
         it != harqEntities_.end();
         ++it)
    { // foreach user
        if (it->second->getPeerProcessesWithRetransmissions().size() > 0)
        {
            u.insert(it->first);
        }
    }
    return u;
}

std::list<int>
HARQ::getPeerProcessesWithRetransmissions(wns::scheduler::UserID peer) const
{
    if(harqEntities_.knows(peer))
    {
        return harqEntities_.find(peer)->getPeerProcessesWithRetransmissions();
    }
    else
    {
        std::list<int> empty;
        return empty;
    }
}

int
HARQ::getNumberOfPeerRetransmissions(wns::scheduler::UserID peer, int processID) const
{
    if(harqEntities_.knows(peer))
    {
        return harqEntities_.find(peer)->getNumberOfPeerRetransmissions(processID);
    }

    return 0;
}

bool
HARQ::hasFreeSenderProcess(wns::scheduler::UserID peer)
{
    if(harqEntities_.knows(peer))
    {
        return harqEntities_.find(peer)->hasCapacity(0);
    }
    return true;
}

bool
HARQ::hasFreeReceiverProcess(wns::scheduler::UserID peer)
{
    if (harqEntities_.knows(peer))
    {
        return harqEntities_.find(peer)->hasReceiverCapacity();
    }
    return true;
}

void
HARQ::schedulePeerRetransmissions(wns::scheduler::UserID peer, int processID)
{
    if(harqEntities_.knows(peer))
    {
        return harqEntities_.find(peer)->schedulePeerRetransmissions(processID);
    }
}

void
HARQ::schedulePeerRetransmission(wns::scheduler::UserID peer, int processID)
{
    if(harqEntities_.knows(peer))
    {
        return harqEntities_.find(peer)->schedulePeerRetransmission(processID);
    }
}

void
HARQ::sendPendingFeedback()
{
    for(HARQEntityContainer::const_iterator it = harqEntities_.begin();
    it != harqEntities_.end();
    ++it)
    {
        it->second->sendPendingFeedback();
    }
}

HARQEntity::HARQEntity(const wns::pyconfig::View& config, int numSenderProcesses, int numReceiverProcesses, int numRVs, int retransmissionLimit, wns::logger::Logger logger):
    numSenderProcesses_(numSenderProcesses),
    numReceiverProcesses_(numReceiverProcesses),
    numRVs_(numRVs),
    retransmissionLimit_(retransmissionLimit),
    logger_(logger),
    scheduledPeerRetransmissionCounter_(0)
{
    decoder_ = std::auto_ptr<IDecoder>(STATIC_FACTORY_NEW_INSTANCE(IDecoder, wns::PyConfigViewCreator, config.get("decoder"), config.get("decoder")));

    for (int ii=0; ii < numSenderProcesses_; ++ii)
    {
        senderProcesses_.push_back(HARQSenderProcess(this, ii, numRVs_, config.get<int>("retransmissionLimit"), logger_));
    }

    for (int ii=0; ii < numReceiverProcesses_; ++ii)
    {
        receiverProcesses_.push_back(HARQReceiverProcess(config.get("receiverConfig"), this, ii, numRVs_, config.get<int>("retransmissionLimit"), logger_));
    }
    /**
     * @todo dbn: A probe would be great, how many processes are occupied over time...
     */
}

HARQEntity::HARQEntity(const HARQEntity& other):
    senderProcesses_(other.senderProcesses_),
    receiverProcesses_(other.receiverProcesses_),
    numSenderProcesses_(other.numSenderProcesses_),
    numReceiverProcesses_(other.numReceiverProcesses_),
    numRVs_(other.numRVs_),
    retransmissionLimit_(other.retransmissionLimit_),
    logger_(other.logger_),
    scheduledPeerRetransmissionCounter_(other.scheduledPeerRetransmissionCounter_),
    decoder_(wns::clone(other.decoder_))
{
    for (int ii=0; ii < numSenderProcesses_; ++ii)
    {
        senderProcesses_[ii].setEntity(this);
    }

    for (int ii=0; ii < numReceiverProcesses_; ++ii)
    {
        receiverProcesses_[ii].setEntity(this);
    }
}

void
HARQEntity::newTransmission(long int transportBlockID, const wns::scheduler::SchedulingTimeSlotPtr& resourceBlock)
{
    //assure(resourceBlock->physicalResources[0].hasScheduledCompounds(), "No resources in RB");

    if (!resourceBlock->isHARQEnabled())
    {
        return;
    }

    assure(hasCapacity(transportBlockID), "HARQ Entity is busy but you wanted to start a new transmission!");

    // find free sender process
    for (int ii=0; ii < numSenderProcesses_; ++ii)
    {
        if (senderProcesses_[ii].hasCapacity(transportBlockID))
        {
            // Let process put in proper HARQInfo
            senderProcesses_[ii].newTransmission(transportBlockID, resourceBlock);

            MESSAGE_SINGLE(NORMAL, logger_, "newTransmission for sender process " << ii << "(TID: " << transportBlockID << ")");
            return; // break; // found
        }
    }
    assure(false, "HARQEntity::newTransmission: cannot find free senderProcess");
}

bool
HARQEntity::hasCapacity(long int transportBlockID)
{
    for (int ii=0; ii < numSenderProcesses_; ++ii)
    {
        // Any of my send processes idle?
        if (senderProcesses_[ii].hasCapacity(transportBlockID))
        {
            return true;
        }
    }
    return false;
}

bool
HARQEntity::hasReceiverCapacity()
{
    int count = 0;
    for (int ii=0; ii < numReceiverProcesses_; ++ii)
    {
        // Any of my send processes idle?
        if (!receiverProcesses_[ii].isFree())
        {
            count++;
        }
    }

    if (count > 7)
    {
        return false;
    }

    return true;
}

void
HARQEntity::onTimeSlotReceived(const wns::scheduler::SchedulingTimeSlotPtr& resourceBlock,
                               HARQInterface::TimeSlotInfo info)
{
    if (!resourceBlock->isHARQEnabled())
    {
        return;
    }

    assure(resourceBlock->harq.processID < receiverProcesses_.size(), "Invalid receiver processID=" << resourceBlock->harq.processID);
    receiverProcesses_[resourceBlock->harq.processID].onTimeSlotReceived(resourceBlock, info);

}

HARQInterface::DecodeStatusContainer
HARQEntity::decode()
{
    HARQInterface::DecodeStatusContainer tmp;

    for (int ii=0; ii < numReceiverProcesses_; ++ii)
    {
        HARQInterface::DecodeStatusContainer perEntity;

        perEntity = receiverProcesses_[ii].decode();

        HARQInterface::DecodeStatusContainer::iterator it;

        for(it=perEntity.begin(); it!=perEntity.end(); ++it)
        {
            tmp.push_back(*it);
        }
    }
    return tmp;
}

std::list<int>
HARQEntity::getProcessesWithRetransmissions() const
{
    std::list<int> tmp;

    std::vector<HARQSenderProcess>::const_iterator it;

    for(it = senderProcesses_.begin(); it != senderProcesses_.end(); ++it)
    {
        if (it->getNumberOfRetransmissions() > 0)
        {
            tmp.push_back(it->processID());
        }
    }
    return tmp;
}

int
HARQEntity::getNumberOfRetransmissions(int processID)
{
  return senderProcesses_[processID].getNumberOfRetransmissions();
}

bool
HARQEntity::hasRetransmissions()
{
  for (int ii=0; ii < numSenderProcesses_; ++ii)
  {
    if (senderProcesses_[ii].getNumberOfRetransmissions() > 0)
    {
      return true;
    }
  }
  return false;
}

wns::scheduler::SchedulingTimeSlotPtr
HARQEntity::getNextRetransmission(int processID)
{
  return senderProcesses_[processID].getNextRetransmission();
}

wns::scheduler::SchedulingTimeSlotPtr
HARQEntity::peekNextRetransmission(int processID) const
{
  return senderProcesses_[processID].peekNextRetransmission();
}

std::list<int>
HARQEntity::getPeerProcessesWithRetransmissions() const
{
    std::list<int> tmp;

    std::vector<HARQReceiverProcess>::const_iterator it;

    for(it = receiverProcesses_.begin(); it != receiverProcesses_.end(); ++it)
    {
        if (it->numPendingPeerRetransmissions() > 0)
        {
            tmp.push_back(it->processID());
        }
    }
    return tmp;
}

int
HARQEntity::getNumberOfPeerRetransmissions(int processID) const
{
    return receiverProcesses_[processID].numPendingPeerRetransmissions();
}

void
HARQEntity::schedulePeerRetransmissions(int processID)
{
    receiverProcesses_[processID].schedulePeerRetransmissions();
}

void
HARQEntity::schedulePeerRetransmission(int processID)
{
    receiverProcesses_[processID].schedulePeerRetransmission();
}

void
HARQEntity::sendPendingFeedback()
{
    std::vector<HARQReceiverProcess>::iterator it;

    for(it = receiverProcesses_.begin(); it != receiverProcesses_.end(); ++it)
    {
        it->sendPendingFeedback();
    }
}

HARQReceiverProcess::HARQReceiverProcess(const wns::pyconfig::View& config, HARQEntity* entity, int processID, int numRVs, int retransmissionLimit, wns::logger::Logger logger):
    entity_(entity),
    processID_(processID),
    numRVs_(numRVs),
    retransmissionLimit_(retransmissionLimit),
    logger_(logger),
    receptionBuffer_(numRVs),
    numPendingPeerRetransmissions_(0)
{
}

HARQReceiverProcess::HARQReceiverProcess(const HARQReceiverProcess& other):
    entity_(NULL),
    processID_(other.processID_),
    numRVs_(other.numRVs_),
    retransmissionLimit_(other.retransmissionLimit_),
    logger_(other.logger_),
    receptionBuffer_(other.numRVs_),
    numPendingPeerRetransmissions_(0),
    waitingForRetransmissions_(false)
{
}

void
HARQReceiverProcess::setEntity(HARQEntity* entity)
{
    entity_ = entity;
}

void
HARQReceiverProcess::endReception()
{
    MESSAGE_SINGLE(NORMAL, logger_, "End Reception is now");
    receptionDelta_ = wns::events::scheduler::IEventPtr();
}

void
HARQReceiverProcess::onTimeSlotReceived(const wns::scheduler::SchedulingTimeSlotPtr& resourceBlock, HARQInterface::TimeSlotInfo info)
{
    MESSAGE_BEGIN(NORMAL, logger_, m, "HarqReceiverProcess::receive processID = " << processID_);
    m << ", RV = " << resourceBlock->harq.rv << ", RetryCounter=" << resourceBlock->harq.retryCounter << ", SINR=" << info.powerMeasurement_->getSINR();
    MESSAGE_END();

    // We received something, a new decoding attempt makes sense.
    waitingForRetransmissions_ = false;

    if (receptionDelta_ == wns::events::scheduler::IEventPtr())
    {
        if (resourceBlock->harq.NDI)
        {
            receptionBuffer_.clear();
        }

        // All timeslots of one frame arrive at the same time
        receptionDelta_= wns::simulator::getEventScheduler()->scheduleDelay(boost::bind(&HARQReceiverProcess::endReception, this), 0.000001);
    }

    std::list<int> positions = receptionBuffer_.getAvailablePosInTB();

    if (positions.size() > 0)
    {
      // Not the first one
      int tid = receptionBuffer_.getEntriesForRV(positions.front(), 0).front().timeSlot_->harq.transportBlockID;
      assure(tid == resourceBlock->harq.transportBlockID, "More than on TID in a reception buffer is wrong.wrong.wrong.");
    }

    SchedulingTimeSlotInfo tsinfo(resourceBlock, info.powerMeasurement_);
    
    if (resourceBlock->harq.ackCallback.empty())
      {
	std::cout << "Tried to store resource block with empty ack callback" << std::endl;
	exit(1);
      }

    if (resourceBlock->harq.nackCallback.empty())
      {
	std::cout << "Tried to store resource block with empty nack callback" << std::endl;
	exit(1);
      }

    receptionBuffer_.appendEntryForRV(resourceBlock->harq.tbPos, resourceBlock->harq.rv, tsinfo);

    return;
}

HARQInterface::DecodeStatusContainer
HARQReceiverProcess::decode()
{
    HARQInterface::DecodeStatusContainer tmp;

    if (waitingForRetransmissions_)
    {
        // No changes since last decode. Do nothing.
        return tmp;
    }
    
    std::list<int> positions = receptionBuffer_.getAvailablePosInTB();

    // Nothing received, nothing to decode
    if(positions.size() == 0)
    {
        return tmp;
    }

    /* mue: The whole TB has not been received yet. 
    This can happen if there were not enough RBs 
    for the whole retransmission. The scheduler should
    only retransmit complete TBs. */
    std::list<int>::iterator it;
    int sisSize = 0;
    for(it = positions.begin(); it != positions.end(); it++)
    {
        std::list<SchedulingTimeSlotInfo> sis = receptionBuffer_.getEntriesForRV(*it, 0);
        std::list<SchedulingTimeSlotInfo>::iterator iter;
        for(iter = sis.begin(); iter != sis.end(); ++iter)
        {
            if(sisSize != 0 && sisSize != sis.size())
            {
                assure(false, "Partial HARQ retransmission in receive buffer.");
            }
            sisSize = sis.size();
        }
    }

    // After we leave this function we will always be waiting for new input
    waitingForRetransmissions_ = true;

    int transportBlockID = receptionBuffer_.getEntriesForRV(positions.front(), 0).front().timeSlot_->harq.transportBlockID;
    if(entity_->decoder_->canDecode(receptionBuffer_))
    {
        MESSAGE_SINGLE(NORMAL, logger_, "HARQReceiver processID=" << processID_ << " sucessful decoded"
                       << " on transportBlock "<< transportBlockID);
        // Next frame is not necessarily the right direction
        // If e.g. in TDD Uplink we need to wait with the feedback
        // until the next UL frame.
        // sendPendingFeedback will then be triggered by the Timingscheduler
	HARQReceiverProcess::Feedback fb;
	fb.callback_ = receptionBuffer_.getEntriesForRV(positions.front(), 0).front().timeSlot_->harq.ackCallback;
	fb.retransmissionLimitHit_ = false;

        pendingFeedback_.push_back(fb);

        std::list<int>::iterator it;

        for(it=positions.begin();it!=positions.end();++it)
        {
            wns::scheduler::SchedulingTimeSlotPtr ts;
            ts = receptionBuffer_.getEntriesForRV(*it, 0).back().timeSlot_;
            ts->harq.successfullyDecoded = true;
            tmp.push_back(HARQInterface::DecodeStatusContainerEntry(ts,
                                                                    HARQInterface::TimeSlotInfo(wns::service::phy::power::PowerMeasurementPtr(), 0)
                                                                    ));
        }

        receptionBuffer_.clear();
        numPendingPeerRetransmissions_ = 0;
    }
    else
    {
        int retryCounter= receptionBuffer_.getEntriesForRV(positions.front(), 0).back().timeSlot_->harq.retryCounter;

        MESSAGE_SINGLE(NORMAL, logger_, "HARQReceiver processID=" << processID_ << " failed to decode"
                       << " on transportBlock "<< transportBlockID << " Retries: " << retryCounter);

	HARQReceiverProcess::Feedback fb;
	fb.callback_ = receptionBuffer_.getEntriesForRV(positions.front(), 0).front().timeSlot_->harq.nackCallback;
	fb.retransmissionLimitHit_ = retryCounter >= retransmissionLimit_;

        // Same is true for the NACKs
        pendingFeedback_.push_back(fb);
	
        if(retryCounter >= retransmissionLimit_)
        {
	    MESSAGE_SINGLE(NORMAL, logger_, "HARQReceiver processID=" << processID_ << " retransmission limit of "
			   << retransmissionLimit_ << " exceeded! Dropping resource blocks");

            receptionBuffer_.clear();
            setNumPendingPeerRetransmissions(0);
        }
    }
    return tmp;
}

void
HARQReceiverProcess::setNumPendingPeerRetransmissions(int num)
{
    MESSAGE_SINGLE(NORMAL, logger_, "setNumPendingPeerRetransmissins(" << num << ")");
    numPendingPeerRetransmissions_ = num;
}

void
HARQReceiverProcess::decreaseNumPendingPeerRetransmissions()
{
    assure(numPendingPeerRetransmissions_ > 0, "Cannot decrease HARQ pending ret. count, already 0");
    numPendingPeerRetransmissions_--;
    MESSAGE_SINGLE(NORMAL, logger_, "decreseNumPendingPeerRetransmissins() decreased to " 
        << numPendingPeerRetransmissions_);
}

void
HARQReceiverProcess::sendPendingFeedback()
{
    BOOST_FOREACH(HARQReceiverProcess::Feedback fb, pendingFeedback_)
    {
      if(!fb.callback_.empty())
	{
	  fb.callback_();
	}
      else
	{
	  std::cout << "Empty callback in HARQReceiverProcess::sendPendingFeedback()" << std::endl;
	  exit(1);
	}
    }

    if (pendingFeedback_.size() > 0)
    {
      if (!pendingFeedback_.front().retransmissionLimitHit_)
      {
        // Receiver of feedback needs processing delay, only then
        // the retransmissions are available, and only then report
        // peer retransmissions to our local uplink scheduler
        std::list<int> positions = receptionBuffer_.getAvailablePosInTB();

        wns::simulator::getEventScheduler()->scheduleDelay(boost::bind(&HARQReceiverProcess::setNumPendingPeerRetransmissions, this, positions.size()), 0.001999);
      }
      pendingFeedback_.clear();
    }

}

void
HARQReceiverProcess::schedulePeerRetransmissions()
{
    setNumPendingPeerRetransmissions(0);
}

void
HARQReceiverProcess::schedulePeerRetransmission()
{
    decreaseNumPendingPeerRetransmissions();
}

int
HARQReceiverProcess::numPendingPeerRetransmissions() const
{
    return numPendingPeerRetransmissions_;
}

int
HARQReceiverProcess::processID() const
{
    return processID_;
}

SoftCombiningContainer
HARQReceiverProcess::receptionBuffer() const
{
    return receptionBuffer_;
}

wns::scheduler::SchedulingTimeSlotPtr
HARQReceiverProcess::schedulingTimeSlot() const
{
    assure(false, "Need to fix this,i.e. the decoding");
    return wns::scheduler::SchedulingTimeSlotPtr();
}

bool
HARQReceiverProcess::isFree() const
{
    return (receptionBuffer_.getAvailablePosInTB().size() == 0);
}

HARQSenderProcess::HARQSenderProcess(HARQEntity* entity, int processID, int numRVs, int retransmissionLimit, wns::logger::Logger logger):
    entity_(entity),
    processID_(processID),
    numRVs_(numRVs),
    retransmissionLimit_(retransmissionLimit),
    logger_(logger),
    transportBlockID_(0),
    retransmissionCounter_(0),
    NDI_(true),
    nextPositionInTB_(0),
    pendingRetransmissions_()
{
}

void
HARQSenderProcess::setEntity(HARQEntity* entity)
{
    entity_ = entity;
}

void
HARQSenderProcess::newTransmission(long int transportBlockID,
                                   const wns::scheduler::SchedulingTimeSlotPtr& resourceBlock)
{
    assure(resourceBlock->physicalResources[0].hasScheduledCompounds(), "No resources in RB");
    assure(hasCapacity(transportBlockID), "HARQSender Process " << processID_ << " is busy but you wanted to start a new transmission!");

    BOOST_FOREACH(wns::scheduler::SchedulingTimeSlotPtr& slot, timeslots_)
    {
        int sc = resourceBlock->physicalResources[0].getSubChannelIndex();
        int oldsc = slot->physicalResources[0].getSubChannelIndex();
        assure(oldsc != sc, "Duplicate transmission for SC " << sc << " detected");
    }

    transportBlockID_ = transportBlockID;

    resourceBlock->harq.processID = processID_;
    resourceBlock->harq.rv = 0;
    resourceBlock->harq.ackCallback = boost::bind(&HARQSenderProcess::ACK, this);
    resourceBlock->harq.nackCallback = boost::bind(&HARQSenderProcess::NACK, this);
    resourceBlock->harq.NDI = true;
    resourceBlock->harq.transportBlockID = transportBlockID_;
    resourceBlock->harq.tbPos = nextPositionInTB_;

    nextPositionInTB_++;

    // Take a copy
    wns::scheduler::SchedulingTimeSlot* theCopy = NULL;
    // Only after that we store a copy
    theCopy = new wns::scheduler::SchedulingTimeSlot(*resourceBlock);

    theCopy->consistencyCheck();

    timeslots_.push_back(wns::scheduler::SchedulingTimeSlotPtr(theCopy));

    assure(theCopy->physicalResources[0].hasScheduledCompounds(), "No resources in RB");
}

bool
HARQSenderProcess::hasCapacity(long int transportBlockID)
{
    bool hasCapacity = (transportBlockID_ == 0 || transportBlockID_ == transportBlockID);

    if (hasCapacity)
    {
        MESSAGE_BEGIN(VERBOSE, logger_, m, "HARQSender processID=" << processID_);
        m << ", TID=" << transportBlockID_;
        m << ", TIDreq=" << transportBlockID;
        m << " has capacity";
        MESSAGE_END();
    }
    else
    {
        MESSAGE_BEGIN(VERBOSE, logger_, m, "HARQSender processID=" << processID_);
        m << ", TID=" << transportBlockID_;
        m << ", TIDreq=" << transportBlockID;
        m << " has no capacity";
        MESSAGE_END();
    }
    return hasCapacity;
}

void
HARQSenderProcess::ACK()
{
    // after approx 3ms processing delay
    wns::simulator::getEventScheduler()->scheduleDelay(
    boost::bind(&HARQSenderProcess::postDecodingACK, this), 0.002999);
}

void
HARQSenderProcess::postDecodingACK()
{
    MESSAGE_SINGLE(NORMAL, logger_, "HARQSender processID=" << processID_ << " received ACK"
                   << " on transportBlock "<< transportBlockID_);
    transportBlockID_ = 0;
    nextPositionInTB_ = 0;
    NDI_ = true;
    retransmissionCounter_ = 0;
    timeslots_.clear();
}

void
HARQSenderProcess::NACK()
{
    // after approx 3ms processing delay
    wns::simulator::getEventScheduler()->scheduleDelay(
    boost::bind(&HARQSenderProcess::postDecodingNACK, this), 0.002999);
}

void
HARQSenderProcess::postDecodingNACK()
{
    if(transportBlockID_ == 0)
    {
        MESSAGE_SINGLE(NORMAL, logger_, "WARNING: RECEIVED NACK but resourceBlock_ is empty");
        return;
    }

    //MESSAGE_SINGLE(NORMAL, logger_, "HARQ process " << processID_ << " received NACK");
    MESSAGE_SINGLE(NORMAL, logger_, "HARQSender processID=" << processID_ << " received NACK"
                   <<" on transportBlock "<< transportBlockID_);

    NDI_ = false;
    retransmissionCounter_++;

    // at most 3 retransmissions
    if (retransmissionCounter_ > retransmissionLimit_) {
        MESSAGE_SINGLE(NORMAL, logger_, "HARQSender processID=" << processID_ << " retransmission limit of "
                       << retransmissionLimit_ << " exceeded! Dropping resource blocks");
        transportBlockID_ = 0;
	retransmissionCounter_ = 0;
	nextPositionInTB_ = 0;
        timeslots_.clear();
    }
    else
    {
        // Take a copy and enqueue retransmission
        TimeSlotList::iterator it;
        for (it=timeslots_.begin(); it!=timeslots_.end(); ++it)
        {
            wns::scheduler::SchedulingTimeSlot* theCopy = NULL;
            wns::scheduler::SchedulingTimeSlotPtr resourceBlock = *it;
            theCopy = new wns::scheduler::SchedulingTimeSlot(*resourceBlock);
            wns::scheduler::SchedulingTimeSlotPtr retransmission = wns::scheduler::SchedulingTimeSlotPtr(theCopy);
            retransmission->consistencyCheck();
            retransmission->harq.NDI = NDI_;
            retransmission->harq.processID = processID_;
            retransmission->harq.transportBlockID = transportBlockID_;
            retransmission->harq.retryCounter = retransmissionCounter_;
           
	    if (retransmission->harq.ackCallback.empty())
	      {
		std::cout << "Trying to retransmit resource block with empty ack callback in HARQSenderProcess::posDecodingNACK" << std::endl;
		exit(1);
	      }

	    if (retransmission->harq.nackCallback.empty())
	      {
		std::cout << "Trying to retransmit resource block with empty nack callback in HARQSenderProcess::posDecodingNACK" << std::endl;
		exit(1);
	      }
 
	    pendingRetransmissions_.push_back(retransmission);
        }
    }
}

int
HARQSenderProcess::getNumberOfRetransmissions() const
{
  return pendingRetransmissions_.size();
}

wns::scheduler::SchedulingTimeSlotPtr
HARQSenderProcess::getNextRetransmission()
{
  wns::scheduler::SchedulingTimeSlotPtr r;

  if (pendingRetransmissions_.size() > 0)
    {
      r = pendingRetransmissions_.front();
      pendingRetransmissions_.pop_front();
    }

  if (r != wns::scheduler::SchedulingTimeSlotPtr())
  {
    if (r->harq.ackCallback.empty())
      {
	std::cout << "Returning empty ack callback in HARQSenderProcess::getNextRetransmission" << std::endl;
	exit(1);
      }

    if (r->harq.nackCallback.empty())
      {
	std::cout << "Returning empty nack callback in HARQSenderProcess::getNextRetransmission" << std::endl;
	exit(1);
      }
  }

  return r;
}

wns::scheduler::SchedulingTimeSlotPtr
HARQSenderProcess::peekNextRetransmission() const
{
  wns::scheduler::SchedulingTimeSlotPtr r;

  if (pendingRetransmissions_.size() > 0)
    {
      r = pendingRetransmissions_.front();
    }

  return r;

}

int
HARQSenderProcess::processID() const
{
  return processID_;
}
