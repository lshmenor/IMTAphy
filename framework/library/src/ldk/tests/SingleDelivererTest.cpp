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
#include <WNS/ldk/SingleDeliverer.hpp>
#include <WNS/ldk/tools/FakeFU.hpp>

namespace wns { namespace ldk { namespace tests {

	class SingleDelivererTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( SingleDelivererTest );
		CPPUNIT_TEST( testFull );
		CPPUNIT_TEST( testEmpty );
		CPPUNIT_TEST_SUITE_END();

		void
		prepare()
		{

		}

		void
		cleanup()
		{

		}

		void
		testFull()
		{
			SingleDeliverer sd;
			tools::FakeFU fu;
			sd.add(&fu);
			CPPUNIT_ASSERT( sd.getAcceptor(CompoundPtr()) == &fu );
		}

		void
		testEmpty()
		{
			SingleDeliverer sd;
			WNS_ASSERT_ASSURE_EXCEPTION( sd.getAcceptor(CompoundPtr()) );
		}

	};

	CPPUNIT_TEST_SUITE_REGISTRATION( SingleDelivererTest );

} // tests
} // ldk
} // wns




