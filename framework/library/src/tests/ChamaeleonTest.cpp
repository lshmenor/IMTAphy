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

#include <WNS/Chamaeleon.hpp>
#include <WNS/TestFixture.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>
#include <vector>

namespace wns {
    /**
     * @brief Test for the Chamaeleon template
     * @author Marc Schinnenburg <marc@schinnenburg.net>
     */
    class ChamaeleonTest :
        public wns::TestFixture
    {
        // begin example "wns.ChamaeleonTestClasses.example"
        // Classic classes for testing. They all have the same "test"
        // method which returns a different integer (depending on the
        // type). Furthermore A and C have a common base type (A).
        class A
        {
        public:
            virtual int
            test()
            {
                return 1;
            }

            virtual
            ~A()
            {
            }
        };

        class B {
        public:
            virtual int
            test()
            {
                return 2;
            }

            virtual
            ~B()
            {
            }
        };

        class C :
            public A
        {
        public:
            virtual int
            test()
            {
                return 3;
            }

            virtual
            ~C()
            {
            }
        };
        // end example

        CPPUNIT_TEST_SUITE( ChamaeleonTest );
        CPPUNIT_TEST( constructor );
        CPPUNIT_TEST( assignment );
        CPPUNIT_TEST( conversion );
        CPPUNIT_TEST( unHide );
        CPPUNIT_TEST( withReference );
        CPPUNIT_TEST( withPointer );
        CPPUNIT_TEST( inVector );
        CPPUNIT_TEST_SUITE_END();
    public:
        CPPUNIT_TEST_SUITE_REGISTRATION(ChamaeleonTest);

        virtual void
        prepare()
        {}

        virtual void
        cleanup()
        {}

        void
        constructor()
        {
            const int xx = 4711;
            // Normal constructor
            Chamaeleon<int> aa(xx);
            CPPUNIT_ASSERT_EQUAL( xx, aa.unHide() );

            Chamaeleon<int> cc(xx);
            // Copy constructor
            Chamaeleon<int> cc2(cc);
            CPPUNIT_ASSERT_EQUAL( xx, cc2.unHide() );
        }

        void
        assignment()
        {
            const int xx = 4711;
            Chamaeleon<int> aa(xx);
            Chamaeleon<int> bb(0);
            bb = aa;
            CPPUNIT_ASSERT_EQUAL( xx, aa.unHide() );
            CPPUNIT_ASSERT_EQUAL( xx, bb.unHide() );
        }

        void
        conversion()
        {
            // This is more a compile test ...
            const long int xx = 4711;
            Chamaeleon<long int> aa(xx);
            Chamaeleon<unsigned long int> bb(aa);

            bb = aa;

            CPPUNIT_ASSERT_EQUAL( static_cast<unsigned long int>(xx), bb.unHide() );

            Chamaeleon<C*> cPtr(NULL);
            Chamaeleon<A*> aPtr(cPtr);
            aPtr = cPtr;
        }

        void
        withReference()
        {
            int xx = 4711;
            Chamaeleon<int&> aa(xx);
            CPPUNIT_ASSERT_EQUAL( xx, aa.unHide() );
        }

        void
        withPointer()
        {
            A* aPtr = new A();
            Chamaeleon<A*> aa(aPtr);
            CPPUNIT_ASSERT_EQUAL( 1, aa.unHide()->test() );
        }

        void
        unHide()
        {
            // begin example "wns.ChamaeleonConstructor.example"
            // The Chamaeleon takes a pointer
            Chamaeleon<A*> a(new A);
            // end example

            CPPUNIT_ASSERT_EQUAL( 1, a.unHide()->test() );

            // begin example "wns.ChamaeleonUnHide.example"
            // Create three Chamaeleons
            Chamaeleon<B*> b(new B);
            Chamaeleon<C*> c(new C);
            Chamaeleon<A*> c_a(new C);

            // unHide the real objects
            B* bPtr = b.unHide();
            C* cPtr = c.unHide();
            A* c_aPtr = c_a.unHide();

            // Check if everything works
            CPPUNIT_ASSERT_EQUAL( 2, bPtr->test() );
            CPPUNIT_ASSERT_EQUAL( 3, cPtr->test() );
            CPPUNIT_ASSERT_EQUAL( 3, c_aPtr->test() );
            // end example

            delete a.unHide();
            delete bPtr;
            delete cPtr;
            delete c_aPtr;
        }

        void
        inVector()
        {
            // begin example "wns.Chamaeleon.example"
            // This example shows how objects without a common base class can be
            // stored in one container with the help of Chamaeleons
            Chamaeleon<A*>* a = new Chamaeleon<A*>(new A);
            Chamaeleon<B*>* b = new Chamaeleon<B*>(new B);

            // Create a vector that takes ChamaeleonBase*
            std::vector<ChamaeleonBase*> v;

            // Now we can put the Chamaeleons in the vector (even though the real
            // objects didn't have a common base class!)
            v.push_back(a);
            v.push_back(b);

            // to get them back we need to remember the position of the Chamaeleons
            Chamaeleon<A*>* a2 = v[0]->downCast<A*>();
            Chamaeleon<B*>* b2 = v[1]->downCast<B*>();

            // This should throw a wns::ChamaeleonBase::BadCast (since the type is wrong)
            CPPUNIT_ASSERT_THROW(v[1]->downCast<A*>(), wns::ChamaeleonBase::BadCast);


            // Finally check if unhiding works:
            CPPUNIT_ASSERT_EQUAL( 1, a2->unHide()->test() );
            CPPUNIT_ASSERT_EQUAL( 2, b2->unHide()->test() );
            // end example

            delete a->unHide();
            delete b->unHide();
            delete a;
            delete b;
        }
    };
}

