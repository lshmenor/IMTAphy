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

#ifndef WNS_INTERVALTEST_HPP
#define WNS_INTERVALTEST_HPP

#include <WNS/Interval.hpp>
#include <WNS/Assure.hpp>

#include <WNS/TestFixture.hpp>

#include <vector>

#include <iostream>

namespace wns
{
    class IntervalTest : public CppUnit::TestFixture
    {
	CPPUNIT_TEST_SUITE( IntervalTest );
	CPPUNIT_TEST( testEmpty<int> );
	CPPUNIT_TEST( testEmpty<double> );
	CPPUNIT_TEST( testBasics<int> );
	CPPUNIT_TEST( testBasics<double> );
	CPPUNIT_TEST( testIdentical<int> );
	CPPUNIT_TEST( testIdentical<double> );
	CPPUNIT_TEST( testDisjointNonAdjacent<int> );
	CPPUNIT_TEST( testDisjointNonAdjacent<double> );
	CPPUNIT_TEST( testAdjacent<int> );
	CPPUNIT_TEST( testAdjacent<double> );
	CPPUNIT_TEST( testOverlapping<int> );
	CPPUNIT_TEST( testOverlapping<double> );
	CPPUNIT_TEST( testSubset<int> );
	CPPUNIT_TEST( testSubset<double> );
	CPPUNIT_TEST_SUITE_END();
    public:
	void setUp() {}
	void tearDown() {}

	template<typename T>
	void testEmpty()
	{
	    typedef Interval<T> tInt;
	    typedef std::vector<tInt> vInt;
	    vInt tInts(5);
	    tInts[0] = tInt::EmptyInterval();
	    tInts[1] = tInt::Between(static_cast<T>(3)).And(static_cast<T>(3));
	    tInts[2] = tInt::From(static_cast<T>(2)).To(static_cast<T>(1));
	    tInts[3] = tInt::Between(T()).And(++makeEnumerable(T()));
	    tInts[4] = tInt::FromIncluding(T()).ToExcluding(T());

	    const tInt o = tInt::From(T()).To(T());
	    const tInt a = tInt::From(static_cast<T>(1)).To(static_cast<T>(2));
	    const tInt b = tInt::From(static_cast<T>(1)).To(static_cast<T>(3));

	    for (typename vInt::size_type i = 0; i < tInts.size(); ++i) {
		CPPUNIT_ASSERT( tInts[i].isEmpty() );

		WNS_ASSERT_ASSURE_EXCEPTION( tInts[i].min() );
		WNS_ASSERT_ASSURE_EXCEPTION( tInts[i].max() );

		CPPUNIT_ASSERT( !tInts[i].isBelow(static_cast<T>(3)) );
		CPPUNIT_ASSERT( !tInts[i].isAbove(static_cast<T>(0)) );

		for (T x = T(); x <= T() + static_cast<T>(4); x += static_cast<T>(1))
		    CPPUNIT_ASSERT( !tInts[i].contains(x) );
		CPPUNIT_ASSERT( !tInts[i].contains(a) );
		CPPUNIT_ASSERT( o.contains(tInts[i]) );

		CPPUNIT_ASSERT( !tInts[i].overlaps(b) );
		CPPUNIT_ASSERT( !b.overlaps(tInts[i]) );

		CPPUNIT_ASSERT( tInts[i].joinableWith(o) );
		CPPUNIT_ASSERT( o.joinableWith(tInts[i]) );

		CPPUNIT_ASSERT( tInts[i].join(o) == o );
		CPPUNIT_ASSERT( o.join(tInts[i]) == o );

		CPPUNIT_ASSERT( tInts[i].intersect(b).isEmpty() );
		CPPUNIT_ASSERT( b.intersect(tInts[i]).isEmpty() );

		for (typename vInt::size_type j = 0; j < 3; ++j) {
		    CPPUNIT_ASSERT( tInts[i] == tInts[j] );
		    CPPUNIT_ASSERT( !tInts[i].isBelow(tInts[j]) );
		    CPPUNIT_ASSERT( !tInts[i].isAbove(tInts[j]) );
		    CPPUNIT_ASSERT( tInts[i].contains(tInts[j]) );
		    CPPUNIT_ASSERT( !tInts[i].overlaps(tInts[j]) );
		    CPPUNIT_ASSERT( tInts[i].joinableWith(tInts[j]) );
		    CPPUNIT_ASSERT( tInts[i].join(tInts[j]).isEmpty() );
		    CPPUNIT_ASSERT( tInts[i].intersect(tInts[j]).isEmpty() );
		}
	    }
	}

