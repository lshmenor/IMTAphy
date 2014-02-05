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

#ifndef WNS_MODULE_TEST_MULTITYPEFACTORYTEST_HPP
#define WNS_MODULE_TEST_MULTITYPEFACTORYTEST_HPP

#include <WNS/module/MultiTypeFactory.hpp>
#include <WNS/Exception.hpp>
#include <WNS/TestFixture.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns { namespace module { namespace tests {
	class MultiTypeFactoryTest :
		public wns::TestFixture
	{
		// Classes for testing
		class A
		{
		public:
			virtual int
			test()
			{
				return 1;
			}
			virtual ~A() {};
		};

		class B
		{
		public:
			virtual int
			test()
			{
				return 2;
			}
			virtual ~B() {};
		};

		class C
			: public A
		{
		public:
			virtual int
			test()
			{
				return 3;
			}
		};

		class TestFactory
			: public MultiTypeFactory<std::string>
		{
		protected:
			ChamaeleonBase* create(const std::string& s)
			{
				if(s=="A") {
					return new Chamaeleon<A*>(new A);
				} else if (s=="B") {
					return new Chamaeleon<B*>(new B);
				} else if (s=="C") {
					return new Chamaeleon<C*>(new C);
				} else if (s=="C_A") {
					return new Chamaeleon<A*>(new C);
				}
				return NULL;
			}
		};

		CPPUNIT_TEST_SUITE( MultiTypeFactoryTest );
		CPPUNIT_TEST( getterTest );
		CPPUNIT_TEST( wrongType );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();
		void getterTest();
		void wrongType();
	private:
	};
}
}
}
#endif // NOT defined WNS_MODULE_TEST_MULTITYPEFACTORYTEST_HPP
