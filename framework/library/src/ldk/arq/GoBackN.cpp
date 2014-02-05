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

#include <WNS/ldk/arq/GoBackN.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/Assure.hpp>

using namespace wns::ldk;
using namespace wns::ldk::arq;


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    GoBackN,
    ARQ,
    "wns.arq.GoBackN",
    FUNConfigCreator);


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    GoBackN,
    FunctionalUnit,
    "wns.arq.GoBackN",
    FUNConfigCreator);

GoBackN::GoBackN(fun::FUN* fuNet, const wns::pyconfig::View& config) :
        ARQ(config),

        wns::ldk::fu::Plain<GoBackN, GoBackNCommand>(fuNet),
        Delayed<GoBackN>(),
        SuspendSupport(fuNet, config),
        CanTimeout(),

        windowSize(config.get<int>("windowSize")),
        sequenceNumberSize(config.get<int>("sequenceNumberSize")),
        NS(0),
        NR(0),
        LA(0),
        activeCompound(CompoundPtr()),
        sentPDUs(),
        toRetransmit(),
        ackPDUs(),
        receivedPDUs(),
        sendNow(false),
        resendTimeout(config.get<double>("resendTimeout")),
        transmissionAttempts( new wns::probe::bus::ContextCollector( wns::probe::bus::ContextProviderCollection(&getFUN()->getLayer()->getContextProviderCollection()),
                                     config.get<std::string>("probeName"))),
        delayingDelivery(false),
        delayedDeliveryNR(0),
        logger(config.get("logger"))
{
    assure(windowSize >= 2, "Invalid windowSize.");
    assure(sequenceNumberSize >= 2 * windowSize, "Maximum sequence number is to small for chosen windowSize");
    // this calculation here is done only once, not each time when
    // GoBackN::calculateSizes() is called:
    bitsPerACKFrame = bitsPerIFrame = (int) ceil(log(sequenceNumberSize) / log(2));
}


GoBackN::~GoBackN()
{
    // empty internal buffers
    ackPDUs.clear();
    sentPDUs.clear();
    toRetransmit.clear();
    receivedPDUs.clear();
}


bool
GoBackN::hasCapacity() const
{
    /* Make sure we
       1. don't have an active PDU we are processing
       2. aren't occupied with retransmissions
       3. don't exceed the sending window
    */
    return (activeCompound == CompoundPtr()
        && retransmissionState() == false
        && NS - LA < windowSize);
}


void
GoBackN::processOutgoing(const CompoundPtr& compound)
{
    assure(hasCapacity(), "processOutgoing called although not accepting.");
    // argument=SDU from above, stored in class member
    activeCompound = compound;

    // construct info element with SEQNR which is glued to outgoing data packet
    GoBackNCommand* command = activateCommand(compound->getCommandPool());

    // indicate that packet is data (+ seqnr)
    command->peer.type = GoBackNCommand::I;
    // pack SEQNR into it
    command->setNS(NS);

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " processOutgoing: SEQNR NS=" << command->getNS();
    MESSAGE_END();

    ++NS;

    sendNow = true;
}


const wns::ldk::CompoundPtr
GoBackN::hasACK() const
{
    if(!ackPDUs.empty())
    {
        return ackPDUs.front();
    }

    return CompoundPtr();
}


const wns::ldk::CompoundPtr
GoBackN::hasData() const
{
    if(retransmissionState())
    {
        return toRetransmit.front();
    }

    if(activeCompound != CompoundPtr() && sendNow)
    {
        return activeCompound;
    }

    return CompoundPtr();
}


wns::ldk::CompoundPtr
GoBackN::getACK()
{
    assure(hasACK(), getFUN()->getName() + " hasSomethingToSend has not been called to check whether there is something to send.");

    CompoundPtr nextACKToBeSent = ackPDUs.front();
    ackPDUs.pop_front();

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " Sent "
      << (getCommand(nextACKToBeSent->getCommandPool())->getFrameType()==GoBackNCommand::ACK	?"ACK":"NAK")
      << " frame. NS="
      << getCommand(nextACKToBeSent->getCommandPool())->getNS();
    MESSAGE_END();

    return nextACKToBeSent;
}


