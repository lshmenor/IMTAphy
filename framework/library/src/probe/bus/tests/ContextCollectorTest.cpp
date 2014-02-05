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

#include <WNS/TestFixture.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/probe/bus/tests/ProbeBusStub.hpp>

#include <boost/tuple/tuple.hpp>

namespace wns { namespace probe { namespace bus { namespace tests {

    class ContextCollectorTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( ContextCollectorTest );
        CPPUNIT_TEST( tupleContext );
        CPPUNIT_TEST_SUITE_END();
    public:
        void prepare();
        void cleanup();
        void tupleContext();
    };

    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ContextCollectorTest, wns::testsuite::Default() );

}
}
}
}

using namespace wns::probe::bus::tests;

void
ContextCollectorTest::prepare()
{
}

void
ContextCollectorTest::cleanup()
{
}

void
ContextCollectorTest::tupleContext()
{
    ContextCollector cc_("testSource");

    ProbeBusStub pb;

    pb.startObserving(wns::simulator::getProbeBusRegistry()->getMeasurementSource("testSource"));

    cc_.put(1.23, boost::make_tuple("IntContext", 1, "StringContext", "hansi"));

    CPPUNIT_ASSERT(pb.lastContext == "{IntContext : 1,StringContext : 'hansi',}");
}

