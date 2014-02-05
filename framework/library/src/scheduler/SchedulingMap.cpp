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

#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/SchedulingMap.hpp>
#include <WNS/scheduler/strategy/SchedulerState.hpp> // for RequestForResource
#include <WNS/ldk/Compound.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/service/phy/phymode/PhyModeInterface.hpp>
#include <iostream>
#include <fstream>

using namespace wns::scheduler;

SchedulingCompound::SchedulingCompound()
  : subChannel(0),
    timeSlot(0),
    spatialLayer(0),
    startTime(0.0),
    endTime(0.0),
    connectionID(-1),
    userID(NULL),
    sourceUserID(NULL),
    compoundPtr(),
    phyModePtr(),
    txPower(),
    estimatedCQI(),
    harqEnabled(false)
{
};

SchedulingCompound::SchedulingCompound(const SchedulingCompound& other):
    subChannel(other.subChannel),
    timeSlot(other.timeSlot),
    spatialLayer(other.spatialLayer),
    startTime(other.startTime),
    endTime(other.endTime),
    connectionID(other.connectionID),
    userID(other.userID),
    sourceUserID(other.sourceUserID),
    compoundPtr(),
    phyModePtr(other.phyModePtr),
    txPower(other.txPower),
    pattern(other.pattern),
    estimatedCQI(other.estimatedCQI),
    harqEnabled(other.harqEnabled)
{
    if(other.compoundPtr != NULL)
    {
        compoundPtr = wns::ldk::CompoundPtr(other.compoundPtr->clone());
    }
};

SchedulingCompound::SchedulingCompound(int _subChannel,
                                       int _timeSlot,
                                       int _spatialLayer,
                                       simTimeType _startTime,
                                       simTimeType _endTime,
                                       wns::scheduler::ConnectionID _connectionID,
                                       wns::scheduler::UserID _userID,
                                       wns::scheduler::UserID _sourceUserID,
                                       wns::ldk::CompoundPtr _compoundPtr,
                                       wns::service::phy::phymode::PhyModeInterfacePtr _phyModePtr,
                                       wns::Power _txPower,
                                       wns::service::phy::ofdma::PatternPtr _pattern,
                                       ChannelQualityOnOneSubChannel _estimatedCQI,
                                       bool _harqEnabled
    )
    : subChannel(_subChannel),
      timeSlot(_timeSlot),
      spatialLayer(_spatialLayer),
      startTime(_startTime),
      endTime(_endTime),
      connectionID(_connectionID),
      userID(_userID),
      sourceUserID(_sourceUserID),
      compoundPtr(_compoundPtr),
      phyModePtr(_phyModePtr),
      txPower(_txPower),
      pattern(_pattern),
      estimatedCQI(_estimatedCQI),
      harqEnabled(_harqEnabled)
{
}

SchedulingCompound::~SchedulingCompound()
{
}

std::string
SchedulingCompound::toString() const
{
    std::stringstream s;
    s.setf(std::ios::fixed,std::ios::floatfield);   // floatfield set to fixed
    s.precision(1);
    s << "SchedulingCompound(";
    if (harqEnabled) s << " H ";
    s << "cid="<<connectionID;
    s << ", user="<<userID.getName();
    if (compoundPtr != wns::ldk::CompoundPtr()) {
        s << ", bits="<< compoundPtr->getLengthInBits();
    }
    s << ", T=[" << startTime*1e6 << "-" << endTime*1e6 << "]us";
    s << ", d=" << (endTime-startTime)*1e6 << "us";
    s << ", estI=" << estimatedCQI.interference.get_dBm() << "dBm";
    s << ", estC=" << estimatedCQI.carrier.get_dBm() << "dBm";
    s << ", estPL=" << estimatedCQI.pathloss.get_dB() << "dB";
    s << ")";
    return s.str();
}

/**************************************************************/
PhysicalResourceBlock::PhysicalResourceBlock()
    : subChannelIndex(0),
      timeSlotIndex(0),
      spatialIndex(0),
      slotLength(0.0),
      freeTime(0.0),
      nextPosition(0.0),
      scheduledCompounds(),
      phyModePtr(),
      txPower(),
      estimatedCQI(),
      antennaPattern()
{
}

PhysicalResourceBlock::PhysicalResourceBlock(int _subChannelIndex, int _timeSlotIndex, int _spatialLayer, simTimeType _slotLength)
    : subChannelIndex(_subChannelIndex),
      timeSlotIndex(_timeSlotIndex),
      spatialIndex(_spatialLayer),
      slotLength(_slotLength),
      freeTime(_slotLength),
      nextPosition(0.0),
      scheduledCompounds(),
      userID(NULL),
      sourceUserID(NULL),
      phyModePtr(),
      txPower(),
      estimatedCQI(),
      antennaPattern()
{
}

PhysicalResourceBlock::PhysicalResourceBlock(const PhysicalResourceBlock& other):
    subChannelIndex(other.subChannelIndex),
    timeSlotIndex(other.timeSlotIndex),
    spatialIndex(other.spatialIndex),
    slotLength(other.slotLength),
    freeTime(other.freeTime),
    nextPosition(other.nextPosition),
    scheduledCompounds(),
    userID(other.userID),
    sourceUserID(other.sourceUserID),
    phyModePtr(other.phyModePtr),
    txPower(other.txPower),
    estimatedCQI(other.estimatedCQI),
    antennaPattern(other.antennaPattern),
    metaUserID (other.metaUserID)
{
    for (ScheduledCompoundsList::const_iterator it=other.scheduledCompounds.begin();
         it != other.scheduledCompounds.end();
         ++it)
    {
        scheduledCompounds.push_back(SchedulingCompound(*it));
    }
}

PhysicalResourceBlock::~PhysicalResourceBlock()
{
}

simTimeType
PhysicalResourceBlock::getUsedTime() const
{
    assure(slotLength-freeTime-nextPosition<wns::scheduler::slotLengthRoundingTolerance,"slotLength mismatch");
    return slotLength-freeTime;
}

simTimeType
PhysicalResourceBlock::getFreeTime() const
{
    assure(slotLength-freeTime-nextPosition<wns::scheduler::slotLengthRoundingTolerance,"slotLength mismatch");
    return freeTime;
}

simTimeType
PhysicalResourceBlock::getNextPosition() const
{
    return nextPosition;
}

bool
PhysicalResourceBlock::pduFitsInto(strategy::RequestForResource& request,
                                   MapInfoEntryPtr mapInfoEntry // <- must not contain compounds yet
    ) const
{
    // mapInfoEntry can contain compounds when in while loop:
    //assure(mapInfoEntry->compounds.empty(),"mapInfoEntry->compounds must be empty here");
    wns::service::phy::phymode::PhyModeInterfacePtr mapPhyModePtr = mapInfoEntry->phyModePtr;
    assure(mapPhyModePtr!=wns::service::phy::phymode::PhyModeInterfacePtr(),"phyModePtr==NULL");
    assure(scheduledCompounds.empty()
           || (*mapPhyModePtr == *phyModePtr),
           "all PhyModes must match on a (used) PhysicalResourceBlock: "<<*phyModePtr<<" != "<<*mapPhyModePtr);
    double dataRate = mapPhyModePtr->getDataRate();
    simTimeType compoundDuration = request.bits / dataRate;
    simTimeType startTime = this->nextPosition;
    simTimeType endTime = startTime + compoundDuration;
    return (endTime <= slotLength+wns::scheduler::slotLengthRoundingTolerance);
} // pduFitsInto (PhysicalResourceBlock)

