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

#ifndef WNS_SCHEDULER_HARQ_HARQ_HPP
#define WNS_SCHEDULER_HARQ_HARQ_HPP

#include <WNS/scheduler/harq/HARQInterface.hpp>
#include <WNS/service/phy/power/PowerMeasurement.hpp>
#include <WNS/ldk/harq/softcombining/Container.hpp>
#include <WNS/distribution/Uniform.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/container/Registry.hpp>
#include <WNS/Cloneable.hpp>

namespace wns { namespace scheduler { namespace harq {

class SchedulingTimeSlotInfo
{
public:
    SchedulingTimeSlotInfo(wns::scheduler::SchedulingTimeSlotPtr timeSlot, wns::service::phy::power::PowerMeasurementPtr measurement):
        timeSlot_(timeSlot),
        measurement_(measurement)
    {}

    SchedulingTimeSlotPtr timeSlot_;
    wns::service::phy::power::PowerMeasurementPtr measurement_;
};

typedef wns::ldk::harq::softcombining::Container<SchedulingTimeSlotInfo> SoftCombiningContainer;

class HARQEntity;

class IDecoder:
    public virtual wns::CloneableInterface
{
public:

    virtual bool
    canDecode(const SoftCombiningContainer& input) = 0;
};
STATIC_FACTORY_DEFINE(IDecoder, wns::PyConfigViewCreator);

    /**
     * @brief Very simple decoder without utilizing SINR measurements. Just for basic tests.
     */
class UniformRandomDecoder:
    public IDecoder,
    public wns::Cloneable<UniformRandomDecoder>
{
public:
    UniformRandomDecoder(const wns::pyconfig::View&);

    UniformRandomDecoder(const UniformRandomDecoder& other);

    virtual ~UniformRandomDecoder() {}

    virtual bool
    canDecode(const SoftCombiningContainer& input);
private:

    std::auto_ptr<wns::distribution::Distribution> dis_;

    double initialPER_;

    double rolloffFactor_;

    wns::logger::Logger logger_;
};

    /**
     * @brief ChaseCombining: ...
     */
class ChaseCombiningDecoder:
    public IDecoder,
    public wns::Cloneable<ChaseCombiningDecoder>
{
public:
    ChaseCombiningDecoder(const wns::pyconfig::View&);

    ChaseCombiningDecoder(const ChaseCombiningDecoder&);

    virtual ~ChaseCombiningDecoder() {}

    virtual bool
    canDecode(const SoftCombiningContainer& input);
private:

    std::auto_ptr<wns::distribution::Distribution> dis_;

    wns::logger::Logger logger_;

    wns::probe::bus::ContextCollector effSINRCC_;
};

    /**
     * @brief Receiving side of the HARQ protocol.
     * Take resource block and try to decode. prepare ACK/NACK depending on result.
     */
class HARQReceiverProcess
{
    class Feedback
    {
    public:
      boost::function<void ()> callback_;      
      bool retransmissionLimitHit_;
    };

public:
    HARQReceiverProcess(const wns::pyconfig::View&, HARQEntity*, int processID, int numRVs, int retransmissionLimit, const wns::logger::Logger);

    HARQReceiverProcess(const HARQReceiverProcess&);

    void
    setEntity(HARQEntity*);

    void
    onTimeSlotReceived(const wns::scheduler::SchedulingTimeSlotPtr&, HARQInterface::TimeSlotInfo);

    HARQInterface::DecodeStatusContainer
    decode();

    void
    sendPendingFeedback();

    int
    numPendingPeerRetransmissions() const;

    void
    setNumPendingPeerRetransmissions(int num);

    void
    decreaseNumPendingPeerRetransmissions();

    void
    schedulePeerRetransmissions();

    void
    schedulePeerRetransmission();

    int
    processID() const;

    SoftCombiningContainer
    receptionBuffer() const;

    wns::scheduler::SchedulingTimeSlotPtr
    schedulingTimeSlot() const;

    bool
    isFree() const;

private:

    void
    endReception();

    HARQEntity* entity_;

    int processID_;

    int numRVs_;

    int retransmissionLimit_;

