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

#include <WNS/scheduler/strategy/staticpriority/persistentvoip/LinkAdaptation.hpp>

using namespace wns::scheduler::strategy::staticpriority::persistentvoip;


STATIC_FACTORY_REGISTER_WITH_CREATOR(AtStart, ILinkAdaptation, 
    "AtStart", wns::PyConfigViewCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(All, ILinkAdaptation, 
    "All", wns::PyConfigViewCreator);

LinkAdaptation::LinkAdaptation(const wns::pyconfig::View& config) :
    lproxy_(NULL),
    slotDuration_(0.0),
    reduceMCS_(config.get<bool>("reduceMCS")),
    pNull_(config.get<wns::Power>("pNull")),
    alpha_(config.get<double>("alpha"))
{
}

Frame::SearchResultSet
LinkAdaptation::setTBSizes(const Frame::SearchResultSet& tbs, ConnectionID cid, Bit pduSize)
{
    assure(lproxy_ != NULL, "Need RegistryProxy");
    assure(slotDuration_ > 0, "Need positive slot duration");

    return doSetTBSizes(tbs, cid, pduSize);

}

void
LinkAdaptation::setLinkAdaptationProxy(ILinkAdaptationProxy* lp)
{
    assure(lp != NULL, "Cannot set RegistryProxy to NULL");

    lproxy_ = lp;
}

void
LinkAdaptation::setSlotDuration(wns::simulator::Time sd)
{
    assure(sd > 0.0, "Need positive slot duration");

    slotDuration_ = sd;
}

void
LinkAdaptation::setSchedulerSpot(wns::scheduler::SchedulerSpotType spot)
{
    spot_ = spot;
}

unsigned int
LinkAdaptation::getTBSize(Bit pduSize, 
    wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr)
{
    Bit bitPerRB = phyModePtr->getBitCapacityFractional(slotDuration_);
    return ceil(double(pduSize) / double(bitPerRB));   
}

wns::Power
LinkAdaptation::getTxPower(UserID user)
{
    if(spot_ == wns::scheduler::SchedulerSpot::DLMaster())
        return lproxy_->getPowerCapabilities().nominalPerSubband;
    else
    {            
        wns::Ratio pathloss = lproxy_->estimateRxSINROf(user).pathloss;
        wns::Ratio scalePL;
        scalePL.set_dB(pathloss.get_dB() * alpha_);
        wns::Power txPower = pNull_ * scalePL;
        return txPower;
    }
}

wns::service::phy::phymode::PhyModeInterfacePtr
LinkAdaptation::getMoreRobustMCS(Bit pduSize, 
    wns::service::phy::phymode::PhyModeInterfacePtr currentMCS)
{
    unsigned int currentMCSindex;
    currentMCSindex = lproxy_->getPhyModeMapper()->getIndexForPhyMode(*currentMCS);

    if(currentMCSindex == 0)
        return currentMCS;

    unsigned int currentTBsize = getTBSize(pduSize, currentMCS); 
    wns::service::phy::phymode::PhyModeInterfacePtr mcs;


    int i;
    for(i = currentMCSindex - 1; i > 0; i--)
    {
        mcs = lproxy_->getPhyModeMapper()->getPhyModeForIndex(i);
        if(getTBSize(pduSize, mcs) > currentTBsize)
            break;
    }
    mcs = lproxy_->getPhyModeMapper()->getPhyModeForIndex(i + 1);
    return mcs;
}

ILinkAdaptation::CanFitResult
LinkAdaptation::canFit(unsigned int start, unsigned int length, 
    unsigned int frame, ConnectionID cid, Bit pduSize)
{
    CanFitResult result;

    UserID user = lproxy_->getUserForCID(cid);

    std::set<unsigned int> rbs;
    for(int i = 0; i < length; i++)
        rbs.insert(start + i);
    
    wns::Power txp = getTxPower(user);
    wns::Ratio effSINR;
    if(spot_ == wns::scheduler::SchedulerSpot::DLMaster())
        effSINR = lproxy_->getEffectiveDownlinkSINR(user, rbs, frame, txp, true);
    else
        effSINR = lproxy_->getEffectiveUplinkSINR(user, rbs, frame, txp);
    
    result.sinr = effSINR;

    result.phyModePtr = lproxy_->getBestPhyMode(effSINR);
    result.txPower = txp;

    result.length = getTBSize(pduSize, result.phyModePtr);

    result.fits = (result.length <= length);

    return result;
}

AtStart::AtStart(const wns::pyconfig::View& config) :
    LinkAdaptation(config)
{
}

Frame::SearchResultSet
AtStart::doSetTBSizes(const Frame::SearchResultSet& tbs, ConnectionID cid, Bit pduSize)
{
    Frame::SearchResultSet result;
    Frame::SearchResultSet::iterator it;

    for(it = tbs.begin(); it != tbs.end(); it++)
    {
        unsigned int testLength = 0;

        CanFitResult cfResult;
        do
        {
            testLength++;
            cfResult = canFit(it->start, testLength, it->frame, cid, pduSize);
        }
        while(testLength < it->length && !cfResult.fits);
        if(cfResult.fits)
        {
            Frame::SearchResult sr = *it;
            sr.tbLength = cfResult.length;
            sr.tbStart = sr.start;
            if(reduceMCS_)
                sr.phyMode = getMoreRobustMCS(pduSize, cfResult.phyModePtr);
            else
                sr.phyMode = cfResult.phyModePtr;
            sr.estimatedSINR = cfResult.sinr;
            sr.txPower = cfResult.txPower;
            sr.cid = cid;

            assure(sr.tbLength <= sr.length, "TB does not fit.");
            assure(sr.tbStart >= sr.start, "Wrong TB start.");
            assure(sr.phyMode != wns::service::phy::phymode::PhyModeInterfacePtr(),
                "No MCS set");

            result.insert(sr);
        }
        
    }
    return result; 
}

All::All(const wns::pyconfig::View& config) :
    LinkAdaptation(config)
{
}

Frame::SearchResultSet
All::doSetTBSizes(const Frame::SearchResultSet& tbs, ConnectionID cid, Bit pduSize)
{
    Frame::SearchResultSet result;
    Frame::SearchResultSet::iterator it;

    for(it = tbs.begin(); it != tbs.end(); it++)
    {
        for(int s = 0; s < it->length; s++)
        { 
            unsigned int testLength = 0;
            unsigned int neededLength = 0;

            CanFitResult cfResult;
            do
            {
                testLength++;
                cfResult = canFit(it->start + s, testLength, it->frame, cid, pduSize);
            }
            while(testLength + s < it->length && !cfResult.fits);
            if(cfResult.fits)
            {
                Frame::SearchResult sr;
                sr.success = true;
                sr.start = it->start;
                sr.length = it->length;
                sr.frame = it->frame;
                sr.tbLength = cfResult.length;
                sr.tbStart = sr.start + s;
                if(reduceMCS_)
                    sr.phyMode = getMoreRobustMCS(pduSize, cfResult.phyModePtr);
                else
                    sr.phyMode = cfResult.phyModePtr;
                sr.estimatedSINR = cfResult.sinr;
                sr.txPower = cfResult.txPower;
                sr.cid = cid;

                assure(sr.tbLength <= sr.length, "TB does not fit.");
                assure(sr.tbStart >= sr.start, "Wrong TB start.");
                assure(sr.phyMode != wns::service::phy::phymode::PhyModeInterfacePtr(),
                    "No MCS set");

                result.insert(sr);
            }
        }
    }
    return result; 
}