int
PhysicalResourceBlock::getFreeBitsOnPhysicalResourceBlock(MapInfoEntryPtr mapInfoEntry) const
{
    wns::service::phy::phymode::PhyModeInterfacePtr mapPhyModePtr = mapInfoEntry->phyModePtr;
    assure(mapPhyModePtr!=wns::service::phy::phymode::PhyModeInterfacePtr(),"phyModePtr==NULL");
    assure(scheduledCompounds.empty()
           || (*mapPhyModePtr == *phyModePtr),
           "all PhyModes must match on a (used) PhysicalResourceBlock: "<<*phyModePtr<<" != "<<*mapPhyModePtr);
    double dataRate = mapPhyModePtr->getDataRate();
    return (freeTime+wns::scheduler::slotLengthRoundingTolerance) * dataRate;
} // getFreeBitsOnPhysicalResourceBlock

bool
PhysicalResourceBlock::addCompound(simTimeType compoundDuration,
                                   wns::scheduler::ConnectionID connectionID,
                                   wns::scheduler::UserID _userID,
                                   wns::scheduler::UserID _sourceUserID,
                                   wns::ldk::CompoundPtr compoundPtr,
                                   wns::service::phy::phymode::PhyModeInterfacePtr _phyModePtr,
                                   wns::Power _txPower,
                                   wns::service::phy::ofdma::PatternPtr _pattern,
                                   ChannelQualityOnOneSubChannel _estimatedCQI,
                                   bool _useHARQ
    )
{
    simTimeType startTime = this->nextPosition;
    simTimeType endTime = startTime + compoundDuration;
    if (endTime > slotLength+wns::scheduler::slotLengthRoundingTolerance) return false; // not successful
    assure(endTime <= slotLength+wns::scheduler::slotLengthRoundingTolerance,
           "end time of the compound="<<endTime<<" exceeds slotLength="<<slotLength);
    int predecessors = scheduledCompounds.size();
    // this can happen if DSAStrategy.oneUserOnOneSubChannel=False: no implementation yet
    assure((predecessors==0)||(txPower == _txPower),
           "mismatch: txPower="<<txPower<<"!="<<_txPower<<" please use DSAStrategy.oneUserOnOneSubChannel=True");
    txPower = _txPower; // all compounds must have the same power
    // check if users match (need oneUserOnOneSubChannel for that)
    // we cannot do this here, because in the UL SchedulingMap done by UT.RS-TX the userID is myself
    //if (userID!=NULL) { // && oneUserOnOneSubChannel) {
    //    assure(_userID==userID,
    //           "user mismatch: "<<_userID->getName()<<" != "<<userID->getName());
    //}
    if (predecessors>0) { // && oneUserOnOneSubChannel) {
        wns::scheduler::UserID otherUserID = scheduledCompounds.begin()->userID;
        assure(_userID==otherUserID,
               "user mismatch: "<<_userID.getName()<<" != "<<userID.getName());
    }
    if (!userID.isValid())
    { // master schedulers set this; slave schedulers get this already set.
        userID = _userID; // all compounds must have the same user; oneUserOnOneSubChannel = True ?
     }
    else
    {//ensure userID of comound is the same as defined in PRB
      wns::scheduler::UserID prbUserID = this->getUserID();
      assure((prbUserID == _sourceUserID || prbUserID == _userID),
               "PRB userID mismatches Source: "<<prbUserID.getName()<<" != "<<_sourceUserID.getName()<<_userID.getName());
    }

    if (predecessors>0) { // && oneUserOnOneSubChannel) {
        wns::scheduler::UserID otherUserID = scheduledCompounds.begin()->sourceUserID;
        assure(_sourceUserID==otherUserID,
               "source user mismatch: "<<_sourceUserID.getName()<<" != "<<sourceUserID.getName());
    }
    if (!sourceUserID.isValid())
    { // master schedulers set this; slave schedulers get this already set.
        sourceUserID = _sourceUserID; // all compounds must have the same user; oneUserOnOneSubChannel = True ?
    }

    assure((predecessors==0)||(phyModePtr == _phyModePtr),
           "phyModePtr mismatch: "<<*phyModePtr<<"!="<<*_phyModePtr);
    phyModePtr = _phyModePtr; // all compounds should have the same phyMode
    antennaPattern = _pattern; // all compounds should have the same antenna pattern
    estimatedCQI = _estimatedCQI;
    SchedulingCompound newScheduledCompound(this->subChannelIndex,
                                            this->timeSlotIndex,
                                            this->spatialIndex,
                                            startTime,
                                            endTime,
                                            connectionID,
                                            _userID,
                                            _sourceUserID,
                                            compoundPtr,
                                            phyModePtr,
                                            txPower,
                                            _pattern,
                                            _estimatedCQI,
                                            _useHARQ
        );
    this->scheduledCompounds.push_back(newScheduledCompound);
    this->nextPosition += compoundDuration;
    assure(fabs(nextPosition-endTime)<wns::scheduler::slotLengthRoundingTolerance,"nextPosition!=endTime");
    this->freeTime     -= compoundDuration;
    assure(freeTime>=-wns::scheduler::slotLengthRoundingTolerance,"freeTime="<<freeTime);
    // assure(freeTime>=0.0,"freeTime="<<freeTime); // fails due to double inaccuracies
    // this method cannot count SchedulingMap::numberOfCompounds itself
    return true;
} // PhysicalResourceBlock::addCompound

bool
PhysicalResourceBlock::addCompound(strategy::RequestForResource& request,
                                   MapInfoEntryPtr mapInfoEntry, // <- must not contain compounds yet
                                   wns::ldk::CompoundPtr compoundPtr,
                                   bool _useHARQ
    )
{
    // mapInfoEntry can contain compounds when in while loop:
    //assure(mapInfoEntry->compounds.empty(),"mapInfoEntry->compounds must be empty here");
    //assure(compoundPtr!=wns::ldk::CompoundPtr(),"compoundPtr==NULL"); // empty is allowed for uplink master scheduling
    //int compoundBits = compoundPtr->getLengthInBits();
    //assure(compoundBits==request.bits, "bits mismatch: "<<compoundBits<<" != "<<request.bits);
    // ^ in the UL the real bits may be less than the requested bits.
    //assure(compoundBits<=request.bits, "bits mismatch: "<<compoundBits<<" != "<<request.bits);
    wns::service::phy::phymode::PhyModeInterfacePtr mapPhyModePtr = mapInfoEntry->phyModePtr;
    double dataRate = mapPhyModePtr->getDataRate();
    simTimeType compoundDuration = request.bits / dataRate;

    // check if users match (need oneUserOnOneSubChannel for that)
    // we cannot do this here, because in the UL SchedulingMap done by UT.RS-TX the userID is myself
    //if (userID!=NULL) { // && oneUserOnOneSubChannel) {
    //    assure(compoundUserID==userID,
    //           "user mismatch: "<<compoundUserID->getName()<<" != "<<userID->getName());
    //}
    // assure(compoundUserID==request.user) sometimes fails even with RN2 != RN2
    // Reason: user mismatch: RN2 != RN2, 7 != 5
    // in this case make a string comparison :-(
    //assure(compoundUserID->isEqual(request.user) || compoundUserID->getName().compare(mapInfoEntry->user->getName())==0,
    //       "user mismatch: "<<compoundUserID->getName()<<" != "<<mapInfoEntry->user->getName()<<", "<<compoundUserID->getNodeID()<<" != "<<request.user->getNodeID());
    wns::Power txPowerInMap = mapInfoEntry->txPower;
    wns::scheduler::ConnectionID connectionID = request.cid;
    wns::service::phy::ofdma::PatternPtr patternInMap = mapInfoEntry->pattern; // antenna pattern (grouping)
    ChannelQualityOnOneSubChannel estimatedCQI = mapInfoEntry->estimatedCQI;

    return addCompound(compoundDuration,
                       connectionID,
                       mapInfoEntry->user,
                       mapInfoEntry->sourceUser,
                       compoundPtr,
                       mapPhyModePtr,
                       txPowerInMap,
                       patternInMap,
                       estimatedCQI,
                       _useHARQ
        );
} // addCompound

