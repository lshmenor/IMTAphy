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

#ifndef WNS_DISTRIBUTION_TEST_CAUCHY_HPP
#define WNS_DISTRIBUTION_TEST_CAUCHY_HPP

#include <WNS/distribution/Cauchy.hpp>

#include <WNS/TestFixture.hpp>

namespace wns { namespace distribution { namespace tests {

	class CauchyTest :
		public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( CauchyTest );
		CPPUNIT_TEST( testCDF );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();

		void testCDF();
	private:
	};
}}}

#endif // NOT defined WNS_DISTRIBUTION_TEST_CAUCHY_HPP


