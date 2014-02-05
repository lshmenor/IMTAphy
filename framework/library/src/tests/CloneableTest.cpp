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

#include <WNS/tests/CloneableTest.hpp>

using namespace wns;

CPPUNIT_TEST_SUITE_REGISTRATION( CloneableTest );

void CloneableTest::clone()
{
	// begin example "Cloneable_unsafe.example"
	CloneableTestClass* original = new CloneableTestClass();
	CloneableInterface* copy ( original->clone() );
	CloneableTestClass* testCopy = dynamic_cast<CloneableTest::CloneableTestClass*>(copy);
	// end example

	CPPUNIT_ASSERT( testCopy );
	CPPUNIT_ASSERT( *original == *testCopy );

	delete testCopy;
	delete original;
}

void CloneableTest::cloneTypeSafe()
{
	// begin example "Cloneable_typesafe.example"
	CloneableTestClass* original = new CloneableTestClass();
	CloneableTestClass* testCopy = wns::clone(original);
	// end example

	CPPUNIT_ASSERT( testCopy );
	CPPUNIT_ASSERT( *original == *testCopy );

	delete testCopy;
	delete original;
}

void CloneableTest::cloneAutoPtrTypeSafe()
{
	// begin example "Cloneable_autoptr.example"
	std::auto_ptr<CloneableTestClass> original(new CloneableTestClass());
	std::auto_ptr<CloneableTestClass> testCopy = wns::clone(original);
	// end example

	CPPUNIT_ASSERT( *original == *testCopy );
}

void CloneableTest::notCloneableException()
{
	std::auto_ptr<NotCloneableTestClass> original(new NotCloneableTestClass());
	CPPUNIT_ASSERT_THROW( original->clone(), CloneNotSupported );
}


