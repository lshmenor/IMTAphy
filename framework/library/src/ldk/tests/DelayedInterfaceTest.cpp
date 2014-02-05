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

#include <WNS/ldk/tests/DelayedInterfaceTest.hpp>

using namespace wns::ldk::tests;

DelayedInterfaceTest::DelayedInterfaceTest() :
    testee(NULL)
{
} // DelayedInterfaceTest

void
DelayedInterfaceTest::nothingToSend()
{
    CPPUNIT_ASSERT_MESSAGE( "NULL pointer expected if there is nothing to send",
			    !testee->hasSomethingToSend() );
} // nothingToSend

void
DelayedInterfaceTest::somethingToSend()
{
    getLowerStub()->close();
    CPPUNIT_ASSERT_MESSAGE( "Unsatisfied test prerequisite: fresh testee has something to send",
			    !testee->hasSomethingToSend() );
    sendCompound(newFakeCompound());
    CPPUNIT_ASSERT_MESSAGE( "Unsatisfied test prerequisite: compound was not propagated to testee", 
			    getUpperStub()->sent.size() > 0 );
    CPPUNIT_ASSERT_MESSAGE( "got NULL pointer from hasSomethingToSend although there should be something to send", 
			    testee->hasSomethingToSend() );
    CPPUNIT_ASSERT_MESSAGE( "got NULL pointer from getSomethingToSend although there should be something to send", 
			    testee->getSomethingToSend() );
} // somethingToSend

void
DelayedInterfaceTest::compoundsSameSize()
{
    getLowerStub()->close();
    sendCompound(newFakeCompound());
    CPPUNIT_ASSERT_MESSAGE( "Unsatisfied test prerequisite: compound was not propgated to testee", 
			    getUpperStub()->sent.size() > 0 );
    CompoundPtr hstsCompound = testee->hasSomethingToSend();
    CompoundPtr gstsCompound = testee->getSomethingToSend();
    CPPUNIT_ASSERT_MESSAGE( "Unsatisfied test prerequisite: unexpectedly encountered a NULL pointer",
			    hstsCompound && gstsCompound );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "returned compounds of consecutive calls to hasSomethingToSend and getSomethingToSend not of same size",
				  hstsCompound->getLengthInBits(), gstsCompound->getLengthInBits() );
} // compoundsSameSize

void
DelayedInterfaceTest::compoundsSameCommands()
{
    getLowerStub()->close();
    sendCompound(newFakeCompound());
    CPPUNIT_ASSERT_MESSAGE( "Unsatisfied test prerequisite: compound was not propgated to testee", 
			    getUpperStub()->sent.size() > 0 );
    CompoundPtr hstsCompound = testee->hasSomethingToSend();
    CompoundPtr gstsCompound = testee->getSomethingToSend();
    CPPUNIT_ASSERT_MESSAGE( "Unsatisfied test prerequisite: unexpectedly encountered a NULL pointer",
			    hstsCompound && gstsCompound );
    CPPUNIT_ASSERT_MESSAGE( "returned compounds of consecutive calls to hasSomethingToSend and getSomethingToSend don't have same commands",
			    hstsCompound->getCommandPool()->knowsSameCommandsAs(*gstsCompound->getCommandPool()) );
} // compoundsSameCommands

void
DelayedInterfaceTest::compoundsSameJourney()
{
    getLowerStub()->close();
    sendCompound(newFakeCompound());
    CPPUNIT_ASSERT_MESSAGE( "Unsatisfied test prerequisite: compound was not propgated to testee", 
			    getUpperStub()->sent.size() > 0 );
    CompoundPtr hstsCompound = testee->hasSomethingToSend();
    CompoundPtr gstsCompound = testee->getSomethingToSend();
    CPPUNIT_ASSERT_MESSAGE( "Unsatisfied test prerequisite: unexpectedly encountered a NULL pointer",
			    hstsCompound && gstsCompound );
    CPPUNIT_ASSERT_MESSAGE( "returned compounds of consecutive calls to hasSomethingToSend and getSomethingToSend don't have same journey",
			    hstsCompound->getJourney() == gstsCompound->getJourney() );
} // compoundsSameJourney

void
DelayedInterfaceTest::compoundsSameData()
{
    getLowerStub()->close();
    sendCompound(newFakeCompound());
    CPPUNIT_ASSERT_MESSAGE( "Unsatisfied test prerequisite: compound was not propgated to testee", 
			    getUpperStub()->sent.size() > 0 );
    CompoundPtr hstsCompound = testee->hasSomethingToSend();
    CompoundPtr gstsCompound = testee->getSomethingToSend();
    CPPUNIT_ASSERT_MESSAGE( "Unsatisfied test prerequisite: unexpectedly encountered a NULL pointer",
			    hstsCompound && gstsCompound );
    CPPUNIT_ASSERT_MESSAGE( "returned compounds of consecutive calls to hasSomethingToSend and getSomethingToSend don't have same data",
			    hstsCompound->getData() == gstsCompound->getData() );
} // compoundsSameJourney

void
DelayedInterfaceTest::setUpTestFUs()
{
    testee = newTestee();
    getFUN()->addFunctionalUnit("DelayedInterfaceTestee", testee);
} // setUpTestFUs

void
DelayedInterfaceTest::tearDownTestFUs()
{
    tearDownTestee(testee);
} // tearDownTestFUs

wns::ldk::FunctionalUnit*
DelayedInterfaceTest::getUpperTestFU() const
{
    return testee;
} // getUpperTestFU

wns::ldk::FunctionalUnit*
DelayedInterfaceTest::getLowerTestFU() const
{
    return testee;
} // getLowerTestFU
