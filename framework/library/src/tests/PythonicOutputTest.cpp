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

#include <WNS/PythonicOutput.hpp>
#include <WNS/TestFixture.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

namespace wns { namespace tests {

    /**
     * @brief test for wns::PythonicOutput
     * @author Marc Schinnenburg <marc@schinnenburg.com>
     */
    class PythonicOutputTest :
        public wns::TestFixture
    {
        // begin example "wns.PythonicOutput.default.example"
        class A :
            virtual public wns::PythonicOutput
        {
        };
        // end example

        // begin example "wns.PythonicOutput.special.example"
        class B :
            virtual public wns::PythonicOutput
        {
        public:
            explicit
            B(const std::string& name) :
                name_(name)
            {
            }

        private:
            virtual std::string
            doToString() const
            {
                return "My name is " + name_ + " (" + wns::PythonicOutput::doToString() + ")";
            }

            std::string name_;
        };
        // end example

        CPPUNIT_TEST_SUITE( PythonicOutputTest );
        CPPUNIT_TEST( testDefault );
        CPPUNIT_TEST( testSpecial );
        CPPUNIT_TEST( testDoubleStream );
        CPPUNIT_TEST_SUITE_END();
    public:

        void
        prepare()
        {
        }

        void
        cleanup()
        {
        }

        void
        testDefault()
        {
            std::stringstream expected;
            std::stringstream actual;
            A a;
            expected << "<wns::tests::PythonicOutputTest::A instance at " << &a << ">";
            actual << a;
            CPPUNIT_ASSERT_MESSAGE( actual.str(),  expected.str() == actual.str() );
        }

        void
        testSpecial()
        {
            std::stringstream expected;
            std::stringstream actual;
            B b("Peter");
            expected << "My name is Peter (<wns::tests::PythonicOutputTest::B instance at " << &b << ">)";
            actual << b;

            CPPUNIT_ASSERT_MESSAGE( actual.str(), expected.str() == actual.str() );
        }

        void
        testDoubleStream()
        {
            std::stringstream expected;
            std::stringstream actual;
            B b("Peter");
            expected << "My name is Peter (<wns::tests::PythonicOutputTest::B instance at " << &b << ">)"
                     << "My name is Peter (<wns::tests::PythonicOutputTest::B instance at " << &b << ">)";
            actual << b << b;

            CPPUNIT_ASSERT_MESSAGE( actual.str(), expected.str() == actual.str() );
        }
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( PythonicOutputTest );

} // tests
} // wns
