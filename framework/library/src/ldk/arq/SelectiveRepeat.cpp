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


#include <WNS/ldk/arq/SelectiveRepeat.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/Assure.hpp>

using namespace wns::ldk;
using namespace wns::ldk::arq;


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    SelectiveRepeat,
    ARQ,
    "wns.arq.SelectiveRepeat",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    SelectiveRepeat,
    FunctionalUnit,
    "wns.arq.SelectiveRepeat",
    FUNConfigCreator);

SelectiveRepeat::SelectiveRepeat(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    ARQ(config),

    wns::ldk::fu::Plain<SelectiveRepeat, SelectiveRepeatCommand>(fuNet),
    Delayed<SelectiveRepeat>(),
    SuspendSupport(fuNet, config),
    CanTimeout(),

    windowSize(config.get<int>("windowSize")),
    sequenceNumberSize(config.get<int>("sequenceNumberSize")),
    commandSize(config.get<int>("commandSize")),
    NS(0),
    NR(0),
    LA(0),
    activeCompound(CompoundPtr()),
    sentPDUs(),
    toRetransmit(),
    ackPDUs(),
    receivedPDUs(),
    receivedACKs(),
    sendNow(false),
    resendTimeout(config.get<double>("resendTimeout")),
    retransmissionInterval(resendTimeout),
    transmissionAttemptsProbeBus( new wns::probe::bus::ContextCollector(
        wns::probe::bus::ContextProviderCollection(&fuNet->getLayer()->getContextProviderCollection()),
        config.get<std::string>("probeName"))),
    ackDelayProbeBus( new wns::probe::bus::ContextCollector(
        wns::probe::bus::ContextProviderCollection(&fuNet->getLayer()->getContextProviderCollection()),
        config.get<std::string>("ackDelayProbeName"))),
    roundTripTimeProbeBus( new wns::probe::bus::ContextCollector(
        wns::probe::bus::ContextProviderCollection(&fuNet->getLayer()->getContextProviderCollection()),
        config.get<std::string>("RTTProbeName"))),
    delayingDelivery(false),
    logger(config.get("logger"))
{
    assure(windowSize >= 2, "Invalid windowSize.");
    assure(sequenceNumberSize >= 2 * windowSize, "Maximum sequence number is to small for chosen windowSize");
}


SelectiveRepeat::~SelectiveRepeat()
{
    // empty internal buffers
    ackPDUs.clear();
    sentPDUs.clear();
    toRetransmit.clear();
    receivedPDUs.clear();
    receivedACKs.clear();
}


bool
SelectiveRepeat::hasCapacity() const
{
    /* Make sure we
       1. don't have an active PDU we are processing
       2. aren't occupied with retransmissions
       3. don't exceed the sending window
    */
    return (this->activeCompound == CompoundPtr()
        && this->retransmissionState() == false
        && this->NS - this->LA < this->windowSize);
}


void
SelectiveRepeat::processOutgoing(const CompoundPtr& compound)
{
    assure(hasCapacity(), "processOutgoing called although not accepting.");
    activeCompound = compound;

    SelectiveRepeatCommand* command = activateCommand(compound->getCommandPool());
    this->commitSizes(compound->getCommandPool());

    command->peer.type = SelectiveRepeatCommand::I;
    command->setNS(NS);

    MESSAGE_BEGIN(NORMAL, logger, m,  "processOutgoing NS -> ");
    m << command->getNS();
    MESSAGE_END();

    ++NS;

    sendNow = true;
}


const wns::ldk::CompoundPtr
SelectiveRepeat::hasACK() const
{
    if(ackPDUs.empty() == false)
    {
        return ackPDUs.front();
    }

    return CompoundPtr();
}


const wns::ldk::CompoundPtr
SelectiveRepeat::hasData() const
{
    if(retransmissionState() == true)
    {
        return toRetransmit.front();
    }

    if(activeCompound != CompoundPtr() && sendNow == true)
    {
        return activeCompound;
    }

    return CompoundPtr();
} // hasData


wns::ldk::CompoundPtr
SelectiveRepeat::getACK()
{
    assure(hasACK(), getFUN()->getName() + " hasSomethingToSend has not been called to check whether there is something to send.");

    CompoundPtr nextACKToBeSent = ackPDUs.front();
    ackPDUs.pop_front();

    MESSAGE_BEGIN(NORMAL, logger, m, "Sent ACK frame ");
    m << getCommand(nextACKToBeSent->getCommandPool())->getNS();
    MESSAGE_END();

    return nextACKToBeSent;
} // getACK


wns::ldk::CompoundPtr
SelectiveRepeat::getData()
{
    if(retransmissionState() == true)
    {
        assure(
            toRetransmit.empty() == false,
            getFUN()->getName() + " is in retransmission state without anything to retransmit.");

        MESSAGE_BEGIN(NORMAL, logger, m, "Number of Frames to retransmit: ");
        m << toRetransmit.size();
        MESSAGE_END();

        // get first segment to retransmit
        CompoundPtr nextPDUToBeRetransmit = toRetransmit.front();
        // keep track of the number of retransmissions
        SelectiveRepeatCommand* command = this->getCommand(nextPDUToBeRetransmit);
        command->localTransmissionCounter++;
        // record the simTime when we made the last attempt to send this compound
        command->local.lastSentTime = wns::simulator::getEventScheduler()->getTime();

        // delete from beginning of retransmission buffer and store it
        // to the end of the sent buffer
        toRetransmit.pop_front();
        sentPDUs.push_back(nextPDUToBeRetransmit);

        MESSAGE_BEGIN(NORMAL, logger, m, "Re-Sent I frame ");
        m << getCommand(nextPDUToBeRetransmit->getCommandPool())->getNS();
        MESSAGE_END();

        // print out whether we leave retransmission state
        if(retransmissionState() == false)
        {
            MESSAGE_SINGLE(NORMAL, logger, "Leaving retransmission state");
        }

        // set the Timer
        this->setNewTimeout(resendTimeout);
        // and send a copy
        return nextPDUToBeRetransmit->copy();
    }


    // send a copy
    sendNow = false;
    setNewTimeout(resendTimeout);
    SelectiveRepeatCommand* myCommand = this->getCommand(activeCompound);
    // record the simTime when we made the first and the last attempt to send this compound
    myCommand->local.firstSentTime = wns::simulator::getEventScheduler()->getTime();
    myCommand->local.lastSentTime = wns::simulator::getEventScheduler()->getTime();
    // keep track of the number of retransmissions
    myCommand->localTransmissionCounter++;
    CompoundPtr it = activeCompound->copy();
    // store the PDU we now send in the Retransmission FIFO Buffer
    sentPDUs.push_back(activeCompound);
    // empty the space for new outgoing compounds
    activeCompound = CompoundPtr();

    MESSAGE_BEGIN(NORMAL, logger, m, "Sent I frame ");
    m << getCommand(it->getCommandPool())->getNS();
    MESSAGE_END();

    return it;
}


void
SelectiveRepeat::onTimeout()
{
    assure(sentPDUs.empty() == false, "No timeout without any sent frames possible");

    MESSAGE_SINGLE(NORMAL, logger, "Entering retransmission state on timeout");

    this->prepareRetransmission();

    // initiate retransmissions
    this->tryToSend();
}


void
SelectiveRepeat::processIncoming(const CompoundPtr& compound)
{
    SelectiveRepeatCommand *command = this->getCommand(compound);

    switch(command->peer.type)
    {
    case SelectiveRepeatCommand::I:
    {
        this->onIFrame(compound);
        break;
    }

    case SelectiveRepeatCommand::ACK:
    {
        this->onACKFrame(compound);
        break;
    }
    }
}

void
SelectiveRepeat::onIFrame(const CompoundPtr& compound)
{
    SelectiveRepeatCommand* command = this->getCommand(compound);

    MESSAGE_BEGIN(NORMAL, logger, m, "Received I frame.");
    m << " expected " << NR
      << " received " << command->getNS();
    MESSAGE_END();

    if(command->getNS() == NR && delayingDelivery == false)
    {
        // this is the I frame we waited for.
        MESSAGE_BEGIN(NORMAL, logger, m, "Delivering I frame ");
        m << NR;
        MESSAGE_END();

        this->getDeliverer()->getAcceptor(compound)->onData(compound);
        ++NR;

        // check if there are subsequent frames we have already received
        while (receivedPDUs.empty() == false)
        {
            ARQCommand::SequenceNumber toDeliver =  getCommand(receivedPDUs.front())->getNS();

            if(toDeliver != NR)
            {
                break;
            }

            assure(toDeliver == NR,
                   "NS must be equal to NR since the loop will not be entered if it's greater zero and it cannot be smaller!!");
            // if so, deliver them
            MESSAGE_BEGIN(NORMAL, logger, m, "Delivering I frame ");
            m << NR;
            MESSAGE_END();

            getDeliverer()->getAcceptor(receivedPDUs.front())->onData(receivedPDUs.front());

            // and remove them from the receivedPDUs list.
            receivedPDUs.pop_front();
            MESSAGE_BEGIN(NORMAL, logger, m, "Removing from receivedPDUs: I-Frame ");
            m << NR;
            MESSAGE_END();

            // adjust received PDU counter
            ++NR;
            }

    }
    else
    {
        // we received an out-of-sequence frame
        if(command->getNS() > NR)
        {
            MESSAGE_BEGIN(NORMAL, logger, m,"Buffering out-of-sequence I frame ");
            m << command->getNS();
            MESSAGE_END();

            // store the received frame for later
            keepSorted(compound, receivedPDUs);
        }
        else
        {
            /* we received an old frame (ACK got lost)
               re-send the ACK and discard the frame */
        }
    }

    if (delayingDelivery == false || command->getNS() > NR)
    {
        // acknowledge the received I-Frame
        CommandPool* ackPCI = getFUN()->getProxy()->createReply(compound->getCommandPool(), this);
        CompoundPtr ack = CompoundPtr(new Compound(ackPCI));
        ackPDUs.push_back(ack);
        SelectiveRepeatCommand* ackCommand = activateCommand(ackPCI);
        this->commitSizes(ack->getCommandPool());

        ackCommand->peer.type = SelectiveRepeatCommand::ACK;
        ackCommand->setNS(  getCommand(compound)->getNS() );
        ackCommand->magic.ackSentTime = wns::simulator::getEventScheduler()->getTime();

        MESSAGE_BEGIN(NORMAL, logger, m, "Number of ACKs pending: ");
        m << ackPDUs.size();
        MESSAGE_END();
    }

} // onIFrame

void
SelectiveRepeat::onACKFrame(const CompoundPtr& compound)
{
    SelectiveRepeatCommand* command = this->getCommand(compound);

    // Delete ACKed frame from one of the retransmission buffers
    // it may happen, that due to duplicate ACKs the PDU is neither in sentPDUs
    // nor in toRetransmit
    this->removeACKed(compound, this->sentPDUs);
    this->removeACKed(compound, this->toRetransmit);

    if(command->getNS() == this->LA )
    {
        // received the expected ACK
        MESSAGE_BEGIN(NORMAL, this->logger, m, "Received ACK frame.");
        m << " expected " << this->LA << " received " << command->getNS();
        MESSAGE_END();
        // adjust counter (advance sending window)
        this->LA++;
        // probe the time this ACK took to travel back to me
        this->ackDelayProbeBus->put( compound, wns::simulator::getEventScheduler()->getTime() - command->magic.ackSentTime );

        // Now check if subsequent ACKs have been received before
        while(this->receivedACKs.empty() == false)
        {
            if(this->getCommand(this->receivedACKs.front())->getNS() == this->LA)
            {
                this->LA++;
                this->receivedACKs.front() = CompoundPtr();
                this->receivedACKs.pop_front();
            }
            else
            {
                break;
            }
        }
        this->trySuspend();
    }
    else
    {
        //received out-of-sequence ACK
        MESSAGE_BEGIN(NORMAL, logger, m, "Received Out-of-sequence ACK frame");
        m << " expected " << this->LA << " received " << command->getNS();
        MESSAGE_END();

        // probe the time this ACK took to travel back to me
        this->ackDelayProbeBus->put( compound, wns::simulator::getEventScheduler()->getTime() - command->magic.ackSentTime );

        if (command->getNS() > this->LA)
        {
            // Enter Retransmission State
            MESSAGE_SINGLE(NORMAL, logger,"Entering retransmission state on out-of-sequence ACK");
            // push current ACK to the list of received ones
            this->keepSorted(compound, this->receivedACKs);
            // prepare PDU List for Retransmission
            this->prepareRetransmission();
            if (this->retransmissionState() == false)
            {
                MESSAGE_SINGLE(NORMAL, logger,"Leaving retransmission state, no Retransmissions pending.");
            }
        }
        else
        {
            MESSAGE_SINGLE(NORMAL, logger, "ACK is a duplicate, discarding ...");
        }
    }

    if (this->sentPDUs.empty() == true && this->hasTimeoutSet() == true)
    {
        this->cancelTimeout();
    }
    // Take care of pending retransmissions, if any
    if (this->retransmissionState())
    {
        this->tryToSend();
    }
} // onACKFrame


void
SelectiveRepeat::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    //What are the sizes in the upper Layers
    getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);


    MESSAGE_SINGLE(VERBOSE, logger, "Size calc - Command: "<<commandPoolSize<<" Payload: "<<sduSize);

    MESSAGE_SINGLE(VERBOSE, logger, "Size of SR-ARQ Command: "<<commandSize<<" Bit");

    commandPoolSize += commandSize;

    MESSAGE_SINGLE(VERBOSE, logger, "Size calc - Command: "<<commandPoolSize<<" Payload: "<<sduSize);
} // calculateSizes