// dumpContents(): machine-readable format (table for Matlab,Gnuplot,etc)
std::string
PhysicalResourceBlock::dumpContents(const std::string& prefix) const
{
    std::stringstream s;
    s.setf(std::ios::fixed,std::ios::floatfield);   // floatfield set to fixed
    s.precision(4);
    //s << prefix << subChannelIndex << "\t" << spatialIndex;
    s << prefix;
    if (phyModePtr != PhyModePtr())
    {
        s << phyModePtr->getBitsPerSymbol() << "\t";
    }
    else
    {
        s << "?" << "\t";
    }

    s  << txPower.get_dBm() << "\t"
       << nextPosition/slotLength << "\t";
    if ( nextPosition>0.0 ) // not empty
    {
        s << scheduledCompounds.size() << "\t";
        if (scheduledCompounds.size() > 0)
        {
            int totalbits = 0;

            s << scheduledCompounds.begin()->userID.getName() << "\t";
            for ( ScheduledCompoundsList::const_iterator iter = scheduledCompounds.begin(); iter != scheduledCompounds.end(); ++iter )
            {
                s << iter->connectionID << "(" << iter->compoundPtr->getLengthInBits() << "),";
                totalbits += iter->compoundPtr->getLengthInBits();
            }
            s << "total (" << totalbits << ")";
        }
    } else {
        s << "0\t0";
    }
    s << std::endl;
    return s.str();
} // dumpContents

// doToString(): human-readable format
std::string
PhysicalResourceBlock::toString() const
{
    std::stringstream s;
    s.setf(std::ios::fixed,std::ios::floatfield);   // floatfield set to fixed
    s.precision(1);
    s << "PhysicalResourceBlock(";
    s << "#"<<subChannelIndex<<"."<<timeSlotIndex<<"."<<spatialIndex;
    if ( nextPosition>0.0 ) { // not empty
        s << ", user="<<userID.getName();
        s << ", free=" << freeTime*1e6 << "us";
        s << ", next=" << nextPosition*1e6 << "us";
        s << ", used=" << 100.0*nextPosition/slotLength << "%";
        s << ", compounds=" << scheduledCompounds.size();
        s << ")";
        if (scheduledCompounds.size()>0)
        {
            s << ":"<<std::endl;
            for ( ScheduledCompoundsList::const_iterator iter = scheduledCompounds.begin(); iter != scheduledCompounds.end(); ++iter )
            {
                s << "  " << iter->toString();
                if (iter!=scheduledCompounds.end()) s << std::endl;
            }
        }
    } else if (phyModePtr!=wns::service::phy::phymode::PhyModeInterfacePtr()) {
        // this is a master schedulingMap (from BS to UT). Compounds have been deleted, values reset
        s << ", user="<<userID.getName();
        s << ", phyMode="<<*phyModePtr;
        s << ", txPower="<<txPower;
        s << ")";
    } else {
        s << ", unused)";
    }
    s << std::endl;
    return s.str();
} // toString

int
PhysicalResourceBlock::countScheduledCompounds() const
{
    return scheduledCompounds.size();
}

bool
PhysicalResourceBlock::isEmpty() const
{
    // nextPosition can be 0.0 in a master schedulingMap, although phyMode and user is set
    // so this && check is necessary in the master schedulers
    // For the uplink the master map entries have isEmpty==false because nextPosition>0 and PhyMode set.
    return ((nextPosition==0.0) &&
            (phyModePtr==wns::service::phy::phymode::PhyModeInterfacePtr()));
}

wns::scheduler::UserID
PhysicalResourceBlock::getUserID() const
{
    // In UL slave scheduler, the userIDs are different! (UT vs BS)
    //assure(userID==NULL || scheduledCompounds.size()==0 || (scheduledCompounds.begin()->userID == userID),
    //       "userID mismatch: userID="<<userID->getName());
    return userID;
    //return scheduledCompounds.begin()->userID;
}

wns::scheduler::UserID
PhysicalResourceBlock::getSourceUserID() const
{
    return sourceUserID;
}

wns::Power
PhysicalResourceBlock::getTxPower() const
{
    return txPower;
}

void
PhysicalResourceBlock::setTxPower(wns::Power power)
{
    txPower=power;
    // adjust contents
    if (scheduledCompounds.size() > 0)
    {
        for ( ScheduledCompoundsList::iterator iter = scheduledCompounds.begin(); iter != scheduledCompounds.end(); ++iter )
        {
            SchedulingCompound& schedulingCompound = *iter;
            schedulingCompound.txPower = power;
            // can we and do we have to adjust something inside the compound?
            // we cannot access the command here.
            // So the PhyUser is responsible for setting the power of the compounds.
            //schedulingCompound.compoundPtr->...;
        }
    }
}

void
PhysicalResourceBlock::deleteCompounds()
{
    scheduledCompounds.clear();
} // deleteCompounds

void
PhysicalResourceBlock::grantFullResources()
{
    // Used in MasterMap: extend UL resource size to full length, so that UT can make use of it.
    if (!isEmpty())
    {
        // with these values the resourceUsage statistics counts the full resource:
        nextPosition = slotLength;
        freeTime = 0.0;
    }
}

void
PhysicalResourceBlock::processMasterMap()
{
    // Used in MasterMap: Extend UL resource size to full length, so that UT can make use of it.
    if (!isEmpty())
    {
        scheduledCompounds.clear();
        // with these values the DSASlave strategy can put new UL packets into it:
        nextPosition = 0.0;
        freeTime = slotLength;
    }
    assure(scheduledCompounds.size() == 0,"scheduledCompounds is not empty but must be");
}

bool
PhysicalResourceBlock::hasResourcesForUser(wns::scheduler::UserID user) const
{
    return (user == userID);
}

void
PhysicalResourceBlock::consistencyCheck()
{
    for ( ScheduledCompoundsList::const_iterator iter = scheduledCompounds.begin(); iter != scheduledCompounds.end(); ++iter )
    {
        assure(iter->compoundPtr != wns::ldk::CompoundPtr(), "No compound here");
    }
}

