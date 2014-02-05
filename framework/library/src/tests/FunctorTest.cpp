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

#include <WNS/Functor.hpp>
#include <WNS/TestFixture.hpp>

#include <vector>
#include <algorithm>
#include <memory>

namespace wns { namespace tests {
	class FunctorTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( FunctorTest );
		CPPUNIT_TEST( TestFunctor );
		CPPUNIT_TEST( TestArgumentFunctor );
		CPPUNIT_TEST_SUITE_END();
	public:
		void
		prepare();

		void
		cleanup();

		void
		TestFunctor();

		void
		TestArgumentFunctor();

	private:
		// test class
		class A
		{
			bool member;
		public:
			A() :
				member(false)
			{}

			// membermber functions which take ...

			// a) no argument
			void
			toggle()
			{
				member = (!member);
			}

			// b) a POD as argument
			void
			set(const bool& value)
			{
				member = value;
			}

			// c) a Ptr as argument
			void
			copy(const A* a)
			{
				member = a->member;
			}

			// d) return a value and are const
  			bool
			get() const
			{
				return member;
			}

			// e) take an object as argument and return a value
			bool
			exor(const bool& value)
			{
				return member = (member && !value) || (!member && value);
			}

			// just to retrieve A's state
			bool
			operator()() const
			{
				return member;
			}
		};
	};



	CPPUNIT_TEST_SUITE_REGISTRATION( FunctorTest );

	void
	FunctorTest::prepare()
	{
	}

	void
	FunctorTest::cleanup()
	{
	}


	void
	FunctorTest::TestFunctor()
	{
		std::auto_ptr<A> temp(new A());
		temp->set(false);

		CPPUNIT_ASSERT(  temp->get() == false );
		wns::TFunctor<A> myFunctor(temp.get(), &A::toggle);

		myFunctor();

		CPPUNIT_ASSERT( temp->get() );

		wns::TFunctor<A, bool, tfunctor::constFunc> myRetValFunctor(temp.get(), &A::get);

		CPPUNIT_ASSERT( myRetValFunctor() );
	}

	void
	FunctorTest::TestArgumentFunctor()
	{
		std::auto_ptr<A> temp(new A());
		temp->set(false);

		CPPUNIT_ASSERT( temp->get() == false );

		wns::TArgumentFunctor<A, const bool&> myFunctor(temp.get(), &A::set);

		myFunctor(true);

		CPPUNIT_ASSERT( temp->get() );

		wns::TArgumentFunctor<A, const bool&, bool> myRetValFunctor(temp.get(), &A::exor);

		CPPUNIT_ASSERT( myRetValFunctor(false) ); // should set and return true
		CPPUNIT_ASSERT( !myRetValFunctor(true) ); // should then return false
	}
} // tests
} // wns


