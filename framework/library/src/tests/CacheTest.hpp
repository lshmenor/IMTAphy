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

#include <WNS/Cache.hpp>
#include <WNS/PowerRatio.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

#ifndef classCacheTest
#define classCacheTest

namespace wns {
	class CacheTest : public CppUnit::TestFixture  {
		CPPUNIT_TEST_SUITE( CacheTest );
		CPPUNIT_TEST( testSetClean );
		CPPUNIT_TEST( testSetDirty );
		CPPUNIT_TEST( testContent );
		CPPUNIT_TEST( testImplicitSetClean );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();
		void testSetClean();
		void testSetDirty();
		void testContent();
		void testImplicitSetClean();

	private:
		Cache<Power> p;
		Cache<Class<double> > d;
	};
}
#endif


