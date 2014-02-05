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


#include <WNS/Exception.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace tests {

    class ExceptionTest :
        public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( ExceptionTest );
        CPPUNIT_TEST( defaultConstructor );
        CPPUNIT_TEST( stringConstructor );
        CPPUNIT_TEST( copyConstructor );
        CPPUNIT_TEST( assignment );
        CPPUNIT_TEST( streams );
        CPPUNIT_TEST( catchStdException );
        CPPUNIT_TEST( catchWnsException );
        CPPUNIT_TEST( rethrow );
        CPPUNIT_TEST_SUITE_END();

    public:

        void
        prepare()
        {}

        void
        cleanup()
        {}

        void
        defaultConstructor()
        {
            Exception e;
            CPPUNIT_ASSERT( std::string(e.what()) == std::string("") );
        }

        void
        stringConstructor()
        {
            Exception e("foobar");
            CPPUNIT_ASSERT( std::string(e.what()) == std::string("foobar") );

            std::string s("foobar");
            Exception e2(s);
            CPPUNIT_ASSERT( std::string(e2.what()) == s );
        }

        void
        copyConstructor()
        {
            Exception e("foobar");
            Exception e2(e);
            CPPUNIT_ASSERT( std::string(e2.what()) == "foobar" );
        }

        void
        assignment()
        {
            Exception e("foobar");
            Exception e2;
            e2 = e;
            CPPUNIT_ASSERT( std::string(e2.what()) == "foobar" );
        }

        void
        streams()
        {
            Exception e;
            e << "foo" << "bar";
            CPPUNIT_ASSERT( std::string(e.what()) == std::string("foobar") );

            std::stringstream ss;
            ss << e;
            CPPUNIT_ASSERT( ss.str() == std::string("foobar") );
        }

        void
        catchStdException()
        {
            try
            {
                throw wns::Exception();
            }
            catch(const std::exception&)
            {
                // should be catched here
            }
            catch(...)
            {
                CPPUNIT_FAIL("wns::Exception should be catchable as std::Exception");
            }
        }

        void
        catchWnsException()
        {
            try
            {
                throw wns::Exception();
            }
            catch(const wns::Exception&)
            {
                // should be catched here
            }
            catch(...)
            {
                CPPUNIT_FAIL("wns::Exception should be catchable as wns::Exception ;-)");
            }
        }

        void
        rethrow()
        {
            try
            {
                try
                {
                    throw wns::Exception();
                }
                catch(...)
                {
                    throw;
                }
            }
            catch(const wns::Exception&)
            {
                // should be catched here
            }
            catch(...)
            {
                CPPUNIT_FAIL("wns::Exception should be catchable as wns::Exception ;-)");
            }
        }
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( ExceptionTest );

} // namespace tests
} // namespace wns

