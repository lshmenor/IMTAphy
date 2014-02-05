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

#include <WNS/scheduler/strategy/staticpriority/persistentvoip/ResourceGrid.hpp>
#include <WNS/scheduler/strategy/staticpriority/persistentvoip/TBChoser.hpp>
#include <WNS/scheduler/strategy/staticpriority/persistentvoip/LinkAdaptation.hpp>

#include <WNS/Backtrace.hpp>

using namespace std;
using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::staticpriority;
using namespace wns::scheduler::strategy::staticpriority::persistentvoip;

ResourceBlock::ResourceBlock(Frame* parent, unsigned int index) :
    parent_(parent),
    subChannel_(index),
    free_(true),
    neverUsed_(true),
    logger_(parent_->getLogger())
{
};

ResourceBlock::~ResourceBlock()
{
};

bool
ResourceBlock::operator<(const ResourceBlock& other) const
{
    return this->subChannel_ < other.subChannel_;
};  

bool
ResourceBlock::isFree() 
{
    return free_;
};  

void
ResourceBlock::setFree() 
{
    assure(!isFree(), "RB is already free.");
    free_ = true;
};  

void
ResourceBlock::setOccupied() 
{
    assure(isFree(), "RB is already occupied.");

    free_ = false;
};  

unsigned int
ResourceBlock::getSubChannelIndex()
{
    return subChannel_;
}

unsigned int
ResourceBlock::getFrameIndex()
{
    assure(parent_ != NULL, "Invalid frame pointer");

    return parent_->getFrameIndex();
}

TransmissionBlock::TransmissionBlock(ResourceBlockVectorIt& start, 
        ResourceBlockVectorIt& end,
        wns::service::phy::phymode::PhyModeInterfacePtr phyMode,
        wns::Power txPower,    
        ConnectionID cid) :
    cid_(cid),
    phyMode_(phyMode),
    txPower_(txPower),
    length_(0)
{
    frame_ = (*start)->getFrameIndex();

    ResourceBlockVectorIt it;
    for(it = start; it != end; it++)
    {
        assure((*it)->isFree(), "RB occupied");
        assure((*it)->getFrameIndex() == frame_, "RBs belong to different frames.");
        (*it)->setOccupied();
        rbs_.insert(*it);
        length_++;
    }
    start_ = (*start)->getSubChannelIndex();
} 

TransmissionBlock::~TransmissionBlock()
{
    ResourceBlockPointerSetIt it;
    for(it = rbs_.begin(); it != rbs_.end(); it++)
    {
        (*it)->setFree();
    }   
}

bool
TransmissionBlock::operator==(const TransmissionBlock& other) const
{
    return start_ == other.start_ && 
        length_ == other.length_ && 
        cid_ == other.cid_;    
}

bool
TransmissionBlock::operator<(const TransmissionBlock& other) const
{
    assure(start_ != other.start_, "Overlapping TBs");
    return start_ < other.start_;   
}

unsigned int
TransmissionBlock::getCID()
{
    return cid_;
}

unsigned int
TransmissionBlock::getStart()
{
    return start_;
}

unsigned int
TransmissionBlock::getLength()
{
    return length_;
}

wns::service::phy::phymode::PhyModeInterfacePtr
TransmissionBlock::getMCS()
{
    return phyMode_;
}

wns::Power
TransmissionBlock::getTxPower()
{
    return txPower_;
}

wns::Ratio
TransmissionBlock::getEstimatedSINR()
{
    return estimatedSINR_;
}

void
TransmissionBlock::setEstimatedSINR(wns::Ratio es)
{
    estimatedSINR_ = es;
}

Frame::Frame(ResourceGrid* parent,
        unsigned int index):
    parent_(parent),
    frame_(index),
    numReserved_(0),
    logger_(parent_->getLogger())
{
    numberOfSubChannels_ = parent_->getSubChannelsPerFrame();
    assure(numberOfSubChannels_ > 0, "Need more than zero subchannels");

    for(int i = 0; i < numberOfSubChannels_; i++)
    {
        rbs_.push_back(new ResourceBlock(this, i));
    }
}

Frame::~Frame()
{
    unpersistentSchedule_.clear();
    persistentSchedule_.clear();
    blocked_.clear();

    for(int i = 0; i < numberOfSubChannels_; i++)
    {
        delete rbs_[i];
    }

}

unsigned int
Frame::getFrameIndex()
{
    return frame_;
}

Frame::SearchResultSet
Frame::findTransmissionBlocks()
{
    MESSAGE_SINGLE(NORMAL, *logger_, "Reserved: " << *this);

    unsigned int start = 0;

    SearchResult sr;
    SearchResultSet srs;

    do
    {
        sr = SearchResult();
        sr = findTransmissionBlock(start);
        if(sr.success)
        {
            srs.insert(sr);
            start = sr.start + sr.length;
        }
    }
    while(sr.success && start < numberOfSubChannels_);

    return srs;
}

