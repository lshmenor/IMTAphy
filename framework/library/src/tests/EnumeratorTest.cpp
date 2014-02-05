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

#include <vector>

#include <WNS/tests/EnumeratorTest.hpp>

using namespace wns;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( EnumeratorTest );

void EnumeratorTest::setUp()
{
}

void EnumeratorTest::tearDown()
{
}

void EnumeratorTest::testEnumeratorInt()
{
    CPPUNIT_ASSERT( Enumerator::succ((unsigned char) 0) == 1 );
    CPPUNIT_ASSERT( Enumerator::succ((unsigned char) 255) == 0 );
    CPPUNIT_ASSERT( Enumerator::pred((unsigned char) 1) == 0 );
    CPPUNIT_ASSERT( Enumerator::pred((unsigned char) 0) == 255 );
    CPPUNIT_ASSERT( Enumerator::succ((signed char) -1) == 0 );
    CPPUNIT_ASSERT( Enumerator::succ((signed char) 127) == -128 );
    CPPUNIT_ASSERT( Enumerator::pred((signed char) 0) == -1 );
    CPPUNIT_ASSERT( Enumerator::pred((signed char) -128) == 127 );
}

void EnumeratorTest::testEnumerableInt()
{
    typedef Enumerable<unsigned char> MyUInt;
    CPPUNIT_ASSERT( ++MyUInt(0) == 1 );
    CPPUNIT_ASSERT( ++MyUInt(255) == 0 );
    CPPUNIT_ASSERT( --MyUInt(1) == 0 );
    CPPUNIT_ASSERT( --MyUInt(0) == 255 );

    typedef Enumerable<signed char> MySInt;
    CPPUNIT_ASSERT( ++MySInt(-1) == 0 );
    CPPUNIT_ASSERT( ++MySInt(127) == -128 );
    CPPUNIT_ASSERT( --MySInt(0) == -1 );
    CPPUNIT_ASSERT( --MySInt(-128) == 127 );

    MySInt i(0);
    ++i;
    CPPUNIT_ASSERT( i == 1 );

    unsigned char uc = 0;
    CPPUNIT_ASSERT( ++makeEnumerable(uc) == 1 );
    CPPUNIT_ASSERT( --makeEnumerable(uc) == 255 );
}

void EnumeratorTest::testEnumeratorClass()
{
    vector<int> v(2);
    v[0] = 1; v[1] = 2;

    vector<int>::iterator i = v.begin();

    CPPUNIT_ASSERT(*Enumerator::succ(i) == 2);
    i = v.begin()+1;
    CPPUNIT_ASSERT(*Enumerator::pred(i) == 1);
}

void EnumeratorTest::testEnumerableClass()
{
    typedef vector<int> MyVector;
    typedef Enumerable<MyVector::iterator> MyEnumerable;

    MyVector v(2);
    v[0] = 1; v[1] = 2;

    MyEnumerable i = MyEnumerable(v.begin());

    CPPUNIT_ASSERT(*(++i) == 2);
    CPPUNIT_ASSERT(*(i.get()) == 2);
    i = MyEnumerable( v.begin()+1 );
    CPPUNIT_ASSERT(*(--i) == 1);
}

