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

#include <WNS/scheduler/strategy/staticpriority/persistentvoip/StateTracker.hpp>
#include <sstream>

using namespace wns::scheduler;
using namespace wns::scheduler::strategy;
using namespace wns::scheduler::strategy::staticpriority;
using namespace wns::scheduler::strategy::staticpriority::persistentvoip;

std::string
StateTracker::ClassifiedConnections::doToString() const
{
    std::stringstream s;
    ConnectionSet::iterator it;
    s << "NewPersistent: ";
    for(it = newPersistentCIDs.begin(); it != newPersistentCIDs.end(); it++)
        s << *it << " ";
    s << "\nReactivatedPersistent: ";
    for(it = reactivatedPersistentCIDs.begin(); it != reactivatedPersistentCIDs.end(); it++)
        s << *it << " ";
    s << "\nPersistent: ";
    for(it = persistentCIDs.begin(); it != persistentCIDs.end(); it++)
        s << *it << " ";
    s << "\nUnpersistent: ";
    for(it = unpersistentCIDs.begin(); it != unpersistentCIDs.end(); it++)
        s << *it << " ";
    s << "\nSilenced: ";
    for(it = silencedCIDs.begin(); it != silencedCIDs.end(); it++)
        s << *it << " ";
    s << "\nFrom different Frame: ";
    for(it = otherFrameCIDs.begin(); it != otherFrameCIDs.end(); it++)
        s << *it << " ";
    s << "\n";
    return s.str();
}

StateTracker::StateTracker(unsigned int numberOfFrames, wns::logger::Logger& logger) : 
    numberOfFrames_(numberOfFrames),
    logger_(&logger),
    expectedCIDs_(numberOfFrames_),
    pastPeriodCIDs_(numberOfFrames_),
    timeRelocatedCIDs_(numberOfFrames_)
{
}

StateTracker::~StateTracker()
{
}

