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

#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/CppUnit.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/ldk/RoundRobinDeliverer.hpp>

using namespace wns::ldk;
using namespace wns::ldk::tests;

class RoundRobinDelivererTest :
	public wns::TestFixture
{
	CPPUNIT_TEST_SUITE( RoundRobinDelivererTest );
	CPPUNIT_TEST_SUITE_END();

protected:
	void
	prepare()
	{
	}

	void
	cleanup()
	{
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( RoundRobinDelivererTest );



