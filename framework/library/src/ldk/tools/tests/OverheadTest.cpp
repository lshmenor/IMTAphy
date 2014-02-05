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

#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tools/Overhead.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/CppUnit.hpp>

namespace wns { namespace ldk { namespace tools { namespace tests {

	class OverheadTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( OverheadTest );
		CPPUNIT_TEST( testSize );
		CPPUNIT_TEST_SUITE_END();
	public:

		virtual void
		prepare()
		{
			layer = new wns::ldk::tests::LayerStub();
			fuNet = new fun::Main(layer);

			pyconfig::Parser emptyConfig;
			pyconfig::View config =
				pyconfig::Parser::fromString(
					"import openwns.ldk\n"
					"overheadFU = openwns.ldk.Tools.Overhead(overhead=42, commandName='overhead')\n");
			upper = new Stub(fuNet, emptyConfig);
			overhead = new Overhead(fuNet, config.get("overheadFU"));
			lower = new Stub(fuNet, emptyConfig);

			upper
				->connect(overhead)
				->connect(lower);
			fuNet->addFunctionalUnit("upperStub", upper);
			fuNet->addFunctionalUnit("overhead", overhead);
			fuNet->addFunctionalUnit("lowerStub", lower);
		}


		virtual void
		cleanup()
		{
			delete fuNet;
			delete layer;
		}

		void
		testSize()
		{
			CompoundPtr compound(fuNet->createCompound());
			upper->sendData(compound);
			CPPUNIT_ASSERT(lower->sent.size() == 1);
			CPPUNIT_ASSERT(lower->sent[0] == compound);
			CPPUNIT_ASSERT_EQUAL(Bit(42), compound->getLengthInBits());
		}


	private:
		ILayer* layer;
		fun::FUN* fuNet;

		Stub* upper;
		Overhead* overhead;
		Stub* lower;
	};

	CPPUNIT_TEST_SUITE_REGISTRATION( OverheadTest );

} // tests
} // tools
} // ldk
} // wns



