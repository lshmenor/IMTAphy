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

#include <WNS/ldk/tests/FunctionalUnitTest.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <iostream>

using namespace wns;
using namespace testFunctionalUnit;

CPPUNIT_TEST_SUITE_REGISTRATION( FunctionalUnitTest );

void
FunctionalUnitTest::setUp()
{
	wns::ldk::CommandProxy::clearRegistries();

	layer = new tests::LayerStub();
	fuNet = new fun::Main(layer);

	fsl = new FunkyFunctionalUnit(fuNet);
	psl = new PunkyFunctionalUnit(fuNet);

	fuNet->addFunctionalUnit("ernie", fsl);
	fuNet->addFunctionalUnit("bert", psl);

	bangFunctionalUnit = NULL;
	bangCollection = NULL;
	bangLayer = NULL;
	bangFUN = NULL;

	fsl->connect(psl);
} // setUp


void
FunctionalUnitTest::tearDown()
{
	delete layer;
	// delete this first (before fsl!!)
	if(NULL != bangCollection)
		delete bangCollection;
	if(NULL != fsl)
		delete fsl;
	if(NULL != psl)
		delete psl;
	if(NULL != bangFunctionalUnit)
		delete bangFunctionalUnit;
	if(NULL != bangLayer)
		delete bangLayer;
	if(NULL != bangFUN)
		delete bangFUN;
} // tearDown


void
FunctionalUnitTest::testProxy()
{
	CommandPool* commandPool = fuNet->createCommandPool();

	CPPUNIT_ASSERT(fsl->foo(commandPool) == 23);
	CPPUNIT_ASSERT(psl->foo(commandPool) == 42);

	delete commandPool;
} // testProxy


void
FunctionalUnitTest::testDestructor()
{
	CommandPool* commandPool = fuNet->createCommandPool();

	CPPUNIT_ASSERT( !fsl->destructorCalled );

		// this will initialize the destructorCalled pointer in
		// the FunkyCommand:
	fsl->foo(commandPool);

	delete commandPool;
	CPPUNIT_ASSERT( fsl->destructorCalled );
} // testDestructor


// void
// FunctionalUnitTest::testProxyFixation()
// {
// 		// after the first CommandPool has been created,
// 		// the CommandProxy should throw an error, whenever
// 		// new functional units are added.
// 	CommandPool* commandPool = fuNet->createCommandPool();
// 	delete commandPool;

// 	bangFunctionalUnit = new FunkyFunctionalUnit(fuNet);

// 	fuNet->getProxy()->addFunctionalUnit(bangFunctionalUnit); // <-- this should raise an exception (FatalErr)
// } // testProxyFixation


// void
// FunctionalUnitTest::testWrongCollection()
// {
// 	bangLayer = new tests::LayerStub();
// 	bangFUN = new fun::Main(bangLayer);
// 	bangCollection = bangFUN->createCommandPool();

// 	fsl->foo(bangCollection); // <-- this should raise an exception (FatalErr)
// } // testWrongCollection


void
FunctionalUnitTest::testWrongRoleName()
{
	bangCollection = fuNet->createCommandPool();

	fsl->foo(bangCollection);

	bangLayer = new tests::LayerStub();
	bangFUN = new fun::Main(bangLayer);
	FunkyFunctionalUnit* fslWrong = new FunkyFunctionalUnit(bangFUN);
	PunkyFunctionalUnit* pslWrong = new PunkyFunctionalUnit(bangFUN);

	bangFUN->addFunctionalUnit("bernie", pslWrong);
	bangFUN->addFunctionalUnit("ert", fslWrong); // wrong role name

	fslWrong->bar(bangCollection);	// <-- this should raise an exception (FatalErr)
} // testWrongRoleName


void
FunctionalUnitTest::testDuplicateFUName()
{
	FunkyFunctionalUnit* fslWrong = new FunkyFunctionalUnit(bangFUN);

	// FU with FU name "ernie" has been added to FUN already
	fuNet->addFunctionalUnit("ernie", fslWrong);
} // testDuplicateRoleName

