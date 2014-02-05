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

#include <WNS/SmartPtr.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <set>
#include <map>
#include <list>
#include <stdexcept>
#include <algorithm>

namespace wns { namespace tests {

	class SmartPtrTest :
		public CppUnit::TestFixture
	{
	protected:
		// begin example "wns.RefCountableDerive.example"
		class A :
			virtual public RefCountable
		{
		public:
			A()
			{}

			long int foo()
			{
				return 42;
			};
		};
		// end example

		class B :
			public A
		{
		public:
			B(SmartPtrTest* SPT) :
				A(),
				spt(SPT)
			{}

			virtual
			~B()
			{
				spt->destructorHasBeenCalled = true;
			}

			SmartPtrTest* spt;
		};

		class C :
			public A
		{
		public:
			C() :
				A()
			{}
		};

		class D :
			public B,
			public C
		{
		public:
			D(SmartPtrTest* SPT) :
				B(SPT),
				C()
			{}
		};

		class E :
			public virtual RefCountable
		{
		public:
			E() :
				executed(false)
			{}

			void
			execute()
			{
				executed = true;
			}
			bool executed;
		};

		class F2;
		class F1 :
			public virtual RefCountable
		{
		public:
			F1()
			{}
			~F1()
			{
			  // should never occur in this test
			  std::cout<<"F1:~F1() called. Should not happen."<<std::endl;
			}

			SmartPtr<F2> peer;
		};
		class F2 :
			public virtual RefCountable
		{
		public:
			F2()
			{}
			~F2()
			{
			  std::cout<<"F1:~F1() called"<<std::endl;
			}

			SmartPtr<F1> peer;
		};

		CPPUNIT_TEST_SUITE( SmartPtrTest );
		CPPUNIT_TEST( refCountable );
		CPPUNIT_TEST( STLContainer );
		CPPUNIT_TEST( constructor );
		CPPUNIT_TEST( operators );
		CPPUNIT_TEST( deleteAndDestroy );
		CPPUNIT_TEST( upCast );
		CPPUNIT_TEST( dynamicCastTest );
		CPPUNIT_TEST( staticCastTest );
		CPPUNIT_TEST( stlContainer );
		CPPUNIT_TEST( functionAdapterTest );
		CPPUNIT_TEST( isNull );
		CPPUNIT_TEST( cyclicDependencyTest );
		CPPUNIT_TEST_SUITE_END();
	public:
		bool destructorHasBeenCalled;
		void setUp();
		void tearDown();
		void refCountable();
		void STLContainer();
		void constructor();
		void operators();
		void deleteAndDestroy();
		void upCast();
		void dynamicCastTest();
		void staticCastTest();
		void stlContainer();
		void classDerivedFromSPEETLObject();
		void functionAdapterTest();
		void isNull();
		void cyclicDependencyTest();
	};


	CPPUNIT_TEST_SUITE_REGISTRATION( SmartPtrTest );

	void SmartPtrTest::setUp()
	{
	}

	void SmartPtrTest::tearDown()
	{
	}

	void SmartPtrTest::refCountable()
	{
		A a;
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(0), a.getRefCount() );
	}

