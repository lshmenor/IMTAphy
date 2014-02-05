/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <WNS/ldk/arq/StopAndWaitRC.hpp>
#include <WNS/ldk/Port.hpp>

#include <WNS/Assure.hpp>

using namespace wns::ldk::arq;
using namespace wns::ldk;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wns::ldk::arq::StopAndWaitRC,
    wns::ldk::FunctionalUnit,
    "wns.arq.StopAndWaitRC",
    wns::ldk::FUNConfigCreator);


const std::string Data::name = "Data";
const std::string Ack::name = "Ack";

StopAndWaitRC::StopAndWaitRC(fun::FUN* fuNet, const wns::pyconfig::View& config)
    : FunctionalUnitRC< StopAndWaitRC >(this),
      CommandTypeSpecifier< StopAndWaitRCCommand >(fuNet),
      HasUpPort< StopAndWaitRC >(this),
      HasDownPort< StopAndWaitRC, Port<Data> >(this),
      HasDownPort< StopAndWaitRC, Port<Ack> >(this),
      Cloneable< StopAndWaitRC >(),
      events::CanTimeout(),

      resendTimeout(config.get<double>("resendTimeout")),
      bitsPerIFrame(config.get<int>("bitsPerIFrame")),
      bitsPerRRFrame(config.get<int>("bitsPerRRFrame")),

      NS(0),
      NR(0),
      activeCompound(CompoundPtr()),
      ackCompound(CompoundPtr()),
      sendNowData(false),
      logger(config.get("logger"))
{
}

StopAndWaitRC::~StopAndWaitRC()
{
}

bool
StopAndWaitRC::doIsAccepting(const CompoundPtr& compound) const
{
    if (!activeCompound)
    {
        CompoundPtr compoundCopy = compound->copy();

        StopAndWaitRCCommand* command = activateCommand(compoundCopy->getCommandPool());
        command->peer.type = StopAndWaitRCCommand::I;
        command->peer.NS = NS;

        return getPortConnector< Port<Data> >()->hasAcceptor(compoundCopy);
    }

    return false;
}

void
StopAndWaitRC::doSendData(const CompoundPtr& compound)
{
    assure(isAccepting(compound), "sendData called although not accepting!");

    activeCompound = compound;

    StopAndWaitRCCommand* command = activateCommand(activeCompound->getCommandPool());

    command->peer.type = StopAndWaitRCCommand::I;
    command->peer.NS = NS;
    command->localTransmissionCounter = 1;

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " processOutgoing(compound),"
      << " sequence number (NS) of compound: " << command->peer.NS;
    MESSAGE_END();

    ++NS;

    if (getPortConnector< Port<Data> >()->hasAcceptor(activeCompound))
    {
        setTimeout(resendTimeout);
        getPortConnector< Port<Data> >()->getAcceptor(activeCompound)->sendData(activeCompound->copy());
    }
    else
    {
        sendNowData = true;
    }
}

namespace wns { namespace ldk { namespace arq {

            template <>
            void
            StopAndWaitRC::doWakeup(Port<Data>)
            {
                if (sendNowData &&
                    getPortConnector< Port<Data> >()->hasAcceptor(activeCompound))
                {
                    setTimeout(resendTimeout);
                    getPortConnector< Port<Data> >()->getAcceptor(activeCompound)->sendData(activeCompound->copy());

                    sendNowData = false;
                }

                getReceptor()->wakeup();
            }

            template <>
            void
            StopAndWaitRC::doWakeup(Port<Ack>)
            {
                if (ackCompound &&
                    getPortConnector< Port<Ack> >()->hasAcceptor(ackCompound))
                {
                    getPortConnector< Port<Ack> >()->getAcceptor(ackCompound)
                        ->sendData(ackCompound);

                    ackCompound = CompoundPtr();
                }
            }