StateTracker::ClassifiedConnections
StateTracker::updateState(const ConnectionSet& activeCIDs, unsigned int currentFrame)
{
    assure(currentFrame < numberOfFrames_, "Current frame exceeds number of frames.");

    if(activeCIDs.size() == 0 && 
        expectedCIDs_[currentFrame].size() == 0)
    {
        pastPeriodCIDs_[currentFrame].clear();
        return ClassifiedConnections();
    }

    ConnectionSet::iterator it;

    MESSAGE_SINGLE(NORMAL, *logger_, "------------------------ Update CIDs frame: " 
        << currentFrame << " -------------------" );

    MESSAGE_BEGIN(NORMAL, *logger_, m, "CIDs silent before update: ");
    for(it = silentCIDs_.begin();
        it != silentCIDs_.end();    
        it++)
    {
        m << *it << " ";
    }
    MESSAGE_END();

    MESSAGE_BEGIN(NORMAL, *logger_, m, "CIDs active: ");
    for(it = activeCIDs.begin();
        it != activeCIDs.end();    
        it++)
    {
        m << *it << " ";
    }
    MESSAGE_END();    

    std::pair<ConnectionSet, ConnectionSet> filteredFrameCIDs;
    filteredFrameCIDs = filterCIDsForFrame(activeCIDs, currentFrame);
    ConnectionSet activeFrameCIDs = filteredFrameCIDs.first;

    // Not from this frame and not relocated to other frame
    ConnectionSet otherFrameCIDs;
    std::insert_iterator<ConnectionSet> iiOther(otherFrameCIDs, otherFrameCIDs.begin()); 
    set_intersection(filteredFrameCIDs.second.begin(), 
        filteredFrameCIDs.second.end(), 
        unservedCIDs_.begin(), 
        unservedCIDs_.end(), iiOther);

    MESSAGE_BEGIN(NORMAL, *logger_, m, "CIDs not from this frame and not relocated to other frame: ");
    for(it = otherFrameCIDs.begin();
        it != otherFrameCIDs.end();    
        it++)
    {
        m << *it << " ";
    }
    MESSAGE_END();

    MESSAGE_BEGIN(NORMAL, *logger_, m, "CIDs active belonging to this frame: ");
    for(it = activeFrameCIDs.begin();
        it != activeFrameCIDs.end();    
        it++)
    {
        m << *it << " ";
    }
    MESSAGE_END();

    MESSAGE_BEGIN(NORMAL, *logger_, m, "CIDs expected: ");
    for(it = expectedCIDs_[currentFrame].begin();
        it != expectedCIDs_[currentFrame].end();    
        it++)
    {
        m << *it << " ";
    }
    MESSAGE_END();    

    // Currently present and present in this frame before
    ConnectionSet oldCIDs;
    std::insert_iterator<ConnectionSet> iiOld(oldCIDs, oldCIDs.begin()); 
    set_intersection(expectedCIDs_[currentFrame].begin(), 
        expectedCIDs_[currentFrame].end(), 
        activeFrameCIDs.begin(), 
        activeFrameCIDs.end(), iiOld);

    MESSAGE_BEGIN(NORMAL, *logger_, m, "CIDs expected and active: ");
    for(it = oldCIDs.begin();
        it != oldCIDs.end();    
        it++)
    {
        m << *it << " ";
    }
    MESSAGE_END();  

    // Expected but not there => silenced 
    ConnectionSet silencedCIDs;
    std::insert_iterator<ConnectionSet> iiSilenced(silencedCIDs, silencedCIDs.begin()); 
    set_difference(expectedCIDs_[currentFrame].begin(), 
        expectedCIDs_[currentFrame].end(), 
        activeFrameCIDs.begin(), 
        activeFrameCIDs.end(), iiSilenced);

    MESSAGE_BEGIN(NORMAL, *logger_, m, "CIDs not active anymore (silenced): ");
    for(it = silencedCIDs.begin();
        it != silencedCIDs.end();    
        it++)
    {
        m << *it << " ";
    }
    MESSAGE_END();

    // Currently present but not known yet
    ConnectionSet unknownCIDs;
    std::insert_iterator<ConnectionSet> iiUnknown(unknownCIDs, unknownCIDs.begin()); 
    set_difference(activeFrameCIDs.begin(), 
        activeFrameCIDs.end(), 
        allCIDs_.begin(), 
        allCIDs_.end(), iiUnknown);

    // We saw those last period but never before => new CIDs
    ConnectionSet newCIDs;
    std::insert_iterator<ConnectionSet> iiNew(newCIDs, newCIDs.begin()); 
    set_intersection(unknownCIDs.begin(), 
        unknownCIDs.end(), 
        pastPeriodCIDs_[currentFrame].begin(), 
        pastPeriodCIDs_[currentFrame].end(), iiNew);

    MESSAGE_BEGIN(NORMAL, *logger_, m, "New CIDs: ");
    for(it = newCIDs.begin();
        it != newCIDs.end();    
        it++)
    {
        m << *it << " ";
    }
    MESSAGE_END();  

    // This CID was not there last period, see if it will be there next one
    ConnectionSet firstTimeCIDs;
    std::insert_iterator<ConnectionSet> iiFT(firstTimeCIDs, firstTimeCIDs.begin()); 
    set_difference(unknownCIDs.begin(), 
        unknownCIDs.end(), 
        newCIDs.begin(), 
        newCIDs.end(), iiFT);

    // Currently present and known
    ConnectionSet knownCIDs;
    std::insert_iterator<ConnectionSet> iiKnown(knownCIDs, knownCIDs.begin()); 
    set_difference(activeFrameCIDs.begin(), 
        activeFrameCIDs.end(), 
        unknownCIDs.begin(), 
        unknownCIDs.end(), iiKnown);

    // Currently present and known but not expected
    ConnectionSet unexpectedCIDs;
    std::insert_iterator<ConnectionSet> iiUnexp(unexpectedCIDs, unexpectedCIDs.begin()); 
    set_difference(knownCIDs.begin(),
        knownCIDs.end(),
        expectedCIDs_[currentFrame].begin(), 
        expectedCIDs_[currentFrame].end(), iiUnexp);

    // Currently present and known but not expected 
    // but seen in last period => talking again
    ConnectionSet reactivatedCIDs;
    std::insert_iterator<ConnectionSet> iiReact(reactivatedCIDs, reactivatedCIDs.begin()); 
    set_intersection(unexpectedCIDs.begin(),
        unexpectedCIDs.end(),
        pastPeriodCIDs_[currentFrame].begin(), 
        pastPeriodCIDs_[currentFrame].end(), iiReact);

    MESSAGE_BEGIN(NORMAL, *logger_, m, "Reactivated CIDs: ");
    for(it = reactivatedCIDs.begin();
        it != reactivatedCIDs.end();    
        it++)
    {
        m << *it << " ";
    }
    MESSAGE_END();
    for(it = reactivatedCIDs.begin();
        it != reactivatedCIDs.end();    
        it++)
    {
        assure(silentCIDs_.find(*it) != silentCIDs_.end(),
            "Reactivated CID " << *it << " was not silenced before.");
        silentCIDs_.erase(*it);
    }

    // Currently present, silenced and known but not expected 
    // and not seen in last period => comfort noise or talking again
    //
    // Unexpected CIDs that are not in silent mode must have
    // leftover PDUs that did not fit before
    ConnectionSet possibleReactCIDs;
    std::insert_iterator<ConnectionSet> iiPosReact(possibleReactCIDs, possibleReactCIDs.begin()); 
    set_difference(unexpectedCIDs.begin(),
        unexpectedCIDs.end(),
        pastPeriodCIDs_[currentFrame].begin(), 
        pastPeriodCIDs_[currentFrame].end(), iiPosReact);
    ConnectionSet possibleReactSilCIDs;
    std::insert_iterator<ConnectionSet> iiPosReactSil(possibleReactSilCIDs, possibleReactSilCIDs.begin()); 
    set_intersection(possibleReactCIDs.begin(),
        possibleReactCIDs.end(),
        silentCIDs_.begin(), 
        silentCIDs_.end(), iiPosReactSil);

    MESSAGE_BEGIN(NORMAL, *logger_, m, "Comfort noise CIDs: ");
    for(it = possibleReactSilCIDs.begin();
        it != possibleReactSilCIDs.end();    
        it++)
    {
        m << *it << " ";
    }
    MESSAGE_END();

    // Those are known VoIP connections
    allCIDs_.insert(newCIDs.begin(), newCIDs.end());

    // Check next period if these CIDs are still present
    pastPeriodCIDs_[currentFrame] = firstTimeCIDs;
    pastPeriodCIDs_[currentFrame].insert(possibleReactSilCIDs.begin(), possibleReactSilCIDs.end());

    // We expect these CIDs next period
    expectedCIDs_[currentFrame] = oldCIDs;
    expectedCIDs_[currentFrame].insert(newCIDs.begin(), newCIDs.end());
    expectedCIDs_[currentFrame].insert(reactivatedCIDs.begin(), reactivatedCIDs.end());

    // Those CIDs are now inactive
    silentCIDs_.insert(silencedCIDs.begin(), silencedCIDs.end());

    MESSAGE_BEGIN(NORMAL, *logger_, m, "CIDs silent after update: ");
    for(it = silentCIDs_.begin();
        it != silentCIDs_.end();    
        it++)
    {
        m << *it << " ";
    }
    MESSAGE_END();

    MESSAGE_SINGLE(NORMAL, *logger_, "------------------------ Update CIDs done-------------------------" );


    ClassifiedConnections cc;
    cc.newPersistentCIDs = newCIDs;
    cc.reactivatedPersistentCIDs = reactivatedCIDs;
    cc.persistentCIDs = oldCIDs;
    cc.unpersistentCIDs = pastPeriodCIDs_[currentFrame];
    cc.silencedCIDs = silencedCIDs;
    cc.otherFrameCIDs = otherFrameCIDs;
    cc.totalAppActive = filteredFrameCIDs.first.size() - timeRelocatedCIDs_[currentFrame].size();
    assure(cc.totalAppActive >= 0, "Negative number of App active CIDs");

    cc.totalActive = filteredFrameCIDs.first.size();
    cc.totalQueued = activeCIDs.size();

    setFrameForCIDs(cc.newPersistentCIDs, currentFrame);

    return cc;
}

