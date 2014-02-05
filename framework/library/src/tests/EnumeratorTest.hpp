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

#ifndef WNS_TESTS_ENUMERATORTEST_HPP
#define WNS_TESTS_ENUMERATORTEST_HPP

#include <limits>

#include <WNS/TestFixture.hpp>

#include <WNS/Enumerator.hpp>

namespace wns
{

    class EnumeratorTest : public CppUnit::TestFixture
    {
	CPPUNIT_TEST_SUITE( EnumeratorTest );
	CPPUNIT_TEST( testEnumeratorInt );
	CPPUNIT_TEST( testEnumerableInt );
	CPPUNIT_TEST( testEnumeratorClass );
	CPPUNIT_TEST( testEnumerableClass );
	CPPUNIT_TEST( testEnumeratorFP<float> );
	CPPUNIT_TEST( testEnumerableFP<float> );
	CPPUNIT_TEST( testEnumeratorFP<double> );
	CPPUNIT_TEST( testEnumerableFP<double> );
	CPPUNIT_TEST( testEnumeratorFP<long double> );
	CPPUNIT_TEST( testEnumerableFP<long double> );
	CPPUNIT_TEST_SUITE_END();
    public:
	void setUp();
	void tearDown();
	void testEnumeratorInt();
	void testEnumerableInt();
	void testEnumeratorClass();
	void testEnumerableClass();

	template<typename T>
	void testEnumeratorFP()
	{
	    const T epsilon = std::numeric_limits<T>::epsilon();
	    const T denormMin = std::numeric_limits<T>::denorm_min();
	    const T max = std::numeric_limits<T>::max();
	    const T inf = std::numeric_limits<T>::infinity();
	    
	    CPPUNIT_ASSERT( Enumerator::succ((T) 1.0) == 1.0 + epsilon );
	    CPPUNIT_ASSERT( Enumerator::pred((T) 1.0 + epsilon) == 1.0 );
	    CPPUNIT_ASSERT( Enumerator::succ((T) 0.0) == denormMin );
	    CPPUNIT_ASSERT( Enumerator::pred(denormMin) == 0.0 );
	    CPPUNIT_ASSERT( Enumerator::pred((T) 0.0) == -denormMin );
	    CPPUNIT_ASSERT( Enumerator::succ((T) -denormMin) == 0.0 );
	    CPPUNIT_ASSERT( Enumerator::succ(max) == inf );
	    CPPUNIT_ASSERT( Enumerator::pred(inf) == max );
	    CPPUNIT_ASSERT( Enumerator::pred(-max) == -inf );
	    CPPUNIT_ASSERT( Enumerator::succ(-inf) == -max );
	    
	    const T epsilon2 = epsilon * std::numeric_limits<T>::radix;
	    const T value = std::numeric_limits<T>::radix;
	    CPPUNIT_ASSERT( Enumerator::succ(value) == value + epsilon2 );
	}

	template<typename T>
	void testEnumerableFP()
	{
	    typedef Enumerable<T> MyFP;
	    MyFP f((T) 1.0);
	    CPPUNIT_ASSERT( ++f == Enumerator::succ((T) 1.0) );
	    CPPUNIT_ASSERT( --f == (T) 1.0 );
	}

    private:
    };

}

#endif // WNS_TESTS_ENUMERATORTEST_HPP
