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

#ifndef WNS_SCHEDULER_STRATEGY_STATICPRIORITY_PERSISTENTVOIP_RESOURCEGRID_HPP
#define WNS_SCHEDULER_STRATEGY_STATICPRIORITY_PERSISTENTVOIP_RESOURCEGRID_HPP

#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/SchedulingMap.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/RefCountable.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>

#include <map>
#include <set>
#include <sstream>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority { namespace persistentvoip {

class Frame;

class ResourceBlock
{
    public:        
        ResourceBlock(Frame* parent, unsigned int index);
        ~ResourceBlock();

        bool
        operator<(const ResourceBlock& other) const;

        bool
        isFree();

        void
        setOccupied();

        void
        setFree();

        unsigned int
        getSubChannelIndex();

        unsigned int
        getFrameIndex();

    private:
        bool free_;
        bool neverUsed_;
        unsigned int subChannel_;
        Frame* parent_;

        wns::logger::Logger* logger_;
}; 

typedef std::vector<ResourceBlock*> ResourceBlockPtrVector;
typedef ResourceBlockPtrVector::iterator ResourceBlockVectorIt;

typedef std::set<ResourceBlock*> ResourceBlockPointerSet;
typedef ResourceBlockPointerSet::iterator ResourceBlockPointerSetIt;

class TransmissionBlock :
    public wns::RefCountable
{
    public:
        TransmissionBlock(ResourceBlockVectorIt& start, 
            ResourceBlockVectorIt& end,
            wns::service::phy::phymode::PhyModeInterfacePtr phyMode,
            wns::Power txPower,
            ConnectionID cid);

        ~TransmissionBlock();

        bool
        operator==(const TransmissionBlock& other) const;

        bool
        operator<(const TransmissionBlock& other) const;

        unsigned int
        getCID();

        unsigned int
        getStart();

        unsigned int
        getLength();

        wns::service::phy::phymode::PhyModeInterfacePtr
        getMCS();

        wns::Power
        getTxPower();    
    
        wns::Ratio
        getEstimatedSINR();

        void
        setEstimatedSINR(wns::Ratio);

    private:
        ResourceBlockPointerSet rbs_;
        ConnectionID cid_;
        unsigned int start_;
        unsigned int length_;
        unsigned int frame_; 
        wns::service::phy::phymode::PhyModeInterfacePtr phyMode_;
        wns::Ratio estimatedSINR_;
        wns::Power txPower_;           
};

typedef wns::SmartPtr<TransmissionBlock> TransmissionBlockPtr; 

class ResourceGrid;

class Frame :
    public wns::IOutputStreamable
{
    public:
        struct SearchResult
        {
            SearchResult() :
                success(false),
                start(0),
                length(0),
                tbStart(0),
                tbLength(0),
                frame(0)
            {};

            bool
            operator<(const SearchResult& other) const
            {
                if(this->start != other.start)
                    return this->start < other.start;
                else
                    if(this->tbStart != other.tbStart)
                        return this->tbStart < other.tbStart;
                    else
                        if(this->length != other.length)
                            return this->length < other.length;
                        else
                            return this->tbLength < other.tbLength;
            };

            bool success;
            unsigned int start;
            unsigned int length;
            unsigned int tbStart;
            unsigned int tbLength;
            unsigned int frame;
            wns::service::phy::phymode::PhyModeInterfacePtr phyMode;
            wns::Ratio estimatedSINR;
            wns::Power txPower;
            ConnectionID cid;
        };

        typedef std::set<SearchResult> SearchResultSet;

        Frame(ResourceGrid* parent, unsigned int index);
        
        ~Frame();

        SearchResult
        findTransmissionBlock(unsigned int start);

        SearchResultSet
        findTransmissionBlocks();

        void
        reserve(ConnectionID cid, const SearchResult& sr, bool persistent);

        void
        block(unsigned int RBIndex);

        void
        removeReservation(ConnectionID cid);

        bool
        hasReservation(ConnectionID cid, bool persistent);

        TransmissionBlockPtr
        getReservation(ConnectionID cid, bool persistent);

        unsigned int
        getFrameIndex();
    
        void
        clearUnpersistentSchedule();

        void
        clearBlocked();

        wns::logger::Logger*
        getLogger();

        unsigned int
        getNumReserved();

        bool
        isFree(unsigned int rbIndex);

    private:
        virtual std::string
        doToString() const;

        ResourceBlockPtrVector rbs_;
        std::map<ConnectionID, TransmissionBlockPtr> persistentSchedule_;
        std::map<ConnectionID, TransmissionBlockPtr> unpersistentSchedule_;
        std::set<TransmissionBlockPtr> blocked_;

        unsigned int numberOfSubChannels_;
        unsigned int frame_;
        ResourceGrid* parent_;

        unsigned int numReserved_;

        wns::logger::Logger* logger_;
};

typedef std::vector<Frame*> FramePtrVector;
typedef FramePtrVector::iterator FramePtrVectorIt;

class ITBChoser;
class ILinkAdaptation;

class ResourceGrid
{
    public:
        ResourceGrid(const wns::pyconfig::View& config,
            wns::logger::Logger& logger, 
            unsigned int numberOfFrames, 
            unsigned int subChannels,
            RegistryProxyInterface* registry,
            wns::simulator::Time slotDuration,
            wns::scheduler::SchedulerSpotType spot);

        ~ResourceGrid();

        bool
        scheduleCID(unsigned int frame, ConnectionID cid, Bit pduSize, bool persistent);

        void
        unscheduleCID(unsigned int frame, ConnectionID cid);

        void
        unscheduleCID(unsigned int frame, const ConnectionSet& cids);

        bool
        hasReservation(unsigned int frame, ConnectionID cid, bool persistent);
    
        bool
        fitsPersistentReservation(unsigned int frame, ConnectionID cid, Bit pduSize);

        TransmissionBlockPtr
        getReservation(unsigned int frame, ConnectionID cid, bool persistent);

        unsigned int
        getSubChannelsPerFrame();

        unsigned int
        getNumberOfFrames();

        void
        onNewFrame(unsigned int index, 
                   const wns::scheduler::SchedulingMapPtr& schedulingMap);

        void
        onNewFrame(unsigned int index);

        void
        checkBlocked(unsigned int index, 
                    const wns::scheduler::SchedulingMapPtr& schedulingMap);

        wns::logger::Logger*
        getLogger();

        void
        updateReservationCount(unsigned int frame, 
            unsigned int count, unsigned int oldCount);

        unsigned int
        getMostEmptyFrame();

        /* only for testing */
        Frame*
        getFrame(unsigned int index);

    private:
        unsigned int numberOfFrames_;
        unsigned int subChannelsPerFrame_;
        FramePtrVector frames_;

        /* This is a sorted multimap: num. reserved RBs in frame -> frame number */
        std::multimap<unsigned int, unsigned int> frameOccupations_;

        ITBChoser* tbChoser_;
        ILinkAdaptation* linkAdaptor_;

        wns::logger::Logger* logger_;
};

}}}}}

#endif
