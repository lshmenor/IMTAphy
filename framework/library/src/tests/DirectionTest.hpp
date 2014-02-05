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

#ifndef _DIRECTIONTEST_HPP
#define _DIRECTIONTEST_HPP

#include <WNS/Direction.hpp>
#include <cppunit/extensions/HelperMacros.h>

namespace wns {
	class DirectionTest
		: public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( DirectionTest );
		CPPUNIT_TEST( testConstructors );
		CPPUNIT_TEST( testDirectionCalcAzimuthUntwisted );
		CPPUNIT_TEST( testDirectionCalcElevationUntwisted );
		CPPUNIT_TEST( testDirectionCalcAzimuthTwisted );
		CPPUNIT_TEST( testDirectionCalcElevationTwisted );
		CPPUNIT_TEST( testNoNegativeResult );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();
		void testConstructors();
		void testDirectionCalcAzimuthUntwisted();
		void testDirectionCalcElevationUntwisted();
		void testDirectionCalcAzimuthTwisted();
		void testDirectionCalcElevationTwisted();
		void testNoNegativeResult();
	private:
	};
}
#endif


