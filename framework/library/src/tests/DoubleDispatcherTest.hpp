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

#ifndef classDoubleDispatcherTest
#define classDoubleDispatcherTest

#include <WNS/DoubleDispatcher.hpp>
#include <WNS/SmartPtr.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns {
	class DoubleDispatcherTest
		: public CppUnit::TestFixture
	{

		// For testing purpose
		class A
			: virtual public RefCountable
		{
		public:
			virtual ~A()
				{}
		};

		class B
			: public A
		{};

		class C
			: public A
		{};

		class D
			: public A
		{};

		class BaseReceiver
		{
		public:
			BaseReceiver()
				{}

			virtual ~BaseReceiver()
				{}

			virtual int receive(A& a) = 0;
			virtual int receive2(A& a) = 0;
			virtual int receive3(A* a) = 0;
			virtual int receive4(A* a) = 0;
			virtual int receive5(const A& a) = 0;
			virtual int receive6(const SmartPtr<A>& a) = 0;
		};

		class DerivedReceiver
			: public BaseReceiver
		{
		public:
			typedef DoubleDispatcher<DerivedReceiver, A&, int> DD;
			typedef DoubleDispatcher<DerivedReceiver, A*, int> DD2;
			typedef DoubleDispatcher<DerivedReceiver, const A&, int> DD3;
			typedef DoubleDispatcher<DerivedReceiver, const SmartPtr<A>&, int> DD4;

			DerivedReceiver()
				: BaseReceiver(),
				  dd(DD()),
				  dd2(DD2()),
				  dd3(DD3()),
				  dd4(DD4())
				{}


			int	handle(B&){return 2;}
			int	handle(C&){return 3;}
			int	handle(B*){return 6;}
			int	handle(C*){return 7;}
			int	handle(const B&){return 10;}
			int	handle(const C&){return 11;}
			int	handle(const SmartPtr<B>&){return 14;}
			int	handle(const SmartPtr<C>&){return 15;}
			int	handle(A&){return 18;}
			int	handle(A*){return 19;}
			int	handle(const A&){return 20;}
			int	handle(const SmartPtr<A>&){return 21;}

			DD& getDoubleDispatcher() { return dd; }
			DD2& getDoubleDispatcher2() { return dd2; }
			DD3& getDoubleDispatcher3()	{ return dd3; }
			DD4& getDoubleDispatcher4()	{ return dd4; }

			int receive(A& a) {return getDoubleDispatcher().dispatch(this, a);};
			int receive2(A& a) {return getDoubleDispatcher().dispatch(*this, a);};
			int receive3(A* a) {return getDoubleDispatcher2().dispatch(this, a);};
			int receive4(A* a) {return getDoubleDispatcher2().dispatch(*this, a);};
			int receive5(const A& a) {return getDoubleDispatcher3().dispatch(this, a);};
			int receive6(const SmartPtr<A>& a) {return getDoubleDispatcher4().dispatch(this, a);};

		private:
			DD dd;
			DD2 dd2;
			DD3 dd3;
			DD4 dd4;
		};

		class AnotherDerivedReceiver
			: public BaseReceiver
		{
		public:
			typedef DoubleDispatcher<AnotherDerivedReceiver, A&, int> DD;
			typedef DoubleDispatcher<AnotherDerivedReceiver, A*, int> DD2;
			typedef DoubleDispatcher<AnotherDerivedReceiver, const A&, int> DD3;
			typedef DoubleDispatcher<AnotherDerivedReceiver, const SmartPtr<A>&, int> DD4;

			AnotherDerivedReceiver()
				: BaseReceiver(),
				  dd(DD()),
				  dd2(DD2()),
				  dd3(DD3()),
				  dd4(DD4())
				{}

			int	handle(B&){return 4;}
			int	handle(C&){return 5;}
			int	handle(B*){return 8;}
			int	handle(C*){return 9;}
			int	handle(const B&){return 12;}
			int	handle(const C&){return 13;}
			int	handle(const SmartPtr<B>&){return 16;}
			int	handle(const SmartPtr<C>&){return 17;}
			int	handle(A&){return 22;}
			int	handle(A*){return 23;}
			int	handle(const A&){return 24;}
			int	handle(const SmartPtr<A>&){return 25;}

			DD& getDoubleDispatcher() { return dd; }
			DD2& getDoubleDispatcher2() { return dd2; }
			DD3& getDoubleDispatcher3()	{ return dd3; }
			DD4& getDoubleDispatcher4()	{ return dd4; }

			int receive(A& a) {return getDoubleDispatcher().dispatch(this, a);};
			int receive2(A& a) {return getDoubleDispatcher().dispatch(*this, a);};
			int receive3(A* a) {return getDoubleDispatcher2().dispatch(this, a);};
			int receive4(A* a) {return getDoubleDispatcher2().dispatch(*this, a);};
			int receive5(const A& a) {return getDoubleDispatcher3().dispatch(this, a);};
			int receive6(const SmartPtr<A>& a) {return getDoubleDispatcher4().dispatch(this, a);};
		private:
			DD dd;
			DD2 dd2;
			DD3 dd3;
			DD4 dd4;
		};
		CPPUNIT_TEST_SUITE( DoubleDispatcherTest );
		CPPUNIT_TEST( dispatch );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();
		void dispatch();
	};
}

#endif