    wns::logger::Logger logger_;

    SoftCombiningContainer receptionBuffer_;

    int numPendingPeerRetransmissions_;

    wns::events::scheduler::IEventPtr receptionDelta_;

    std::list<Feedback> pendingFeedback_;

    bool waitingForRetransmissions_;
};

    /**
     * @brief Sending side of the HARQ protocol.
     * There is one process per resource block in transit.
     * Receives ACK/NACK by callbacks currently.
     */
class HARQSenderProcess
{
public:
    HARQSenderProcess(HARQEntity*, int processID, int numRVs, int retransmissionLimit, const wns::logger::Logger);

    void
    setEntity(HARQEntity*);

    bool
    hasCapacity(long int transportBlockID);

    void
    newTransmission(long int transportBlockID, const wns::scheduler::SchedulingTimeSlotPtr&);

    void
    ACK();

    void
    NACK();

    void
    postDecodingACK();
    
    void
    postDecodingNACK();

    int
    getNumberOfRetransmissions() const;

    wns::scheduler::SchedulingTimeSlotPtr
    getNextRetransmission();

    wns::scheduler::SchedulingTimeSlotPtr
    peekNextRetransmission() const;

    int
    processID() const; 

private:
    HARQEntity* entity_;

    int processID_;

    int numRVs_;

    int retransmissionLimit_;

    long int transportBlockID_;

    int retransmissionCounter_;

    bool NDI_;

    typedef std::list<wns::scheduler::SchedulingTimeSlotPtr> TimeSlotList;

    TimeSlotList timeslots_;

    wns::logger::Logger logger_;

    int nextPositionInTB_;

    std::list<wns::scheduler::SchedulingTimeSlotPtr> pendingRetransmissions_;
};

    /**
     * @brief There is one HARQEntity per user (link) in BS. UTs only have one.
     */
class HARQEntity:
    public wns::Cloneable<HARQEntity>
{
public:
    HARQEntity(const wns::pyconfig::View&, int numSenderProcesses, int numReceiverProcesses, int numRVs, int retransmissionLimit, wns::logger::Logger logger);

    HARQEntity(const HARQEntity&);

    void
    newTransmission(long int transportBlockID, const wns::scheduler::SchedulingTimeSlotPtr&);

    bool
    hasCapacity(long int transportBlockID);

    void
    onTimeSlotReceived(const wns::scheduler::SchedulingTimeSlotPtr&, HARQInterface::TimeSlotInfo);

    HARQInterface::DecodeStatusContainer
    decode();

    void
    enqueueRetransmission(wns::scheduler::SchedulingTimeSlotPtr&);

    std::list<int>
    getProcessesWithRetransmissions() const;

    int
    getNumberOfRetransmissions(int processID);

    bool
    hasRetransmissions();

    bool
    hasReceiverCapacity();

    /**
     * @brief Returns the next HARQ Retransmission block and removes it from
     * the pending retransmission list.
     *
     * No matter which size [bits]. No matter which PhyMode.
     * The order is defined internally (FCFS).
     */
    wns::scheduler::SchedulingTimeSlotPtr
    getNextRetransmission(int processID);

    /**
     * @brief Returns the next HARQ Retransmission block, but keeps it in
     * the pending retransmission list.
     *
     * No matter which size [bits]. No matter which PhyMode.
     * The order is defined internally (FCFS).
     */
    virtual wns::scheduler::SchedulingTimeSlotPtr
    peekNextRetransmission(int processID) const;

    std::list<int>
    getPeerProcessesWithRetransmissions() const;

    int
    getNumberOfPeerRetransmissions(int processID) const;

    void
    schedulePeerRetransmissions(int processID);

    void
    schedulePeerRetransmission(int processID);

    void
    sendPendingFeedback();

    std::auto_ptr<IDecoder> decoder_;

private:
    std::vector<HARQSenderProcess> senderProcesses_;

    std::vector<HARQReceiverProcess> receiverProcesses_;

    int numSenderProcesses_;

    int numReceiverProcesses_;

    int numRVs_;

    int retransmissionLimit_;

    wns::logger::Logger logger_;

    int scheduledPeerRetransmissionCounter_;

    //typedef std::map<long int, IDecoder::DecoderInput> TBContainer;
    //TBContainer perTB;
};

