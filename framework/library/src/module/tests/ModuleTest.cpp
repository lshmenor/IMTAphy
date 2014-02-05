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

#include <WNS/module/tests/ModuleTest.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>

using namespace wns::module::tests;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( ModuleTest );

ModuleTest::ModuleTest() :
	pyco(wns::pyconfig::helper::createViewFromString(
		"import openwns.logger\n"
		"logger = openwns.logger.Logger('FOO', 'BAR', True)\n"))
{
}

void ModuleTest::prepare()
{
}

void ModuleTest::cleanup()
{}

void ModuleTest::instantiated()
{
	CPPUNIT_ASSERT(!Module1::isInstantiated());
	Module1 m = Module1(pyco);
	CPPUNIT_ASSERT(Module1::isInstantiated());
}

void ModuleTest::doubleRegister()
{
	Module1 m1 = Module1(pyco);
	Module3 m3 = Module3(pyco);
}

void ModuleTest::doubleInstantiation()
{
	Module1 m1 = Module1(pyco);
	Module1 m2 = Module1(pyco);
}

void ModuleTest::existsService()
{
	Module1 m1 = Module1(pyco);
	CPPUNIT_ASSERT(m1.existsService("A"));
	CPPUNIT_ASSERT(m1.existsService("B"));
	CPPUNIT_ASSERT(!m1.existsService("C"));
	CPPUNIT_ASSERT(!m1.existsService("D"));

	{
		Module2 m2 = Module2(pyco);
		CPPUNIT_ASSERT(m1.existsService("A"));
		CPPUNIT_ASSERT(m1.existsService("B"));
		CPPUNIT_ASSERT(m1.existsService("B2"));
		CPPUNIT_ASSERT(m1.existsService("C"));
		CPPUNIT_ASSERT(!m1.existsService("D"));
		CPPUNIT_ASSERT(m2.existsService("A"));
		CPPUNIT_ASSERT(m2.existsService("B"));
		CPPUNIT_ASSERT(m2.existsService("B2"));
		CPPUNIT_ASSERT(m2.existsService("C"));
		CPPUNIT_ASSERT(!m2.existsService("D"));
	}

	CPPUNIT_ASSERT(m1.existsService("A"));
	CPPUNIT_ASSERT(m1.existsService("B"));
	CPPUNIT_ASSERT(!m1.existsService("C"));
	CPPUNIT_ASSERT(!m1.existsService("D"));
}

void ModuleTest::getService()
{
	Module1 m1 = Module1(pyco);
	Module2 m2 = Module2(pyco);
	A* a = m1.getService<A*>("A");
	B* b = m1.getService<B*>("B");
	B* b2 = m2.getService<B*>("B2");
	C* c = m2.getService<C*>("C");
	CPPUNIT_ASSERT_EQUAL(1, a->test());
	CPPUNIT_ASSERT_EQUAL(2, b->test());
	CPPUNIT_ASSERT_EQUAL(2, b2->test());
	CPPUNIT_ASSERT_EQUAL(3, c->test());
	delete a;
	delete b;
	delete b2;
	delete c;
}

void ModuleTest::pyConfig()
{
	pyco = wns::pyconfig::helper::createViewFromString(
		"import openwns.logger\n"
		"logger = openwns.logger.Logger('BAR', 'FOO', True)\n"
		"test = 1\n"
		);
	wns::pyconfig::View pyco2 = wns::pyconfig::helper::createViewFromString(
		"import openwns.logger\n"
		"logger = openwns.logger.Logger('BAR', 'FOO', True)\n"
		"test = 2\n"
		);

	Module1 m1 = Module1(pyco);
	Module2 m2 = Module2(pyco2);

	CPPUNIT_ASSERT_EQUAL( 1, Module<ModuleTest::Module1>::getPyConfigView().get<int>("test") );
	CPPUNIT_ASSERT_EQUAL( 2, Module<ModuleTest::Module2>::getPyConfigView().get<int>("test") );
}

void ModuleTest::getUnavailableService()
{
	Module1 m = Module1(pyco);
	m.getService<C*>("D");
}
