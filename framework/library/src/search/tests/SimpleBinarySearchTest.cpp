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

#include <WNS/search/ISearch.hpp>
#include <WNS/search/SearchCreator.hpp>

#include <WNS/TestFixture.hpp>

namespace wns { namespace search { namespace tests {

            class SimpleBinarySearchTest
                : public wns::TestFixture
            {
                CPPUNIT_TEST_SUITE( SimpleBinarySearchTest );
                CPPUNIT_TEST( testSearch );
                CPPUNIT_TEST( testNoResult );
                CPPUNIT_TEST_SUITE_END();

            public:
                void
                testSearch();

                void
                testNoResult();

            private:

                class SimpleComparator
                {
                public:
                    SimpleComparator(int value)
                        : value_(value)
                    {}

                    bool
                    operator()(int currentValue)
                    {
                        if (currentValue <= value_)
                            return true;
                        return false;
                    }

                private:
                    int value_;
                };

                virtual void
                prepare();

                virtual void
                cleanup();

            }; // SimpleBinarySearchTest

            CPPUNIT_TEST_SUITE_REGISTRATION( SimpleBinarySearchTest );

            void
            SimpleBinarySearchTest::prepare()
            {} // prepare

            void
            SimpleBinarySearchTest::cleanup()
            {} // cleanup

            void
            SimpleBinarySearchTest::testSearch()
            {
                for (int upperBound = 1; upperBound < 1024; upperBound++)
                {
                    for (int value = 0; value < upperBound; value++)
                    {
                        ISearch* search = SearchFactory::creator("wns.search.SimpleBinarySearch")
                            ->create(0, 1, SimpleComparator(value));

                        CPPUNIT_ASSERT_EQUAL(value, search->search(upperBound));

                        delete search;
                    }
                }
            } // testSearch

            void
            SimpleBinarySearchTest::testNoResult()
            {
                ISearch* search = SearchFactory::creator("wns.search.SimpleBinarySearch")
                    ->create(1, 1, SimpleComparator(0));

                CPPUNIT_ASSERT_EQUAL(0, search->search(16));

                delete search;
            } // testNoResult

        }
    }
}


