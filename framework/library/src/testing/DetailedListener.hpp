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

#ifndef WNS_TESTING_DETAILEDLISTENER_HPP
#define WNS_TESTING_DETAILEDLISTENER_HPP

#include <WNS/StopWatch.hpp>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/Test.h>

namespace wns { namespace testing {
    /**
     * @brief Prints the name of each test and "OK" or "FAILED" plus the time needed
     * for the test
     * @author Daniel Bültmann <dbn@comnets.rwth-aachen.de>
     * @author Marc Schinnenburg <marc@schinnenburg.net>
     */
    class DetailedListener :
        public CppUnit::TextTestProgressListener
    {

    public:
        void
        startTest(CppUnit::Test* test);

        void
        endTest(CppUnit::Test* test);

        void
        addFailure(const CppUnit::TestFailure& failure);

    private:
        bool failed;
        wns::StopWatch sw;
    };
} // namespace testing
} // namespace wns

#endif // NOT defined WNS_TESTING_DETAILEDLISTENER_HPP