void
StateTracker::setFrameForCIDs(const ConnectionSet& cids, unsigned int frame)
{
    ConnectionSet::iterator it;

    for(it = cids.begin(); it != cids.end(); it++)
    {
        assure(CIDtoFrame_.find(*it) == CIDtoFrame_.end(), 
            "Frame for CID " << *it << " already set");
    
        CIDtoFrame_[*it] = frame;
    }
}

std::pair<ConnectionSet, ConnectionSet>
StateTracker::filterCIDsForFrame(const ConnectionSet& cids, unsigned int frame)
{
    ConnectionSet::iterator it;
    std::pair<ConnectionSet, ConnectionSet> result;

    for(it = cids.begin(); it != cids.end(); it++)
    {
        std::map<ConnectionID, unsigned int>::iterator f;
        f = CIDtoFrame_.find(*it);
        if(f == CIDtoFrame_.end() || f->second == frame)
        {
            result.first.insert(*it);
        }
        else
        {
            result.second.insert(*it);
        }
    }

    MESSAGE_BEGIN(NORMAL, *logger_, m, "CIDs not from this frame: ");
    for(it = result.second.begin();
        it != result.second.end();    
        it++)
    {
        m << *it << " ";
    }
    MESSAGE_END();

    assure(result.first.size() + result.second.size() == cids.size(), 
        "Size mismatch after filtering");

    return result;
}

