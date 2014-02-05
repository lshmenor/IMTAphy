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
#include <WNS/ldk/tests/FUTestBase.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/pyconfig/Parser.hpp>

using namespace wns::ldk::tests;


FUTestBase::FUTestBase()
    : layer(NULL),
      fun(NULL),
      upperStub(NULL),
      lowerStub(NULL)
{
}


FUTestBase::~FUTestBase()
{
    if (fun != NULL) delete fun;
    if (layer != NULL) delete layer;
}


wns::ldk::tools::Stub*
FUTestBase::getLowerStub() const
{
    return lowerStub;
}


wns::ldk::tools::Stub*
FUTestBase::getUpperStub() const
{
    return upperStub;
}


simTimeType
FUTestBase::getSojurnTime(const CompoundPtr& compound) const
{
	wns::ldk::tools::Stub* us = this->getUpperStub();
	wns::ldk::tools::Stub* ls = this->getLowerStub();
	wns::ldk::tools::StubCommand* uc = us->getCommand(compound->getCommandPool());
	wns::ldk::tools::StubCommand* lc = ls->getCommand(compound->getCommandPool());
	assure(lc->magic.sendDataTime >= 0.0, "invalid simulation time");
	assure(uc->magic.sendDataTime >= 0.0, "invalid simulation time");
	assure(uc->magic.sendDataTime <= lc->magic.sendDataTime,
	       "Travelling back in time! "
	       "Seems the compound visited the lower stub before the upper "
	       "stub in the outgoing data flow");
	return lc->magic.sendDataTime - uc->magic.sendDataTime;
}


void
FUTestBase::prepare()
{
    accepted = 0;
    received = 0;

    layer = newLayer();
    fun = new wns::ldk::fun::Main(layer);

    upperStub = newUpperStub();
    lowerStub = newLowerStub();

    fun->addFunctionalUnit("upperStub", upperStub);
    fun->addFunctionalUnit("lowerStub", lowerStub);

    setUpTestFUs();

    upperStub
	->connect(getUpperTestFU());
    getLowerTestFU()
	->connect(lowerStub);

    fun->onFUNCreated();
}


wns::ldk::tools::Stub*
FUTestBase::newUpperStub()
{
    wns::pyconfig::Parser emptyConfig;
    return new tools::Stub(fun, emptyConfig);
}


wns::ldk::tools::Stub*
FUTestBase::newLowerStub()
{
    wns::pyconfig::Parser emptyConfig;
    return new tools::Stub(fun, emptyConfig);
}

wns::ldk::ILayer*
FUTestBase::newLayer()
{
    return new wns::ldk::tests::LayerStub();
}

void
FUTestBase::cleanup()
{
    tearDownTestFUs();

    if (fun != NULL) {
	delete fun;
	fun = NULL;
    }
    if (layer != NULL) {
	delete layer;
	layer = NULL;
    }
}


wns::ldk::CompoundPtr
FUTestBase::newFakeCompound()
{
    return CompoundPtr(new Compound(getFUN()->createCommandPool(), helper::FakePDUPtr()));
}


wns::ldk::CompoundPtr
FUTestBase::sendCompound(const CompoundPtr& compound)
{
    unsigned int initialCount = compoundsSent();
    if (upperStub->isAccepting(compound))
	upperStub->sendData(compound);
    ++accepted;
    if (compoundsSent() > initialCount)
	return lowerStub->sent.back();
    else
	return CompoundPtr();
}


wns::ldk::CompoundPtr
FUTestBase::receiveCompound(const CompoundPtr& compound)
{
    unsigned int initialCount = compoundsDelivered();
    lowerStub->onData(compound);
    ++received;
    if (compoundsDelivered() > initialCount)
	return upperStub->received.back();
    else
	return CompoundPtr();
}


unsigned int
FUTestBase::compoundsAccepted() const
{
    return accepted;
}


unsigned int
FUTestBase::compoundsSent() const
{
    return lowerStub->sent.size();
}


unsigned int
FUTestBase::compoundsReceived() const
{
    return received;
}


unsigned int
FUTestBase::compoundsDelivered() const
{
    return upperStub->received.size();
}


wns::ldk::fun::FUN*
FUTestBase::getFUN()
{
    return fun;
}