	template<typename T>
	void testBasics()
	{
	    typedef Interval<T> tInt;
	    typedef std::vector<tInt> vInt;
	    vInt tInts(4);
	    const T o = static_cast<T>(0);
	    const T a = static_cast<T>(1);
	    const T b = static_cast<T>(3);
	    const T z = static_cast<T>(4);
	    tInts[0] = tInt::From(a).To(b);
	    tInts[1] = tInt::Between(a).And(b);
	    tInts[2] = tInt::FromIncluding(a).ToExcluding(b);
	    tInts[3] = tInt::FromExcluding(a).ToIncluding(b);
	    
	    const tInt ground = tInt::From(o).To(o);
	    const tInt heaven = tInt::From(z).To(z);

	    for (typename vInt::size_type i = 0; i < 4; ++i) {
		CPPUNIT_ASSERT( !tInts[i].isEmpty() );
		CPPUNIT_ASSERT( tInts[i].isBelow(z) );
		CPPUNIT_ASSERT( tInts[i].isBelow(heaven) );
		CPPUNIT_ASSERT( !heaven.isBelow(tInts[i]) );
		CPPUNIT_ASSERT( tInts[i].isAbove(o) );
		CPPUNIT_ASSERT( tInts[i].isAbove(ground) );
		CPPUNIT_ASSERT( !ground.isAbove(tInts[i]) );
		CPPUNIT_ASSERT( !tInts[i].contains(o) );
		CPPUNIT_ASSERT( tInts[i].contains(tInts[i].min()) );
		CPPUNIT_ASSERT( tInts[i].contains((a + b) / 2) );
		CPPUNIT_ASSERT( tInts[i].contains(tInts[i].max()) );
		CPPUNIT_ASSERT( !tInts[i].contains(z) );
	    }

	    CPPUNIT_ASSERT( tInts[1].isBelow(b) );
	    CPPUNIT_ASSERT( tInts[2].isBelow(b) );

	    CPPUNIT_ASSERT( tInts[1].isAbove(a) );
	    CPPUNIT_ASSERT( tInts[3].isAbove(a) );

	    CPPUNIT_ASSERT( tInts[0].min() == a );
	    CPPUNIT_ASSERT( tInts[1].min() == ++makeEnumerable(a) );
	    CPPUNIT_ASSERT( tInts[2].min() == a );
	    CPPUNIT_ASSERT( tInts[3].min() == ++makeEnumerable(a) );

	    CPPUNIT_ASSERT( tInts[0].max() == b );
	    CPPUNIT_ASSERT( tInts[1].max() == --makeEnumerable(b) );
	    CPPUNIT_ASSERT( tInts[2].max() == --makeEnumerable(b) );
	    CPPUNIT_ASSERT( tInts[3].max() == b );

	    CPPUNIT_ASSERT( !tInts[1].contains(a) );
	    CPPUNIT_ASSERT( !tInts[3].contains(a) );
	    CPPUNIT_ASSERT( !tInts[1].contains(b) );
	    CPPUNIT_ASSERT( !tInts[2].contains(b) );
	}

	template<typename T>
	void testIdentical()
	{
	    typedef Interval<T> tInt;
	    typedef std::vector<tInt> vInt;
	    vInt tInts(4);
	    vInt tInts2(4);
	    const T a = static_cast<T>(1);
	    const T b = static_cast<T>(3);
	    tInts[0] = tInts2[0] = tInt::From(a).To(b);
	    tInts[1] = tInts2[1] = tInt::Between(a).And(b);
	    tInts[2] = tInts2[2] = tInt::FromIncluding(a).ToExcluding(b);
	    tInts[3] = tInts2[3] = tInt::FromExcluding(a).ToIncluding(b);
	    
	    for (typename vInt::size_type i = 0; i < 4; ++i) {
		CPPUNIT_ASSERT( tInts[i] == tInts2[i] );
		CPPUNIT_ASSERT( tInts2[i] == tInts[i] );
		CPPUNIT_ASSERT( !tInts[i].isBelow(tInts[i]) );
		CPPUNIT_ASSERT( !tInts[i].isAbove(tInts[i]) );
		CPPUNIT_ASSERT( tInts[i].contains(tInts[i]) );
		CPPUNIT_ASSERT( tInts[0].contains(tInts[i]) );
		CPPUNIT_ASSERT( tInts[i].contains(tInts[1]) );
		CPPUNIT_ASSERT( tInts[i].contains(tInts[0]) == (i == 0) );
		CPPUNIT_ASSERT( tInts[1].contains(tInts[i]) == (i == 1) );
		for (typename vInt::size_type j = 0; j < 4; ++j) {
		    CPPUNIT_ASSERT( (tInts[i] == tInts[j]) == (i == j) );
		    CPPUNIT_ASSERT( tInts[i].overlaps(tInts[j]) );
		    CPPUNIT_ASSERT( tInts[i].joinableWith(tInts[j]) );
		}
		CPPUNIT_ASSERT( tInts[i].join(tInts[i]) == tInts[i] );
		CPPUNIT_ASSERT( tInts[0].join(tInts[i]) == tInts[0] );
		CPPUNIT_ASSERT( tInts[i].join(tInts[0]) == tInts[0] );
		CPPUNIT_ASSERT( tInts[1].join(tInts[i]) == tInts[i] );
		CPPUNIT_ASSERT( tInts[i].join(tInts[1]) == tInts[i] );
		CPPUNIT_ASSERT( tInts[i].intersect(tInts[i]) == tInts[i] );
		CPPUNIT_ASSERT( tInts[0].intersect(tInts[i]) == tInts[i] );
		CPPUNIT_ASSERT( tInts[i].intersect(tInts[0]) == tInts[i] );
		CPPUNIT_ASSERT( tInts[1].intersect(tInts[i]) == tInts[1] );
		CPPUNIT_ASSERT( tInts[i].intersect(tInts[1]) == tInts[1] );
	    }

	    CPPUNIT_ASSERT( !tInts[2].contains(tInts[3]) );
	    CPPUNIT_ASSERT( !tInts[3].contains(tInts[2]) );

	    CPPUNIT_ASSERT( tInts[2].join(tInts[3]) == tInts[0] );
	    CPPUNIT_ASSERT( tInts[3].join(tInts[2]) == tInts[0] );

	    CPPUNIT_ASSERT( tInts[2].intersect(tInts[3]) == tInts[1] );
	    CPPUNIT_ASSERT( tInts[3].intersect(tInts[2]) == tInts[1] );
	}