int
PhysicalResourceBlock::getNetBlockSizeInBits() const
{
    if ( nextPosition>0.0 ) // not empty
    {
        if (scheduledCompounds.size() > 0)
        {
            int totalbits = 0;
            for ( ScheduledCompoundsList::const_iterator iter = scheduledCompounds.begin(); iter != scheduledCompounds.end(); ++iter )
            {
                assure(iter->compoundPtr != wns::ldk::CompoundPtr(), "No compound here");
                totalbits += iter->compoundPtr->getLengthInBits();
            }
            return totalbits;
        }
    }
    return 0;
}

bool
PhysicalResourceBlock::isHARQEnabled() const
{
    for ( ScheduledCompoundsList::const_iterator iter = scheduledCompounds.begin(); iter != scheduledCompounds.end(); ++iter )
    {
        if(iter->harqEnabled)
        {
            return true;
        }
    }
    return false;
}

/**************************************************************/
SchedulingTimeSlot::SchedulingTimeSlot()
    : subChannelIndex(0),
      timeSlotIndex(0),
      numSpatialLayers(0),
      slotLength(0.0),
      timeSlotStartTime(0.0)
{
}

SchedulingTimeSlot::SchedulingTimeSlot(int _subChannel,
                                       int _timeSlot,
                                       int _numSpatialLayers,
                                       simTimeType _slotLength
    )
    : subChannelIndex(_subChannel),
      timeSlotIndex(_timeSlot),
      numSpatialLayers(_numSpatialLayers),
      slotLength(_slotLength),
      timeSlotStartTime(_slotLength*_timeSlot),
      timeSlotIsUsable(true)
{
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        // create one PhysicalResourceBlock object per spatialLayer (MIMO channel)
        PhysicalResourceBlock emptyPRB(subChannelIndex,timeSlotIndex,spatialIndex,slotLength);
        // available as physicalResources[spatialIndex]
        physicalResources.push_back(emptyPRB); // object copied
    }
}

SchedulingTimeSlot::SchedulingTimeSlot(const SchedulingTimeSlot& other):
    subChannelIndex(other.subChannelIndex),
    timeSlotIndex(other.timeSlotIndex),
    numSpatialLayers(other.numSpatialLayers),
    slotLength(other.slotLength),
    timeSlotStartTime(other.timeSlotStartTime),
    timeSlotIsUsable(other.timeSlotIsUsable),
    harq(other.harq)
{
    for (PhysicalResourceBlockVector::const_iterator it = other.physicalResources.begin();
         it != other.physicalResources.end();
         ++it)
    {
        physicalResources.push_back(PhysicalResourceBlock(*it));
    }
}

SchedulingTimeSlot::~SchedulingTimeSlot()
{
}

void
SchedulingTimeSlot::consistencyCheck()
{
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        physicalResources[spatialIndex].consistencyCheck();
    }
}

simTimeType
SchedulingTimeSlot::getUsedTime() const
{
    //if (!IsUsable) return 0.0; // must be different result than getFreeTime()
    simTimeType usedTime = 0.0;
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        usedTime += physicalResources[spatialIndex].getUsedTime();
    }
    return usedTime;
}

simTimeType
SchedulingTimeSlot::getFreeTime() const
{
    //if (!IsUsable) return 0.0; // must be different result than getUsedTime()
    simTimeType freeTime = 0.0;
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        freeTime += physicalResources[spatialIndex].getFreeTime();
    }
    return freeTime;
}

int
SchedulingTimeSlot::countScheduledCompounds() const
{
    int count=0;
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        count += physicalResources[spatialIndex].countScheduledCompounds();
    }
    return count;
}

bool
SchedulingTimeSlot::isEmpty() const
{
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        if (!physicalResources[spatialIndex].isEmpty()) return false;
    }
    return true;
}

bool
SchedulingTimeSlot::pduFitsInto(strategy::RequestForResource& request,
                                MapInfoEntryPtr mapInfoEntry // <- must not contain compounds yet
    ) const
{
    //if (!IsUsable) return false;
    // is it correct to ask like this?
    // or do we have to loop over all spatialLayers?
    int spatialLayer = mapInfoEntry->spatialLayer;
    assure(spatialLayer>=0 && spatialLayer<numSpatialLayers,"spatialLayer="<<spatialLayer<<" is out of bounds");
    return physicalResources[spatialLayer].pduFitsInto(request,mapInfoEntry);
} // pduFitsInto (ResourceBlock)

wns::scheduler::UserID
SchedulingTimeSlot::getUserID() const
{
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        wns::scheduler::UserID userID = physicalResources[spatialIndex].getUserID();
        if (userID.isValid())
        	return userID;
    }
    return UserID();
}

wns::scheduler::UserID
SchedulingTimeSlot::getSourceUserID() const
{
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        wns::scheduler::UserID userID = physicalResources[spatialIndex].getSourceUserID();
	if (userID.isValid())
	  return userID;
    }
    return UserID();
}

wns::Power
SchedulingTimeSlot::getTxPower() const
{
    wns::Power txPower;
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        txPower = physicalResources[spatialIndex].getTxPower();
        if (txPower!=wns::Power())
            return txPower;
    }
    return txPower;
}

void
SchedulingTimeSlot::setTxPower(wns::Power power)
{
    // adjust contents
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        physicalResources[spatialIndex].setTxPower(power);
    }
}

// dumpContents(): machine-readable format (table for Matlab,Gnuplot,etc)
std::string
SchedulingTimeSlot::dumpContents(const std::string& prefix) const
{
    std::stringstream s;
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        std::stringstream p;
        p << prefix << spatialIndex << "\t";
        if (timeSlotIsUsable) {
            s << physicalResources[spatialIndex].dumpContents(p.str());
        } else {
            s << prefix << spatialIndex << "\t" << "LOCKED" << std::endl;
        }
    }
    return s.str();
}

// doToString(): human-readable format
std::string
SchedulingTimeSlot::toString() const
{
    std::stringstream s;
    if (timeSlotIsUsable) {
        for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
        {
            s << physicalResources[spatialIndex].toString();
        }
    } else {
        s << "TimeSlot(#"<<timeSlotIndex<<"): locked/unusable" << std::endl;
    }
    return s.str();
}

void
SchedulingTimeSlot::deleteCompounds()
{
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        physicalResources[spatialIndex].deleteCompounds();
    }
}

void
SchedulingTimeSlot::grantFullResources()
{
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        physicalResources[spatialIndex].grantFullResources();
    }
}

void
SchedulingTimeSlot::processMasterMap()
{
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        physicalResources[spatialIndex].processMasterMap();
    }
}

bool
SchedulingTimeSlot::hasResourcesForUser(wns::scheduler::UserID user) const
{
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        if (physicalResources[spatialIndex].hasResourcesForUser(user))
            return true;
    }
    return false;
}

wns::scheduler::ChannelQualityOnOneSubChannel
SchedulingTimeSlot::getEstimatedCQI(wns::scheduler::UserID user) const
{
    assure(hasResourcesForUser(user),"TimeSlot has not resources for user");
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        if (physicalResources[spatialIndex].hasResourcesForUser(user))
            return physicalResources[spatialIndex].getEstimatedCQI();
    }
}

int
SchedulingTimeSlot::getNetBlockSizeInBits() const
{
    int netBits = 0;
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        netBits += physicalResources[spatialIndex].getNetBlockSizeInBits();
    }
    return netBits;
}

