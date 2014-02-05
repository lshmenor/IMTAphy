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

#include <WNS/ldk/arq/CumulativeACK.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/Assure.hpp>

#include <algorithm>

using namespace wns::ldk;
using namespace wns::ldk::arq;


STATIC_FACTORY_REGISTER_WITH_CREATOR(CumulativeACK, ARQ, "wns.arq.CumulativeACK", FUNConfigCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(CumulativeACK, FunctionalUnit, "wns.arq.CumulativeACK", FUNConfigCreator);

CumulativeACK::CumulativeACK(fun::FUN* fuNet, const wns::pyconfig::View& config) :
    ARQ(config),
    wns::ldk::fu::Plain<CumulativeACK, CumulativeACKCommand>(fuNet),
    Delayed<CumulativeACK>(),
    SuspendSupport(fuNet, config),
    CanTimeout(),
    ackCompound(CompoundPtr()),
    wS(config.get<unsigned long int>("windowSize")),
    NS(0),
    NSack(0),
    NR(0),
    sequenceNumberSize(config.get<int>("sequenceNumberSize")),
    receivingCompounds(),
    sendingCompounds(),
    resendTimeout(config.get<double>("resendTimeout")),
    delayingDelivery(false),
    delayedDeliveryNR(0),
    logger(config.get("logger"))
{
    assure((wS % 2) == 0, " Invalid windowSize, it has to be even.");
    assure(wS >= 4, " Invalid windowSize, it is to small.");
    CAElement foo(this);
    fill_n(back_inserter(sendingCompounds),wS,foo);
    fill_n(back_inserter(receivingCompounds),wS,foo);
} // CumulativeACK

CumulativeACK::~CumulativeACK()
{
    receivingCompounds.clear();
    sendingCompounds.clear();
} // ~CumulativeACK


bool
CumulativeACK::hasCapacity() const
{
    //avoid warning
    int foo = wS / 2;
    int ret = NS - NSack;
    if (NS < NSack)
        ret = NS + wS - NSack;
        assure(ret <= foo, " Error :  Negative Capacity !");
    return (ret < foo);
} // hasCapacity


void CumulativeACK::processOutgoing(const wns::ldk::CompoundPtr& _compound)
{
    assure(hasCapacity(), "processOutgoing called although not accepting.");
    assure(_compound != CompoundPtr(), "given compound is null");

    sendingCompounds[NS].compound = _compound;
    CumulativeACKCommand *command = activateCommand(_compound->getCommandPool());
    command->peer.type = CumulativeACKCommand::I;
    command->peer.NS = NS;
    sendingCompounds[NS].sendNow = true;

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " processOutgoing NS -> " << command->peer.NS;
    MESSAGE_END();

    NS = (NS + 1) % wS;
} // processOutgoing


const wns::ldk::CompoundPtr
CumulativeACK::hasACK() const
{
    return this->ackCompound;
} // hasACK


const wns::ldk::CompoundPtr
CumulativeACK::hasData() const
{
    unsigned int tmpNS = NS;
    int diff = NS - NSack ;
    if(diff < 0)
        tmpNS = NS + wS;
    for(unsigned int i = NSack; i < tmpNS; i++)
    {
        if(sendingCompounds[i % wS].compound != CompoundPtr() && sendingCompounds[i % wS].sendNow)
        return sendingCompounds[(i % wS)].compound;
    }

    return CompoundPtr();
} // hasData


wns::ldk::CompoundPtr
CumulativeACK::getACK()
{
    wns::ldk::CompoundPtr it = this->ackCompound;
    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    m << " Send ACK        NR -> " << getCommand(this->ackCompound->getCommandPool())->peer.NR;
    MESSAGE_END();
    this->ackCompound = CompoundPtr();
    return it;
} // getACK


wns::ldk::CompoundPtr
CumulativeACK::getData()
{
    unsigned int tmpNS = NS;
    int diff = NS-NSack;
    if (diff < 0)
        tmpNS = NS + wS;
    for (unsigned int i = NSack ; i <= tmpNS; i++ )
    {
        if( sendingCompounds[(i % wS)].compound != CompoundPtr() && sendingCompounds[(i % wS)].sendNow)
        {
            sendingCompounds[(i % wS)].sendNow = false;
            sendingCompounds[(i % wS)].setTimeout(resendTimeout);
            MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
            m << " Send IFrame     NS -> " << getCommand(sendingCompounds[i % wS].getCompound()->getCommandPool())->peer.NS;
            MESSAGE_END();
            //sending a copy
            return sendingCompounds[(i % wS)].getCompound()->copy();
        }
    }

    assure(false, "oops!");
    return CompoundPtr();
} // getData


void CumulativeACK::processIncoming(const wns::ldk::CompoundPtr& _compound)
{
    wns::ldk::CompoundPtr compound = _compound;
    CumulativeACKCommand* command = getCommand(compound->getCommandPool());
    switch(command->peer.type)
    {
    case CumulativeACKCommand::I:
        MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
        m << " Received IFrame NS -> " << command->peer.NS <<" expected NR -> " << NR;
        MESSAGE_END();

        if (delayingDelivery)
        {
            if ((command->peer.NS - delayedDeliveryNR + wS) % wS < wS/2)
            {
                receivingCompounds[command->peer.NS].compound = compound;

                if(command->peer.NS == NR)
                {
                    unsigned int tmpNR = NR;
                    NR = (NR + 1) % wS;
                    for(; receivingCompounds[NR].compound != CompoundPtr(); NR = (NR + 1) % wS)
                    {
                        tmpNR = NR;
                    }

                    //send an ACK
                    assure(receivingCompounds[tmpNR].compound != CompoundPtr(), " error ACK can not be sent ");
                    CommandPool* ackPCI = createReply(receivingCompounds[tmpNR].compound->getCommandPool());
                    this->ackCompound = CompoundPtr(new wns::ldk::Compound(ackPCI));
                    CumulativeACKCommand* ackCommand = activateCommand(ackPCI);
                    ackCommand->peer.type = CumulativeACKCommand::RR;
                    ackCommand->peer.NR = NR;
                    ackCommand->peer.NS = NR;
                }
            }
        }
        
        else
        {
            // this is the I Frame we waited for.
            if(command->peer.NS == NR)
            {
                receivingCompounds[NR].compound = compound;
                //Loop assures that RR-message refers to a not received PDU.
                unsigned int tmpNR = NR;
                NR = (NR + 1) % wS;
                for(; receivingCompounds[NR].compound != CompoundPtr(); NR = (NR + 1) % wS)
                {
                    CompoundPtr tmpCompound = receivingCompounds[tmpNR].compound;
                    getDeliverer()->getAcceptor(tmpCompound)->onData(tmpCompound);
                    receivingCompounds[tmpNR].compound = CompoundPtr();
                    tmpNR = NR;
                }

                //send an ACK
                assure(receivingCompounds[tmpNR].compound != CompoundPtr(), " error ACK can not be sent ");
                CommandPool* ackPCI = getFUN()->getProxy()->createReply(receivingCompounds[tmpNR].compound->getCommandPool(), this);
                this->ackCompound = CompoundPtr(new wns::ldk::Compound(ackPCI));
                CumulativeACKCommand* ackCommand = activateCommand(ackPCI);
                ackCommand->peer.type = CumulativeACKCommand::RR;
                ackCommand->peer.NR = NR;
                ackCommand->peer.NS = NR;

                getDeliverer()->getAcceptor(receivingCompounds[tmpNR].compound)->onData(receivingCompounds[tmpNR].compound);
                receivingCompounds[tmpNR].compound = CompoundPtr();
            }
            // in wS but after NR -> no ACK is send as there is at least a missing PDU before
            else if(((command->peer.NS > NR) && (command->peer.NS < (NR + wS / 2))) // NR and NS are not divided by a frame-boundary
                    || ((command->peer.NS + wS) < (NR + wS / 2)))
            { // divided by a frame-boundary
                receivingCompounds[command->peer.NS].compound = compound;
            }
            //earlier compound is received again -> send RR-message with current NR
            else
            {
                if (this->ackCompound == CompoundPtr()){
                    CommandPool* ackPCI = getFUN()->getProxy()->createReply(compound->getCommandPool(), this);
                    compound = CompoundPtr();
                    this->ackCompound = CompoundPtr(new wns::ldk::Compound(ackPCI));
                    CumulativeACKCommand* ackCommand = activateCommand(ackPCI);
                    ackCommand->peer.type = CumulativeACKCommand::RR;
                    ackCommand->peer.NS = NR;
                    ackCommand->peer.NR = NR;
                }
            }
        }
        break;
    case CumulativeACKCommand::RR:
    {
        // ACK received although unacknowledged packet does not exist
        if (NSack == NS)
            return;
        if ((command->peer.NR - NSack + wS - 1) % wS >= wS/2)
        return;
        //2.index helping to cope with indexHop
        unsigned int tmpNS = NS;
        //prevent warning
        int dif = NS - NSack;
        if (dif < 0)
            tmpNS = NS + wS;
        //lower index by one
        unsigned int tmpI = NSack;
        unsigned int i;

        for ( i = (NSack + 1) % wS ; i <= tmpNS; ++i )
        {
            //assumes that no wrong command->peer.NS is received
            this->statusCollector->onSuccessfullTransmission(sendingCompounds[tmpI % wS].compound);
            sendingCompounds[tmpI % wS].sendNow = false;
            sendingCompounds[tmpI % wS].compound = CompoundPtr();
            if (sendingCompounds[tmpI % wS].hasTimeoutSet())
                sendingCompounds[tmpI % wS].cancelTimeout();
            if( (i % wS) == command->peer.NR )
            {
                MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
                m << " Received RRFrame NR-> " << command->peer.NR;
                MESSAGE_END();
                NSack = i % wS;
                compound = CompoundPtr();
                break;
            }
            tmpI = i;
        }
        //assure(compound==CompoundPtr(),"ACK compound has been received and processed but it is still referred to");
    }
    break;
    default: assure(0," Unexpected PDU received!");
    }
    trySuspend();
} // processOutgoing

void
CumulativeACK::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    //What are the sizes in the upper Layers
    getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);

    //Calculate PCI size
    commandPoolSize += static_cast<int>(ceil(log(sequenceNumberSize) / log(2)));
    commandPoolSize += 2; // 3 Frametypes can be represented with 2 Bits

} // calculateSizes

bool
CumulativeACK::onSuspend() const
{
    return NS == NSack;
} // onSuspend

void
CumulativeACK::doDelayDelivery()
{
    delayingDelivery = true;
    delayedDeliveryNR = NR;
} // doDelayDelivery

void
CumulativeACK::doDeliver()
{
    delayingDelivery = false;

    for(; receivingCompounds[delayedDeliveryNR].compound != CompoundPtr(); delayedDeliveryNR = (delayedDeliveryNR + 1) % wS) {
        CompoundPtr tmpCompound = receivingCompounds[delayedDeliveryNR].compound;
        getDeliverer()->getAcceptor(tmpCompound)->onData(tmpCompound);
        receivingCompounds[delayedDeliveryNR].compound = CompoundPtr();
    }
} // doDeliver