	template<typename T>
	void testDisjointNonAdjacent()
	{
	    typedef Interval<T> tInt;
	    typedef std::vector<tInt> vInt;
	    vInt tInts1(4);
	    vInt tInts2(4);
	    const T a = static_cast<T>(1);
	    const T b = static_cast<T>(3);
	    const T c = static_cast<T>(5);
	    const T d = static_cast<T>(7);
	    tInts1[0] = tInt::From(a).To(b);
	    tInts2[0] = tInt::From(c).To(d);
	    tInts1[1] = tInt::Between(a).And(b);
	    tInts2[1] = tInt::Between(c).And(d);
	    tInts1[2] = tInt::FromIncluding(a).ToExcluding(b);
	    tInts2[2] = tInt::FromIncluding(c).ToExcluding(d);
	    tInts1[3] = tInt::FromExcluding(a).ToIncluding(b);
	    tInts2[3] = tInt::FromExcluding(c).ToIncluding(d);
	    for (typename vInt::size_type i = 0; i < 4; ++i) {
		for (typename vInt::size_type j = 0; j < 4; ++j) {
		    CPPUNIT_ASSERT( tInts1[i] != tInts2[j] );
		    CPPUNIT_ASSERT( tInts2[i] != tInts1[j] );
		    CPPUNIT_ASSERT( tInts1[i].isBelow(tInts2[j]) );
		    CPPUNIT_ASSERT( !tInts2[i].isBelow(tInts1[j]) );
		    CPPUNIT_ASSERT( !tInts1[i].isAbove(tInts2[j]) );
		    CPPUNIT_ASSERT( tInts2[i].isAbove(tInts1[j]) );
		    CPPUNIT_ASSERT( !tInts1[i].contains(tInts2[j]) );
		    CPPUNIT_ASSERT( !tInts2[i].contains(tInts1[j]) );
		    CPPUNIT_ASSERT( !tInts1[i].overlaps(tInts2[j]) );
		    CPPUNIT_ASSERT( !tInts2[i].overlaps(tInts1[j]) );
		    CPPUNIT_ASSERT( !tInts1[i].joinableWith(tInts2[j]) );
		    CPPUNIT_ASSERT( !tInts2[i].joinableWith(tInts1[j]) );
		    CPPUNIT_ASSERT( tInts1[i].intersect(tInts2[j]).isEmpty() );
		    CPPUNIT_ASSERT( tInts2[i].intersect(tInts1[j]).isEmpty() );
		}
	    }
	}

