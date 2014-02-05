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

#include <WNS/ldk/arq/StopAndWait.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/Assure.hpp>

using namespace wns::ldk;
using namespace wns::ldk::arq;


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    StopAndWait,
    ARQ,
    "wns.arq.StopAndWait",
    FUNConfigCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    StopAndWait,
    FunctionalUnit,
    "wns.arq.StopAndWait",
    FUNConfigCreator);

StopAndWait::StopAndWait(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    ARQ(config),
    wns::ldk::fu::Plain<StopAndWait, StopAndWaitCommand>(fuNet),
    Delayed<StopAndWait>(),
    SuspendSupport(fuNet, config),
    CanTimeout(),

    resendTimeout(config.get<double>("resendTimeout")),
    bitsPerIFrame(config.get<int>("bitsPerIFrame")),
    bitsPerRRFrame(config.get<int>("bitsPerRRFrame")),

    NS(0),
    NR(0),
    activeCompound(CompoundPtr()),
    ackCompound(CompoundPtr()),
    sendNow(false),
    logger(config.get("logger"))
{
} // StopAndWait


StopAndWait::~StopAndWait()
{
} // ~StopAndWait


bool
StopAndWait::hasCapacity() const
{
    return !this->activeCompound;
} // hasCapacity


void
StopAndWait::processOutgoing(const CompoundPtr& compound)
{
    assure(this->hasCapacity(), "processOutgoing called although not accepting.");
    this->activeCompound = compound;

    StopAndWaitCommand* command = this->activateCommand(compound->getCommandPool());

    command->peer.type = StopAndWaitCommand::I;
    command->peer.NS = this->NS;
    command->localTransmissionCounter = 1;

    MESSAGE_BEGIN(NORMAL, this->logger, m, this->getFUN()->getName());
    m << " processOutgoing(compound),"
      << " sequence number (NS) of compound: " << command->peer.NS;
    MESSAGE_END();

    ++this->NS;

    this->sendNow = true;
} // processOutgoing


const wns::ldk::CompoundPtr
StopAndWait::hasACK() const
{
    return this->ackCompound;
} // hasACK


const wns::ldk::CompoundPtr
StopAndWait::hasData() const
{
    if(this->sendNow)
    {
        return this->activeCompound;
    }
    else
    {
        return CompoundPtr();
    }
} // hasData


wns::ldk::CompoundPtr
StopAndWait::getACK()
{
    CompoundPtr it = this->ackCompound;
    this->ackCompound = CompoundPtr();

    return it;
} // getACK


wns::ldk::CompoundPtr
StopAndWait::getData()
{
    this->sendNow = false;
    this->setTimeout(this->resendTimeout);

    // send a copy
    return this->activeCompound->copy();
} // getData


void
StopAndWait::onTimeout()
{
    assure(this->activeCompound, "Unexpected timeout.");

    this->statusCollector->onFailedTransmission(this->activeCompound);

    StopAndWaitCommand* command = this->getCommand(this->activeCompound);
    command->localTransmissionCounter++;

    MESSAGE_SINGLE(
        NORMAL,
        this->logger,
        "Timeout for compound with sequence number (NS) " << command->peer.NS <<
        ". Increased transmission counter to " << command->localTransmissionCounter);

    this->sendNow = true;
    this->tryToSend();
} // onTimeout


void
StopAndWait::processIncoming(const CompoundPtr& compound)
{
    StopAndWaitCommand *command = this->getCommand(compound->getCommandPool());

    switch(command->peer.type)
    {
    case StopAndWaitCommand::I:
    {
        MESSAGE_BEGIN(NORMAL, this->logger, m, this->getFUN()->getName());
        m << " processIncoming(compound), Received I frame "
          << " expected (this->NR) " << this->NR
          << " received (command->peer.NS) " << command->peer.NS;
        MESSAGE_END();

        // The I-Frame must be either the one we're expecting (NR) or
        // the one before the one we're expecting (NR-1). If it is NR-1
        // it is a duplicate, that we've already acknowledged. May be
        // our ACK got lost. So we send it again.
        if(command->peer.NS == NR || command->peer.NS == NR-1)
        {
            if(command->peer.NS == NR)
            {
                MESSAGE_SINGLE(NORMAL, this->logger, this->getFUN()->getName() << " This was the next expected I frame");
                this->getDeliverer()->getAcceptor(compound)->onData(compound);
                this->NR++;
            }
            else
            {
                MESSAGE_SINGLE(NORMAL, this->logger, this->getFUN()->getName() << " Already received this I frame (duplicate)");
            }

            // As stated above ACK is sent in any case
            CommandPool* ackPCI = this->getFUN()->getProxy()->createReply(compound->getCommandPool(), this);
            ackCompound = CompoundPtr(new Compound(ackPCI));
            StopAndWaitCommand* ackCommand = this->activateCommand(ackPCI);

            ackCommand->peer.type = StopAndWaitCommand::RR;
            ackCommand->peer.NS = NR;
            MESSAGE_SINGLE(NORMAL, this->logger, this->getFUN()->getName() << " Prepared RR frame (ACK) with NS=" << ackCommand->peer.NS << " (the next I frame we're expexcting)");
        }
        else
        {
            // this can not happen.
            wns::Exception e;
            e << "StopAndWait-ARQ received an I frame that neither (NR) nor (NR-1). "
              << "This can not happen and is most probably an implementation error";
            throw e;
        }

        break;
    }

    case StopAndWaitCommand::RR:
    {
        MESSAGE_BEGIN(NORMAL, this->logger, m, this->getFUN()->getName());
        m << " processIncoming(compound), Received RR frame "
          << " expected (this->NS) " << this->NS
          << " received (command->peer.NS) " << command->peer.NS;
        MESSAGE_END();

        if(!(command->peer.NS == NS || command->peer.NS == NS-1))
        {
            // this can not happen.
            wns::Exception e;
            e << "StopAndWait-ARQ received an ACK that with neither (NS) nor (NS-1). "
              << "This can not happen and is most probably an implementation error";
            // stop here
            throw e;
        }

        // only compounds with NS == NS or NS == NS-1 are left
        if(command->peer.NS == NS - 1)
        {
            // duplicate ACK (must be due to duplicate I)
            MESSAGE_BEGIN(NORMAL, this->logger, m, this->getFUN()->getName());
            m << " Unexpected RR frame (due to duplicate I frame).";
            m << "\nHINT: Check your resend timeout. This indicates it is too short.";
            MESSAGE_END();
            break;
        }
        else
        {
            // this is the ACK we've been waiting for -> we can stop
            // the timeout
            MESSAGE_SINGLE(NORMAL, this->logger, this->getFUN()->getName() << " This is the RR frame (ACK) for the last sent I frame (compound)");
            this->statusCollector->onSuccessfullTransmission(this->activeCompound);

            this->activeCompound = CompoundPtr();
            this->trySuspend();

            if (this->hasTimeoutSet() == true)
            {
                MESSAGE_SINGLE(VERBOSE, this->logger, this->getFUN()->getName() << "Stopping timeout.");
                this->cancelTimeout();
            }
            MESSAGE_SINGLE(VERBOSE, this->logger, this->getFUN()->getName() << " Ready for next compound from higher FU.");
        }
        break;
    }

    default:
    {
        wns::Exception e;
        e << this->getFUN()->getName()
          << " StopAndWait-ARQ: Unknown frame type received (" << command->peer.type << ").";
        // stop here
        throw e;
        break;
    }
    }
}

void
StopAndWait::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    //What are the sizes in the upper Layers
    this->getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);

    StopAndWaitCommand* command = this->getCommand(commandPool);

    switch(command->peer.type)
    {
    case StopAndWaitCommand::I:
    {
        commandPoolSize += this->bitsPerIFrame;
        break;
    }
    case StopAndWaitCommand::RR:
    {
        commandPoolSize += this->bitsPerRRFrame;
        break;
    }
    default:
    {
        wns::Exception e;
        e << this->getFUN()->getName()
          << " StopAndWait-ARQ: Unknown frame type in size calculation (" << command->peer.type << ").";
        // stop here
        throw e;
        break;
    }
    }
} // calculateSizes


bool
StopAndWait::onSuspend() const
{
    return !activeCompound;
} // onSuspend



