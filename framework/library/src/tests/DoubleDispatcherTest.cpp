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


#include <WNS/tests/DoubleDispatcherTest.hpp>

using namespace std;
using namespace wns;

CPPUNIT_TEST_SUITE_REGISTRATION( DoubleDispatcherTest );

void
DoubleDispatcherTest::setUp()
{
}

void
DoubleDispatcherTest::tearDown()
{
}

void
DoubleDispatcherTest::dispatch()
{
	AnotherDerivedReceiver adr = AnotherDerivedReceiver();
	DerivedReceiver dr = DerivedReceiver();

	CPPUNIT_ASSERT_EQUAL( 0, (int)dr.getDoubleDispatcher().cm.size() );
	CPPUNIT_ASSERT_EQUAL( 0, (int)adr.getDoubleDispatcher().cm.size() );
	CPPUNIT_ASSERT_EQUAL( 0, (int)dr.getDoubleDispatcher2().cm.size() );
	CPPUNIT_ASSERT_EQUAL( 0, (int)adr.getDoubleDispatcher2().cm.size() );
	CPPUNIT_ASSERT_EQUAL( 0, (int)dr.getDoubleDispatcher3().cm.size() );
 	CPPUNIT_ASSERT_EQUAL( 0, (int)adr.getDoubleDispatcher3().cm.size() );
	CPPUNIT_ASSERT_EQUAL( 0, (int)dr.getDoubleDispatcher4().cm.size() );
	CPPUNIT_ASSERT_EQUAL( 0, (int)adr.getDoubleDispatcher4().cm.size() );

	DerivedReceiver::DD& dd = dr.getDoubleDispatcher();
	dd.addMemberFunction<B, &DerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 1, (int)dd.cm.size() );
	dd.addMemberFunction<C, &DerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 2, (int)dd.cm.size() );
	dd.addMemberFunction<A, &DerivedReceiver::handle>();

	adr.getDoubleDispatcher().addMemberFunction<B, &AnotherDerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 1, (int)adr.getDoubleDispatcher().cm.size() );
	adr.getDoubleDispatcher().addMemberFunction<C, &AnotherDerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 2, (int)adr.getDoubleDispatcher().cm.size() );
	adr.getDoubleDispatcher().addMemberFunction<A, &AnotherDerivedReceiver::handle>();

	dr.getDoubleDispatcher2().addMemberFunction<B, &DerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 1, (int)dr.getDoubleDispatcher2().cm.size() );
  	dr.getDoubleDispatcher2().addMemberFunction<C, &DerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 2, (int)dr.getDoubleDispatcher2().cm.size() );
  	dr.getDoubleDispatcher2().addMemberFunction<A, &DerivedReceiver::handle>();

  	adr.getDoubleDispatcher2().addMemberFunction<B, &AnotherDerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 1, (int)adr.getDoubleDispatcher2().cm.size() );
  	adr.getDoubleDispatcher2().addMemberFunction<C, &AnotherDerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 2, (int)adr.getDoubleDispatcher2().cm.size() );
  	adr.getDoubleDispatcher2().addMemberFunction<A, &AnotherDerivedReceiver::handle>();

 	dr.getDoubleDispatcher3().addMemberFunction<B, &DerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 1, (int)dr.getDoubleDispatcher3().cm.size() );
 	dr.getDoubleDispatcher3().addMemberFunction<C, &DerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 2, (int)dr.getDoubleDispatcher3().cm.size() );
 	dr.getDoubleDispatcher3().addMemberFunction<A, &DerivedReceiver::handle>();

   	adr.getDoubleDispatcher3().addMemberFunction<B, &AnotherDerivedReceiver::handle>();
  	CPPUNIT_ASSERT_EQUAL( 1, (int)adr.getDoubleDispatcher3().cm.size() );
   	adr.getDoubleDispatcher3().addMemberFunction<C, &AnotherDerivedReceiver::handle>();
  	CPPUNIT_ASSERT_EQUAL( 2, (int)adr.getDoubleDispatcher3().cm.size() );
   	adr.getDoubleDispatcher3().addMemberFunction<A, &AnotherDerivedReceiver::handle>();

 	dr.getDoubleDispatcher4().addMemberFunction<B, &DerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 1, (int)dr.getDoubleDispatcher4().cm.size() );
 	dr.getDoubleDispatcher4().addMemberFunction<C, &DerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 2, (int)dr.getDoubleDispatcher4().cm.size() );
 	dr.getDoubleDispatcher4().addMemberFunction<A, &DerivedReceiver::handle>();

 	adr.getDoubleDispatcher4().addMemberFunction<B, &AnotherDerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 1, (int)adr.getDoubleDispatcher4().cm.size() );
 	adr.getDoubleDispatcher4().addMemberFunction<C, &AnotherDerivedReceiver::handle>();
	CPPUNIT_ASSERT_EQUAL( 2, (int)adr.getDoubleDispatcher4().cm.size() );
 	adr.getDoubleDispatcher4().addMemberFunction<A, &AnotherDerivedReceiver::handle>();

	A a;
	B b;
	C c;
	D d;
 	A* aPtr = new A;
 	B* bPtr = new B;
 	C* cPtr = new C;
 	D* dPtr = new D;
	SmartPtr<A> aSPtr(aPtr);
	SmartPtr<B> bSPtr(bPtr);
	SmartPtr<C> cSPtr(cPtr);
	SmartPtr<D> dSPtr(dPtr);

	CPPUNIT_ASSERT_EQUAL( 2, dr.receive(b) );
	CPPUNIT_ASSERT_EQUAL( 3, dr.receive(c) );
	CPPUNIT_ASSERT_EQUAL( 4, adr.receive(b) );
	CPPUNIT_ASSERT_EQUAL( 5, adr.receive(c) );

	CPPUNIT_ASSERT_EQUAL( 2, dr.receive2(b) );
	CPPUNIT_ASSERT_EQUAL( 3, dr.receive2(c) );
	CPPUNIT_ASSERT_EQUAL( 4, adr.receive2(b) );
	CPPUNIT_ASSERT_EQUAL( 5, adr.receive2(c) );

 	CPPUNIT_ASSERT_EQUAL( 6, dr.receive3(bPtr) );
 	CPPUNIT_ASSERT_EQUAL( 7, dr.receive3(cPtr) );
 	CPPUNIT_ASSERT_EQUAL( 8, adr.receive3(bPtr) );
 	CPPUNIT_ASSERT_EQUAL( 9, adr.receive3(cPtr) );

 	CPPUNIT_ASSERT_EQUAL( 6, dr.receive4(bPtr) );
 	CPPUNIT_ASSERT_EQUAL( 7, dr.receive4(cPtr) );
 	CPPUNIT_ASSERT_EQUAL( 8, adr.receive4(bPtr) );
 	CPPUNIT_ASSERT_EQUAL( 9, adr.receive4(cPtr) );

 	CPPUNIT_ASSERT_EQUAL( 10, dr.receive5(b) );
 	CPPUNIT_ASSERT_EQUAL( 11, dr.receive5(c) );
   	CPPUNIT_ASSERT_EQUAL( 12, adr.receive5(b) );
   	CPPUNIT_ASSERT_EQUAL( 13, adr.receive5(c) );

 	CPPUNIT_ASSERT_EQUAL( 14, dr.receive6(bSPtr) );
 	CPPUNIT_ASSERT_EQUAL( 15, dr.receive6(cSPtr) );
 	CPPUNIT_ASSERT_EQUAL( 16, adr.receive6(bSPtr) );
 	CPPUNIT_ASSERT_EQUAL( 17, adr.receive6(cSPtr) );



	// Check default method
	CPPUNIT_ASSERT_EQUAL( 18, dr.receive(a) );
	CPPUNIT_ASSERT_EQUAL( 18, dr.receive(d) );
	CPPUNIT_ASSERT_EQUAL( 22, adr.receive(a) );
	CPPUNIT_ASSERT_EQUAL( 22, adr.receive(d) );

	CPPUNIT_ASSERT_EQUAL( 18, dr.receive2(a) );
	CPPUNIT_ASSERT_EQUAL( 18, dr.receive2(d) );
	CPPUNIT_ASSERT_EQUAL( 22, adr.receive2(a) );
	CPPUNIT_ASSERT_EQUAL( 22, adr.receive2(d) );

 	CPPUNIT_ASSERT_EQUAL( 19, dr.receive3(aPtr) );
 	CPPUNIT_ASSERT_EQUAL( 19, dr.receive3(dPtr) );
 	CPPUNIT_ASSERT_EQUAL( 23, adr.receive3(aPtr) );
 	CPPUNIT_ASSERT_EQUAL( 23, adr.receive3(dPtr) );

 	CPPUNIT_ASSERT_EQUAL( 19, dr.receive4(aPtr) );
 	CPPUNIT_ASSERT_EQUAL( 19, dr.receive4(dPtr) );
 	CPPUNIT_ASSERT_EQUAL( 23, adr.receive4(aPtr) );
 	CPPUNIT_ASSERT_EQUAL( 23, adr.receive4(dPtr) );

 	CPPUNIT_ASSERT_EQUAL( 20, dr.receive5(a) );
 	CPPUNIT_ASSERT_EQUAL( 20, dr.receive5(d) );
   	CPPUNIT_ASSERT_EQUAL( 24, adr.receive5(a) );
   	CPPUNIT_ASSERT_EQUAL( 24, adr.receive5(d) );

 	CPPUNIT_ASSERT_EQUAL( 21, dr.receive6(aSPtr) );
 	CPPUNIT_ASSERT_EQUAL( 21, dr.receive6(dSPtr) );
 	CPPUNIT_ASSERT_EQUAL( 25, adr.receive6(aSPtr) );
 	CPPUNIT_ASSERT_EQUAL( 25, adr.receive6(dSPtr) );
}



