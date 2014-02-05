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

#include <DLL/tests/FUTestBase.hpp>
#include <DLL/tests/LayerStub.hpp>
#include <DLL/UpperConvergence.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/node/tests/Stub.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/module/Module.hpp>

using namespace dll::tests;

FUTestBase::FUTestBase()
  : stationManager(NULL),
    upperConvergence(NULL),
    node(NULL),
    layer(NULL),
    fun(NULL),
    upperStub(NULL),
    lowerStub(NULL)
{
} // FUTestBase

FUTestBase::~FUTestBase()
{
    if (layer != NULL) delete layer;
    if (node != NULL) delete node;
    if (stationManager != NULL) delete stationManager;
} // ~FUTestBase

wns::ldk::tools::StubBase*
FUTestBase::getLowerStub() const
{
    return lowerStub;
} // getLowerStub

wns::ldk::tools::StubBase*
FUTestBase::getUpperStub() const
{
    return upperStub;
} // getUpperStub

void
FUTestBase::prepare()
{
    accepted = 0;
    received = 0;

    stationManager = new dll::StationManager();

    //Create Layer and FUN
    node = new wns::node::tests::Stub();
    wns::pyconfig::Parser pycoL;

    std::string stationType = this->getStationType();
    std::string stationID   = this->getStationID();
    std::string ring        = this->getRing();

    pycoL.loadString("from dll.Layer2 import Layer2\n"
             "import dll.UpperConvergence\n"
             "from wns.Node import Node\n"
             "from wns.FUN import FUN\n"

             "node  = Node('isi_NodeName')\n"
             "layer = Layer2(node,'isi_StationName', None)\n"

             "layer.setStationType('"+ stationType +"')\n"
             "layer.setStationID('" + stationID  +"')\n"
             "layer.setRing('"+ ring +"')\n"

             "layer.fun = FUN()\n"
             "layer.fun.add(dll.UpperConvergence.No(functionalUnitName='"+getUpperConvergenceName()+"',commandName='"+getUpperConvergenceName()+"'))\n"
             "layer.upperConvergenceName = '"+getUpperConvergenceName()+"'\n"
            );
    wns::pyconfig::View lView(pycoL, "layer");

    layer = new dll::tests::LayerStub();
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
} // catchExceptionSetUp

wns::ldk::tools::StubBase*
FUTestBase::newUpperStub()
{
    wns::pyconfig::Parser emptyConfig;
    return new wns::ldk::tools::Stub(fun, emptyConfig);
} // newUpperStub

wns::ldk::tools::StubBase*
FUTestBase::newLowerStub()
{
    wns::pyconfig::Parser emptyConfig;
    return new wns::ldk::tools::Stub(fun, emptyConfig);
} // newLowerStub

void
FUTestBase::cleanup()
{
    tearDownTestFUs();

    if (layer != NULL) {
    delete layer;
    layer = NULL;
    }
    if (node != NULL) {
    delete node;
    node = NULL;
    }
    if (stationManager != NULL) {
    delete stationManager;
    stationManager = NULL;
    }
} // catchExceptionTearDown

wns::ldk::CompoundPtr
FUTestBase::newFakeCompound()
{
    return wns::ldk::CompoundPtr(new wns::ldk::Compound(getFUN()->createCommandPool(), wns::ldk::helper::FakePDUPtr()));
} // newFakeCompound

wns::ldk::CompoundPtr
FUTestBase::sendCompound(const wns::ldk::CompoundPtr& compound)
{
    unsigned int initialCount = compoundsSent();
    if (upperStub->isAccepting(compound))
    upperStub->sendData(compound);
    ++accepted;
    if (compoundsSent() > initialCount)
    return lowerStub->sent.back();
    else
    return wns::ldk::CompoundPtr();
} // sendCompound

wns::ldk::CompoundPtr
FUTestBase::receiveCompound(const wns::ldk::CompoundPtr& compound)
{
    unsigned int initialCount = compoundsDelivered();
    lowerStub->onData(compound);
    ++received;
    if (compoundsDelivered() > initialCount)
    return upperStub->received.back();
    else
    return wns::ldk::CompoundPtr();
} // receiveCompound

unsigned int
FUTestBase::compoundsAccepted() const
{
    return accepted;
} // compoundsAccepted

unsigned int
FUTestBase::compoundsSent() const
{
    return lowerStub->sent.size();
} // compoundsSent

unsigned int
FUTestBase::compoundsReceived() const
{
    return received;
} // compoundsReceived

unsigned int
FUTestBase::compoundsDelivered() const
{
    return upperStub->received.size();
} // compoundsDelivered

wns::ldk::fun::FUN*
FUTestBase::getFUN()
{
    return fun;
} // getFUN