bool
SchedulingTimeSlot::isHARQEnabled() const
{
    for(int spatialIndex=0; spatialIndex<numSpatialLayers; spatialIndex++)
    {
        if (physicalResources[spatialIndex].isHARQEnabled())
        {
            return true;
        }
    }
    return false;
}

/**************************************************************/

SchedulingSubChannel::SchedulingSubChannel()
    : subChannelIndex(0),
      numSpatialLayers(0),
      slotLength(0.0),
      subChannelIsUsable(true)
{
}

SchedulingSubChannel::SchedulingSubChannel(int _subChannelIndex, int _numberOfTimeSlots, int _numSpatialLayers, simTimeType _slotLength)
    : subChannelIndex(_subChannelIndex),
      numberOfTimeSlots(_numberOfTimeSlots),
      numSpatialLayers(_numSpatialLayers),
      slotLength(_slotLength),
      subChannelIsUsable(true)
{
    for ( int timeSlotIndex = 0; timeSlotIndex < numberOfTimeSlots; ++timeSlotIndex )
    {
        SchedulingTimeSlotPtr emptyTimeSlotPtr // SmartPtr...
            = SchedulingTimeSlotPtr(new SchedulingTimeSlot(subChannelIndex,timeSlotIndex,numSpatialLayers,slotLength));
        temporalResources.push_back(emptyTimeSlotPtr); // SmartPtr copied
    }
}

SchedulingSubChannel::~SchedulingSubChannel()
{
    temporalResources.clear();
}

// dumpContents(): machine-readable format (table for Matlab,Gnuplot,etc)
std::string
SchedulingSubChannel::dumpContents(const std::string& prefix) const
{
    std::stringstream s;
    for ( int timeSlotIndex = 0; timeSlotIndex < numberOfTimeSlots; ++timeSlotIndex )
    {
        std::stringstream p;
        p << prefix << timeSlotIndex << "\t";
        if (subChannelIsUsable) {
            s << temporalResources[timeSlotIndex]->dumpContents(p.str());
        } else {
            s << prefix << timeSlotIndex << "\t" << "LOCKED" << std::endl;
        }
    }
    return s.str();
}

// doToString(): human-readable format
std::string
SchedulingSubChannel::toString() const
{
    std::stringstream s;
    if (subChannelIsUsable) {
        for ( int timeSlotIndex = 0; timeSlotIndex < numberOfTimeSlots; ++timeSlotIndex )
        {
            s << temporalResources[timeSlotIndex]->toString();
        }
    } else {
        s << "SubChannel(#"<<subChannelIndex<<"): locked/unusable" << std::endl;
    }
    return s.str();
}

simTimeType
SchedulingSubChannel::getUsedTime() const
{
    if (!subChannelIsUsable) return 0.0; // must be different result than getFreeTime()
    simTimeType usedTime = 0.0;
    for ( int timeSlotIndex = 0; timeSlotIndex < numberOfTimeSlots; ++timeSlotIndex )
    {
        usedTime += temporalResources[timeSlotIndex]->getUsedTime();
    }
    return usedTime;
}

simTimeType
SchedulingSubChannel::getFreeTime() const
{
    if (!subChannelIsUsable) return 0.0; // must be different result than getUsedTime()
    simTimeType freeTime = 0.0;
    for ( int timeSlotIndex = 0; timeSlotIndex < numberOfTimeSlots; ++timeSlotIndex )
    {
        freeTime += temporalResources[timeSlotIndex]->getFreeTime();
    }
    return freeTime;
}

bool
SchedulingSubChannel::pduFitsInto(strategy::RequestForResource& request,
                                  MapInfoEntryPtr mapInfoEntry // <- must not contain compounds yet
    ) const
{
    if (!subChannelIsUsable) return false;
    int timeSlot = mapInfoEntry->timeSlot;
    assure(timeSlot>=0 && timeSlot<numberOfTimeSlots,"timeSlot="<<timeSlot<<" is out of bounds");
    // is it correct to ask like this?
    // or do we have to loop over all spatialLayers?
    int spatialLayer = mapInfoEntry->spatialLayer;
    assure(spatialLayer>=0 && spatialLayer<numSpatialLayers,"spatialLayer="<<spatialLayer<<" is out of bounds");
    return temporalResources[timeSlot]->pduFitsInto(request,mapInfoEntry);
} // pduFitsInto (SubChannel)

int
SchedulingSubChannel::getFreeBitsOnSubChannel(MapInfoEntryPtr mapInfoEntry) const
{
    if (!subChannelIsUsable) return 0;
    int timeSlot = mapInfoEntry->timeSlot;
    assure(timeSlot>=0 && timeSlot<numberOfTimeSlots,"timeSlot="<<timeSlot<<" is out of bounds");
    // is it correct to ask like this?
    // or do we have to loop over all spatialLayers?
    int spatialLayer = mapInfoEntry->spatialLayer;
    assure(spatialLayer>=0 && spatialLayer<numSpatialLayers,"spatialLayer="<<spatialLayer<<" is out of bounds");
    return temporalResources[timeSlot]->physicalResources[spatialLayer].getFreeBitsOnPhysicalResourceBlock(mapInfoEntry);
} // getFreeBitsOnSubChannel

bool
SchedulingSubChannel::isEmpty() const
{
    for (int timeSlotIndex = 0; timeSlotIndex < numberOfTimeSlots; ++timeSlotIndex)
    {
        if (!temporalResources[timeSlotIndex]->isEmpty()) return false;
    }
    return true;
}

void
SchedulingSubChannel::deleteCompounds()
{
    for ( int timeSlotIndex = 0; timeSlotIndex < numberOfTimeSlots; ++timeSlotIndex )
    {
        temporalResources[timeSlotIndex]->deleteCompounds();
    }
}

void
SchedulingSubChannel::grantFullResources()
{
    for ( int timeSlotIndex = 0; timeSlotIndex < numberOfTimeSlots; ++timeSlotIndex )
    {
        temporalResources[timeSlotIndex]->grantFullResources();
    }
}

void
SchedulingSubChannel::processMasterMap()
{
    for ( int timeSlotIndex = 0; timeSlotIndex < numberOfTimeSlots; ++timeSlotIndex )
    {
        temporalResources[timeSlotIndex]->processMasterMap();
    }
}

bool SchedulingSubChannel::hasResourcesForUser(wns::scheduler::UserID user) const
{
    for ( int timeSlotIndex = 0; timeSlotIndex < numberOfTimeSlots; ++timeSlotIndex )
    {
        if (temporalResources[timeSlotIndex]->hasResourcesForUser(user))
        return true;
    }
    return false;
}

wns::scheduler::ChannelQualityOnOneSubChannel
SchedulingSubChannel::getEstimatedCQI(wns::scheduler::UserID user) const
{
    assure(hasResourcesForUser(user), "SubChannel has not resources for this user");
    for ( int timeSlotIndex = 0; timeSlotIndex < numberOfTimeSlots; ++timeSlotIndex )
    {
        if (temporalResources[timeSlotIndex]->hasResourcesForUser(user))
            return temporalResources[timeSlotIndex]->getEstimatedCQI(user);
    }
}
/**************************************************************/