void
FunctionalUnitTest::testDuplicateRoleName()
{
	FunkyFunctionalUnit* role1 = new FunkyFunctionalUnit(fuNet);
	FunkyFunctionalUnit* role2 = new FunkyFunctionalUnit(fuNet);

	// 2 FUs with the same role but different names
	fuNet->addFunctionalUnit("ernie", "ernie_does_this", role1);
	fuNet->addFunctionalUnit("ernie", "ernie_does_that", role2);

	CommandPool* commandPool = fuNet->createCommandPool();

	CPPUNIT_ASSERT(role1->foo(commandPool) == 23);
	CPPUNIT_ASSERT_THROW(role2->foo(commandPool), wns::Exception);
	CPPUNIT_ASSERT(role2->bar(commandPool) == 23);
}

void
FunctionalUnitTest::testWrongCommandType()
{
	bangCollection = fuNet->createCommandPool();

	fsl->foo(bangCollection);

	bangLayer = new tests::LayerStub();
	bangFUN = new fun::Main(bangLayer);
	FunkyFunctionalUnit* fslWrong = new FunkyFunctionalUnit(bangFUN);
	PunkyFunctionalUnit* pslWrong = new PunkyFunctionalUnit(bangFUN);

 	bangFUN->addFunctionalUnit("bert", fslWrong);
 	bangFUN->addFunctionalUnit("ernie", pslWrong);

	fslWrong->bar(bangCollection);
} // testWrongCommandType


void
FunctionalUnitTest::testCopy()
{
	CommandPool* commandPool = fuNet->createCommandPool();
	CommandPool other = *commandPool;
	CPPUNIT_ASSERT(fsl->foo(&other) == 23);
	CPPUNIT_ASSERT(psl->foo(&other) == 42);
	delete commandPool;
} // testCopy


void
FunctionalUnitTest::testPartialCopy()
{
	CommandPool* commandPool = fuNet->createCommandPool();
	fsl->foo(commandPool);
	CPPUNIT_ASSERT(fuNet->getProxy()->commandIsActivated(commandPool, fsl));
	CPPUNIT_ASSERT(!fuNet->getProxy()->commandIsActivated(commandPool, psl));

	psl->foo(commandPool);
	CPPUNIT_ASSERT(fuNet->getProxy()->commandIsActivated(commandPool, fsl));
	CPPUNIT_ASSERT(fuNet->getProxy()->commandIsActivated(commandPool, psl));

	CommandPool* other = fuNet->createCommandPool();
	fuNet->getProxy()->partialCopy(psl, other, commandPool);
	CPPUNIT_ASSERT(fuNet->getProxy()->commandIsActivated(other, fsl));
	CPPUNIT_ASSERT(!fuNet->getProxy()->commandIsActivated(other, psl));

	delete commandPool;
	delete other;
} // testPartialCopy


void
FunctionalUnitTest::testRegisterCommandTypeSpecifier()
{
	CommandTypeSpecifier<FunkyCommand> provider(fuNet);

	fuNet->getProxy()->addFunctionalUnit("FU", &provider);
	CommandPool* commandPool = fuNet->createCommandPool();
	CPPUNIT_ASSERT(commandPool);

	FunkyCommand* command = provider.activateCommand(commandPool);
	CPPUNIT_ASSERT(command);
	CPPUNIT_ASSERT(command == provider.getCommand(commandPool));

	delete commandPool;
} // testRegisterCommandTypeSpecifier


void
FunctionalUnitTest::testRemoveFunctionalUnit()
{
	fuNet->getProxy()->removeFunctionalUnit("ernie");
} // testRemoveFunctionalUnit


void
FunctionalUnitTest::testRemoveUnknownFunctionalUnit()
{
	fuNet->getProxy()->removeFunctionalUnit("lala");
} // testRemoveUnknownFunctionalUnit


void
FunctionalUnitTest::testRemoveFunctionalUnitNotAddedToFUN()
{
	fuNet->getProxy()->removeFunctionalUnit("foo");
} // testRemoveFunctionalUnitNotAddedToFUN



