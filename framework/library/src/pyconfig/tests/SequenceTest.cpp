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

#include "SequenceTest.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <set>

using namespace std;
using namespace wns::pyconfig;

CPPUNIT_TEST_SUITE_REGISTRATION( SequenceTest );

void
SequenceTest::setUp()
{
} // setUp


void
SequenceTest::tearDown()
{
} // tearDown


void
SequenceTest::testEmpty()
{
	Parser config;

	config.loadString(
		"foo = []\n"
		"bar = [1]\n"
		);

	pyconfig::Sequence seq = config.getSequence("foo");
	CPPUNIT_ASSERT( seq.empty() );

	seq = config.getSequence("bar");
	CPPUNIT_ASSERT( !seq.empty() );
} // testEmpty


void
SequenceTest::testSize()
{
	Parser config;

	config.loadString(
		"foo = []\n"
		"bar = [1]\n"
		"baz = ['a', 'b']\n"
		);

	pyconfig::Sequence seq = config.getSequence("foo");
	CPPUNIT_ASSERT_EQUAL( 0, seq.size() );

	seq = config.getSequence("bar");
	CPPUNIT_ASSERT_EQUAL( 1, seq.size() );

	seq = config.getSequence("baz");
	CPPUNIT_ASSERT_EQUAL( 2, seq.size() );
} // testSize


void
SequenceTest::testAt()
{
	Parser config;
	config.loadString(
		"foo = ['a', 'b', 'c']\n"
		"bar = [ [1,2], [3,4]]\n"
		);

	pyconfig::Sequence seq = config.getSequence("foo");
	CPPUNIT_ASSERT_EQUAL(std::string("b"),
			     seq.at<std::string>(1));

	CPPUNIT_ASSERT_EQUAL(std::string("c"),
			     seq.at<std::string>(-1));

 	seq = config.getSequence("bar");
 	CPPUNIT_ASSERT_EQUAL(2,
 			     seq.size());

	CPPUNIT_ASSERT(seq.isSequenceAt(0));
	CPPUNIT_ASSERT(seq.isSequenceAt(1));

 	pyconfig::Sequence seq2 = seq.getSequenceAt(0);

 	CPPUNIT_ASSERT_EQUAL( 2, seq2.at<int>(1) );

} // testAt


void
SequenceTest::testTypedEmpty()
{
	Parser config;
	config.loadString(
		"foo = []\n"
		"bar = [1]\n"
		);

	pyconfig::Sequence seq = config.getSequence("foo");
	CPPUNIT_ASSERT( seq.begin<int>() == seq.end<int>() );

	seq = config.getSequence("bar");
	CPPUNIT_ASSERT( seq.begin<int>() != seq.end<int>() );
} // testTypedEmpty


void
SequenceTest::testTypedIter()
{
	Parser config;
	config.loadString(
		"foo = [1, 2]\n"
		);
	pyconfig::Sequence seq = config.getSequence("foo");

	// post
	pyconfig::Sequence::iterator<int> iter = seq.begin<int>();
	CPPUNIT_ASSERT( iter == seq.begin<int>() );
	CPPUNIT_ASSERT( iter != seq.end<int>() );

	iter++;
	CPPUNIT_ASSERT( iter == iter );
	CPPUNIT_ASSERT( iter != seq.end<int>() );

	iter++;
	CPPUNIT_ASSERT( iter == iter );
	CPPUNIT_ASSERT( iter == seq.end<int>() );

	// pre
	iter = seq.begin<int>();
	CPPUNIT_ASSERT( iter != seq.end<int>() );

	++iter;
	CPPUNIT_ASSERT( iter != seq.end<int>() );

	++iter;
	CPPUNIT_ASSERT( iter == seq.end<int>() );
} // testTypedIter


void
SequenceTest::testTypedDeref()
{
	Parser config;
	config.loadString(
		"foo = [1, 2]\n"
		"bar = ['a', 'b']\n"
		);
	{
		pyconfig::Sequence seq = config.getSequence("foo");
		pyconfig::Sequence::iterator<int> iter = seq.begin<int>();
		CPPUNIT_ASSERT_EQUAL( 1, *iter++ );
		CPPUNIT_ASSERT_EQUAL( 2, *iter++ );
	}

	{
		pyconfig::Sequence seq = config.getSequence("bar");
		pyconfig::Sequence::iterator<std::string> iter = seq.begin<std::string>();
		CPPUNIT_ASSERT_EQUAL( std::string("b"), *++iter);
	}
} // testTypedDeref