SchedulingMap::SchedulingMap( simTimeType _slotLength, int _numberOfSubChannels, int _numberOfTimeSlots, int _numSpatialLayers, int _frameNr )
    : frameNr(_frameNr),
      slotLength(_slotLength),
      numberOfSubChannels(_numberOfSubChannels),
      numberOfTimeSlots(_numberOfTimeSlots),
      numSpatialLayers(_numSpatialLayers),
      numberOfCompounds(0),
      resourceUsage(0.0)
{
    assure(numberOfSubChannels>0,"numberOfSubChannels="<<numberOfSubChannels);
    assure(slotLength>0.0,"slotLength="<<slotLength);
    for ( int subChannelIndex = 0; subChannelIndex < numberOfSubChannels; ++subChannelIndex )
    {
        SchedulingSubChannel subChannel(subChannelIndex,numberOfTimeSlots,numSpatialLayers,slotLength);
        subChannels.push_back(subChannel); // object copied
    }
    //#include <WNS/logger/Logger.hpp>
    //wns::logger::Logger logger("WNS", "SchedulingMap");
} // SchedulingMap::SchedulingMap

SchedulingMap::~SchedulingMap()
{
    subChannels.clear();
}

bool
SchedulingMap::pduFitsInto(strategy::RequestForResource& request,
                           MapInfoEntryPtr mapInfoEntry // <- must not contain compounds yet
    ) const
{
    // mapInfoEntry can contain compounds when in while loop:
    //assure(mapInfoEntry->compounds.empty(),"mapInfoEntry->compounds must be empty here");
    int subChannelIndex = mapInfoEntry->subBand;
    //int timeSlot = mapInfoEntry->timeSlot;
    //int spatialLayer = mapInfoEntry->spatialLayer;
    return subChannels[subChannelIndex].pduFitsInto(request,mapInfoEntry);
    //return subChannels[subChannelIndex].temporalResources[timeSlot]->physicalResources[spatialLayer].pduFitsInto(request,mapInfoEntry);
} // pduFitsInto (SubChannel)

int
SchedulingMap::getFreeBitsOnSubChannel(MapInfoEntryPtr mapInfoEntry) const
{
    // mapInfoEntry can contain compounds when in while loop:
    //assure(mapInfoEntry->compounds.empty(),"mapInfoEntry->compounds must be empty here");
    int subChannelIndex = mapInfoEntry->subBand;
    //int timeSlot = mapInfoEntry->timeSlot;
    //int spatialLayer = mapInfoEntry->spatialLayer;
    return subChannels[subChannelIndex].getFreeBitsOnSubChannel(mapInfoEntry);
}

bool
SchedulingMap::addCompound(int subChannelIndex,
                           int timeSlot,
                           int spatialLayer,
                           simTimeType compoundDuration,
                           wns::scheduler::ConnectionID connectionID,
                           wns::scheduler::UserID userID,
                           wns::scheduler::UserID sourceUserID,
                           wns::ldk::CompoundPtr compoundPtr,
                           wns::service::phy::phymode::PhyModeInterfacePtr phyModePtr,
                           wns::Power txPower,
                           wns::service::phy::ofdma::PatternPtr pattern,
                           ChannelQualityOnOneSubChannel estimatedCQI,
                           bool useHARQ
    )
{
    bool ok =
        subChannels[subChannelIndex].temporalResources[timeSlot]->physicalResources[spatialLayer].addCompound(
            compoundDuration,
            connectionID,
            userID,
            sourceUserID,
            compoundPtr,
            phyModePtr,
            txPower,
            pattern,
            estimatedCQI,
            useHARQ
            );
    if (ok) {
        numberOfCompounds++;
    }
    return ok;
} // addCompound

bool
SchedulingMap::addCompound(strategy::RequestForResource& request,
                           MapInfoEntryPtr mapInfoEntry, // <- must not contain compounds yet
                           wns::ldk::CompoundPtr compoundPtr,
                           bool useHARQ
    )
{
    // mapInfoEntry can contain compounds when in while loop:
    //assure(mapInfoEntry->compounds.empty(),"mapInfoEntry->compounds must be empty here");
    int subChannelIndex = mapInfoEntry->subBand;
    int timeSlot = mapInfoEntry->timeSlot;
    int spatialLayer = mapInfoEntry->spatialLayer;
    bool ok =
        subChannels[subChannelIndex].temporalResources[timeSlot]->physicalResources[spatialLayer].addCompound(
            request,
            mapInfoEntry,
            compoundPtr,
            useHARQ
            );
    if (ok) {
        numberOfCompounds++;
    }
    return ok;
} // addCompound

simTimeType
SchedulingMap::getNextPosition(int subChannel, int timeSlot, int spatialLayer) const
{
    assure(subChannel<numberOfSubChannels,"subChannel="<<subChannel<<" >= numberOfSubChannels="<<numberOfSubChannels);
    assure(timeSlot>=0 && timeSlot<numberOfTimeSlots,"timeSlot="<<timeSlot<<" >= numberOfTimeSlots="<<numberOfTimeSlots);
    assure(spatialLayer<numSpatialLayers,"spatialLayer="<<spatialLayer<<" >= numSpatialLayers="<<numSpatialLayers);
    return subChannels[subChannel].temporalResources[timeSlot]->physicalResources[spatialLayer].getNextPosition();
}

bool
SchedulingMap::isEmpty() const
{
    for ( unsigned int subChannelIndex = 0; subChannelIndex < subChannels.size(); ++subChannelIndex )
    {
        if (!subChannels[subChannelIndex].isEmpty()) return false;
    }
    return true;
}

int
SchedulingMap::getFrameNr() const
{
    return this->frameNr;
}

void 
PhysicalResourceBlock::setUserID(wns::scheduler::UserID aUserID)
{
  userID = aUserID;
}

double
SchedulingMap::getResourceUsage()
{
    simTimeType totalUsedTime = getUsedTime();
    simTimeType totalTimeResources = slotLength * numberOfSubChannels * numSpatialLayers * numberOfTimeSlots;
    double result = totalUsedTime / totalTimeResources;
    assure(numberOfSubChannels==subChannels.size(),"mismatch in numberOfSubChannels: "<<numberOfSubChannels<<" != "<<subChannels.size());
    assure((result >= -0.01)/*tolerance*/
           &&(result <= 1.01)
           , "Percentage of used resources="<<result<<" must be within [0..1] !"
           <<" totalUsedTime="<<totalUsedTime
           <<", totalTime="<<totalTimeResources
           <<", subChannels.size="<<subChannels.size());
    if (result<0.0) result=0.0;
    if (result>1.0) result=1.0;
    resourceUsage = result; // store result in member
    return result;
} // getResourceUsage

simTimeType
SchedulingMap::getUsedTime() const
{
    simTimeType usedTime = 0;
    assure(numberOfSubChannels==subChannels.size(),"mismatch in numberOfSubChannels: "<<numberOfSubChannels<<" != "<<subChannels.size());
    for ( unsigned int subChannelIndex = 0; subChannelIndex < subChannels.size(); ++subChannelIndex )
    {
        usedTime += subChannels[subChannelIndex].getUsedTime();
    }
    return usedTime;
} // getUsedTime

