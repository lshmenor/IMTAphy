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

#include <WNS/ldk/tools/Stub.hpp>

#include <cstdlib>
#include <functional>

using namespace wns::ldk;
using namespace wns::ldk::tools;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Stub, FunctionalUnit, "wns.tools.Stub", FUNConfigCreator);

StubBase::StubBase() :
		HasReceptor<>(),
		HasConnector<>(),
		HasDeliverer<>(),

		received(ContainerType()),
		sent(ContainerType()),
		wakeupCalled(),
		onFUNCreatedCalled(),
		stepping(false),
		accepting(true),
		addToPCISize(0),
                addToPDUSize(0),
                compoundAcceptanceSize(0)
{
}

StubBase::~StubBase()
{
	flush();
}

void
StubBase::setStepping(bool _stepping)
{
	stepping = _stepping;
}


void
StubBase::setSizes(Bit _addToPCISize, Bit _addToPDUSize)
{
	addToPCISize = _addToPCISize;
	addToPDUSize = _addToPDUSize;
}


void
StubBase::setAcceptanceSize(Bit compoundSize)
{
	compoundAcceptanceSize = compoundSize;
}


void
StubBase::flush()
{
	received.clear();
	sent.clear();
}


void
StubBase::open(bool wakeup)
{
	accepting = true;
	if(wakeup)
		getReceptor()->wakeup();
}


void
StubBase::close()
{
	accepting = false;
}


void
StubBase::step()
{
	stepping = true;
	open();
	stepping = false;
}


bool
StubBase::doIsAccepting(const CompoundPtr& compound) const
{
	if(!accepting)
	{
		return false;
	}

        if (compoundAcceptanceSize > 0)
        {
            Bit commandPoolSize = 0;
            Bit dataSize = 0;

            CompoundPtr compoundCopy = compound->copy();
            activateCommand(compoundCopy->getCommandPool());
            getFUN()->calculateSizes(compoundCopy->getCommandPool(), commandPoolSize, dataSize, this);

            if(commandPoolSize + dataSize > compoundAcceptanceSize)
                return false;
        }

	if(getConnector()->size() == 0)
	{
		return true;
	}

	return getConnector()->hasAcceptor(compound);
}


void
StubBase::doSendData(const CompoundPtr& compound)
{
	assure(compound != CompoundPtr(), "sendData called with an invalid compound.");

	if (compound->getCommandPool() && this->registeredAtProxy())
	{
		if (this->getFUN()->getProxy()->commandIsActivated(compound->getCommandPool(), this) == false)
		{
		        this->activateCommand(compound->getCommandPool());
		}
	}

	if (this->stepping)
	{
		this->close();
	}

	sent.push_back(compound);

	if (this->getConnector()->size())
	{
		this->getConnector()->getAcceptor(compound)->sendData(compound);
	}

	assure(this->integrityCheck(), "Integrity check failed.");
}


void
StubBase::doOnData(const CompoundPtr& compound)
{
	assure(compound != CompoundPtr(), "Not a valid PDU.");

	received.push_back(compound);

	if(getDeliverer()->size()) {
		getDeliverer()->getAcceptor(compound)->onData(compound);
	}

	assure(integrityCheck(), "Integrity check failed.");
}


void
StubBase::doWakeup()
{
	++wakeupCalled;

	getReceptor()->wakeup();
}

void
StubBase::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
{
	getFUN()->calculateSizes(commandPool, commandPoolSize, dataSize, this);

	commandPoolSize += addToPCISize;
	dataSize += addToPDUSize;
}

void
StubBase::onFUNCreated()
{
	++onFUNCreatedCalled;
}

bool
StubBase::integrityCheck()
{
	for(ContainerType::iterator i = received.begin(); i != received.end(); ++i)
		if((*i)->getRefCount() < 1)
			return false;

	for(ContainerType::iterator i = sent.begin(); i != sent.end(); ++i)
		if((*i)->getRefCount() < 1)
			return false;

	return true;
}


Stub::Stub(fun::FUN* fuNet, const pyconfig::View& /* config */) :
	StubBase(),
	CommandTypeSpecifier<StubCommand>(fuNet),
        Cloneable<Stub>(),
        sequenceNumber(0)
{
}


void
Stub::doSendData(const CompoundPtr& compound)
{
	if (compound->getCommandPool() && this->registeredAtProxy())
	{
		if (this->getFUN()->getProxy()->commandIsActivated(compound->getCommandPool(), this) == false)
		{
			this->activateCommand(compound->getCommandPool());
		}
		StubCommand* sc = this->getCommand(compound->getCommandPool());
		sc->magic.sendDataTime = wns::simulator::getEventScheduler()->getTime();
                sc->magic.sequenceNumber = sequenceNumber;
                sequenceNumber++;
	}
	StubBase::doSendData(compound);
}

void
Stub::doOnData(const CompoundPtr& compound)
{
	if (compound->getCommandPool() && this->registeredAtProxy())
	{
		if (this->getFUN()->getProxy()->commandIsActivated(compound->getCommandPool(), this) == false)
		{
			this->activateCommand(compound->getCommandPool());
		}
		StubCommand* sc = this->getCommand(compound->getCommandPool());
		sc->magic.onDataTime = wns::simulator::getEventScheduler()->getTime();
	}
	StubBase::doOnData(compound);
}


