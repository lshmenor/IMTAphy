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

#include <WNS/ldk/fcf/NewFrameProviderObserver.hpp>

#include <WNS/Exception.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk { namespace fcf {

    class NewFrameProviderObserverTest :
        public wns::TestFixture
    {
    CPPUNIT_TEST_SUITE( NewFrameProviderObserverTest );
    CPPUNIT_TEST( testAttachObserver );
    CPPUNIT_TEST( testAttachObserverAlreadyAttached );
    CPPUNIT_TEST( testDetachObserver );
    CPPUNIT_TEST( testDetachObserverNotAttached );
    CPPUNIT_TEST( notifyNewFrameObservers );
    CPPUNIT_TEST( testDestroyObserver );
    CPPUNIT_TEST( testDestroyProvider );
    CPPUNIT_TEST_SUITE_END();

    public:
        class NewFrameObserverTest :
            public NewFrameObserver
        {
        public:
            NewFrameObserverTest();

            virtual void
            messageNewFrame();

            virtual void
            newFrameProviderDeleted();

            int newFrameProviderDeleted_;
            int messageNewFrameGet_;

        };

        void prepare();
        void cleanup();

        void testAttachObserver();
        void testAttachObserverAlreadyAttached();
        void testDetachObserver();
        void testDetachObserverNotAttached();
        void notifyNewFrameObservers();
        void testDestroyObserver();
        void testDestroyProvider();

    private:
        NewFrameProvider* newFrameProvider;
        NewFrameObserverTest* newFrameObserverTest;
        NewFrameObserverTest* secondNewFrameObserverTest;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( NewFrameProviderObserverTest );

    NewFrameProviderObserverTest::NewFrameObserverTest::NewFrameObserverTest() :
        NewFrameObserver("TestObserver"),
        newFrameProviderDeleted_(0),
            messageNewFrameGet_(0)
    {
    } // NewFrameObserverTest



    void
    NewFrameProviderObserverTest::NewFrameObserverTest::newFrameProviderDeleted()
    {
        ++newFrameProviderDeleted_;
    } // NewFrameProviderDeleted



    void
    NewFrameProviderObserverTest::NewFrameObserverTest::messageNewFrame()
    {
        ++messageNewFrameGet_;
    } // messageNewFrame



    void
    NewFrameProviderObserverTest::prepare()
    {
        newFrameProvider = new NewFrameProvider("TestStation");
        newFrameObserverTest = new NewFrameObserverTest();
        secondNewFrameObserverTest = new NewFrameObserverTest();
    } // prepare



    void
    NewFrameProviderObserverTest::cleanup()
    {
        if (newFrameObserverTest)
            delete newFrameObserverTest;
        if (secondNewFrameObserverTest)
            delete secondNewFrameObserverTest;
        if (newFrameProvider)
            delete newFrameProvider;
    } // catch ExceptionTearDown




    /********** Tests *****************************/

    void
    NewFrameProviderObserverTest::testAttachObserver()
    {

        for (int i = 1; i <= 5; i++)
        {
            newFrameProvider->notifyNewFrameObservers();
        }
        CPPUNIT_ASSERT_EQUAL(0, newFrameObserverTest->messageNewFrameGet_);
        CPPUNIT_ASSERT_EQUAL(0, secondNewFrameObserverTest->
                     messageNewFrameGet_);



        newFrameProvider->attachObserver(newFrameObserverTest);

        for (int i = 1; i <= 5; i++)
        {
            newFrameProvider->notifyNewFrameObservers();
        }
        CPPUNIT_ASSERT_EQUAL(5, newFrameObserverTest->messageNewFrameGet_);
        CPPUNIT_ASSERT_EQUAL(0, secondNewFrameObserverTest->
                     messageNewFrameGet_);


        newFrameProvider->attachObserver(secondNewFrameObserverTest);

        for (int i = 1; i <= 5; i++)
        {
            newFrameProvider->notifyNewFrameObservers();
        }
        CPPUNIT_ASSERT_EQUAL(10, newFrameObserverTest->
                     messageNewFrameGet_);
        CPPUNIT_ASSERT_EQUAL(5, secondNewFrameObserverTest->
                     messageNewFrameGet_);

    } // testAttachObserver



    void
    NewFrameProviderObserverTest::testAttachObserverAlreadyAttached()
    {
        newFrameProvider->attachObserver(newFrameObserverTest);

        WNS_ASSERT_ASSURE_EXCEPTION(newFrameProvider
                    ->attachObserver(newFrameObserverTest));
    } // testAttachObserverAlreadyAttached



    void
    NewFrameProviderObserverTest::testDetachObserver()
    {
        newFrameProvider->attachObserver(newFrameObserverTest);
        newFrameProvider->attachObserver(secondNewFrameObserverTest);

        for (int i = 1; i <= 5; i++)
        {
            newFrameProvider->notifyNewFrameObservers();
        }
        CPPUNIT_ASSERT_EQUAL(5, newFrameObserverTest->
                     messageNewFrameGet_);
        CPPUNIT_ASSERT_EQUAL(5, secondNewFrameObserverTest->
                     messageNewFrameGet_);



        newFrameProvider->detachObserver(newFrameObserverTest);
        CPPUNIT_ASSERT_EQUAL(1, newFrameObserverTest->
                     newFrameProviderDeleted_);
        CPPUNIT_ASSERT_EQUAL(0, secondNewFrameObserverTest->
                     newFrameProviderDeleted_);

        for (int i = 1; i <= 5; i++)
        {
            newFrameProvider->notifyNewFrameObservers();
        }
        CPPUNIT_ASSERT_EQUAL(5, newFrameObserverTest->
                     messageNewFrameGet_);
        CPPUNIT_ASSERT_EQUAL(10, secondNewFrameObserverTest->
                     messageNewFrameGet_);



        newFrameProvider->detachObserver(secondNewFrameObserverTest);
        CPPUNIT_ASSERT_EQUAL(1, newFrameObserverTest->
                     newFrameProviderDeleted_);
        CPPUNIT_ASSERT_EQUAL(1, secondNewFrameObserverTest->
                     newFrameProviderDeleted_);

        for (int i = 1; i <= 5; i++)
        {
            newFrameProvider->notifyNewFrameObservers();
        }
        CPPUNIT_ASSERT_EQUAL(5, newFrameObserverTest->
                     messageNewFrameGet_);
        CPPUNIT_ASSERT_EQUAL(10, secondNewFrameObserverTest->
                     messageNewFrameGet_);

    } // testDetachObserver



    void
    NewFrameProviderObserverTest::testDetachObserverNotAttached()
    {
        WNS_ASSERT_ASSURE_EXCEPTION(newFrameProvider->detachObserver(newFrameObserverTest));
    } // testDetachObserverNotAttached



    void
    NewFrameProviderObserverTest::notifyNewFrameObservers()
    {
        newFrameProvider->attachObserver(newFrameObserverTest);
        newFrameProvider->attachObserver(secondNewFrameObserverTest);

        for (int i = 1; i <= 5; i++)
        {
            newFrameProvider->notifyNewFrameObservers();
        }

        CPPUNIT_ASSERT_EQUAL(5, newFrameObserverTest-> messageNewFrameGet_);
        CPPUNIT_ASSERT_EQUAL(5, secondNewFrameObserverTest-> messageNewFrameGet_);

    } // notifyNewFrameObservers



    void
    NewFrameProviderObserverTest::testDestroyObserver()
    {
        newFrameProvider->attachObserver(newFrameObserverTest);
        newFrameProvider->attachObserver(secondNewFrameObserverTest);

        delete newFrameObserverTest;

        NewFrameProvider::NewFrameObservers newFrameObservers;
        newFrameObservers = newFrameProvider->testGetNewFrameObservers_();
        CPPUNIT_ASSERT(std::find(newFrameObservers.begin(),
                           newFrameObservers.end(),
                           newFrameObserverTest) 
                     == newFrameObservers.end());

        newFrameObserverTest = NULL;

        for (int i = 1; i <= 5; i++)
        {
            newFrameProvider->notifyNewFrameObservers();
        }

        CPPUNIT_ASSERT_EQUAL(5, secondNewFrameObserverTest->
                 messageNewFrameGet_);

    } // testDestroyObserver



    void
    NewFrameProviderObserverTest::testDestroyProvider()
    {
        newFrameProvider->attachObserver(newFrameObserverTest);
        newFrameProvider->attachObserver(secondNewFrameObserverTest);


        delete newFrameProvider;
        newFrameProvider = NULL;

        CPPUNIT_ASSERT(NULL == newFrameObserverTest->
                   testGetNewFrameProvider_());
        CPPUNIT_ASSERT(NULL == secondNewFrameObserverTest->
                   testGetNewFrameProvider_());

    } // testDestroyProvider

}
}
} // wns::ldk::fcf