simTimeType
SchedulingMap::getFreeTime() const
{
    simTimeType leftoverTime = 0;
    assure(numberOfSubChannels==subChannels.size(),"mismatch in numberOfSubChannels: "<<numberOfSubChannels<<" != "<<subChannels.size());
    for ( unsigned int subChannelIndex = 0; subChannelIndex < subChannels.size(); ++subChannelIndex )
    {
        leftoverTime += subChannels[subChannelIndex].getFreeTime();
    }
    return leftoverTime;
} // getFreeTime

wns::Power
SchedulingMap::getUsedPower(int timeSlot) const
{
    assure(timeSlot>=0 && timeSlot<numberOfTimeSlots,"timeSlot="<<timeSlot<<" >= numberOfTimeSlots="<<numberOfTimeSlots);
    wns::Power usedPower; // = 0W
    for(unsigned int subChannelIndex=0; subChannelIndex<numberOfSubChannels; subChannelIndex++)
    {
        // what is the right handling of MIMO? Do we count=add txPower per spatialLayer or do we assume this is all "one" power?
        //wns::Power usedTxPowerOnThisChannel = subChannels[subChannelIndex].txPower;
        // we assume that txPower is the same on all PRBs, so reading the first is sufficient:
        wns::Power usedTxPowerOnThisChannel = subChannels[subChannelIndex].temporalResources[timeSlot]->physicalResources[0/*first beam*/].getTxPower();
        // if we have no PDU allocated on this channel, just skip it.
        if (usedTxPowerOnThisChannel == wns::Power())
            continue;
        usedPower += usedTxPowerOnThisChannel;
    }
    return usedPower;
} // getUsedPower

wns::Power
SchedulingMap::getRemainingPower(wns::Power totalPower, int timeSlot) const
{
	assure(timeSlot>=0 && timeSlot<numberOfTimeSlots,"timeSlot="<<timeSlot<<" >= numberOfTimeSlots="<<numberOfTimeSlots);
    wns::Power remainingPower = totalPower;
    for(unsigned int subChannelIndex=0; subChannelIndex<numberOfSubChannels; subChannelIndex++)
    {
        // what is the right handling of MIMO? Do we count=add txPower per spatialLayer or do we assume this is all "one" power?
        //wns::Power usedTxPowerOnThisChannel = subChannels[subChannelIndex].txPower;
        // we assume that txPower is the same on all PRBs, so reading the first is sufficient:
        wns::Power usedTxPowerOnThisChannel = subChannels[subChannelIndex].temporalResources[timeSlot]->physicalResources[0/*first spatialLayer*/].getTxPower();
        // if we have no PDU allocated on this channel, just skip it.
        if (usedTxPowerOnThisChannel == wns::Power())
            continue;
        if (remainingPower<usedTxPowerOnThisChannel)
            return wns::Power(); // zero Watts;
        remainingPower -= usedTxPowerOnThisChannel;
    }
    // the counter totalRemainingPower should be used in the future:
    //assure(remainingPower == totalRemainingPower,"remainingPower="<<remainingPower<<" != totalRemainingPower="<<totalRemainingPower);
    return remainingPower;
} // getRemainingPower

wns::service::phy::phymode::PhyModeInterfacePtr
SchedulingMap::getPhyModeUsedInResource(int subChannelIndex, int timeSlot, int spatialLayer) const
{
    assure(subChannelIndex>=0 && subChannelIndex<numberOfSubChannels,"subChannelIndex="<<subChannelIndex);
    assure(timeSlot>=0 && timeSlot<numberOfTimeSlots,"timeSlot="<<timeSlot<<" >= numberOfTimeSlots="<<numberOfTimeSlots);
    assure(spatialLayer>=0 && spatialLayer < numSpatialLayers,"spatialLayer="<<spatialLayer);
    return subChannels[subChannelIndex].temporalResources[timeSlot]->physicalResources[spatialLayer].getPhyMode();
}

wns::Power
SchedulingMap::getTxPowerUsedInResource(int subChannelIndex, int timeSlot, int spatialLayer) const
{
    assure(subChannelIndex>=0 && subChannelIndex<numberOfSubChannels,"subChannelIndex="<<subChannelIndex);
    assure(timeSlot>=0 && timeSlot<numberOfTimeSlots,"timeSlot="<<timeSlot<<" >= numberOfTimeSlots="<<numberOfTimeSlots);
    assure(spatialLayer>=0 && spatialLayer < numSpatialLayers,"spatialLayer="<<spatialLayer);
    
    wns::Power powerInResource = subChannels[subChannelIndex].temporalResources[timeSlot]->getTxPower();
    assure(fabs(subChannels[subChannelIndex].temporalResources[timeSlot]->physicalResources[spatialLayer].getTxPower().get_mW() - powerInResource.get_mW())<1e-3,
           "mismatch in powerInResource="<<powerInResource);
    return powerInResource;
}

void
SchedulingMap::maskOutSubChannels(const UsableSubChannelVector& usableSubChannels)
{
    //std::cout <<"maskOutSubChannels(#="<<usableSubChannels.size()<<"): usable="<<printBoolVector(usableSubChannels)<<std::endl;
    assure(usableSubChannels.size()>=numberOfSubChannels,"#usableSubChannels="<<usableSubChannels.size());
    for ( int subChannelIndex = 0; subChannelIndex < numberOfSubChannels; ++subChannelIndex )
    {
        bool before = subChannels[subChannelIndex].subChannelIsUsable;
        bool mask   = usableSubChannels[subChannelIndex];
        bool after  = before & mask; // 0&1=0
        subChannels[subChannelIndex].subChannelIsUsable = after;
    }
} // maskOutSubChannels

// void
// SchedulingMap::convertToMapInfoCollection(MapInfoCollectionPtr collection /*return value*/)
// {
//     // translate result into currentBurst to allow bursts.push_back(currentBurst)
//     for ( SubChannelVector::iterator iterSubChannel = subChannels.begin();
//           iterSubChannel != subChannels.end(); ++iterSubChannel)
//     {
//         SchedulingSubChannel& subChannel = *iterSubChannel;
//         for ( SchedulingTimeSlotPtrVector::iterator iterTimeSlot = subChannel.temporalResources.begin();
//               iterTimeSlot != subChannel.temporalResources.end(); ++iterTimeSlot)
//         {
//             SchedulingTimeSlotPtr timeSlotPtr = *iterTimeSlot;
//             for ( PhysicalResourceBlockVector::iterator iterPRB = timeSlotPtr->physicalResources.begin();
//                   iterPRB != timeSlotPtr->physicalResources.end(); ++iterPRB)
//             {
//                 UserID lastScheduledUserID;
//                 MapInfoEntryPtr currentBurst;
//                 double currentBurstStartTime = 0.0;
//                 while ( !iterPRB->scheduledCompounds.empty() )
//                 {
//                     SchedulingCompound compound = iterPRB->scheduledCompounds.front(); // .front()
//                     iterPRB->scheduledCompounds.pop_front(); // pop_front()
//                     if ( compound.userID != lastScheduledUserID ) // new User, starts new Burst
//                     {
//                         //MESSAGE_SINGLE(NORMAL, logger, "New compund of cid=" << iterCompound.userID->getName());
//                         // new user -> new newburst=new map entry
//                         currentBurst = MapInfoEntryPtr(new MapInfoEntry());
//                         currentBurst->start          = compound.startTime;
//                         currentBurst->end            = compound.startTime; // intentionally not endTime;
//                         currentBurst->user           = compound.userID;
//                         currentBurst->subBand        = compound.subChannel;
//                         currentBurst->timeSlot       = compound.timeSlot;
//                         currentBurst->beam           = compound.beam;
//                         currentBurst->txPower        = compound.txPower;
//                         currentBurst->phyModePtr     = compound.phyModePtr;
//                         //currentBurst->estimatedCandI = ? how to get it here?
//                         collection->push_back(currentBurst);
//                     }
//                     //bursts.back()->end += iterCompound.compoundDuration;
//                     simTimeType compoundDuration = compound.getCompoundDuration();
//                     currentBurst->end     += compoundDuration;
//                     currentBurstStartTime += compoundDuration;
//                     //compound gehort zum selben connection fuege hinten ein
//                     collection->back()->compounds.push_back(compound.compoundPtr);