	void SmartPtrTest::STLContainer()
	{
		SmartPtr<A> spt(new A);
		std::map<SmartPtr<A>, int> container;
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), spt.getPtr()->getRefCount() );
		container[spt] = 5;
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(2), spt.getPtr()->getRefCount() );
		container.clear();
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), spt.getPtr()->getRefCount() );
	}

	void SmartPtrTest::constructor()
	{
		{
			SmartPtr<A> null;
			CPPUNIT_ASSERT( !null.getPtr() );
		}

		{
			SmartPtr<A> a(new A);
			CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), a.getRefCount() );
		}
		{
			SmartPtr<A> a(new A);
			CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), a.getRefCount() );
			SmartPtr<A> a2(a);
			CPPUNIT_ASSERT_EQUAL( static_cast<long int>(2), a.getRefCount() );
			CPPUNIT_ASSERT_EQUAL( static_cast<long int>(2), a2.getRefCount() );
		}

		{
			A* aPtr = new A;
			SmartPtr<A> a(aPtr);
			CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), a.getRefCount() );
		}
	}

	void SmartPtrTest::operators()
	{
		SmartPtr<A> a(new A);
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(42), a->foo() );
		SmartPtr<A> a2 = a;
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(2), a2.getRefCount() );
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(2), a.getRefCount() );
		CPPUNIT_ASSERT( a==a2 );
		a2 = SmartPtr<A>(new A);
		CPPUNIT_ASSERT( a!=a2 );
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), a2.getRefCount() );
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), a.getRefCount() );
		A copyOfA = *a;
		// make sure a copy of the object has refcount 0;
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(0), copyOfA.getRefCount() );

		CPPUNIT_ASSERT( a );
		SmartPtr<A> a3;
		CPPUNIT_ASSERT( !a3 );
	}

	void SmartPtrTest::deleteAndDestroy()
	{
		destructorHasBeenCalled = false;
		B* bPtr = new B(this);
		CPPUNIT_ASSERT( !destructorHasBeenCalled );
		{
			SmartPtr<B> b(bPtr);
			CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), b.getRefCount() );
			CPPUNIT_ASSERT( !destructorHasBeenCalled );
			{
				SmartPtr<B> b2 = b;
				CPPUNIT_ASSERT_EQUAL( static_cast<long int>(2), b2.getRefCount() );
				CPPUNIT_ASSERT_EQUAL( static_cast<long int>(2), b.getRefCount() );
				CPPUNIT_ASSERT( !destructorHasBeenCalled );
			}
			CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), b.getRefCount() );
			CPPUNIT_ASSERT( !destructorHasBeenCalled );
		}
		CPPUNIT_ASSERT( destructorHasBeenCalled );

		destructorHasBeenCalled = false;
		D* dPtr = new D(this);
		{
			SmartPtr<D> d(dPtr);
			CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), d.getRefCount() );
		}
		CPPUNIT_ASSERT( destructorHasBeenCalled );
	}

	void SmartPtrTest::upCast()
	{
		SmartPtr<A> a(new C);
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), a.getRefCount() );
		SmartPtr<C> c(new C);
		CPPUNIT_ASSERT( a!=c );
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), c.getRefCount() );
		a = c;
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(2), a.getRefCount() );
		CPPUNIT_ASSERT_EQUAL( static_cast<long int>(2), c.getRefCount() );
		CPPUNIT_ASSERT( a==c );
	}

	void SmartPtrTest::dynamicCastTest()
	{
		C* c = new C;
		A* a = c;
		SmartPtr<C> cSP(c);
		SmartPtr<A> aSP;
		aSP = cSP;
		CPPUNIT_ASSERT( aSP==cSP );
		SmartPtr<C> cDynamicCastedSP = dynamicCast<C>(aSP);
		CPPUNIT_ASSERT( aSP==cSP );
		CPPUNIT_ASSERT( aSP==cDynamicCastedSP );
		// should also work for normal pointers
		C* cDynamicCasted = dynamicCast<C>(a);
		CPPUNIT_ASSERT( a==c );
		CPPUNIT_ASSERT( a==cDynamicCasted );
	}

	void SmartPtrTest::staticCastTest()
	{
		C* c = new C;
		A* a = c;
		SmartPtr<C> cSP(c);
		SmartPtr<A> aSP;
		aSP = cSP;
		CPPUNIT_ASSERT( aSP==cSP );
		SmartPtr<C> cStaticCastedSP = staticCast<C>(aSP);
		CPPUNIT_ASSERT( aSP==cSP );
		CPPUNIT_ASSERT( aSP==cStaticCastedSP );
		// should also work for normal pointers
		C* cStaticCasted = staticCast<C>(a);
		CPPUNIT_ASSERT( a==c );
		CPPUNIT_ASSERT( a==cStaticCasted );
	}

	void SmartPtrTest::stlContainer()
	{
		SmartPtr<A> a(new A);
		SmartPtr<A> aCopy = a;
		SmartPtr<A> otherA(new A);

		std::set< SmartPtr<A> > as;

		CPPUNIT_ASSERT(as.find(a) == as.end());
		CPPUNIT_ASSERT(as.find(aCopy) == as.end());
		CPPUNIT_ASSERT(as.find(otherA) == as.end());

		as.insert(a);
		CPPUNIT_ASSERT_EQUAL(1, (int)as.size());
		CPPUNIT_ASSERT(as.find(a) != as.end());
		CPPUNIT_ASSERT(as.find(aCopy) != as.end());
		CPPUNIT_ASSERT(as.find(otherA) == as.end());

		as.insert(aCopy);
		CPPUNIT_ASSERT_EQUAL(1, (int)as.size());
		CPPUNIT_ASSERT(as.find(a) != as.end());
		CPPUNIT_ASSERT(as.find(aCopy) != as.end());
		CPPUNIT_ASSERT(as.find(otherA) == as.end());

		as.insert(otherA);
		CPPUNIT_ASSERT_EQUAL(2, (int)as.size());
		CPPUNIT_ASSERT(as.find(a) != as.end());
		CPPUNIT_ASSERT(as.find(aCopy) != as.end());
		CPPUNIT_ASSERT(as.find(otherA) != as.end());
	}

	void SmartPtrTest::functionAdapterTest()
	{
		std::list< SmartPtr<E> > container;

		container.push_back( SmartPtr<E>( new E() ) );
		container.push_back( SmartPtr<E>( new E() ) );
		container.push_back( SmartPtr<E>( new E() ) );

		for_each( container.begin(), container.end(),
			  smart_ptr_mem_fun(&E::execute) );

		for ( std::list<SmartPtr<E> >::const_iterator it = container.begin();
		      it != container.end();
		      ++it )
		{
			CPPUNIT_ASSERT( (*it)->executed );
		}
	}

	void SmartPtrTest::isNull()
	{
		SmartPtr<E> bar;
		CPPUNIT_ASSERT( NULL == bar );

		bar = SmartPtr<E>(new E);
		CPPUNIT_ASSERT( bar != NULL );
	}

    // (cd tests/unit/unitTests/; ./openwns -v -s -y "WNS.masterLogger.enabled = True"  -tT wns::tests::SmartPtrTest::cyclicDependencyTest)
	void SmartPtrTest::cyclicDependencyTest()
	{
	  std::cout<<std::endl;
	  SmartPtr<F1> object1 = SmartPtr<F1>(new F1);
	  SmartPtr<F2> object2 = SmartPtr<F2>(new F2);
	  //std::cout<<"cyclicDependencyTest: object1.getRefCount()="<<object1.getRefCount()<<std::endl;
	  //std::cout<<"cyclicDependencyTest: object2.getRefCount()="<<object1.getRefCount()<<std::endl;
	  object1->peer = object2;
	  object2->peer = object1;
	  //std::cout<<"cyclicDependencyTest: object1.getRefCount()="<<object1.getRefCount()<<std::endl;
	  //std::cout<<"cyclicDependencyTest: object2.getRefCount()="<<object1.getRefCount()<<std::endl;
	  CPPUNIT_ASSERT_EQUAL( static_cast<long int>(2), object1.getRefCount() );
	  CPPUNIT_ASSERT_EQUAL( static_cast<long int>(2), object2.getRefCount() );
	  object1 = SmartPtr<F1>(); // empty
	  CPPUNIT_ASSERT_EQUAL( static_cast<long int>(1), object2->peer.getRefCount() );
	  object2 = SmartPtr<F2>(); // empty
	  // here the SmartPtrs do not call their contained destructors automatically
	  // because there is a cyclic dependency
	  // You can find that with valgrind --tool=memcheck --leak-check=yes --num-callers=20 --leak-resolution=high --log-file=val.log
	  //std::cout<<"cyclicDependencyTest: object1.getRefCount()="<<object1.getRefCount()<<std::endl;
	  //std::cout<<"cyclicDependencyTest: object2.getRefCount()="<<object1.getRefCount()<<std::endl;
	}

} // tests
} // wns


