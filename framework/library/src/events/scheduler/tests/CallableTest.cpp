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

#include <WNS/events/scheduler/Callable.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace events { namespace tests {

    class CallableTest :
        public wns::TestFixture
    {
    public:
        CPPUNIT_TEST_SUITE( CallableTest );
        CPPUNIT_TEST( testCallable );
        CPPUNIT_TEST_SUITE_END();

        struct Foo
        {
            Foo(int& globalBar) :
                bar_(0),
                globalBar_(globalBar)
            {
            }

            void
            operator()()
            {
                ++bar_;
                ++globalBar_;
            }

            int bar_;
            int& globalBar_;
        };

        virtual void
        prepare()
        {
        }

        virtual void
        cleanup()
        {
        }

        void
        testCallable()
        {
            int globalBar_ = 0;
            Foo f(globalBar_);
            scheduler::Callable callable1(f);

            callable1();

            CPPUNIT_ASSERT_EQUAL(0, f.bar_);

            CPPUNIT_ASSERT_EQUAL(1, globalBar_);
        }
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( CallableTest );
}
}
}
