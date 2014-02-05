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

#include <WNS/tests/TimeWeightedAverageTest.hpp>
#include <limits>

using namespace std;
using namespace wns;

CPPUNIT_TEST_SUITE_REGISTRATION( TimeWeightedAverageTest );

void TimeWeightedAverageTest::setUp()
{
	c = new Clock;
	average = TimeWeightedAverage<double, Clock>(c);
	avgPow  = TimeWeightedAverage<Power, Clock>(c);

}

void TimeWeightedAverageTest::tearDown()
{
	delete c;
}

void TimeWeightedAverageTest::testPutAndGet()
{
	CPPUNIT_ASSERT( average.get() == 0 );
	c->time=1;
	average.put(4);
	CPPUNIT_ASSERT( average.get() == 4 );
	c->time=3;
	average.put(7);
	CPPUNIT_ASSERT( average.get() == 6 );
}

void TimeWeightedAverageTest::testReset()
{
	average.reset();
	CPPUNIT_ASSERT( average.get() == 0 );
	c->time = 0.2;
	average.put(5.5);
	CPPUNIT_ASSERT( average.get() == 5.5 );
	average.reset();
	CPPUNIT_ASSERT( average.get() == 0 );

	c->time = 0.8;
	average.put(7.5);
	CPPUNIT_ASSERT( fabs(average.get() - 7.5) < numeric_limits<double>::epsilon()*10 );
}

void TimeWeightedAverageTest::testPutAndGetPower()
{
	CPPUNIT_ASSERT( avgPow.get() == Power::from_mW(0) );
	c->time = 1.0;
	avgPow.put(Power::from_mW(4));
	CPPUNIT_ASSERT( avgPow.get() == Power::from_mW(4) );

	c->time = 3.0;
	avgPow.put(Power::from_mW(7));
	CPPUNIT_ASSERT_DOUBLES_EQUAL( avgPow.get().get_mW(), 6, 0.001 );
}

void TimeWeightedAverageTest::testResetPower()
{
	avgPow.reset();
	CPPUNIT_ASSERT( avgPow.get() == Power::from_mW(0) );
	c->time=0.2;
	avgPow.put(Power::from_mW(5.5));
	CPPUNIT_ASSERT( avgPow.get() == Power::from_mW(5.5) );
	avgPow.reset();
	CPPUNIT_ASSERT( avgPow.get() == Power::from_mW(0) );
	c->time=0.8;
	avgPow.put(Power::from_mW(7.5));
	CPPUNIT_ASSERT( fabs((avgPow.get() - Power::from_mW(7.5)).get_mW()) < numeric_limits<double>::epsilon()*10 );
}


