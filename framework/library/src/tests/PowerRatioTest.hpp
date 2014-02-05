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

#include <WNS/PowerRatio.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

#ifndef classPower_RatioTest
#define classPower_RatioTest

namespace wns {
	class Power_RatioTest : public CppUnit::TestFixture  {
		CPPUNIT_TEST_SUITE( Power_RatioTest );
		CPPUNIT_TEST( testArithmeticPowerOperators );
		CPPUNIT_TEST( testBooleanPowerOperators );
		CPPUNIT_TEST( testArithmeticRatioOperators );
		CPPUNIT_TEST( testBooleanRatioOperators );
		CPPUNIT_TEST( testPowerMemberFunctions );
		CPPUNIT_TEST( testRatioMemberFunctions );
		CPPUNIT_TEST( testRatioIstreamFunctions );
		CPPUNIT_TEST( testPowerIstreamFunctions );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();
		void testBooleanPowerOperators();
		void testArithmeticPowerOperators();
		void testBooleanRatioOperators();
		void testArithmeticRatioOperators();
		void testPowerMemberFunctions();
		void testRatioMemberFunctions();
		void testRatioIstreamFunctions();
		void testPowerIstreamFunctions();

	private:
		Power p_res;
		Power p1;
		Power p2;
		Power p3;
		Ratio r_res;
		Ratio r1;
		Ratio r2;
		Ratio r3;
	};
}
#endif //classPower_RatioTest


