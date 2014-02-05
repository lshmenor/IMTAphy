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

#ifndef WNS_LDK_ACCEPTINGROUNDROBIN_TEST_HPP
#define WNS_LDK_ACCEPTINGROUNDROBIN_TEST_HPP

#include <WNS/ldk/RoundRobinConnector.hpp>
#include <WNS/ldk/tools/Stub.hpp>

#include <WNS/CppUnit.hpp>


namespace wns { namespace ldk {
	class ILayer;
}}

namespace wns { namespace ldk { namespace tests {

	class RoundRobinConnectorTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( RoundRobinConnectorTest );
		CPPUNIT_TEST( testFill );
		CPPUNIT_TEST( testRR );
		CPPUNIT_TEST( testAccepting );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();

		void testFill();
		void testRR();
		void testAccepting();

	private:
		wns::ldk::ILayer* l;
		wns::ldk::fun::FUN* fuNet;
		wns::ldk::tools::Stub* s1;
		wns::ldk::tools::Stub* s2;
		wns::ldk::tools::Stub* s3;
	};

}}}

#endif // NOT defined WNS_LDK_ACCEPTINGROUNDROBIN_TEST_HPP


