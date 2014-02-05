/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#include "MultiTypeFactoryTest.hpp"

using namespace wns::module::tests;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( MultiTypeFactoryTest );

void MultiTypeFactoryTest::prepare()
{}

void MultiTypeFactoryTest::cleanup()
{}

void MultiTypeFactoryTest::getterTest()
{
	TestFactory t;
	A* a = t.get<A*>("A");
	B* b = t.get<B*>("B");
	C* c = t.get<C*>("C");
	A* c_a = t.get<A*>("C_A");
	CPPUNIT_ASSERT_EQUAL( 1, a->test() );
	CPPUNIT_ASSERT_EQUAL( 2, b->test() );
	CPPUNIT_ASSERT_EQUAL( 3, c->test() );
	CPPUNIT_ASSERT_EQUAL( 3, c_a->test() );
	delete a;
	delete b;
	delete c;
	delete c_a;
}

void MultiTypeFactoryTest::wrongType()
{
	TestFactory t;
	CPPUNIT_ASSERT_THROW( t.get<A*>("B"), TestFactory::BadCast);
}