void
SelectiveRepeat::prepareRetransmission()
{
    bool retransmitAll = false;
    ARQCommand::SequenceNumber lastACK(0);
    if (receivedACKs.empty() == true)
    {
        // no ACKs received so far
        retransmitAll = true;
    }
    else
    {
        // remember the last ACK
        lastACK = this->getCommand(receivedACKs.back())->getNS();
    }

    for (CompoundContainer::iterator it = sentPDUs.begin(); it != sentPDUs.end();	)
    {
        SelectiveRepeatCommand* command = this->getCommand(*it);
        ARQCommand::SequenceNumber lookingAt = command->getNS();
        if ( lookingAt < lastACK || retransmitAll )
        {
            if (command->local.lastSentTime+retransmissionInterval <= wns::simulator::getEventScheduler()->getTime()
                ||
                command->local.lastSentTime==command->local.firstSentTime)
            {
                MESSAGE_BEGIN(NORMAL, logger, m,  "Chosing I-Frame ");
                m << lookingAt << " for retransmission:";
                MESSAGE_END();

                CompoundContainer::iterator copyOfIt = it++;
                toRetransmit.push_back((*copyOfIt));

                // collect statistics available for other FUs
                this->statusCollector->onFailedTransmission((*copyOfIt));

                sentPDUs.erase(copyOfIt);
                continue;
            }
        }
        ++it;
    }
} // prepareRetransmission


