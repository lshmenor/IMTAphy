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

#ifndef WNS_PYCONFIG_TESTS_VIEWTEST_HPP
#define WNS_PYCONFIG_TESTS_VIEWTEST_HPP

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/TestFixture.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace wns {	namespace pyconfig { namespace tests {
	class ViewTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( ViewTest );
		CPPUNIT_TEST( testTypes );
		CPPUNIT_TEST( testBool );
		CPPUNIT_TEST( testString );
		CPPUNIT_TEST( testNested );
		CPPUNIT_TEST( testMultipleLoads );
		CPPUNIT_TEST( testKnows );
		CPPUNIT_TEST( testUnknown );
		CPPUNIT_TEST( testView );
		CPPUNIT_TEST( testGetWithView );
		CPPUNIT_TEST( testSequenceView );
		CPPUNIT_TEST( testSequenceGetWithView );
		CPPUNIT_TEST( testContext );
		CPPUNIT_TEST( testInheritance );
		CPPUNIT_TEST( testInstance );
		CPPUNIT_TEST( testInheritanceInstance );
		CPPUNIT_TEST( testCopy );
		CPPUNIT_TEST( testAssignment );
		CPPUNIT_TEST( testAutomatic );
		CPPUNIT_TEST( testSequence );
		CPPUNIT_TEST( testNotASequence1 );
		CPPUNIT_TEST( testNotASequence2 );
		CPPUNIT_TEST( testOutOfRange );
		CPPUNIT_TEST( testGetSequence );
		CPPUNIT_TEST( testPatch );
		CPPUNIT_TEST( testIsNone );
		CPPUNIT_TEST( testMatrix );
		CPPUNIT_TEST( testId );
		CPPUNIT_TEST( testEqualOp );
		CPPUNIT_TEST( testStoreInMap );
		CPPUNIT_TEST( testDict );
		CPPUNIT_TEST( testBoundMethod );
		CPPUNIT_TEST( testDontConvertNone );
		CPPUNIT_TEST( testViewFromNestedSequence );
		CPPUNIT_TEST_SUITE_END(); // FIXME(fds): TEST_EXCEPTION TestCases missing
	public:
		void prepare();
		void cleanup();

		void testTypes();
		void testBool();
		void testString();
		void testNested();
		void testMultipleLoads();
		void testKnows();
		void testUnknown();
		void testView();
		void testGetWithView();
		void testSequenceView();
		void testSequenceGetWithView();
		void testContext();
		void testInheritance();
		void testInstance();
		void testInheritanceInstance();
		void testCopy();
		void testAssignment();
		void testAutomatic();
		void testSequence();
		void testNotASequence1();
		void testNotASequence2();
		void testOutOfRange();
		void testGetSequence();
		void testFromString();
		void testPatch();
		void testIsNone();
		void testMatrix();
		void testId();
		void testEqualOp();
		void testDict();
		void testStoreInMap();
		void testBoundMethod();
		void testDontConvertNone();
		void testViewFromNestedSequence();
	private:
		Parser* pyco;
	};
} // tests
} // pyconfig
} // wns

#endif // NOT defined WNS_PYCONFIG_PARSER_TEST_HPP


