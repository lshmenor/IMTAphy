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

#ifndef WNS_SCHEDULER_STRATEGY_STATICPRIORITY_PERSISTENTVOIP_LINKADAPTATION_HPP
#define WNS_SCHEDULER_STRATEGY_STATICPRIORITY_PERSISTENTVOIP_LINKADAPTATION_HPP

#include <WNS/scheduler/strategy/staticpriority/persistentvoip/ResourceGrid.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/distribution/Uniform.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>

namespace wns { namespace scheduler { namespace strategy { namespace staticpriority { namespace persistentvoip {

class ILinkAdaptation
{
    public:         
        typedef wns::PyConfigViewCreator<ILinkAdaptation> Creator;
        typedef wns::StaticFactory<Creator> Factory;

        struct CanFitResult
        {
            CanFitResult() :
                fits(false),
                length(0),
                sinr(wns::Ratio::from_factor(1.0))
                {};

            bool fits;
            unsigned int length;
            wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr;    
            wns::Ratio sinr;
            wns::Power txPower;
        };

        virtual Frame::SearchResultSet
        setTBSizes(const Frame::SearchResultSet& tbs, ConnectionID cid, Bit pduSize) = 0;

        virtual CanFitResult
        canFit(unsigned int start, unsigned int length, unsigned int frame, 
            ConnectionID cid, Bit pduSize) = 0;

        virtual void
        setLinkAdaptationProxy(ILinkAdaptationProxy* lp) = 0;

        virtual void
        setSlotDuration(wns::simulator::Time sd) = 0;

        virtual void
        setSchedulerSpot(wns::scheduler::SchedulerSpotType spot) = 0;

    private:
        virtual Frame::SearchResultSet
        doSetTBSizes(const Frame::SearchResultSet& tbs, ConnectionID cid, Bit pduSize) = 0;
};

class LinkAdaptation :
    public ILinkAdaptation
{
    public:         
        LinkAdaptation(const wns::pyconfig::View& config);
    
        virtual Frame::SearchResultSet
        setTBSizes(const Frame::SearchResultSet& tbs, ConnectionID cid, Bit pduSize);

        virtual CanFitResult
        canFit(unsigned int start, unsigned int length, unsigned int frame, 
            ConnectionID cid, Bit pduSize);

        virtual void
        setLinkAdaptationProxy(ILinkAdaptationProxy* lp);

        virtual void
        setSlotDuration(wns::simulator::Time sd);

        virtual void
        setSchedulerSpot(wns::scheduler::SchedulerSpotType spot);

    protected:
        unsigned int
        getTBSize(Bit pduSize, 
            wns::service::phy::phymode::PhyModeInterfacePtr phyMode);

        wns::service::phy::phymode::PhyModeInterfacePtr
        getMoreRobustMCS(Bit pduSize, 
            wns::service::phy::phymode::PhyModeInterfacePtr currentMCS);

        virtual wns::Power
        getTxPower(UserID user);

        ILinkAdaptationProxy* lproxy_;
        wns::simulator::Time slotDuration_;
        wns::scheduler::SchedulerSpotType spot_;
        bool reduceMCS_;
        wns::Power pNull_;
        double alpha_;

    private:
        virtual Frame::SearchResultSet
        doSetTBSizes(const Frame::SearchResultSet& tbs, ConnectionID, Bit pduSize) = 0;

};

class AtStart :
    public LinkAdaptation
{
    public:
        AtStart(const wns::pyconfig::View& config);

    private:
        virtual Frame::SearchResultSet
        doSetTBSizes(const Frame::SearchResultSet& tbs, ConnectionID, Bit pduSize);
};

class All :
    public LinkAdaptation
{
    public:
        All(const wns::pyconfig::View& config);

    private:
        virtual Frame::SearchResultSet
        doSetTBSizes(const Frame::SearchResultSet& tbs, ConnectionID, Bit pduSize);
};


}}}}}

#endif
