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

#ifndef WNS_TEST_ROUNDROBINTEST_HPP
#define WNS_TEST_ROUNDROBINTEST_HPP

#include <WNS/Exception.hpp>
#include <WNS/RoundRobin.hpp>
#include <cppunit/extensions/HelperMacros.h>

#include <vector>

namespace wns { namespace tests {

	class RoundRobinTest :
		public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( RoundRobinTest );
		CPPUNIT_TEST( empty );
		CPPUNIT_TEST( remove );
		CPPUNIT_TEST( notEmpty );
		CPPUNIT_TEST( size );
		CPPUNIT_TEST( startRound );
		CPPUNIT_TEST( next );
		CPPUNIT_TEST( hasNext );
		CPPUNIT_TEST( endRound );
		CPPUNIT_TEST( addAfterRound );
		CPPUNIT_TEST( removeAfterRoundCursorBeforeElement);
		CPPUNIT_TEST( removeAfterRoundCursorOnElement );
		CPPUNIT_TEST( removeAfterRoundCursorAfterElement );
		CPPUNIT_TEST( endUnusedRound );
		CPPUNIT_TEST( whileUsage );
		CPPUNIT_TEST( startAndEndRound );

#ifdef WNS_ASSURE_THROWS_EXCEPTION
		CPPUNIT_TEST_EXCEPTION( nextAfterConstructor,
					wns::Assure::Exception );
		CPPUNIT_TEST_EXCEPTION( hasNextAfterContructor,
					wns::Assure::Exception );
		CPPUNIT_TEST_EXCEPTION( endRoundAfterConstructor,
					wns::Assure::Exception );
		CPPUNIT_TEST_EXCEPTION( nextWithoutStartRound,
					wns::Assure::Exception );
		CPPUNIT_TEST_EXCEPTION( hasNextWithoutStartRound,
					wns::Assure::Exception );
		CPPUNIT_TEST_EXCEPTION( endRoundWithoutStartRound,
					wns::Assure::Exception );
		CPPUNIT_TEST_EXCEPTION( startRoundWithoutEndRound,
					wns::Assure::Exception );
		CPPUNIT_TEST_EXCEPTION( nextAlthoughNoNext,
					wns::Assure::Exception );
		CPPUNIT_TEST_EXCEPTION( addWhileInRound,
					wns::Assure::Exception );
		CPPUNIT_TEST_EXCEPTION( removeWhileInRound,
					wns::Assure::Exception );
		CPPUNIT_TEST_EXCEPTION( removeNonExisting,
					wns::Assure::Exception );
#endif
		CPPUNIT_TEST( clear );
		CPPUNIT_TEST( cancelRound );
		CPPUNIT_TEST( current );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();

		// tests
		void empty();
		void add();
		void remove();
		void notEmpty();
		void size();
		void startRound();
		void next();
		void hasNext();
		void endRound();
		void addAfterRound();
		void removeAfterRoundCursorBeforeElement();
		void removeAfterRoundCursorOnElement();
		void removeAfterRoundCursorAfterElement();
		void endUnusedRound();
		void whileUsage();
		void startAndEndRound();
		void nextAfterConstructor();
		void hasNextAfterContructor();
		void endRoundAfterConstructor();
		void nextWithoutStartRound();
		void hasNextWithoutStartRound();
		void endRoundWithoutStartRound();
		void startRoundWithoutEndRound();
		void nextAlthoughNoNext();
		void addWhileInRound();
		void removeWhileInRound();
		void removeNonExisting();
		void clear();
		void cancelRound();
		void current();
	private:
		RoundRobin<int> rr;
	};
} // tests
} // wns
#endif // NOT defined WNS_TEST_ROUNDROBINTEST_HPP