	template<typename T>
	void testAdjacent()
	{
	    typedef Interval<T> tInt;
	    typedef std::vector<tInt> vInt;
	    vInt tInts1(4);
	    vInt tInts2(4);
	    const T a = static_cast<T>(1);
	    const T b = static_cast<T>(3);
	    const T c = static_cast<T>(5);

	    tInts1[0] = tInt::From(a).To(b);
	    tInts2[0] = tInt::From(b).To(c);
	    tInts1[1] = tInt::Between(a).And(b);
	    tInts2[1] = tInt::Between(b).And(c);
	    tInts1[2] = tInt::FromIncluding(a).ToExcluding(b);
	    tInts2[2] = tInt::FromIncluding(b).ToExcluding(c);
	    tInts1[3] = tInt::FromExcluding(a).ToIncluding(b);
	    tInts2[3] = tInt::FromExcluding(b).ToIncluding(c);

	    tInt b2b = tInt::From(b).To(b);

	    for (typename vInt::size_type i = 0; i < 4; ++i) {
		CPPUNIT_ASSERT( tInts1[i].isBelow(tInts2[1]) );
		CPPUNIT_ASSERT( tInts1[1].isBelow(tInts2[i]) );
		CPPUNIT_ASSERT( tInts1[i].isBelow(tInts2[3]) );
		CPPUNIT_ASSERT( tInts1[2].isBelow(tInts2[i]) );
		CPPUNIT_ASSERT( tInts2[1].isAbove(tInts1[i]) );
		CPPUNIT_ASSERT( tInts2[i].isAbove(tInts1[1]) );
		CPPUNIT_ASSERT( tInts2[3].isAbove(tInts1[i]) );
		CPPUNIT_ASSERT( tInts2[i].isAbove(tInts1[2]) );
		CPPUNIT_ASSERT( !tInts1[i].overlaps(tInts2[1]) );
		CPPUNIT_ASSERT( !tInts1[1].overlaps(tInts2[i]) );
		CPPUNIT_ASSERT( !tInts1[i].overlaps(tInts2[3]) );
		CPPUNIT_ASSERT( !tInts1[2].overlaps(tInts2[i]) );
		CPPUNIT_ASSERT( !tInts2[1].overlaps(tInts1[i]) );
		CPPUNIT_ASSERT( !tInts2[i].overlaps(tInts1[1]) );
		CPPUNIT_ASSERT( !tInts2[3].overlaps(tInts1[i]) );
		CPPUNIT_ASSERT( !tInts2[i].overlaps(tInts1[2]) );
		CPPUNIT_ASSERT( tInts1[0].joinableWith(tInts2[i]) );
		CPPUNIT_ASSERT( tInts1[i].joinableWith(tInts2[0]) );
		CPPUNIT_ASSERT( tInts1[3].joinableWith(tInts2[i]) );
		CPPUNIT_ASSERT( tInts1[i].joinableWith(tInts2[2]) );
		CPPUNIT_ASSERT( tInts2[i].joinableWith(tInts1[0]) );
		CPPUNIT_ASSERT( tInts2[0].joinableWith(tInts1[i]) );
		CPPUNIT_ASSERT( tInts2[i].joinableWith(tInts1[3]) );
		CPPUNIT_ASSERT( tInts2[2].joinableWith(tInts1[i]) );
		CPPUNIT_ASSERT( tInts1[1].intersect(tInts2[i]).isEmpty() );
		CPPUNIT_ASSERT( tInts1[i].intersect(tInts2[1]).isEmpty() );
		CPPUNIT_ASSERT( tInts1[2].intersect(tInts2[i]).isEmpty() );
		CPPUNIT_ASSERT( tInts1[i].intersect(tInts2[3]).isEmpty() );
		CPPUNIT_ASSERT( tInts2[i].intersect(tInts1[1]).isEmpty() );
		CPPUNIT_ASSERT( tInts2[1].intersect(tInts1[i]).isEmpty() );
		CPPUNIT_ASSERT( tInts2[i].intersect(tInts1[2]).isEmpty() );
		CPPUNIT_ASSERT( tInts2[3].intersect(tInts1[i]).isEmpty() );
		for (typename vInt::size_type j = 0; j < 4; ++j) {
		    CPPUNIT_ASSERT( tInts1[i] != tInts2[j] );
		    CPPUNIT_ASSERT( tInts2[i] != tInts1[j] );
		    CPPUNIT_ASSERT( !tInts2[j].isBelow(tInts1[i]) );
		    CPPUNIT_ASSERT( !tInts1[i].isAbove(tInts2[j]) );
		    CPPUNIT_ASSERT( !tInts1[i].contains(tInts2[j]) );
		    CPPUNIT_ASSERT( !tInts2[j].contains(tInts1[i]) );
		}
	    }

	    CPPUNIT_ASSERT( !tInts1[0].isBelow(tInts2[0]) );
	    CPPUNIT_ASSERT( !tInts1[0].isBelow(tInts2[2]) );
	    CPPUNIT_ASSERT( !tInts1[3].isBelow(tInts2[0]) );
	    CPPUNIT_ASSERT( !tInts1[3].isBelow(tInts2[2]) );

	    CPPUNIT_ASSERT( !tInts2[0].isAbove(tInts1[0]) );
	    CPPUNIT_ASSERT( !tInts2[0].isAbove(tInts1[3]) );
	    CPPUNIT_ASSERT( !tInts2[2].isAbove(tInts1[0]) );
	    CPPUNIT_ASSERT( !tInts2[2].isAbove(tInts1[3]) );

	    CPPUNIT_ASSERT( tInts1[0].overlaps(tInts2[0]) );
	    CPPUNIT_ASSERT( tInts1[0].overlaps(tInts2[2]) );
	    CPPUNIT_ASSERT( tInts1[3].overlaps(tInts2[0]) );
	    CPPUNIT_ASSERT( tInts1[3].overlaps(tInts2[2]) );

	    CPPUNIT_ASSERT( tInts2[0].overlaps(tInts1[0]) );
	    CPPUNIT_ASSERT( tInts2[0].overlaps(tInts1[3]) );
	    CPPUNIT_ASSERT( tInts2[2].overlaps(tInts1[0]) );
	    CPPUNIT_ASSERT( tInts2[2].overlaps(tInts1[3]) );

	    CPPUNIT_ASSERT( !tInts1[1].joinableWith(tInts2[1]) );
	    CPPUNIT_ASSERT( !tInts1[1].joinableWith(tInts2[3]) );
	    CPPUNIT_ASSERT( !tInts1[2].joinableWith(tInts2[1]) );
	    CPPUNIT_ASSERT( !tInts1[2].joinableWith(tInts2[3]) );

	    CPPUNIT_ASSERT( !tInts2[1].joinableWith(tInts1[1]) );
	    CPPUNIT_ASSERT( !tInts2[1].joinableWith(tInts1[2]) );
	    CPPUNIT_ASSERT( !tInts2[3].joinableWith(tInts1[1]) );
	    CPPUNIT_ASSERT( !tInts2[3].joinableWith(tInts1[2]) );

	    CPPUNIT_ASSERT( tInts1[0].join(tInts2[0]) == tInt::From(a).To(c) );
	    CPPUNIT_ASSERT( tInts1[0].join(tInts2[1]) == tInt::FromIncluding(a).ToExcluding(c) );
	    CPPUNIT_ASSERT( tInts1[0].join(tInts2[2]) == tInt::FromIncluding(a).ToExcluding(c) );
	    CPPUNIT_ASSERT( tInts1[0].join(tInts2[3]) == tInt::From(a).To(c) );

	    CPPUNIT_ASSERT( tInts1[1].join(tInts2[0]) == tInt::FromExcluding(a).ToIncluding(c) );
	    CPPUNIT_ASSERT( tInts1[1].join(tInts2[2]) == tInt::Between(a).And(c) );

	    CPPUNIT_ASSERT( tInts1[2].join(tInts2[0]) == tInt::From(a).To(c) );
	    CPPUNIT_ASSERT( tInts1[2].join(tInts2[2]) == tInt::FromIncluding(a).ToExcluding(c) );

	    CPPUNIT_ASSERT( tInts1[3].join(tInts2[0]) == tInt::FromExcluding(a).ToIncluding(c) );
	    CPPUNIT_ASSERT( tInts1[3].join(tInts2[1]) == tInt::Between(a).And(c) );
	    CPPUNIT_ASSERT( tInts1[3].join(tInts2[2]) == tInt::Between(a).And(c) );
	    CPPUNIT_ASSERT( tInts1[3].join(tInts2[3]) == tInt::FromExcluding(a).ToIncluding(c) );

	    CPPUNIT_ASSERT( tInts2[0].join(tInts1[0]) == tInts1[0].join(tInts2[0]) );
	    CPPUNIT_ASSERT( tInts2[0].join(tInts1[1]) == tInts1[1].join(tInts2[0]) );
	    CPPUNIT_ASSERT( tInts2[0].join(tInts1[2]) == tInts1[2].join(tInts2[0]) );
	    CPPUNIT_ASSERT( tInts2[0].join(tInts1[3]) == tInts1[3].join(tInts2[0]) );
	    
	    CPPUNIT_ASSERT( tInts2[1].join(tInts1[0]) == tInts1[0].join(tInts2[1]) );
	    CPPUNIT_ASSERT( tInts2[1].join(tInts1[3]) == tInts1[3].join(tInts2[1]) );

	    CPPUNIT_ASSERT( tInts2[2].join(tInts1[0]) == tInts1[0].join(tInts2[2]) );
	    CPPUNIT_ASSERT( tInts2[2].join(tInts1[1]) == tInts1[1].join(tInts2[2]) );
	    CPPUNIT_ASSERT( tInts2[2].join(tInts1[2]) == tInts1[2].join(tInts2[2]) );
	    CPPUNIT_ASSERT( tInts2[2].join(tInts1[3]) == tInts1[3].join(tInts2[2]) );
	    
	    CPPUNIT_ASSERT( tInts2[3].join(tInts1[0]) == tInts1[0].join(tInts2[3]) );
	    CPPUNIT_ASSERT( tInts2[3].join(tInts1[3]) == tInts1[3].join(tInts2[3]) );

	    CPPUNIT_ASSERT( tInts1[0].intersect(tInts2[0]) == b2b );
	    CPPUNIT_ASSERT( tInts1[0].intersect(tInts2[2]) == b2b );
	    CPPUNIT_ASSERT( tInts1[3].intersect(tInts2[0]) == b2b );
	    CPPUNIT_ASSERT( tInts1[3].intersect(tInts2[2]) == b2b );

	    CPPUNIT_ASSERT( tInts2[0].intersect(tInts1[0]) == b2b );
	    CPPUNIT_ASSERT( tInts2[0].intersect(tInts1[3]) == b2b );
	    CPPUNIT_ASSERT( tInts2[2].intersect(tInts1[0]) == b2b );
	    CPPUNIT_ASSERT( tInts2[2].intersect(tInts1[3]) == b2b );
	}