Frame::SearchResult
Frame::findTransmissionBlock(unsigned int start)
{
    SearchResult sr;

    for(int i = start; i < numberOfSubChannels_; i++)
    {
        if(rbs_[i]->isFree())
        {
            int start = i;
            int nFree = 0;
            while(i < numberOfSubChannels_ && rbs_[i]->isFree())
            {
                nFree++;
                i++;
            }

            sr.success = true;
            sr.start = start;
            sr.length = nFree;
            sr.frame = getFrameIndex();
            break;
        }
    }
    assure(!sr.success || sr.length > 0, "TB size must be greater zero.");
    return sr;    
}

bool
Frame::isFree(unsigned int rbIndex)
{
    assure(rbIndex < numberOfSubChannels_, "SubChannel out of range!");
    return rbs_[rbIndex]->isFree();
}

void
Frame::block(unsigned int RBIndex)
{
    assure(rbs_[RBIndex]->isFree(), "Cannot block reserved RB " << RBIndex);

    ResourceBlockVectorIt start = rbs_.begin() + RBIndex;
    ResourceBlockVectorIt end = start + 1;

    TransmissionBlockPtr tb;
    tb = TransmissionBlockPtr(
        new TransmissionBlock(start, end, 
            wns::service::phy::phymode::PhyModeInterfacePtr(), 
            wns::Power(),
            ConnectionID()));

    MESSAGE_SINGLE(NORMAL, *logger_, "Blocked RB " << RBIndex);    

    blocked_.insert(tb);
    assure(!rbs_[RBIndex]->isFree(), "Failed to block RB " << RBIndex);

    numReserved_++;
    parent_->updateReservationCount(frame_, numReserved_, numReserved_ - 1);
}

void
Frame::reserve(ConnectionID cid, const SearchResult& sr, bool persistent)
{
    /* Further assures in TransmissionBlock will check the rest */
    assure(sr.tbStart < numberOfSubChannels_, "Request start exceeds number of subchannels");
    assure(sr.tbStart + sr.tbLength - 1 < numberOfSubChannels_, "Request exceeds number of subchannels");

    assure(persistentSchedule_.find(cid) == persistentSchedule_.end(), 
        "CID already scheduled persistently");

    assure(unpersistentSchedule_.find(cid) == unpersistentSchedule_.end(), 
        "CID already scheduled unpersistently");

    ResourceBlockVectorIt start;
    ResourceBlockVectorIt end;
    start = rbs_.begin() + sr.tbStart;
    end = start + sr.tbLength;

    TransmissionBlockPtr tb = TransmissionBlockPtr(new TransmissionBlock(start, end, 
        sr.phyMode, sr.txPower, cid));
    tb->setEstimatedSINR(sr.estimatedSINR);

    if(persistent)
        persistentSchedule_.insert(std::pair<ConnectionID, TransmissionBlockPtr>(cid, tb));
    else    
        unpersistentSchedule_.insert(std::pair<ConnectionID, TransmissionBlockPtr>(cid, tb));

    MESSAGE_SINGLE(NORMAL, *logger_, "Reserved " << sr.tbLength << " RBs strating at " 
        << sr.tbStart << " for CID " << cid << "; persistent: " << (persistent?"yes":"no") 
        << "; MCS: " << *(tb->getMCS()));

    unsigned int oldNumReserved = numReserved_;
    numReserved_ += sr.tbLength;
    parent_->updateReservationCount(frame_, numReserved_, oldNumReserved);

    assure(numReserved_ <= numberOfSubChannels_, "Number of reserved RBs exceeds total RB amount.");
}

void
Frame::removeReservation(ConnectionID cid)
{
    std::map<ConnectionID, TransmissionBlockPtr>::iterator it;

    it = persistentSchedule_.find(cid);
    assure(it != persistentSchedule_.end(), "No persistent reservation for CID " << cid);

    unsigned int oldNumReserved = numReserved_;
    numReserved_ -= it->second->getLength();
    parent_->updateReservationCount(frame_, numReserved_, oldNumReserved);

    assure(numReserved_ >= 0, "Number of reserved RBs below zero.");
        
    persistentSchedule_.erase(it);

    MESSAGE_SINGLE(NORMAL, *logger_, "Canceled persistent reservation for CID " << cid);
}

bool
Frame::hasReservation(ConnectionID cid, bool persistent)
{
    if(persistent)
    {
        return(persistentSchedule_.find(cid) != persistentSchedule_.end());
    }
    else
    {
        return(unpersistentSchedule_.find(cid) != unpersistentSchedule_.end());
    }
}

