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

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Link.hpp>
#include <WNS/ldk/RoundRobinLink.hpp>

#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/CppUnit.hpp>

namespace wns { namespace ldk { namespace tests {

	class RoundRobinLinkTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( RoundRobinLinkTest );
                CPPUNIT_TEST( testAdd );
                CPPUNIT_TEST( testClear );
                CPPUNIT_TEST( testGet );
                CPPUNIT_TEST( testSet );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();

                void testAdd();
                void testClear();
                void testGet();
                void testSet();

	private:
                ILayer* layer;
                fun::FUN* fuNet;
                FunctionalUnit* fu1;
                FunctionalUnit* fu2;
                RoundRobinLink<IConnectorReceptacle>* candidate;
	};

}
}
}


using namespace wns::ldk;
using namespace wns::ldk::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( RoundRobinLinkTest );


void
RoundRobinLinkTest::prepare()
{
        pyconfig::Parser emptyConfig;

        layer = new LayerStub();
        fuNet = new fun::Main(layer);
        fu1 = new tools::Stub(fuNet, emptyConfig);
        fu2 = new tools::Stub(fuNet, emptyConfig);
        candidate = new RoundRobinLink<IConnectorReceptacle>();
} // setUp


void
RoundRobinLinkTest::cleanup()
{
        delete layer;
        delete fuNet;
        delete fu1;
        delete fu2;
        delete candidate;
} // tearDown


void
RoundRobinLinkTest::testAdd()
{
        CPPUNIT_ASSERT_EQUAL((unsigned long int)(0), candidate->size());
        candidate->add(fu1);
        CPPUNIT_ASSERT_EQUAL((unsigned long int)(1), candidate->size());
        candidate->add(fu2);
        CPPUNIT_ASSERT_EQUAL((unsigned long int)(2), candidate->size());
} // testAdd


void
RoundRobinLinkTest::testClear()
{
        candidate->add(fu1);
        candidate->add(fu2);
        candidate->clear();
        CPPUNIT_ASSERT_EQUAL((unsigned long int)(0), candidate->size());
} // testClear


void
RoundRobinLinkTest::testGet()
{
        candidate->add(fu1);
        Link<IConnectorReceptacle>::ExchangeContainer exchange = candidate->get();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), exchange.size());
        CPPUNIT_ASSERT(exchange[0] == fu1);
} // testGet


void
RoundRobinLinkTest::testSet()
{
        Link<IConnectorReceptacle>::ExchangeContainer exchange;
        exchange.push_back(fu1);

        candidate->set(exchange);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), exchange.size());

        exchange = candidate->get();
        CPPUNIT_ASSERT(exchange[0] == fu1);
} // testSet