wns::ldk::CompoundPtr
GoBackN::getData()
{
    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " getData(): RetransmitQueue=" << toRetransmit.size()
      << ", SentQueue=" << sentPDUs.size() << " frames";
    MESSAGE_END();

    if(retransmissionState() == true)
    {
        assure( !toRetransmit.empty(), getFUN()->getName() + " is in retransmission state without anything to retransmit.");

        CompoundPtr nextPDUToBeRetransmit = toRetransmit.front();
        // keep track of the number of retransmissions (counter per PDU)
        GoBackNCommand* command = this->getCommand(nextPDUToBeRetransmit);
        command->localTransmissionCounter++;

        toRetransmit.pop_front();
        sentPDUs.push_back(nextPDUToBeRetransmit);

        MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
        m << " getData(): Outgoing/downstack: Re-Sent I frame NS=" << command->getNS();
        MESSAGE_END();

        if( retransmissionState() == false )
        {
            MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
            m << " getData(): Leaving retransmission state";
            MESSAGE_END();
        }

        // set the Timer
        setNewTimeout(resendTimeout);
        /**
         * @todo only one timer??? in GoBackN, there should be one timer
         * per sent frame (isn't that N-Channel-Stop-And-Wait then?)
         */

        // and send a copy
        return nextPDUToBeRetransmit->copy();
    }

    // send a copy
    sendNow = false; // nothing more to send after this packet
    // if packet isn't ACK'd within this time it will be retransmitted.
    setNewTimeout(resendTimeout); // inherited from events::CanTimeout

    CompoundPtr it = activeCompound->copy();
    // store the PDU we now send in the Retransmission FIFO Buffer
    sentPDUs.push_back(activeCompound); // queue it in sentPDUs
    // keep track of the number of retransmissions
    getCommand(activeCompound)->localTransmissionCounter++; // first transmission of this packet here
    // empty the space for new outgoing compounds
    activeCompound = CompoundPtr(); // empty


    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " getData(): Outgoing/downstack: Sent I frame. NS=" << getCommand(it->getCommandPool())->getNS();
    MESSAGE_END();

    return it; // current packet (copy)
}


void
GoBackN::onTimeout()
{
    assure(!sentPDUs.empty(), "No timeout without any sent frames possible");

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " Entering retransmission state on timeout";
    MESSAGE_END();

    // search & find first packet out of sentPDUs to put into toRetransmit
    // queue
    prepareRetransmission();

    // initiate retransmissions
    tryToSend(); // inherited from 	wns::ldk::Delayed< GoBackN >
    // Use this method if the FunctionalUnit changes state
    // spontanteously. That is, if it may have compounds ready to send while
    // not in processIncoming/processOutgoing.  For example functional units
    // that send compounds after a timeout have to inform the Delayed
    // implementation that they are to be queried again for compounds to be
    // sent.
}


void
GoBackN::processIncoming(const CompoundPtr& compound)
{
    GoBackNCommand *command = getCommand(compound->getCommandPool());

    switch(command->peer.type)
    {
    case GoBackNCommand::I:
        this->onIFrame(compound);
        break;
    case GoBackNCommand::ACK:
        this->onACKFrame(compound);
        break;
    case GoBackNCommand::NAK:
        this->onNAKFrame(compound);
        break;
    }
}