	template<typename T>
	void testOverlapping()
	{
	    typedef Interval<T> tInt;
	    typedef std::vector<tInt> vInt;
	    vInt tInts1(4);
	    vInt tInts2(4);

	    const T a = static_cast<T>(1);
	    const T b = static_cast<T>(5);
	    const T c = static_cast<T>(3);
	    const T d = static_cast<T>(7);

	    tInts1[0] = tInt::From(a).To(b);
	    tInts2[0] = tInt::From(c).To(d);
	    tInts1[1] = tInt::Between(a).And(b);
	    tInts2[1] = tInt::Between(c).And(d);
	    tInts1[2] = tInt::FromIncluding(a).ToExcluding(b);
	    tInts2[2] = tInt::FromIncluding(c).ToExcluding(d);
	    tInts1[3] = tInt::FromExcluding(a).ToIncluding(b);
	    tInts2[3] = tInt::FromExcluding(c).ToIncluding(d);

	    for (typename vInt::size_type i = 0; i < 4; ++i) {
		for (typename vInt::size_type j = 0; j < 4; ++j) {
		    CPPUNIT_ASSERT( tInts1[i] != tInts2[j] );
		    CPPUNIT_ASSERT( tInts2[j] != tInts1[i] );
		    CPPUNIT_ASSERT( !tInts1[i].isBelow(tInts2[j]) );
		    CPPUNIT_ASSERT( !tInts2[j].isBelow(tInts1[i]) );
		    CPPUNIT_ASSERT( !tInts1[i].isAbove(tInts2[j]) );
		    CPPUNIT_ASSERT( !tInts2[j].isAbove(tInts1[i]) );
		    CPPUNIT_ASSERT( !tInts1[i].contains(tInts2[j]) );
		    CPPUNIT_ASSERT( !tInts2[j].contains(tInts1[i]) );
		    CPPUNIT_ASSERT( tInts1[i].overlaps(tInts2[j]) );
		    CPPUNIT_ASSERT( tInts2[j].overlaps(tInts1[i]) );
		    CPPUNIT_ASSERT( tInts1[i].joinableWith(tInts2[j]) );
		    CPPUNIT_ASSERT( tInts2[j].joinableWith(tInts1[i]) );
		}
	    }

	    CPPUNIT_ASSERT( tInts1[0].join(tInts2[0]) == tInt::From(a).To(d) );
	    CPPUNIT_ASSERT( tInts1[0].join(tInts2[1]) == tInt::FromIncluding(a).ToExcluding(d) );
	    CPPUNIT_ASSERT( tInts1[0].join(tInts2[2]) == tInt::FromIncluding(a).ToExcluding(d) );
	    CPPUNIT_ASSERT( tInts1[0].join(tInts2[3]) == tInt::From(a).To(d) );
	    
	    CPPUNIT_ASSERT( tInts1[1].join(tInts2[0]) == tInt::FromExcluding(a).ToIncluding(d) );
	    CPPUNIT_ASSERT( tInts1[1].join(tInts2[1]) == tInt::Between(a).And(d) );
	    CPPUNIT_ASSERT( tInts1[1].join(tInts2[2]) == tInt::Between(a).And(d) );
	    CPPUNIT_ASSERT( tInts1[1].join(tInts2[3]) == tInt::FromExcluding(a).ToIncluding(d) );

	    CPPUNIT_ASSERT( tInts1[2].join(tInts2[0]) == tInt::From(a).To(d) );
	    CPPUNIT_ASSERT( tInts1[2].join(tInts2[1]) == tInt::FromIncluding(a).ToExcluding(d) );
	    CPPUNIT_ASSERT( tInts1[2].join(tInts2[2]) == tInt::FromIncluding(a).ToExcluding(d) );
	    CPPUNIT_ASSERT( tInts1[2].join(tInts2[3]) == tInt::From(a).To(d) );

	    CPPUNIT_ASSERT( tInts1[3].join(tInts2[0]) == tInt::FromExcluding(a).ToIncluding(d) );
	    CPPUNIT_ASSERT( tInts1[3].join(tInts2[1]) == tInt::Between(a).And(d) );
	    CPPUNIT_ASSERT( tInts1[3].join(tInts2[2]) == tInt::Between(a).And(d) );
	    CPPUNIT_ASSERT( tInts1[3].join(tInts2[3]) == tInt::FromExcluding(a).ToIncluding(d) );

	    CPPUNIT_ASSERT( tInts2[0].join(tInts1[0]) == tInt::From(a).To(d) );
	    CPPUNIT_ASSERT( tInts2[0].join(tInts1[1]) == tInt::FromExcluding(a).ToIncluding(d) );
	    CPPUNIT_ASSERT( tInts2[0].join(tInts1[2]) == tInt::From(a).To(d) );
	    CPPUNIT_ASSERT( tInts2[0].join(tInts1[3]) == tInt::FromExcluding(a).ToIncluding(d) );

	    CPPUNIT_ASSERT( tInts2[1].join(tInts1[0]) == tInt::FromIncluding(a).ToExcluding(d) );
	    CPPUNIT_ASSERT( tInts2[1].join(tInts1[1]) == tInt::Between(a).And(d) );
	    CPPUNIT_ASSERT( tInts2[1].join(tInts1[2]) == tInt::FromIncluding(a).ToExcluding(d) );
	    CPPUNIT_ASSERT( tInts2[1].join(tInts1[3]) == tInt::Between(a).And(d) );
	    
	    CPPUNIT_ASSERT( tInts2[2].join(tInts1[0]) == tInt::FromIncluding(a).ToExcluding(d) );
	    CPPUNIT_ASSERT( tInts2[2].join(tInts1[1]) == tInt::Between(a).And(d) );
	    CPPUNIT_ASSERT( tInts2[2].join(tInts1[2]) == tInt::FromIncluding(a).ToExcluding(d) );
	    CPPUNIT_ASSERT( tInts2[2].join(tInts1[3]) == tInt::Between(a).And(d) );

	    CPPUNIT_ASSERT( tInts2[3].join(tInts1[0]) == tInt::From(a).To(d) );
	    CPPUNIT_ASSERT( tInts2[3].join(tInts1[1]) == tInt::FromExcluding(a).ToIncluding(d) );
	    CPPUNIT_ASSERT( tInts2[3].join(tInts1[2]) == tInt::From(a).To(d) );
	    CPPUNIT_ASSERT( tInts2[3].join(tInts1[3]) == tInt::FromExcluding(a).ToIncluding(d) );

	    CPPUNIT_ASSERT( tInts1[0].intersect(tInts2[0]) == tInt::From(c).To(b) );
	    CPPUNIT_ASSERT( tInts1[0].intersect(tInts2[1]) == tInt::FromExcluding(c).ToIncluding(b) );
	    CPPUNIT_ASSERT( tInts1[0].intersect(tInts2[2]) == tInt::From(c).To(b) );
	    CPPUNIT_ASSERT( tInts1[0].intersect(tInts2[3]) == tInt::FromExcluding(c).ToIncluding(b) );
	    
	    CPPUNIT_ASSERT( tInts1[1].intersect(tInts2[0]) == tInt::FromIncluding(c).ToExcluding(b) );
	    CPPUNIT_ASSERT( tInts1[1].intersect(tInts2[1]) == tInt::Between(c).And(b) );
	    CPPUNIT_ASSERT( tInts1[1].intersect(tInts2[2]) == tInt::FromIncluding(c).ToExcluding(b) );
	    CPPUNIT_ASSERT( tInts1[1].intersect(tInts2[3]) == tInt::Between(c).And(b) );

	    CPPUNIT_ASSERT( tInts1[2].intersect(tInts2[0]) == tInt::FromIncluding(c).ToExcluding(b) );
	    CPPUNIT_ASSERT( tInts1[2].intersect(tInts2[1]) == tInt::Between(c).And(b) );
	    CPPUNIT_ASSERT( tInts1[2].intersect(tInts2[2]) == tInt::FromIncluding(c).ToExcluding(b) );
	    CPPUNIT_ASSERT( tInts1[2].intersect(tInts2[3]) == tInt::Between(c).And(b) );

	    CPPUNIT_ASSERT( tInts1[3].intersect(tInts2[0]) == tInt::From(c).To(b) );
	    CPPUNIT_ASSERT( tInts1[3].intersect(tInts2[1]) == tInt::FromExcluding(c).ToIncluding(b) );
	    CPPUNIT_ASSERT( tInts1[3].intersect(tInts2[2]) == tInt::From(c).To(b) );
	    CPPUNIT_ASSERT( tInts1[3].intersect(tInts2[3]) == tInt::FromExcluding(c).ToIncluding(b) );

	    CPPUNIT_ASSERT( tInts2[0].intersect(tInts1[0]) == tInt::From(c).To(b) );
	    CPPUNIT_ASSERT( tInts2[0].intersect(tInts1[1]) == tInt::FromIncluding(c).ToExcluding(b) );
	    CPPUNIT_ASSERT( tInts2[0].intersect(tInts1[2]) == tInt::FromIncluding(c).ToExcluding(b) );
	    CPPUNIT_ASSERT( tInts2[0].intersect(tInts1[3]) == tInt::From(c).To(b) );
	  
	    CPPUNIT_ASSERT( tInts2[1].intersect(tInts1[0]) == tInt::FromExcluding(c).ToIncluding(b) );
	    CPPUNIT_ASSERT( tInts2[1].intersect(tInts1[1]) == tInt::Between(c).And(b) );
	    CPPUNIT_ASSERT( tInts2[1].intersect(tInts1[2]) == tInt::Between(c).And(b) );
	    CPPUNIT_ASSERT( tInts2[1].intersect(tInts1[3]) == tInt::FromExcluding(c).ToIncluding(b) );

	    CPPUNIT_ASSERT( tInts2[2].intersect(tInts1[0]) == tInt::From(c).To(b) );
	    CPPUNIT_ASSERT( tInts2[2].intersect(tInts1[1]) == tInt::FromIncluding(c).ToExcluding(b) );
	    CPPUNIT_ASSERT( tInts2[2].intersect(tInts1[2]) == tInt::FromIncluding(c).ToExcluding(b) );
	    CPPUNIT_ASSERT( tInts2[2].intersect(tInts1[3]) == tInt::From(c).To(b) );

	    CPPUNIT_ASSERT( tInts2[3].intersect(tInts1[0]) == tInt::FromExcluding(c).ToIncluding(b) );
	    CPPUNIT_ASSERT( tInts2[3].intersect(tInts1[1]) == tInt::Between(c).And(b) );
	    CPPUNIT_ASSERT( tInts2[3].intersect(tInts1[2]) == tInt::Between(c).And(b) );
	    CPPUNIT_ASSERT( tInts2[3].intersect(tInts1[3]) == tInt::FromExcluding(c).ToIncluding(b) );
	}

