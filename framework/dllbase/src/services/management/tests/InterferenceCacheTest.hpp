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

#ifndef DLL_SERVICES_MANAGEMENT_TESTS_INTERFERENCECACHETEST_HPP
#define DLL_SERVICES_MANAGEMENT_TESTS_INTERFERENCECACHETEST_HPP


#include <WNS/ldk/ldk.hpp>
#include <WNS/ldk/ManagementServiceInterface.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <cppunit/extensions/HelperMacros.h>


namespace dll { 

class ILayer2;

namespace services { namespace management { namespace tests {

class InterferenceCacheTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( InterferenceCacheTest );
    CPPUNIT_TEST( writeData );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void writeData();

private:
    std::auto_ptr<wns::ldk::tests::LayerStub> layer_;
    std::auto_ptr<wns::ldk::ManagementServiceRegistry> msr_;
};

}
}
}
}
#endif 

