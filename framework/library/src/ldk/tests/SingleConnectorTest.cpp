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

#include <WNS/CppUnit.hpp>
#include <WNS/ldk/SingleConnector.hpp>
#include <WNS/ldk/tools/FakeFU.hpp>

namespace wns { namespace ldk { namespace tests {

	class SingleConnectorTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( SingleConnectorTest );

 		CPPUNIT_TEST( testHasAcceptorAccepting );
		CPPUNIT_TEST( testHasAcceptorNotAccepting );
		CPPUNIT_TEST( testHasAcceptorNoAcceptor );

 		CPPUNIT_TEST( testGetAcceptorAccepting );
		CPPUNIT_TEST( testGetAcceptorNotAccepting );
		CPPUNIT_TEST( testGetAcceptorNoAcceptor );

		CPPUNIT_TEST_SUITE_END();


		class TestFU :
			public tools::FakeFU
		{
		public:
			explicit
			TestFU(bool _accepting) :
				FakeFU(),
				accepting(_accepting)
			{
			}

			virtual bool
			isAcceptingForwarded(const CompoundPtr&) const
			{
				return accepting;
			}

			bool accepting;
		};


		void
		prepare()
		{

		}

		void
		cleanup()
		{

		}


		void
		testHasAcceptorAccepting()
		{
			SingleConnector sc;
			TestFU fu(true);
			sc.add(&fu);
			CPPUNIT_ASSERT( sc.hasAcceptor(CompoundPtr()) == true );
		}

		void
		testHasAcceptorNotAccepting()
		{
			SingleConnector sc;
			TestFU fu(false);
			sc.add(&fu);
			CPPUNIT_ASSERT( sc.hasAcceptor(CompoundPtr()) == false );
		}

		void
		testHasAcceptorNoAcceptor()
		{
			SingleConnector sc;
			WNS_ASSERT_ASSURE_EXCEPTION( sc.hasAcceptor(CompoundPtr()) );
		}

		void
		testGetAcceptorAccepting()
		{
			SingleConnector sc;
			TestFU fu(true);
			sc.add(&fu);
			CPPUNIT_ASSERT( sc.getAcceptor(CompoundPtr()) == &fu );
		}

		void
		testGetAcceptorNotAccepting()
		{
			SingleConnector sc;
			TestFU fu(false);
			sc.add(&fu);
			WNS_ASSERT_ASSURE_EXCEPTION( sc.getAcceptor(CompoundPtr()) );
		}

		void
		testGetAcceptorNoAcceptor()
		{
			SingleConnector sc;
			WNS_ASSERT_ASSURE_EXCEPTION( sc.getAcceptor(CompoundPtr()) );
		}

	};

	CPPUNIT_TEST_SUITE_REGISTRATION( SingleConnectorTest );

} // tests
} // ldk
} // wns




