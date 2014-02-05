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

#include "MainGenericTest.hpp"

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/FunctionalUnit.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/tools/Stub.hpp>

#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/tools/Synchronizer.hpp>
#include <WNS/ldk/tools/Stub.hpp>

#include <WNS/ldk/FlowSeparator.hpp>

#include <WNS/ldk/utils.hpp>

#include <iostream>

using namespace wns::ldk;
using namespace wns::ldk::fun;


CPPUNIT_TEST_SUITE_REGISTRATION( MainGenericTest );

void
MainGenericTest::prepare()
{
    this->layer = new tests::LayerStub();
    this->fuNet = new fun::Main(this->layer);
} // setUp


void
MainGenericTest::cleanup()
{
    delete fuNet;
    delete layer;
} // tearDown


void
MainGenericTest::testFunctionalUnit()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.FUN import FUN, Node\n"
        "from openwns.Buffer import Bounded\n"
        "from openwns.Tools import Synchronizer\n"
        "fuNet = FUN()\n"
        "n1 = Node('mon', Bounded())\n"
        "n2 = Node('dieu', Synchronizer())\n"
        "fuNet.setFunctionalUnits(n1, n2)\n"
        );

    pyconfig::View funConfig(config, "fuNet");
    configureFUN(this->fuNet, funConfig);

    wns::ldk::buffer::Buffer* mon = this->fuNet->findFriend<wns::ldk::buffer::Buffer*>("mon");
    wns::ldk::tools::Synchronizer* dieu = this->fuNet->findFriend<wns::ldk::tools::Synchronizer*>("dieu");

    CPPUNIT_ASSERT( mon );
    CPPUNIT_ASSERT( dieu );
} // testFunctionalUnit


void
MainGenericTest::testConnect()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.FUN import FUN, Node\n"
        "from openwns.Buffer import Bounded\n"
        "from openwns.Tools import Synchronizer\n"
        "fuNet = FUN()\n"
        "n1 = Node('mon', Bounded())\n"
        "n2 = Node('dieu', Synchronizer())\n"
        "fuNet.setFunctionalUnits(n1, n2)\n"
        "n1.connect(n2)\n"
        );

    pyconfig::View funConfig(config, "fuNet");
    configureFUN(this->fuNet, funConfig);

    wns::ldk::buffer::Buffer* mon = this->fuNet->findFriend<wns::ldk::buffer::Buffer*>("mon");
    wns::ldk::tools::Synchronizer* dieu = this->fuNet->findFriend<wns::ldk::tools::Synchronizer*>("dieu");
    CPPUNIT_ASSERT( mon->getReceptor()->size() == 0 );
    CPPUNIT_ASSERT( mon->getConnector()->size() == 1 );
    CPPUNIT_ASSERT( mon->getDeliverer()->size() == 0 );

    CPPUNIT_ASSERT( dieu->getReceptor()->size() == 1 );
    CPPUNIT_ASSERT( dieu->getConnector()->size() == 0 );
    CPPUNIT_ASSERT( dieu->getDeliverer()->size() == 1 );

    CPPUNIT_ASSERT( mon->getConnector()->getAcceptor(CompoundPtr()) == dieu );

    CPPUNIT_ASSERT( dieu->getDeliverer()->getAcceptor(CompoundPtr()) == mon );
} // testConnect


void
MainGenericTest::testUpConnect()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.FUN import FUN, Node\n"
        "from openwns.Buffer import Bounded\n"
        "from openwns.Tools import Synchronizer\n"
        "fuNet = FUN()\n"
        "n1 = Node('mon', Bounded())\n"
        "n2 = Node('dieu', Synchronizer())\n"
        "fuNet.setFunctionalUnits(n1, n2)\n"
        "n1.upConnect(n2)\n"
        );

    pyconfig::View funConfig(config, "fuNet");
    configureFUN(this->fuNet, funConfig);

    wns::ldk::buffer::Buffer* mon = this->fuNet->findFriend<wns::ldk::buffer::Buffer*>("mon");
    wns::ldk::tools::Synchronizer* dieu = this->fuNet->findFriend<wns::ldk::tools::Synchronizer*>("dieu");
    CPPUNIT_ASSERT( mon->getReceptor()->size() == 0 );
    CPPUNIT_ASSERT( mon->getConnector()->size() == 0 );
    CPPUNIT_ASSERT( mon->getDeliverer()->size() == 0 );

    CPPUNIT_ASSERT( dieu->getReceptor()->size() == 0 );
    CPPUNIT_ASSERT( dieu->getConnector()->size() == 0 );
    CPPUNIT_ASSERT( dieu->getDeliverer()->size() == 1 );

    CPPUNIT_ASSERT( dieu->getDeliverer()->getAcceptor(CompoundPtr()) == mon );
} // testUpConnect