void
StateTracker::silenceCID(ConnectionID cid, unsigned int currentFrame)
{
    assure(silentCIDs_.find(cid) == silentCIDs_.end(), 
        "CID " << cid << " already silent.");

    assure(expectedCIDs_[currentFrame].find(cid) != expectedCIDs_[currentFrame].end(),
        "CID " << cid << " not expected in frame " << currentFrame);

    MESSAGE_SINGLE(NORMAL, *logger_, "CID " << cid 
        << " no longer expected in frame " << currentFrame);

    expectedCIDs_[currentFrame].erase(cid);

    if(timeRelocatedCIDs_[currentFrame].find(cid) != timeRelocatedCIDs_[currentFrame].end())
        timeRelocatedCIDs_[currentFrame].erase(cid);

    silentCIDs_.insert(cid);
}

void
StateTracker::relocateCID(ConnectionID cid, 
    unsigned int currentFrame, unsigned int newFrame)
{
    assure(CIDtoFrame_.find(cid) != CIDtoFrame_.end(), "Unknown frame for CID " << cid);
    assure(CIDtoFrame_[cid] == currentFrame, "CID " << cid 
        << " is not from this frame.");
    assure(silentCIDs_.find(cid) != silentCIDs_.end(), 
        "CID must be silenced before relocation");

    CIDtoFrame_[cid] = newFrame;
    expectedCIDs_[newFrame].insert(cid);

    timeRelocatedCIDs_[newFrame].insert(cid);
}


void
StateTracker::unservedCID(ConnectionID cid)
{
    unservedCIDs_.insert(cid);
}

void
StateTracker::servedInOtherFrameCID(ConnectionID cid)
{
    assure(unservedCIDs_.find(cid) != unservedCIDs_.end(), 
        cid << " was served in different frame but not previously markt as not served");

    unservedCIDs_.erase(cid);
}


