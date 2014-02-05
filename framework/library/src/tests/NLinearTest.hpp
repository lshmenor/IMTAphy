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


#include <cppunit/extensions/HelperMacros.h>
#include <WNS/NLinear.hpp>
#include <WNS/container/Matrix.hpp>

#ifndef WNS_NLINEARTEST_HPP
#define WNS_NLINEARTEST_HPP

namespace wns {

	class NLinearTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE( NLinearTest );
		CPPUNIT_TEST( testLinear );
		CPPUNIT_TEST( testBilinear );
		CPPUNIT_TEST( testTrilinear );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();
		void testLinear();
		void testBilinear();
		void testTrilinear();

	private:

		typedef NLinear<double, 1> Linear;
		typedef NLinear<double, 2> Bilinear;
		typedef NLinear<double, 3> Trilinear;

		Linear::InitType m1;
		Bilinear::InitType m2;
		Trilinear::InitType m3;

		Linear* linear;
		Bilinear* bilinear;
		Trilinear* trilinear;

		static Linear::ValueType linearInterpolation(const Linear::CoordType& x,
							     const Linear::InitType& m);

		static Bilinear::ValueType bilinearInterpolation(const Bilinear::CoordType& x,
								 const Bilinear::CoordType& y,
								 const Bilinear::InitType& m);

		static Trilinear::ValueType trilinearInterpolation(const Trilinear::CoordType& x,
								   const Trilinear::CoordType& y,
								   const Trilinear::CoordType& z,
								   const Trilinear::InitType& m);

	};

}

#endif // NOT defined WNS_NLINEARTEST_HPP
