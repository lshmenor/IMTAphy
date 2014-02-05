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

#include <WNS/markovchain/MarkovDiscreteTime.hpp>
#include <WNS/markovchain/MarkovBase.hpp>
#include <WNS/CppUnit.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <stdint.h>

#ifndef classMarkovDiscreteTimeTest
#define classMarkovDiscreteTimeTest



namespace wns { namespace markovchain {

	class MarkovDiscreteTimeTest
		: public wns::TestFixture {
		CPPUNIT_TEST_SUITE( MarkovDiscreteTimeTest );
		CPPUNIT_TEST( testCheckSum );
		CPPUNIT_TEST( testNextState );
		CPPUNIT_TEST( testCalculateStateProbabilities );
		CPPUNIT_TEST_SUITE_END() ;

	public:
		MarkovDiscreteTimeTest();
		~MarkovDiscreteTimeTest();
		void prepare();
		void cleanup();
		void testCheckSum();
		void testNextState();
		void testCalculateStateProbabilities();
		bool useCout;
	};//MarkovDiscreteTimeTest


}//markovchain
}//wns



#endif // classMarkovDiscreteTimeTest
