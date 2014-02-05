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

#include <WNS/tests/RoundRobinTest.hpp>
#include <WNS/RoundRobin.hpp>

using namespace wns::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( RoundRobinTest );

void RoundRobinTest::setUp()
{
	rr = RoundRobin<int>();
	rr.add(1);
	rr.add(2);
	rr.add(3);
	rr.add(4);
}

void RoundRobinTest::tearDown()
{
}

void RoundRobinTest::empty()
{
	rr = RoundRobin<int>();
	CPPUNIT_ASSERT( rr.empty() );
}

void RoundRobinTest::notEmpty()
{
	CPPUNIT_ASSERT( !rr.empty() );
}

void RoundRobinTest::remove()
{
	rr.remove(2);
	CPPUNIT_ASSERT_EQUAL( static_cast<long int>(3), rr.size() );
}

void RoundRobinTest::size()
{
	CPPUNIT_ASSERT_EQUAL( static_cast<long int>(4), rr.size() );
}

void RoundRobinTest::startRound()
{
	rr.startRound();
	CPPUNIT_ASSERT_EQUAL( 1, rr.next() );
}

void RoundRobinTest::next()
{
	rr.startRound();
	rr.next();
	CPPUNIT_ASSERT_EQUAL( 2, rr.next() );
}

void RoundRobinTest::hasNext()
{
	rr.startRound();
	rr.next();
	CPPUNIT_ASSERT( rr.hasNext() );
	rr.next();
	CPPUNIT_ASSERT( rr.hasNext() );
	rr.next();
	CPPUNIT_ASSERT( rr.hasNext() );
	rr.next();
	CPPUNIT_ASSERT( !rr.hasNext() );
}

void RoundRobinTest::endRound()
{
	rr.startRound();
	rr.next();
	rr.next();
	rr.endRound();
	rr.startRound();
	CPPUNIT_ASSERT_EQUAL( 3, rr.next() );
}

void RoundRobinTest::addAfterRound()
{
	rr.startRound();
	rr.next();
	rr.endRound();
	rr.add(5);
	rr.startRound();
	rr.next();
	rr.next();
	rr.next();
	rr.next();
	CPPUNIT_ASSERT_EQUAL( 5, rr.next() );
}

void RoundRobinTest::removeAfterRoundCursorBeforeElement()
{
	rr.startRound();
	rr.next();
	rr.endRound();
	rr.remove(3);
	rr.startRound();
	CPPUNIT_ASSERT_EQUAL( 2, rr.next() );
	CPPUNIT_ASSERT_EQUAL( 4, rr.next() );
	CPPUNIT_ASSERT_EQUAL( 1, rr.next() );
}

void RoundRobinTest::removeAfterRoundCursorOnElement()
{
	rr.startRound();
	rr.next();
	rr.next();
	rr.endRound();
	rr.remove(3);
	rr.startRound();
	CPPUNIT_ASSERT_EQUAL( 4, rr.next() );
	CPPUNIT_ASSERT_EQUAL( 1, rr.next() );
	CPPUNIT_ASSERT_EQUAL( 2, rr.next() );
}

void RoundRobinTest::removeAfterRoundCursorAfterElement()
{
	rr.startRound();
	rr.next();
	rr.next();
	rr.next();
	rr.endRound();
	rr.remove(3);
	rr.startRound();
	CPPUNIT_ASSERT_EQUAL( 4, rr.next() );
	CPPUNIT_ASSERT_EQUAL( 1, rr.next() );
	CPPUNIT_ASSERT_EQUAL( 2, rr.next() );
}

void RoundRobinTest::endUnusedRound()
{
	rr.startRound();
	rr.next();
	rr.next();
	rr.next();
	rr.next();
	rr.endRound();
	rr.startRound();
	CPPUNIT_ASSERT_EQUAL( 1, rr.next() );
}

void RoundRobinTest::whileUsage()
{
	rr.startRound();
	int i = 1;
	while(rr.hasNext())
	{
		int result = rr.next();
		CPPUNIT_ASSERT_EQUAL( i++, result );
	}
	rr.endRound();
	rr.startRound();
	i = 1;
	while(rr.hasNext())
	{
		int result = rr.next();
		CPPUNIT_ASSERT_EQUAL( i++, result );
	}
}

void RoundRobinTest::startAndEndRound()
{
	rr.startRound();
	rr.endRound();
	rr.startRound();
	CPPUNIT_ASSERT_EQUAL( 1, rr.next() );
}

void
RoundRobinTest::nextAfterConstructor()
{
	rr.next();
}

void
RoundRobinTest::hasNextAfterContructor()
{
	rr.hasNext();
}

void
RoundRobinTest::endRoundAfterConstructor()
{
	rr.endRound();
}

void
RoundRobinTest::nextWithoutStartRound()
{
	rr.startRound();
	rr.next();
	rr.endRound();
	rr.next();
}

void
RoundRobinTest::hasNextWithoutStartRound()
{
	rr.startRound();
	rr.next();
	rr.endRound();
	rr.hasNext();
}

void
RoundRobinTest::endRoundWithoutStartRound()
{
	rr.startRound();
	rr.next();
	rr.endRound();
	rr.endRound();
}

void
RoundRobinTest::startRoundWithoutEndRound()
{
	rr.startRound();
	rr.next();
	rr.startRound();
}

void
RoundRobinTest::nextAlthoughNoNext()
{
	rr.next();
	rr.next();
	rr.next();
	rr.next();
	rr.next();
}

void
RoundRobinTest::addWhileInRound()
{
	rr.startRound();
	rr.add(6);
}

void
RoundRobinTest::removeWhileInRound()
{
	rr.startRound();
	rr.remove(3);
}

void
RoundRobinTest::removeNonExisting()
{
	rr.remove(5);
}

void
RoundRobinTest::clear()
{
	rr.clear();
	CPPUNIT_ASSERT_EQUAL(static_cast<long int>(0), rr.size());
}


void
RoundRobinTest::cancelRound()
{
	rr.startRound();
	const int it = rr.next();
	rr.next();
	rr.next();

	rr.cancelRound();
	rr.startRound();
	CPPUNIT_ASSERT_EQUAL(it, rr.next());
}

void
RoundRobinTest::current()
{
	CPPUNIT_ASSERT_EQUAL( 1, rr.current() );

	rr.startRound();
	CPPUNIT_ASSERT_EQUAL( 1, rr.next() );
	CPPUNIT_ASSERT_EQUAL( 2, rr.current() );
	CPPUNIT_ASSERT_EQUAL( 2, rr.next() );
	CPPUNIT_ASSERT_EQUAL( 3, rr.current() );
	CPPUNIT_ASSERT_EQUAL( 3, rr.next() );
	CPPUNIT_ASSERT_EQUAL( 4, rr.current() );
	rr.cancelRound();

	CPPUNIT_ASSERT_EQUAL( 1, rr.current() );

	rr.startRound();
	CPPUNIT_ASSERT_EQUAL( 1, rr.next() );
	CPPUNIT_ASSERT_EQUAL( 2, rr.current() );
	CPPUNIT_ASSERT_EQUAL( 2, rr.next() );
	CPPUNIT_ASSERT_EQUAL( 3, rr.current() );
	CPPUNIT_ASSERT_EQUAL( 3, rr.next() );
	rr.endRound();

	CPPUNIT_ASSERT_EQUAL( 4, rr.current() );

}

