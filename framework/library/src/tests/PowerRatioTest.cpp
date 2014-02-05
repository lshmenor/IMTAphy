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

#include <WNS/tests/PowerRatioTest.hpp>

using namespace wns;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( Power_RatioTest );

void Power_RatioTest::setUp()
{
 	p_res = Power();
	p1.set_mW(1.0);
 	p1 = Power();
	p1.set_mW(1.0);
	p2 = Power();
	p2.set_mW(10.0);
	p3 = Power();
	p3.set_mW(100.0);

	r_res = Ratio();
	r1 = Ratio();
	r1.set_dB(0.0);
	r2 = Ratio();
	r2.set_dB(10.0);
	r3 = Ratio();
	r3.set_dB(20.0);
}

void Power_RatioTest::tearDown()
{
}

void Power_RatioTest::testArithmeticPowerOperators()
{
	p_res.set_mW(1.0);
	p1.set_mW(9.0);
	p_res += p1;
	CPPUNIT_ASSERT( ( p_res == p2) );
	p1.set_mW(1.0);

	p_res.set_mW(11.0);
	p_res -= p2;
	CPPUNIT_ASSERT( ( p_res == p1) );

	p_res.set_mW(1.0);
	p1.set_mW(9.0);
	p_res = p_res + p1;
	CPPUNIT_ASSERT( ( p_res == p2) );
	p1.set_mW(1.0);

	p_res.set_mW(11.0);
	p_res = p_res - p2;
	CPPUNIT_ASSERT( ( p_res == p1) );

	p_res.set_mW(1.0);
	p_res += r2;
	CPPUNIT_ASSERT( ( p_res == p2) );

	p_res.set_mW(10.0);
	p_res -= r2;
	CPPUNIT_ASSERT( ( p_res == p1) );

	r_res = p2/p1;
	CPPUNIT_ASSERT( (r_res == r2) );

	p_res = p1 * (double)10;
	CPPUNIT_ASSERT( ( p_res == p2) );
	p_res = p1 * (float)10;
	CPPUNIT_ASSERT( ( p_res == p2) );
	p_res = p1 * (long int)10;
	CPPUNIT_ASSERT( ( p_res == p2) );
	p_res = p1 * (unsigned long int)10;
	CPPUNIT_ASSERT( ( p_res == p2) );

	p_res.set_mW(1.0);
	p_res *= (double)10;
	CPPUNIT_ASSERT( ( p_res == p2) );
	p_res.set_mW(1.0);
	p_res *= (float)10;
	CPPUNIT_ASSERT( ( p_res == p2) );
	p_res.set_mW(1.0);
	p_res *= (long int)10;
	CPPUNIT_ASSERT( ( p_res == p2) );
	p_res.set_mW(1.0);
	p_res *= (unsigned long int)10;
	CPPUNIT_ASSERT( ( p_res == p2) );
	p_res.set_mW(1.0);

	p_res = p2 / (double)10;
	CPPUNIT_ASSERT( ( p_res == p1) );
	p_res = p2 / (float)10;
	CPPUNIT_ASSERT( ( p_res == p1) );
	p_res = p2 / (long int)10;
	CPPUNIT_ASSERT( ( p_res == p1) );
	p_res = p2 / (unsigned long int)10;
	CPPUNIT_ASSERT( ( p_res == p1) );

	p3 = p1 + p2;
	CPPUNIT_ASSERT( (p3.get_mW() == 11.0) );
	p3 += p1;
	CPPUNIT_ASSERT( (p3.get_mW() == 12.0) );
	p3 = p2 - p1;
	CPPUNIT_ASSERT( (p3.get_mW() == 9.0) );
	p3 -= p1;
	CPPUNIT_ASSERT( (p3.get_mW() == 8.0) );
	p3 = p1;
	CPPUNIT_ASSERT( (p3.get_mW() == 1.0) );
}

void Power_RatioTest::testBooleanPowerOperators()
{
	CPPUNIT_ASSERT( !(p1 == p2) );
	CPPUNIT_ASSERT( (p1 < p2) );
	CPPUNIT_ASSERT( (p1 <= p2) );
	CPPUNIT_ASSERT( (p1 <= p1) );
	CPPUNIT_ASSERT( (p2 > p1) );
	CPPUNIT_ASSERT( (p2 >= p1) );
	CPPUNIT_ASSERT( (p2 >= p2) );
	CPPUNIT_ASSERT( (p2 != p1) );
}

void Power_RatioTest::testArithmeticRatioOperators()
{
	r3 = r1 + r2;
	CPPUNIT_ASSERT( (r3.get_dB() == 10.0) );
	r3.set_dB(20.0);
	r3 += r2;
	CPPUNIT_ASSERT( (r3.get_dB() == 30.0) );
	r3 -= r2;
	CPPUNIT_ASSERT( (r3.get_dB() == 20.0) );
	r3 = r2 - r1;
	CPPUNIT_ASSERT( (r3.get_dB() == 10.0) );
	r3 = r1;
	CPPUNIT_ASSERT( (r3.get_dB() == 0.0) );
}
void Power_RatioTest::testBooleanRatioOperators()
{
	CPPUNIT_ASSERT( !(r1 == r2) );
	CPPUNIT_ASSERT( (r1 < r2) );
	CPPUNIT_ASSERT( (r1 <= r2) );
	CPPUNIT_ASSERT( (r1 <= r1) );
	CPPUNIT_ASSERT( (r2 > r1) );
	CPPUNIT_ASSERT( (r2 >= r1) );
	CPPUNIT_ASSERT( (r2 >= r2) );
	CPPUNIT_ASSERT( (r2 != r1) );
}
void Power_RatioTest::testPowerMemberFunctions()
{
	CPPUNIT_ASSERT( ( p1.get_mW() == 1.0 ) );
	CPPUNIT_ASSERT( ( p1.get_dBm() == 0.0 ) );
	CPPUNIT_ASSERT( ( p2.get_mW() == 10.0 ) );
	CPPUNIT_ASSERT( ( p2.get_dBm() == 10.0 ) );
	CPPUNIT_ASSERT( ( p3.get_mW() == 100.0 ) );
	CPPUNIT_ASSERT( ( p3.get_dBm() == 20.0 ) );
}

void Power_RatioTest::testRatioMemberFunctions()
{
	CPPUNIT_ASSERT( r1.get_factor() == 1.0 );
	CPPUNIT_ASSERT( r1.get_dB() == 0.0 );
	CPPUNIT_ASSERT( r2.get_factor() == 10.0 );
	CPPUNIT_ASSERT( r2.get_dB() == 10.0 );
	CPPUNIT_ASSERT( r3.get_factor() == 100.0 );
	CPPUNIT_ASSERT( r3.get_dB() == 20.0 );
}

void Power_RatioTest::testPowerIstreamFunctions()
{
	Power p;
	stringstream str;
	str << "30.0 dBm ";
	str >> p;
	CPPUNIT_ASSERT( p.get_dBm()==30.0 );

	stringstream str2;
	str2 << "30.0 mW ";
	str2 >> p;
	CPPUNIT_ASSERT( p.get_mW()==30.0 );

	stringstream str3;
	str3 << "30.0 W ";
	str3 >> p;
	CPPUNIT_ASSERT( p.get_mW()==30000.0 );
}

void Power_RatioTest::testRatioIstreamFunctions()
{
	Ratio r;
	stringstream str;
	str << "30.0 dB";
	str >> r;
	CPPUNIT_ASSERT( r.get_dB()==30.0 );
}