void
SelectiveRepeat::keepSorted(const CompoundPtr& compound, CompoundContainer& container)
{
    SelectiveRepeatCommand* command = this->getCommand(compound);

    ARQCommand::SequenceNumber toInsert = command->getNS();
    CompoundContainer::iterator it;
    CompoundContainer::iterator itEnd = container.end();

    for(it = container.begin(); it != itEnd; ++it)
    {
        ARQCommand::SequenceNumber lookingAt = getCommand(*it)->getNS();

        if(lookingAt == toInsert)
        {
            MESSAGE_SINGLE(NORMAL, logger, "Don't need to insert, already in list");
            return;
        }

        if(lookingAt > toInsert)
        {
            break;
        }
    }
    container.insert(it, compound);
} // keepSorted


// remove ACKed PDU from list
void
SelectiveRepeat::removeACKed(const CompoundPtr& ackCompound, CompoundContainer& container)
{
    SelectiveRepeatCommand* command = getCommand(ackCompound->getCommandPool());
    int NS = command->getNS();
    for(CompoundContainer::iterator iter = container.begin();  iter != container.end(); ++iter )
    {
        if(getCommand((*iter)->getCommandPool())->getNS() == NS)
        {
            // a probe counting the number of transmissions needed
            transmissionAttemptsProbeBus->put( ackCompound, getCommand(*iter)->localTransmissionCounter);
            // a probe counting the RoundTripTime needed
            simTimeType rtt = wns::simulator::getEventScheduler()->getTime() - getCommand((*iter)->getCommandPool())->local.firstSentTime;
            roundTripTimeProbeBus->put(ackCompound, rtt);
            // adjust min time between retransmissions to two times the RTT
            retransmissionInterval = std::min<simTimeType>(2*rtt, retransmissionInterval);

            // collect statistics available for other FUs
            this->statusCollector->onSuccessfullTransmission((*iter));

            MESSAGE_BEGIN(NORMAL, logger, m, "ACK frame received after ");
            m << rtt << " s and " << getCommand(*iter)->localTransmissionCounter << " transmission attempts. RTI is now " << retransmissionInterval;
            MESSAGE_END();
            container.erase(iter);
            break;
        }
    }
}