    /**
     * @brief HARQ is the instance collaborating with the scheduler.
     * Contains a collection of HARQEntity's inside; one for each peer.
     */
class HARQ:
    public wns::scheduler::harq::HARQInterface
{
public:
    HARQ(const wns::pyconfig::View&);

    virtual ~HARQ();

    /**
     * @brief Called by the scheduler after the scheduling step has finished
     */
    virtual void
    storeSchedulingTimeSlot(long int transportBlockID, const wns::scheduler::SchedulingTimeSlotPtr&);

    /**
     * @brief Store a received SchedulingTimeSlotPtr and its measurement
     */
    virtual void
    onTimeSlotReceived(const wns::scheduler::SchedulingTimeSlotPtr&, HARQInterface::TimeSlotInfo);

    /**
     * @brief Decode all received data. SchedulingTimeSlotPtr will carry a flag indicating the decode status
     */
    virtual HARQInterface::DecodeStatusContainer
    decode();

    virtual wns::scheduler::UserSet
    getUsersWithRetransmissions() const;

    std::list<int>
    getProcessesWithRetransmissions(wns::scheduler::UserID peer) const;

    /**
     * @brief Returns number of retransmissions to schedule for going out.
     */
    virtual int
    getNumberOfRetransmissions(wns::scheduler::UserID, int processID);

    virtual bool
    hasFreeSenderProcess(wns::scheduler::UserID peer);

    virtual bool
    hasFreeReceiverProcess(wns::scheduler::UserID peer);

    /**
     * @brief Returns the next HARQ Retransmission block and removes it from
     * the pending retransmission list.
     * 
     * No matter which size [bits]. No matter which PhyMode.
     * The order is defined internally (FCFS).
     */
    virtual wns::scheduler::SchedulingTimeSlotPtr
    getNextRetransmission(wns::scheduler::UserID user, int processID);

    /**
     * @brief Returns the next HARQ Retransmission block, but keeps it in
     * the pending retransmission list.
     * 
     * No matter which size [bits]. No matter which PhyMode.
     * The order is defined internally (FCFS).
     */
    virtual wns::scheduler::SchedulingTimeSlotPtr
    peekNextRetransmission(wns::scheduler::UserID user, int processID) const;

    virtual void
    setDownlinkHARQ(HARQInterface* downlinkHARQ);

    /**
     * @brief Returns the peers that have pending retransmissions
     * Intended for uplink master scheduling.
     */
    virtual wns::scheduler::UserSet
    getPeersWithPendingRetransmissions() const;

    std::list<int>
    getPeerProcessesWithRetransmissions(wns::scheduler::UserID peer) const;

    int
    getNumberOfPeerRetransmissions(wns::scheduler::UserID peer, int processID) const;

    void
    schedulePeerRetransmissions(wns::scheduler::UserID peer, int processID);

    void
    schedulePeerRetransmission(wns::scheduler::UserID peer, int processID);

    virtual void
    sendPendingFeedback();

private:
    HARQEntity*
    findEntity(wns::scheduler::UserID userID);

    wns::logger::Logger logger_;

    typedef wns::container::Registry<wns::scheduler::UserID, HARQEntity*> HARQEntityContainer;

    /**
     * @brief Contains a collection of HARQEntity's inside; one for each peer.
     */
    HARQEntityContainer harqEntities_;

    /**
     * @brief Defines maximum number of Sender Processes.
     */
    int numSenderProcesses_;

    /**
     * @brief Defines maximum number of Receiver Processes.
     * Must be equal? numSenderProcesses==numReceiverProcesses ?
     */
    int numReceiverProcesses_;

    /**
     * @brief RV = redundancy version; Defines maximum here.
     */
    int numRVs_;

    int retransmissionLimit_;

    HARQEntity* harqEntityPrototype_;

    wns::probe::bus::ContextCollector numRetransmissionsProbeCC;
};

} // harq
} // scheduler
} // wns

#endif // WNS_SCHEDULER_HARQ_HARQ_HPP
