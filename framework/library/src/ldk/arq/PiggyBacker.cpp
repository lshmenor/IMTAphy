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

#include <WNS/ldk/arq/PiggyBacker.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/StaticFactory.hpp>

using namespace wns::ldk;
using namespace wns::ldk::arq;


STATIC_FACTORY_REGISTER_WITH_CREATOR(PiggyBacker, FunctionalUnit, "wns.arq.PiggyBacker", FUNConfigCreator);


PiggyBacker::PiggyBacker(fun::FUN* fuNet, const pyconfig::View& config) :
        wns::ldk::fu::Plain<PiggyBacker, PiggyBackerCommand>(fuNet),

        arqName(config.get<std::string>("arq")),
        bitsIfPiggyBacked(config.get<Bit>("bitsIfPiggyBacked")),
        bitsIfNotPiggyBacked(config.get<Bit>("bitsIfNotPiggyBacked")),
        addACKPDUSize(config.get<bool>("addACKPDUSize")),

        i(),
        rr(),
        inControl(false),
        logger("WNS", "PiggyBacker")
{
}


void
PiggyBacker::onFUNCreated()
{
    friends.arq = getFUN()->findFriend<ARQ*>(arqName);
    assure(friends.arq, "PiggyBacker requires an ARQ friend with name '" + arqName + "'");
}


bool
PiggyBacker::doIsAccepting(const CompoundPtr& compound) const
{
    ARQCommand* arqPCI = getARQPCI(compound);

    if(arqPCI->isACK())
    {
        return rr == CompoundPtr();
    }
    else
    {
        return i == CompoundPtr();
    }
} // isAccepting


void
PiggyBacker::doSendData(const CompoundPtr& compound)
{
    assure(isAccepting(compound), "sendData called although PiggyBacker is not accepting.");

    activateCommand(compound->getCommandPool());

    ARQCommand* arqPCI = getARQPCI(compound);
    if(arqPCI->isACK())
    {
        rr = compound;
    }
    else
    {
        i = compound;
    }

    if(inControl)
        return;

    wakeup();
} // doSendData


void
PiggyBacker::doOnData(const CompoundPtr& compound)
{
	PiggyBackerCommand* command = getCommand(compound->getCommandPool());

	if(command->peer.piggyBacked) {
		getDeliverer()->getAcceptor(compound)->onData(command->peer.piggyBacked);
	}

	getDeliverer()->getAcceptor(compound)->onData(compound);
} // doOnData


void
PiggyBacker::doWakeup()
{
    inControl = true;
    {
        if(!i)
        {
            friends.arq->preferACK(false);
            getReceptor()->wakeup();
            friends.arq->preferACK(true);
        }
        if(i && !rr)
        {
            getReceptor()->wakeup();
        }
    }
    inControl = false;

    tryToSend();
} // wakeup


void
PiggyBacker::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& sduSize) const
{
    //What are the sizes in the upper Layers
    getFUN()->calculateSizes(commandPool, commandPoolSize, sduSize, this);

    PiggyBackerCommand* command = getCommand(commandPool);
    if(command->peer.piggyBacked)
    {
        commandPoolSize += bitsIfPiggyBacked;

        if(addACKPDUSize)
        {
            Bit ackPCISize;
            Bit ackSDUSize;
            getFUN()->calculateSizes(command->peer.piggyBacked->getCommandPool(), ackPCISize, ackSDUSize, this);

            commandPoolSize += ackPCISize + ackSDUSize;
        }
    }
    else
    {
        commandPoolSize += bitsIfNotPiggyBacked;
    }
} // calculateSizes


void
PiggyBacker::tryToSend()
{
    CompoundPtr it = CompoundPtr();

    MESSAGE_BEGIN(NORMAL, logger, m, getFUN()->getName());
    if(rr || i)
    {
        m << " send (";
        if(i)
            m << "i ";
        if(rr)
            m << "rr";
        m << ")";
    }
    MESSAGE_END();

    if(rr && i)
    {
        PiggyBackerCommand* command = getCommand(i->getCommandPool());
        command->peer.piggyBacked = rr;

        it = i;

    }
    else if(i)
    {
        it = i;

    }
    else if(rr)
    {
        it = rr;

    }
    else
    {
        return;
    }

    if(!getConnector()->hasAcceptor(it))
        return;

    getConnector()->getAcceptor(it)->sendData(it);
    rr = CompoundPtr();
    i = CompoundPtr();
} // tryToSend


ARQCommand*
PiggyBacker::getARQPCI(const CompoundPtr& compound) const
{
    ARQCommand* arqPCI = dynamic_cast<ARQCommand*>(friends.arq->getCommand(compound->getCommandPool()));
    assure(arqPCI, "Expected an ARQCommand instance.");

    return arqPCI;
} // getARQPCI