TransmissionBlockPtr
Frame::getReservation(ConnectionID cid, bool persistent)
{
    if(persistent)
    {
        assure(persistentSchedule_.find(cid) != persistentSchedule_.end(),
            "No persistent reservation for CID " << cid 
            << " in frame " << getFrameIndex());

        return persistentSchedule_[cid];
    }
    else
    {
        assure(unpersistentSchedule_.find(cid) != persistentSchedule_.end(),
            "No unpersistent reservation for CID " << cid 
            << " in frame " << getFrameIndex());

        return unpersistentSchedule_[cid];
    }
}

void
Frame::clearUnpersistentSchedule()
{
    MESSAGE_SINGLE(NORMAL, *logger_, "ClearUnpersistenSchedule frame " << frame_ 
        << " before: " << *this);

    std::map<ConnectionID, TransmissionBlockPtr>::iterator it;

    for(it = unpersistentSchedule_.begin();
        it != unpersistentSchedule_.end();
        it++)
    {
        unsigned int oldNumReserved = numReserved_;
        numReserved_ -= it->second->getLength();
        parent_->updateReservationCount(frame_, numReserved_, oldNumReserved);
        assure(numReserved_ >= 0, "Number of reserved RBs below zero.");
    }

    unpersistentSchedule_.clear();    

    MESSAGE_SINGLE(NORMAL, *logger_, "ClearUnpersistenSchedule frame " << frame_ 
        << " after:  " << *this);
}

void
Frame::clearBlocked()
{
    unsigned int oldNumReserved = numReserved_;
    numReserved_ -= blocked_.size();
    parent_->updateReservationCount(frame_, numReserved_, oldNumReserved);

    assure(numReserved_ <= numberOfSubChannels_, "Number of reserved RBs exceeds total RB amount.");

    blocked_.clear();    
}

wns::logger::Logger*
Frame::getLogger()
{
    return logger_;
}

unsigned int
Frame::getNumReserved()
{
    return numReserved_;
}

std::string
Frame::doToString() const
{
    std::stringstream s;
    for(int i = 0; i < numberOfSubChannels_; i++)
        s << !rbs_.at(i)->isFree();
    return s.str();
}

ResourceGrid::ResourceGrid(const wns::pyconfig::View& config, 
        wns::logger::Logger& logger,
        unsigned int numberOfFrames, 
        unsigned int subChannels,
        RegistryProxyInterface* registry,
        wns::simulator::Time slotDuration,
        wns::scheduler::SchedulerSpotType spot) :
    logger_(&logger),
    numberOfFrames_(numberOfFrames),
    subChannelsPerFrame_(subChannels)
{
    assure(numberOfFrames_ > 0, "Need more than zero frames");

    for(int i = 0; i < numberOfFrames_; i++)
    {
        frames_.push_back(new Frame(this, i));
        frameOccupations_.insert(std::make_pair(0, i));
    }
    
    std::string tbChoserName = config.get<std::string>("tbChoser.__plugin__");
    tbChoser_ = ITBChoser::Factory::creator(tbChoserName)->create(config.get("tbChoser"));

    std::string laName = config.get<std::string>("linkAdaptation.__plugin__");
    linkAdaptor_ = ILinkAdaptation::Factory::creator(laName)->create(config.get("linkAdaptation"));

    linkAdaptor_->setLinkAdaptationProxy(registry);
    linkAdaptor_->setSlotDuration(slotDuration);
    linkAdaptor_->setSchedulerSpot(spot);
}

ResourceGrid::~ResourceGrid()
{
    delete tbChoser_;
    delete linkAdaptor_;

    for(int i = 0; i < numberOfFrames_; i++)
    {
        delete frames_[i];
    }
}

unsigned int
ResourceGrid::getSubChannelsPerFrame()
{
    return subChannelsPerFrame_;
}

unsigned int
ResourceGrid::getNumberOfFrames()
{
    return numberOfFrames_;
}

bool
ResourceGrid::scheduleCID(unsigned int frame, ConnectionID cid, Bit pduSize, bool persistent)
{
    assure(frame < numberOfFrames_, "Invalid frame index.");
    assure(linkAdaptor_ != NULL, "Need LinkAdaptor");

    Frame::SearchResultSet srs;
    /* Find all holes in resource grid */
    srs = frames_[frame]->findTransmissionBlocks();
    /* Check which holes can be used depending on the MCS */
    srs = linkAdaptor_->setTBSizes(srs, cid, pduSize);

    if(!srs.empty())
    {
        assure(tbChoser_ != NULL, "Need TBChoser");

        MESSAGE_SINGLE(NORMAL, *logger_, "Found: " << srs.size() 
            << " potential TBs.");

        Frame::SearchResult sr;
        sr = tbChoser_->choseTB(srs);

        frames_[frame]->reserve(cid, sr, persistent);
        return true;
    }
    else
    {
        MESSAGE_SINGLE(NORMAL, *logger_, "Could not find resources for CID " << cid);
    }
    return false;
}

