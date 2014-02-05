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

#include <WNS/service/tl/PortPool.hpp>

#include <WNS/TestFixture.hpp>

namespace wns { namespace service { namespace tl { namespace tests {
	class PortPoolTest
		: public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( PortPoolTest );
		CPPUNIT_TEST( defaultConstructor );
		CPPUNIT_TEST( portInRange );
		CPPUNIT_TEST( searchFreePort );
		CPPUNIT_TEST( freePort );
		CPPUNIT_TEST( checkTimeout );
		CPPUNIT_TEST( usePort );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();
		void defaultConstructor();
		void portInRange();
		void searchFreePort();
		void freePort();
		void checkTimeout();
		void usePort();
	};
} // test
} // tl
} // service
} // wns

using namespace wns::service::tl::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( PortPoolTest );

void PortPoolTest::setUp()
{
	wns::simulator::getEventScheduler()->reset();
}

void PortPoolTest::tearDown()
{
}

void PortPoolTest::defaultConstructor()
{
	PortPool portPool(60.0);

	CPPUNIT_ASSERT(portPool.getUnbindDelay() == 60.0);
	CPPUNIT_ASSERT(portPool.isAvailable(1));
	CPPUNIT_ASSERT(portPool.getCapacity() == 65534);
}

void PortPoolTest::portInRange()
{
	PortPool portPool(60.0);

	WNS_ASSERT_ASSURE_EXCEPTION(portPool.isAvailable(65536));
	WNS_ASSERT_ASSURE_EXCEPTION(portPool.isAvailable(0));
}

void PortPoolTest::searchFreePort()
{
	PortPool portPool(60.0, 1, 100);

	for(int i=1; i <= 100; ++i)
	{
		portPool.bind(i);
	}
	// Try to bind twice
	for(int i=1; i <= 100; ++i)
	{
		WNS_ASSERT_ASSURE_EXCEPTION(portPool.bind(i));
	}
	WNS_ASSERT_ASSURE_EXCEPTION(portPool.suggestPort());
}

void PortPoolTest::freePort()
{
	PortPool portPool(60.0);

	wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
	CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

	// port numbers out of range
	WNS_ASSERT_ASSURE_EXCEPTION(portPool.unbind(65536));
	WNS_ASSERT_ASSURE_EXCEPTION(portPool.unbind(0));

	// port to unbind not in use
	WNS_ASSERT_ASSURE_EXCEPTION(portPool.unbind(portPool.suggestPort()));

	portPool.bind(500);

	int portsInUse = portPool.getCapacity();

	// Unbind should not directly return the port to the port pool.
	// A configurable timeout should be used to delay the unbinding of the port.

	portPool.unbind(500);

	// No time has passed by now.
	CPPUNIT_ASSERT(portsInUse == portPool.getCapacity());
	WNS_ASSERT_ASSURE_EXCEPTION(portPool.bind(500));

	// Let time pass, i.e. process the delayed unbind event.
	scheduler->processOneEvent();

	CPPUNIT_ASSERT(portsInUse + 1 == portPool.getCapacity());
	CPPUNIT_ASSERT(scheduler->getTime() == 60.0);
}

void PortPoolTest::checkTimeout()
{
	PortPool portPool(60.0);

	wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
	CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler

	portPool.bind(500);
	portPool.unbind(500);

	// Let time pass, i.e. process the delayed unbind event.
	scheduler->processOneEvent();
	CPPUNIT_ASSERT(scheduler->getTime() == 60.0);

	PortPool otherPortPool(12.0);

	otherPortPool.bind(500);
	otherPortPool.unbind(500);

	// Let time pass, i.e. process the delayed unbind event.
	scheduler->processOneEvent();
	CPPUNIT_ASSERT(scheduler->getTime() == 72.0);
}

void PortPoolTest::usePort()
{
	PortPool portPool(60.0);
	int portsInUse = portPool.getCapacity();

	// port numbers out of range
	WNS_ASSERT_ASSURE_EXCEPTION(portPool.bind(65536));
	WNS_ASSERT_ASSURE_EXCEPTION(portPool.bind(0));

	// port is not available
	portPool.bind(500);
	WNS_ASSERT_ASSURE_EXCEPTION(portPool.bind(500));

	CPPUNIT_ASSERT(portsInUse == portPool.getCapacity() + 1);
}

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