void
MainGenericTest::testDownConnect()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.FUN import FUN, Node\n"
        "from openwns.Buffer import Bounded\n"
        "from openwns.Tools import Synchronizer\n"
        "fuNet = FUN()\n"
        "n1 = Node('mon', Bounded())\n"
        "n2 = Node('dieu', Synchronizer())\n"
        "fuNet.setFunctionalUnits(n1, n2)\n"
        "n1.downConnect(n2)\n"
        );

    pyconfig::View funConfig(config, "fuNet");
    configureFUN(this->fuNet, funConfig);

    wns::ldk::buffer::Buffer* mon = this->fuNet->findFriend<wns::ldk::buffer::Buffer*>("mon");
    wns::ldk::tools::Synchronizer* dieu = this->fuNet->findFriend<wns::ldk::tools::Synchronizer*>("dieu");
    CPPUNIT_ASSERT( mon->getReceptor()->size() == 0 );
    CPPUNIT_ASSERT( mon->getConnector()->size() == 1 );
    CPPUNIT_ASSERT( mon->getDeliverer()->size() == 0 );

    CPPUNIT_ASSERT( dieu->getReceptor()->size() == 1 );
    CPPUNIT_ASSERT( dieu->getConnector()->size() == 0 );
    CPPUNIT_ASSERT( dieu->getDeliverer()->size() == 0 );

    CPPUNIT_ASSERT( mon->getConnector()->getAcceptor(CompoundPtr()) == dieu );
} // testDownConnect


void
MainGenericTest::testFindFriends()
{
    pyconfig::Parser config;
    config.loadString(
        "from openwns.FUN import FUN, Node\n"
        "from openwns.Tools import Stub\n"
        "fuNet = FUN()\n"
        "n1 = Node('mon', Stub())\n"
        "n2 = Node('dieu', Stub())\n"
        "fuNet.setFunctionalUnits(n1, n2)\n"
        "n1.connect(n2)\n"
        );

    pyconfig::View funConfig(config, "fuNet");
    configureFUN(this->fuNet, funConfig);

    CPPUNIT_ASSERT( this->fuNet->findFriend<wns::ldk::tools::Stub*>("mon")->onFUNCreatedCalled == 0);
    CPPUNIT_ASSERT( this->fuNet->findFriend<wns::ldk::tools::Stub*>("dieu")->onFUNCreatedCalled == 0);
    this->fuNet->onFUNCreated();
    CPPUNIT_ASSERT( this->fuNet->findFriend<wns::ldk::tools::Stub*>("mon")->onFUNCreatedCalled == 1);
    CPPUNIT_ASSERT( this->fuNet->findFriend<wns::ldk::tools::Stub*>("dieu")->onFUNCreatedCalled == 1);
} // testFindFriends


namespace fungenerictest
    {
    class OneFlowBuilder;
    class OneFlow :
        public Key
    {
    public:
        OneFlow(const OneFlowBuilder* /* factory */, const CompoundPtr& /* compound */, int /* direction */) {}
        bool operator<(const Key& /* other */) const
        {
            return false;
        }

        std::string str() const
        {
            return "false";
        }
    };

    class OneFlowBuilder :
        public KeyBuilder
    {
    public:
        OneFlowBuilder(const fun::FUN* /* fuNet */, const wns::pyconfig::View& /* config */) {}
        virtual void onFUNCreated() {}
        virtual ConstKeyPtr operator () (const CompoundPtr& compound, int direction) const
        {
            return ConstKeyPtr(new OneFlow(this, compound, direction));
        }
    };

    STATIC_FACTORY_REGISTER_WITH_CREATOR(OneFlowBuilder, KeyBuilder,
                                             "wns.fungenerictest.OneFlow", FUNConfigCreator);
}

void
MainGenericTest::testFlowSeparator()
{
    pyconfig::Parser config;
    config.loadString(
    "from openwns.FUN import FUN, Node\n"
    "from openwns.FlowSeparator import FlowSeparator, PrototypeCreator, OneFlow\n"
    "from openwns.Tools import Stub\n"
    "from openwns.Tools import Synchronizer\n"
    "fuNet = FUN()\n"
    "upper = Node('upper', Stub())\n"
    "separator = Node('seppel', FlowSeparator(OneFlow, PrototypeCreator('synchronizer', Synchronizer())))\n"
    "lower = Node('lower', Stub())\n"
    "fuNet.setFunctionalUnits(upper, separator, lower)\n"
    "upper.connect(separator)\n"
    "separator.connect(lower)\n"
    );

    pyconfig::View funConfig(config, "fuNet");
    configureFUN(fuNet, funConfig);

    tools::Stub* upper = fuNet->findFriend<tools::Stub*>("upper");
    CPPUNIT_ASSERT(upper);

    FlowSeparator* separator = fuNet->findFriend<FlowSeparator*>("seppel");
    CPPUNIT_ASSERT(separator);

    tools::Stub* lower = fuNet->findFriend<tools::Stub*>("lower");
    CPPUNIT_ASSERT(lower);

    tools::Synchronizer* synchronizer = fuNet->findFriend<tools::Synchronizer*>("synchronizer");
    CPPUNIT_ASSERT(synchronizer);

    CPPUNIT_ASSERT(separator->size() == 0);
    CPPUNIT_ASSERT(lower->sent.size() == 0);

    lower->close();
    upper->sendData(CompoundPtr(new Compound(fuNet->getProxy()->createCommandPool())));
    CPPUNIT_ASSERT(separator->size() == 1);
    CPPUNIT_ASSERT(lower->sent.size() == 0);

    lower->open();
    CPPUNIT_ASSERT(separator->size() == 1);
    CPPUNIT_ASSERT(lower->sent.size() == 1);
} // testFlowSeparator


