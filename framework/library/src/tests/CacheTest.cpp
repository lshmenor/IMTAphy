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

#include <WNS/tests/CacheTest.hpp>

using namespace std;
using namespace wns;

CPPUNIT_TEST_SUITE_REGISTRATION( CacheTest );

void CacheTest::setUp()
{
}

void CacheTest::tearDown()
{
}

void CacheTest::testSetClean() {
	d.setDirty();
	p.setDirty();
	d.setClean();
	p.setClean();

	CPPUNIT_ASSERT( !d.isDirty() );
	CPPUNIT_ASSERT( !p.isDirty() );
}

void CacheTest::testSetDirty() {
	d.setClean();
	p.setClean();
	d.setDirty();
	p.setDirty();

	CPPUNIT_ASSERT( d.isDirty() );
	CPPUNIT_ASSERT( p.isDirty() );
}


void CacheTest::testContent() {
	d = 4;
	p = Power::from_mW(3);
	CPPUNIT_ASSERT( d==4 );
	CPPUNIT_ASSERT( p==Power::from_mW(3) );
	d += 4;
	p += Power::from_mW(3);
	CPPUNIT_ASSERT( d==8 );
	CPPUNIT_ASSERT( p==Power::from_mW(6) );
}

void CacheTest::testImplicitSetClean() {
	d.setDirty();
	d = 2;
	CPPUNIT_ASSERT( !d.isDirty() );

	d.setDirty();
	d += 2;
	CPPUNIT_ASSERT( !d.isDirty() );

	d.setDirty();
	d -= 2;
	CPPUNIT_ASSERT( !d.isDirty() );

	d.setDirty();
	d *= 2;
	CPPUNIT_ASSERT( !d.isDirty() );

	d.setDirty();
	d /= 2;
	CPPUNIT_ASSERT( !d.isDirty() );

	d.setDirty();
	d %= 2;
	CPPUNIT_ASSERT( !d.isDirty() );

	d.setDirty();
	d ^= 2;
	CPPUNIT_ASSERT( !d.isDirty() );

	d.setDirty();
	d &= 2;
	CPPUNIT_ASSERT( !d.isDirty() );

	d.setDirty();
	d |= 2;
	CPPUNIT_ASSERT( !d.isDirty() );

	d.setDirty();
	d <<= 2;
	CPPUNIT_ASSERT( !d.isDirty() );

	d.setDirty();
	d >>= 2;
	CPPUNIT_ASSERT( !d.isDirty() );

	p.setDirty();
	p = Power::from_mW(2);
	CPPUNIT_ASSERT( !p.isDirty() );

	p.setDirty();
	p += Power::from_mW(2);
	CPPUNIT_ASSERT( !p.isDirty() );

	p.setDirty();
	p -= Power::from_mW(2);
	CPPUNIT_ASSERT( !p.isDirty() );
}