void
ResourceGrid::unscheduleCID(unsigned int frame, ConnectionID cid)
{
    assure(frame < numberOfFrames_, "Invalid frame index.");
    frames_[frame]->removeReservation(cid);   
}

void
ResourceGrid::unscheduleCID(unsigned int frame, const ConnectionSet& cids)
{
    assure(frame < numberOfFrames_, "Invalid frame index.");

    ConnectionSet::iterator it;
    
    for(it = cids.begin(); it != cids.end(); it++)
        frames_[frame]->removeReservation(*it);   
}


TransmissionBlockPtr
ResourceGrid::getReservation(unsigned int frame, ConnectionID cid, bool persistent)
{
    assure(frame < numberOfFrames_, "Invalid frame index.");
    
    return frames_[frame]->getReservation(cid, persistent);
}

bool
ResourceGrid::hasReservation(unsigned int frame, ConnectionID cid, bool persistent)
{
    assure(frame < numberOfFrames_, "Invalid frame index.");
    return frames_[frame]->hasReservation(cid, persistent);
}
    
bool
ResourceGrid::fitsPersistentReservation(unsigned int frame, ConnectionID cid, Bit pduSize)
{
    assure(frame < numberOfFrames_, "Invalid frame index.");
    assure(hasReservation(frame, cid, true), 
        "No persistent reservation for CID " << cid);

    TransmissionBlockPtr tb = frames_[frame]->getReservation(cid, true);

    return linkAdaptor_->canFit(tb->getStart(), tb->getLength(), frame, cid, pduSize).fits;
}

void
ResourceGrid::updateReservationCount(unsigned int frame, 
    unsigned int count, unsigned int oldCount)
{
    assure(frame < numberOfFrames_, "Invalid frame index.");
    assure(count <= subChannelsPerFrame_, "Occupation count out of range.");
    assure(getFrame(frame)->getNumReserved() == count, "Reservation count mismatched");

    typedef std::multimap<unsigned int, unsigned int>::iterator MapIter;
    std::pair<MapIter, MapIter> iters;

    MapIter it;
    iters = frameOccupations_.equal_range(oldCount);

    it = std::find(iters.first, iters.second, 
        std::pair<const unsigned int, unsigned int>(oldCount, frame));

    assure(it != iters.second, "No previous count entry for removal: " 
        << frame << ": " << oldCount << "->" << count);

    frameOccupations_.erase(it);

    frameOccupations_.insert(std::make_pair(count, frame));
}

unsigned int
ResourceGrid::getMostEmptyFrame()
{
    return frameOccupations_.begin()->second;
}

Frame*
ResourceGrid::getFrame(unsigned int index)
{
    assure(index < numberOfFrames_, "Frame index out of range");
    
    return frames_[index];
}

void
ResourceGrid::onNewFrame(unsigned int index,
                        const wns::scheduler::SchedulingMapPtr& schedulingMap)
{
    assure(index < numberOfFrames_, "Frame index out of range");

    onNewFrame(index);

    /* 
    Block RBs already reserved (eg. from higher priorities). 
    This will fail and throw an assure exception if the RBs 
    if the RBs are reserved persistently 
    */
    checkBlocked(index, schedulingMap);

}

void
ResourceGrid::checkBlocked(unsigned int index, 
    const wns::scheduler::SchedulingMapPtr& schedulingMap)
{
    for(int i = 0; i < subChannelsPerFrame_; i++)
    {
        /* Do not care for frames already occupied by PersistentVoIP scheduler */
        if(!frames_[index]->isFree(i))
            continue;

        bool free;
        free = schedulingMap->subChannels[i].temporalResources[0]
                    ->physicalResources[0].isEmpty();

        if(!free)
        {
            MESSAGE_SINGLE(NORMAL, *logger_, "RB " << i << " is occupied by user "
                << schedulingMap->subChannels[i].temporalResources[0]
                    ->physicalResources[0].getUserID()
                << " CID " << schedulingMap->subChannels[i].temporalResources[0]
                    ->physicalResources[0].scheduledCompoundsBegin()->connectionID);;

            frames_[index]->block(i);            
        }
    }

}

void
ResourceGrid::onNewFrame(unsigned int index)
{    
    MESSAGE_SINGLE(NORMAL, *logger_, "Clearing unpersistent schedule of frame " << index);

    frames_[index]->clearUnpersistentSchedule();
    frames_[index]->clearBlocked();
}

wns::logger::Logger*
ResourceGrid::getLogger()
{
    return logger_;
}