// return whether we are in retransmission mode
bool
SelectiveRepeat::retransmissionState() const
{
    return !toRetransmit.empty();
}

bool
SelectiveRepeat::onSuspend() const
{
    return NS == LA;
} // onSuspend

void
SelectiveRepeat::doDelayDelivery()
{
    delayingDelivery = true;
} // doDelayDelivery

void
SelectiveRepeat::doDeliver()
{
    delayingDelivery = false;

    // check if there are subsequent frames we have already received
    while (!receivedPDUs.empty())
    {
        ARQCommand::SequenceNumber toDeliver =  getCommand(receivedPDUs.front())->getNS();

        if(toDeliver != NR)
        {
            break;
        }

        assure(toDeliver == NR,
               "NS must be equal to NR since the loop will not be entered if it's greater zero and it cannot be smaller!!");
        // if so, deliver them
        MESSAGE_BEGIN(NORMAL, logger, m, "Delivering I frame ");
        m << NR;
        MESSAGE_END();

        getDeliverer()->getAcceptor(receivedPDUs.front())->onData(receivedPDUs.front());

        // and remove them from the receivedPDUs list.
        receivedPDUs.pop_front();
        MESSAGE_BEGIN(NORMAL, logger, m, "Removing from receivedPDUs: I-Frame ");
        m << NR;
        MESSAGE_END();

        // adjust received PDU counter
        ++NR;
    }

} // doDeliver

