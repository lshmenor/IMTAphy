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

#include <WNS/ldk/tests/MultiLinkTest.hpp>



using namespace wns::ldk;
using namespace wns::ldk::tests;



CPPUNIT_TEST_SUITE_REGISTRATION( MultiLinkTest );



void
MultiLinkTest::prepare()
{
        pyconfig::Parser emptyConfig;

        layer = new LayerStub();
        fuNet = new fun::Main(layer);
        fu1 = new tools::Stub(fuNet, emptyConfig);
        fu2 = new tools::Stub(fuNet, emptyConfig);
        ml = new MultiLink<IConnectorReceptacle>();
} // setUp



void
MultiLinkTest::cleanup()
{
        delete layer;
        delete fuNet;
        delete fu1;
        delete fu2;
        delete ml;
} // tearDown



void
MultiLinkTest::testAdd()
{
        CPPUNIT_ASSERT_EQUAL((unsigned long int)(0), ml->size());
        ml->add(fu1);
        CPPUNIT_ASSERT_EQUAL((unsigned long int)(1), ml->size());
        ml->add(fu2);
        CPPUNIT_ASSERT_EQUAL((unsigned long int)(2), ml->size());
} // testAdd



void
MultiLinkTest::testClear()
{
        ml->add(fu1);
        ml->add(fu2);
        ml->clear();
        CPPUNIT_ASSERT_EQUAL((unsigned long int)(0), ml->size());
} // testClear



void
MultiLinkTest::testGet()
{
        ml->add(fu1);
        Link<IConnectorReceptacle>::ExchangeContainer exchange = ml->get();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), exchange.size());
        CPPUNIT_ASSERT(exchange[0] == fu1);
} // testGet



void
MultiLinkTest::testSet()
{
        Link<IConnectorReceptacle>::ExchangeContainer exchange;
        exchange.push_back(fu1);

        ml->set(exchange);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), exchange.size());

        exchange = ml->get();
        CPPUNIT_ASSERT(exchange[0] == fu1);
} // testSet