            template <>
            void
            StopAndWaitRC::doOnData(const CompoundPtr& compound, Port<Data>)
            {
                StopAndWaitRCCommand *command = getCommand(compound->getCommandPool());
                assure(command->peer.type == StopAndWaitRCCommand::I,
                       "Received RR frame via 'Data' port");

                MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
                m << " processIncoming(compound), Received I frame "
                  << " expected (NR) " << NR
                  << " received (command->peer.NS) " << command->peer.NS;
                MESSAGE_END();

                // The I-Frame must be either the one we're expecting (NR) or
                // the one before the one we're expecting (NR-1). If it is NR-1
                // it is a duplicate, that we've already acknowledged. May be
                // our ACK got lost. So we send it again.
                if(command->peer.NS == NR || command->peer.NS == NR-1)
                {
                    // As stated above ACK is sent in any case
                    CommandPool* ackPCI = getFUN()->getProxy()->createReply(compound->getCommandPool(), this);
                    ackCompound = CompoundPtr(new Compound(ackPCI));
                    StopAndWaitRCCommand* ackCommand = activateCommand(ackPCI);

                    ackCommand->peer.type = StopAndWaitRCCommand::RR;
                    ackCommand->peer.NS = NR;
                    MESSAGE_SINGLE(NORMAL, logger,
                                   getFUN()->getName() << " Prepared RR frame (ACK) with NS="
                                   << ackCommand->peer.NS << " (the next I frame we're expexcting)");

                    if (getPortConnector< Port<Ack> >()->hasAcceptor(ackCompound))
                    {
                        getPortConnector< Port<Ack> >()->getAcceptor(ackCompound)->sendData(ackCompound);

                        ackCompound = CompoundPtr();
                    }

                    if(command->peer.NS == NR)
                    {
                        MESSAGE_SINGLE(NORMAL, logger,
                                       getFUN()->getName() << " This was the next expected I frame");
                        getDeliverer()->getAcceptor(compound)->onData(compound);
                        NR++;
                    }
                    else
                    {
                        MESSAGE_SINGLE(NORMAL, logger,
                                       getFUN()->getName() << " Already received this I frame (duplicate)");
                    }

                }
                else
                {
                    // this can not happen.
                    wns::Exception e;
                    e << "StopAndWait-ARQ received an I frame that neither (NR) nor (NR-1). "
                      << "This cannot happen and is most probably an implementation error";
                    throw e;
                }
            }

            template <>
            void
            StopAndWaitRC::doOnData(const CompoundPtr& compound, Port<Ack>)
            {
                StopAndWaitRCCommand *command = getCommand(compound->getCommandPool());
                assure(command->peer.type == StopAndWaitRCCommand::RR,
                       "Received I frame via 'Data' port");

                MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
                m << " processIncoming(compound), Received RR frame "
                  << " expected (this->NS) " << NS
                  << " received (command->peer.NS) " << command->peer.NS;
                MESSAGE_END();

                if(!(command->peer.NS == NS || command->peer.NS == NS-1))
                {
                    // this can not happen.
                    wns::Exception e;
                    e << "StopAndWait-ARQ received an ACK that with neither (NS) nor (NS-1). "
                      << "This cannot happen and is most probably an implementation error";
                    // stop here
                    throw e;
                }

                // only compounds with NS == NS or NS == NS-1 are left
                if(command->peer.NS == NS-1)
                {
                    // duplicate ACK (must be due to duplicate I)
                    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
                    m << " Unexpected RR frame (due to duplicate I frame).";
                    m << "\nHINT: Check your resend timeout. This indicates it is too short.";
                    MESSAGE_END();
                }
                else
                {
                    // this is the ACK we've been waiting for -> we can stop
                    // the timeout
                    MESSAGE_SINGLE(NORMAL, logger, getFUN()->getName()
                                   << " This is the RR frame (ACK) for the last sent I frame (compound)");
                    //        statusCollector->onSuccessfullTransmission(activeCompound);

                    activeCompound = CompoundPtr();

                    if (hasTimeoutSet() == true)
                    {
                        MESSAGE_SINGLE(VERBOSE, logger, getFUN()->getName() << "Stopping timeout.");
                        cancelTimeout();
                    }

                    MESSAGE_SINGLE(VERBOSE, logger, getFUN()->getName()
                                   << " Ready for next compound from higher FU.");

                    getReceptor()->wakeup();
                }
            }
        } //arq
    } //ldk
} //wns

void
StopAndWaitRC::onTimeout()
{
    assure(activeCompound || !sendNowData, "Unexpected timeout.");

    //    statusCollector->onFailedTransmission(activeCompound);

    StopAndWaitRCCommand* command = getCommand(activeCompound);
    command->localTransmissionCounter++;

    MESSAGE_SINGLE(NORMAL, logger,
                   "Timeout for compound with sequence number (NS) " << command->peer.NS <<
                   ". Increased transmission counter to " << command->localTransmissionCounter);

    if (getPortConnector< Port<Data> >()->hasAcceptor(activeCompound))
    {
        setTimeout(resendTimeout);
        getPortConnector< Port<Data> >()->getAcceptor(activeCompound)->sendData(activeCompound->copy());
    }
    else
    {
        sendNowData = true;
    }
}

void
StopAndWaitRC::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    //What are the sizes in the upper Layers
    getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);

    StopAndWaitRCCommand* command = getCommand(commandPool);

    switch(command->peer.type)
    {
    case StopAndWaitRCCommand::I:
    {
        commandPoolSize += bitsPerIFrame;
        break;
    }
    case StopAndWaitRCCommand::RR:
    {
        commandPoolSize += bitsPerRRFrame;
        break;
    }
    default:
    {
        wns::Exception e;
        e << getFUN()->getName()
          << " StopAndWait-ARQ: Unknown frame type in size calculation (" << command->peer.type << ").";
        // stop here
        throw e;
        break;
    }
    }
}