void
GoBackN::onIFrame(const CompoundPtr& compound)
{

    GoBackNCommand* command = getCommand(compound);
    ARQCommand::SequenceNumber receivedNS = command->getNS();
    ARQCommand::SequenceNumber ACKNS = receivedNS;
    GoBackNCommand::FrameType ack_type = GoBackNCommand::I; // means none

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " Received I frame "
      << " expected NR=" << NR
      << " received NS=" << receivedNS;
    MESSAGE_END();

    if(receivedNS == NR)
    {
        if (delayingDelivery)
        {
            if (receivedNS - delayedDeliveryNR < windowSize)
            {
                MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
                m << " Delaying delivery of I frame " << NR;
                MESSAGE_END();

                receivedPDUs.push_back(compound);
                ++NR;

                ack_type = GoBackNCommand::ACK; // return ACK
                ACKNS = receivedNS;
            }
        }
        else
        {
            // this is the I frame we waited for.
            MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
            m << " Delivering I frame " << NR;
            MESSAGE_END();

            getDeliverer()->getAcceptor(compound)->onData(compound); // send to upper layer
            ++NR;

            // check if there are subsequent frames we have already received
            // should not occur for GoBackN
            assure( receivedPDUs.empty() , "receivedPDUs nonempty (impossible for GoBackN)");
            ack_type = GoBackNCommand::ACK; // return ACK
            ACKNS = receivedNS;
        }
    }
    else
    {
        // we received an out-of-sequence frame
        MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
        m << " Discarding out-of-sequence I frame NS=" << command->getNS() << ", sending NAK for NR=" << NR;
        MESSAGE_END();
        //assure( distance(command->getNS(),NR)>0 , "received old/duplicate I frame (impossible for GoBackN)");
        //if( distance(receivedNS, NR ) > 0 ) { // NS to high (missing frame in between)
        //m << " Buffering out-of-sequence I frame NS=" << command->getNS();
        // store the received frame for later
        //keepSorted(compound, receivedPDUs);
        ack_type = GoBackNCommand::NAK; // return NAK
        ACKNS = NR; // NAK for oldest missing frame+1
        //} else {
        /* we received an old frame (ACK got lost)
            re-send the ACK and discard the frame */
        // should not occur for GoBackN
        //ack_type = GoBackNCommand::I; // no ACK
        //}
    }

    if (delayingDelivery == false || receivedNS - delayedDeliveryNR < windowSize)
    {
        switch (ack_type)
        {
        case (GoBackNCommand::ACK):
        case (GoBackNCommand::NAK):
        {
            // acknowledge the received I-Frame by ACK or NAK
            CommandPool* ackPCI = getFUN()->getProxy()->createReply(compound->getCommandPool(), this);
            // warum wird der ackPCI-compund jetzt schon reingesteckt?
            ackPDUs.push_back(CompoundPtr(new Compound(ackPCI)));
            // und wie kommt diese Ergaenzung dann da rein?
            GoBackNCommand* ackCommand = activateCommand(ackPCI);
            ackCommand->setNS(ACKNS);
            ackCommand->peer.type = ack_type;
            MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
            m << " Created ACK/NAK type=" << (int)ack_type << " NS=" << ACKNS;
            MESSAGE_END();
            break;
        }
        case (GoBackNCommand::I): // no ACK
        default:
            break;
        }

        MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
        m << " Number of ACKs pending: " << ackPDUs.size();
        MESSAGE_END();
    }

}

void
GoBackN::onACKFrame(const CompoundPtr& compound)
{
    GoBackNCommand* command = getCommand(compound->getCommandPool());
    ARQCommand::SequenceNumber ackedNS = command->getNS();

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " ACK frame received: NS=" << ackedNS << " LA=" << LA;
    MESSAGE_END();

    // Delete ACKed frame from one of the retransmission buffers
    // it may happen, that due to duplicate ACKs the PDU is neither in sentPDUs
    // nor in toRetransmit
    removeACKed(ackedNS, sentPDUs);
    removeACKed(ackedNS, toRetransmit);

    // LA = last ACK received in order
    if(ackedNS == LA)
    {
        // received the expected ACK
        MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
        m << " ACK frame" << " expected=" << LA << " received=" << ackedNS;
        MESSAGE_END();
        // advance sending window
        ++LA;
        trySuspend();
    }
    else
    {
        //received out-of-sequence ACK
        MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
        m << " Out-of-sequence ACK frame"
            << " expected LA=" << LA << " received=" << ackedNS;
        MESSAGE_END();


        if (ackedNS > LA)
        {
            // ackedNS too high means all lower numbers are acked
            // too
            MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
            m << " ACK for NS=" << ackedNS << " also ACKs older frames: "
                << LA << "..." << ackedNS;
            MESSAGE_END();
            LA=ackedNS;
            ++LA;
            trySuspend();
            // TODO: remove all acked frames from sentPDUs / toRetransmit
            // done in removeACKed()
        }
        else
        {
            // else we received a duplicate and discard it.
            MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
            m << " ACK for NS=" << ackedNS << " must not occur for GoBackN";
            MESSAGE_END();
        }
    }

    if (sentPDUs.empty() && hasTimeoutSet())
    {
        cancelTimeout();
    }
    // Take care of pending retransmissions, if any
    if (retransmissionState())
        tryToSend();
}


void
GoBackN::onNAKFrame(const CompoundPtr& compound)
{
    GoBackNCommand* command = getCommand(compound->getCommandPool());
    ARQCommand::SequenceNumber ackedNS = command->getNS();

    // NAK(SN) means repeat all frames beginning with SN
    // also, this means frames LA...SN-1 are acked

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " NAK frame received: NS=" << ackedNS << " LA=" << LA;
    MESSAGE_END();

    ARQCommand::SequenceNumber SNacked(ackedNS-1);
    removeACKed(SNacked, sentPDUs);
    removeACKed(SNacked, toRetransmit);

    assure( ackedNS >= LA, "NAK for already ACKed frame received");

    // frames LA...ackedNS-1 are acked (counter LA is always acked+1)
    LA=ackedNS;

    // prepare PDU List for Retransmission
    prepareRetransmission();
    if (!retransmissionState())
    {
        MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
        m << " Leaving retransmission state, no Retransmissions pending";
        MESSAGE_END();
    }

    if (sentPDUs.empty() && hasTimeoutSet())
    {
        cancelTimeout();
    }
    // Take care of pending retransmissions, if any
    if (retransmissionState())
        tryToSend();
}