	template<typename T>
	void testSubset()
	{
	    typedef Interval<T> tInt;
	    typedef std::vector<tInt> vInt;
	    vInt tInts1(4);
	    vInt tInts2(4);

	    const T a = static_cast<T>(1);
	    const T b = static_cast<T>(7);
	    const T c = static_cast<T>(3);
	    const T d = static_cast<T>(5);

	    tInts1[0] = tInt::From(a).To(b);
	    tInts2[0] = tInt::From(c).To(d);
	    tInts1[1] = tInt::Between(a).And(b);
	    tInts2[1] = tInt::Between(c).And(d);
	    tInts1[2] = tInt::FromIncluding(a).ToExcluding(b);
	    tInts2[2] = tInt::FromIncluding(c).ToExcluding(d);
	    tInts1[3] = tInt::FromExcluding(a).ToIncluding(b);
	    tInts2[3] = tInt::FromExcluding(c).ToIncluding(d);

	    for (typename vInt::size_type i = 0; i < 4; ++i) {
		for (typename vInt::size_type j = 0; j < 4; ++j) {
		    CPPUNIT_ASSERT( tInts1[i] != tInts2[j] );
		    CPPUNIT_ASSERT( tInts2[j] != tInts1[i] );
		    CPPUNIT_ASSERT( !tInts1[i].isBelow(tInts2[j]) );
		    CPPUNIT_ASSERT( !tInts2[j].isBelow(tInts1[i]) );
		    CPPUNIT_ASSERT( !tInts1[i].isAbove(tInts2[j]) );
		    CPPUNIT_ASSERT( !tInts2[j].isAbove(tInts1[i]) );
		    CPPUNIT_ASSERT( tInts1[i].contains(tInts2[j]) );
		    CPPUNIT_ASSERT( !tInts2[j].contains(tInts1[i]) );
		    CPPUNIT_ASSERT( tInts1[i].overlaps(tInts2[j]) );
		    CPPUNIT_ASSERT( tInts2[j].overlaps(tInts1[i]) );
		    CPPUNIT_ASSERT( tInts1[i].joinableWith(tInts2[j]) );
		    CPPUNIT_ASSERT( tInts2[j].joinableWith(tInts1[i]) );
		    CPPUNIT_ASSERT( tInts1[i].join(tInts2[j]) == tInts1[i] );
		    CPPUNIT_ASSERT( tInts2[j].join(tInts1[i]) == tInts1[i] );
		    CPPUNIT_ASSERT( tInts1[i].intersect(tInts2[j]) == tInts2[j] );
		    CPPUNIT_ASSERT( tInts2[j].intersect(tInts1[i]) == tInts2[j] );
		}
	    }
	}
    };
}

#endif // WNS_INTERVALTEST_HPP
