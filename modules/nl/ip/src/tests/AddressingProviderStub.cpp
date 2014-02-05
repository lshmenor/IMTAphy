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

#include <IP/tests/AddressingProviderStub.hpp>

#include <cstdlib>
#include <functional>

using namespace ip::tests;

STATIC_FACTORY_REGISTER_WITH_CREATOR(AddressingProviderStub, FunctionalUnit, "ip.tests.AddressingProviderStub", FUNConfigCreator);

AddressingProviderStub::AddressingProviderStub(fun::FUN* fuNet, const wns::pyconfig::View& ) :
		CommandTypeSpecifier<AddressingProviderCommand>(fuNet),
		HasReceptor<>(),
		HasConnector<>(),
		HasDeliverer<>(),
		wns::Cloneable<AddressingProviderStub>(),

		received(ContainerType()),
		sent(ContainerType()),
		wakeupCalled(),
		onFUNCreatedCalled(),
		stepping(false),
		accepting(true),
		addToPCISize(0),
		addToPDUSize(0)
{
} // AddressingProviderStub


AddressingProviderStub::~AddressingProviderStub()
{
	flush();
} // ~AddressingProviderStub


void
AddressingProviderStub::setStepping(bool _stepping)
{
	stepping = _stepping;
} // setStepping


void
AddressingProviderStub::setSizes(int _addToPCISize, int _addToPDUSize)
{
	addToPCISize = _addToPCISize;
	addToPDUSize = _addToPDUSize;
} // setSizes


void
AddressingProviderStub::flush()
{
	received.clear();
	sent.clear();
} // flush


void
AddressingProviderStub::open(bool wakeup)
{
	accepting = true;
	if(wakeup)
		getReceptor()->wakeup();
} // open


void
AddressingProviderStub::close()
{
	accepting = false;
} // close


void
AddressingProviderStub::step()
{
	stepping = true;
	open();
	stepping = false;
} // step


bool
AddressingProviderStub::doIsAccepting(const CompoundPtr& compound) const
{
	if(!accepting)
		return false;

	if(getConnector()->size() == 0)
		return true;

	return getConnector()->hasAcceptor(compound);
} // isAccepting


void
AddressingProviderStub::doSendData(const CompoundPtr& sdu)
{
	assure(sdu != CompoundPtr(), "sendData called with an invalid sdu.");

	if(sdu->getCommandPool()
	   && registeredAtProxy())
		activateCommand(sdu->getCommandPool());

    AddressingProviderCommand* apc = getCommand(sdu->getCommandPool());
	apc->setSourceAddress(source);
	apc->setDestinationAddress(destination);

	if(stepping)
		close();

	sent.push_back(sdu);

	if(getConnector()->size()) {
		getConnector()->getAcceptor(sdu)->sendData(sdu);
	}

	assure(integrityCheck(), "Integrity check failed.");
} // doSendData


void
AddressingProviderStub::doOnData(const CompoundPtr& compound)
{
	assure(compound != CompoundPtr(), "Not a valid PDU.");

	received.push_back(compound);

	if(getReceptor()->size()) {
		getDeliverer()->getAcceptor(compound)->onData(compound);
	}

	assure(integrityCheck(), "Integrity check failed.");
} // doOnData


void
AddressingProviderStub::doWakeup()
{
	++wakeupCalled;

	getReceptor()->wakeup();
} // wakeup

void
AddressingProviderStub::calculateSizes(const CommandPool* commandPool, Bit& commandPoolSize, Bit& dataSize) const
{
	getFUN()->calculateSizes(commandPool, commandPoolSize, dataSize, this);

	commandPoolSize += addToPCISize;
	dataSize += addToPDUSize;
} // calculateSizes

void
AddressingProviderStub::onFUNCreated()
{
	++onFUNCreatedCalled;
} // onFUNCreated

bool
AddressingProviderStub::integrityCheck()
{
	for(ContainerType::iterator i = received.begin(); i != received.end(); ++i)
		if((*i)->getRef() < 1)
			return false;

	for(ContainerType::iterator i = sent.begin(); i != sent.end(); ++i)
		if((*i)->getRef() < 1)
			return false;

	return true;
} // integrityCheck