//                     // inherited from Strategy.cpp; calls callback():
//                     // compoundReady() // cannot do this here.
//                     lastScheduledUserID = compound.userID;
//                 } // end while ( !iterSubChannel->scheduledCompounds.empty() )
//             } // end for ( beams )
//         } // end for ( timeSlots )
//     } // end for ( SubChannels )
// } // convertToMapInfoCollection


// this is called by the UL master scheduler, because there are no "real" compounds (just fakes).
void
SchedulingMap::deleteCompounds()
{
    for ( int subChannelIndex = 0; subChannelIndex < numberOfSubChannels; ++subChannelIndex )
    {
        subChannels[subChannelIndex].deleteCompounds();
    }
    numberOfCompounds = 0;
}

void
SchedulingMap::grantFullResources()
{
    for ( int subChannelIndex = 0; subChannelIndex < numberOfSubChannels; ++subChannelIndex )
    {
        subChannels[subChannelIndex].grantFullResources();
    }
}

void
SchedulingMap::processMasterMap()
{
    for ( int subChannelIndex = 0; subChannelIndex < numberOfSubChannels; ++subChannelIndex )
    {
        subChannels[subChannelIndex].processMasterMap();
    }
}

bool
SchedulingMap::hasResourcesForUser(wns::scheduler::UserID user) const
{
    for ( int subChannelIndex = 0; subChannelIndex < numberOfSubChannels; ++subChannelIndex )
    {
        if (subChannels[subChannelIndex].hasResourcesForUser(user))
            return true;
    }
    return false;
}

wns::scheduler::ChannelQualityOnOneSubChannel
SchedulingMap::getEstimatedCQI(wns::scheduler::UserID user) const
{
    assure(hasResourcesForUser(user), "SchedulingMap has not resources for user");
    for ( int subChannelIndex = 0; subChannelIndex < numberOfSubChannels; ++subChannelIndex )
    {
        if (subChannels[subChannelIndex].hasResourcesForUser(user))
            return subChannels[subChannelIndex].getEstimatedCQI(user);
    }

}

// toString(): human-readable format (incuding resourceUsage)
std::string
SchedulingMap::toString()
{
    std::stringstream s;
    s.setf(std::ios::fixed,std::ios::floatfield);   // floatfield set to fixed
    s.precision(1);
    assure(numberOfSubChannels==subChannels.size(),"numberOfSubChannels="<<numberOfSubChannels<<" != subChannels.size()="<<subChannels.size());
    if (frameNr>=0) { // valid frameNr
        s << "SchedulingMap(frame="<<frameNr<<": "<<numberOfSubChannels<<"x"<<slotLength*1e6<<"us): ";
    } else {
        s << "SchedulingMap("<<numberOfSubChannels<<"x"<<slotLength*1e6<<"us): ";
    }
    double resourceUsage = this->getResourceUsage(); // getResourceUsage() is not const
    if (this->isEmpty()) {
        s << "empty." << std::endl;
    } else {
        s << resourceUsage*100.0 << "% full." << std::endl;
        for ( int subChannelIndex = 0; subChannelIndex < numberOfSubChannels; ++subChannelIndex )
        {
            s << subChannels[subChannelIndex].toString();
        }
    }
    return s.str();
} // toString

// doToString(): human-readable format
std::string
SchedulingMap::doToString() const
{
    std::stringstream s;
    s.setf(std::ios::fixed,std::ios::floatfield);   // floatfield set to fixed
    s.precision(1);
    assure(numberOfSubChannels==subChannels.size(),"numberOfSubChannels="<<numberOfSubChannels<<" != subChannels.size()="<<subChannels.size());
    if (frameNr>=0) { // valid frameNr
        s << "SchedulingMap(frame="<<frameNr<<": "<<numberOfSubChannels<<"x"<<slotLength*1e6<<"us): ";
    } else {
        s << "SchedulingMap("<<numberOfSubChannels<<"x"<<slotLength*1e6<<"us): ";
    }
    if (this->isEmpty()) {
        s << "empty." << std::endl;
    } else {
        for ( int subChannelIndex = 0; subChannelIndex < numberOfSubChannels; ++subChannelIndex )
        {
            s << subChannels[subChannelIndex].toString();
        }
    }
    return s.str();
} // doToString

// dumpContents(): machine-readable format (table for Matlab,Gnuplot,etc)
std::string
SchedulingMap::dumpContents(const std::string& prefix) const
{
    std::stringstream s;
    //s.setf(std::ios::fixed,std::ios::floatfield);   // floatfield set to fixed
    //s.precision(1);
    assure(numberOfSubChannels==subChannels.size(),"numberOfSubChannels="<<numberOfSubChannels<<" != subChannels.size()="<<subChannels.size());
    for ( int subChannelIndex = 0; subChannelIndex < numberOfSubChannels; ++subChannelIndex )
    {
        std::stringstream p;
        p << prefix << frameNr << "\t" << subChannelIndex << "\t";
        s << subChannels[subChannelIndex].dumpContents(p.str());
    }
    return s.str();
} // dumpContents

// static (does not need an object of this class)
void
SchedulingMap::writeHeaderToFile(std::ofstream& f)
{
    if (f.is_open()) {
        f << "##### SchedulingMap over time #####" << std::endl;
        //f << "# numberOfSubChannels="<<numberOfSubChannels << std::endl;
        //f << "# numberOfTimeSlots="<<numberOfTimeSlots << std::endl;
        //f << "# numSpatialLayers="<<numSpatialLayers << std::endl;
        //f << "# slotLength="<<slotLength << std::endl;
        f << "# (time[s]) frameNr subChannel timeSlot stream/spatialLayer bits/symbol txPower[dBm] filled% #compounds userID cidList(#bits), totalbits" << std::endl;
    } else {
        throw wns::Exception("cannot write to file");
    }
}

void
SchedulingMap::writeToFile(std::ofstream& f, const std::string& prefix) const
{
    if (f.is_open()) {
        f << dumpContents(prefix);
    } else {
        throw wns::Exception("cannot write to file");
    }
}

void
SchedulingMap::writeFile(std::string fileName) const
{
    //std::ofstream *file = new std::ofstream(fileName.c_str());
    //(*file) << doToString();
    //if (*file) file->close();
    std::ofstream file(fileName.c_str(),std::ios_base::app); // append
    if (file.is_open())
    {
        writeToFile(file,"");
        file.close();
    } else {
        throw wns::Exception("cannot open file "+fileName);
    }
}