void
GoBackN::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    //What are the sizes in the upper Layers
    getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);

    //Calculate PCI size
    commandPoolSize += bitsPerACKFrame;
    commandPoolSize += 1; // 2 Frametypes can be represented with 1Bit

}


void
GoBackN::prepareRetransmission()
{
    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " prepareRetransmission(): RetransmitQueue=" << toRetransmit.size()
      << ", SentQueue=" << sentPDUs.size() << " frames";
    MESSAGE_END();
    show_seqnr_list("sentPDUs=",sentPDUs);
    show_seqnr_list("toRetransmit=",toRetransmit);

    // announce failed transmission to status collector for statistic collection (usable by other FUs)
    for ( CompoundContainer::iterator it = sentPDUs.begin() ; it != sentPDUs.end() ; it++ )
    {
        this->statusCollector->onFailedTransmission((*it));
    }

    toRetransmit.insert(toRetransmit.end(), sentPDUs.begin(), sentPDUs.end());
    sentPDUs.clear();

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " prepareRetransmission(): RetransmitQueue=" << toRetransmit.size()
      << ", SentQueue=" << sentPDUs.size() << " frames";
    MESSAGE_END();
}

void
GoBackN::removeACKed(const ARQCommand::SequenceNumber ackedNS, CompoundContainer& container)
{
    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " removeACKed(" << ackedNS << ")";
    MESSAGE_END();
    show_seqnr_list("before removal: ",container);
    CompoundPtr compoundElement;

    while( !container.empty() )
    {
        compoundElement = container.front();
        ARQCommand::SequenceNumber NS = getCommand(compoundElement)->getNS();

        if(NS <= ackedNS)
        {
            // a probe counting the number of transmissions needed
            transmissionAttempts->put(getCommand(compoundElement)->localTransmissionCounter);

            // collect the arq statistics for other FUs
            this->statusCollector->onSuccessfullTransmission(compoundElement);

            MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
            m << " ACK for NS=" << NS
              << " received after " << getCommand(compoundElement)->localTransmissionCounter << " transmission attempts";
            MESSAGE_END();
            container.pop_front();
        }
        else
        {
            break;
        }
    }
    show_seqnr_list("after  removal: ",container);
}

bool
GoBackN::retransmissionState() const
{
    return !toRetransmit.empty();
}

void
GoBackN::show_seqnr_list(const char* name, CompoundContainer& compoundList) const
{
    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " " << name << " [";
    for ( CompoundContainer::iterator it = compoundList.begin() ; it != compoundList.end() ; it++ ) {
        int seqnr = getCommand( (*it)->getCommandPool() )->getNS();
        m << seqnr << " ";
    }
    m << "]";
    MESSAGE_END();
}

bool
GoBackN::onSuspend() const
{
    return NS == LA;
}

void
GoBackN::doDelayDelivery()
{
    delayingDelivery = true;
    delayedDeliveryNR = NR;
}

void
GoBackN::doDeliver()
{
    // check if there are subsequent frames we have already received
    while (!receivedPDUs.empty())
    {
#ifndef NDEBUG
        ARQCommand::SequenceNumber toDeliver =  getCommand(receivedPDUs.front())->getNS();
#endif
        assure(toDeliver == delayedDeliveryNR,
               "NS must be equal to NR since the loop will not be entered if it's greater zero and it cannot be smaller!!");
        // if so, deliver them
        MESSAGE_BEGIN(NORMAL, logger, m, "Delivering I frame ");
        m << delayedDeliveryNR;
        MESSAGE_END();

        getDeliverer()->getAcceptor(receivedPDUs.front())->onData(receivedPDUs.front());

        // and remove them from the receivedPDUs list.
        receivedPDUs.pop_front();
        MESSAGE_BEGIN(NORMAL, logger, m, "Removing from receivedPDUs: I-Frame ");
        m << delayedDeliveryNR;
        MESSAGE_END();

        // adjust received PDU counter
        ++delayedDeliveryNR;
    }

    delayingDelivery = false;

}


