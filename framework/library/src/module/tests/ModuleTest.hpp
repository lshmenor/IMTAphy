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

#ifndef WNS_MODULE_TESTS_MODULETEST_HPP
#define WNS_MODULE_TESTS_MODULETEST_HPP

#include <WNS/module/Module.hpp>
#include <WNS/TestFixture.hpp>
#include <WNS/Exception.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns { namespace module { namespace tests {
	class ModuleTest :
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


		// begin example "module::ModuleClass.example"
		class Module1
		// Tell the module::Module who you are!
			: public Module<Module1>
		{
		public:
			Module1(const pyconfig::View& _pyConfigView)
				// Call the constructor of the base class
				: Module<Module1>(_pyConfigView)
			{
				// register service A and B
 				MODULE_REGISTER_SERVICE(A*, "A", Module1::createA);
				MODULE_REGISTER_SERVICE(B*, "B", Module1::createB);
			}

			// this method is called if an oject of type A is needed
			A* createA()
			{
				return new A;
			}

			// this method is called if an oject of type B is needed
			B* createB()
			{
				return new B;
			}

			// the next three method have to be overloaded, since
			// they are abstract (pure virtual) in the base class
			virtual void
			configure()
			{}

			virtual void
			shutDown()
			{}
		};
		// end example

		class Module2
			: public Module<Module2>
		{
		public:
			Module2(const pyconfig::View& _pyConfigView)
				: Module<Module2>(_pyConfigView)
			{
				MODULE_REGISTER_SERVICE(B*, "B2", Module2::createB);
				MODULE_REGISTER_SERVICE(C*, "C", Module2::createC);
			}

			C* createC()
			{
				return new C;
			}

			B* createB()
			{
				return new B;
			}

			virtual void
			configure()
			{}

			virtual void
			shutDown()
			{}
		};

		class Module3
			: public Module<Module3>
		{
		public:
			Module3(const pyconfig::View& _pyConfigView)
				: Module<Module3>(_pyConfigView)
			{
				MODULE_REGISTER_SERVICE(A*, "A", Module3::createA);
			}

			A* createA() {
				return new A;
			}

			virtual void
			configure()
			{}

			virtual void
			shutDown()
			{}
		};

		CPPUNIT_TEST_SUITE( ModuleTest );
		CPPUNIT_TEST( instantiated );
		CPPUNIT_TEST_EXCEPTION( doubleRegister, Exception);
		CPPUNIT_TEST_EXCEPTION( doubleInstantiation, Exception);
		CPPUNIT_TEST( existsService );
		CPPUNIT_TEST( getService );
		CPPUNIT_TEST( pyConfig );
#ifdef WNS_ASSURE_THROWS_EXCEPTION
		CPPUNIT_TEST_EXCEPTION( getUnavailableService, Exception);
#endif // WNS_ASSURE_THROWS_EXCEPTION
		CPPUNIT_TEST_SUITE_END();
	public:
		ModuleTest();
		void prepare();
		void cleanup();
		void instantiated();
		void doubleRegister();
		void doubleInstantiation();
		void existsService();
		void getService();
		void pyConfig();
		void getUnavailableService();
	private:
		wns::pyconfig::View pyco;
	};
} // tests
} // module
} // wns
#endif // NOT defined WNS_MODULE_TESTS_MODULETEST_HPP
