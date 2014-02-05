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

#ifndef WNS_LDK_TOOLS_FORWARDER_TEST_HPP
#define WNS_LDK_TOOLS_FORWARDER_TEST_HPP

#include <WNS/ldk/tools/Forwarder.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/CppUnit.hpp>

namespace wns { namespace ldk { namespace tools { namespace tests {

	class ForwarderTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( ForwarderTest );
		CPPUNIT_TEST( testDownlink );
		CPPUNIT_TEST( testUplink );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();

		void testDownlink();
		void testUplink();
	private:
		ILayer* layer;
		fun::FUN* fuNet;

		Stub* upper;
		Forwarder* forwarder;
		Stub* lower;
	};

} // tests
} // tools
} // ldk
} // wns


#endif // NOT defined WNS_LDK_TOOLS_FORWARDER_TEST_HPP


