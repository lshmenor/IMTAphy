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

#include <WNS/fsm/FSM.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/CppUnit.hpp>
#include <WNS/Exception.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace fsm { namespace tests {

    class FSMTest :
        public wns::TestFixture
    {
    public:
        /**
         * Specify the Interfaces of the FSM
         */
        // begin example "FSM::LightControlSignals.example"
        class LightControlSignals
        {

        public:
            // the four signals as pure virtual functions
            virtual LightControlSignals*
            light() = 0;

            virtual LightControlSignals*
            dark() = 0;

            virtual LightControlSignals*
            movementDetected() = 0;

            virtual LightControlSignals*
            timeElapsed() = 0;

            // abstract interface with virtual functions needs
            // virtual destructor (otherwise g++ complains)
            virtual
            ~LightControlSignals()
            {
            }

        protected:
            // only called from derived classes
            LightControlSignals()
            {
            }

        private:
            LightControlSignals(const LightControlSignals&);
        };
        // end example

        // begin example "FSM::Lamp.example"
        struct Lamp
        {
            Lamp() :
                on(false)
            {
            }

            bool on;
        };
        // end example

        // begin example "FSM::LightControlInterface.example"
        typedef FSM<LightControlSignals, Lamp> LightControlInterface;
        // end example

        // begin example "FSM::LightControl.example"
        class LightControl :
            public LightControlInterface
        {
        public:
            LightControl(const LightControlInterface::VariablesType& v);

            void
            movementDetected();

            void
            timeElapsed();

            void
            light();

            void
            dark();

            Lamp&
            lamp()
            {
                return getVariables();
            }
        };
        // end example

        // begin example "FSM::Day.example"
        class Day :
            public LightControlInterface::StateInterface
        {
        public:
            Day(LightControlInterface* lci) :
                LightControlInterface::StateInterface(lci, "wns_fsm_tests_Day")
            {
            }

            virtual StateInterface*
            movementDetected();

            virtual StateInterface*
            timeElapsed();

            virtual StateInterface*
            light();

            virtual StateInterface*
            dark();
        };
        // end example

        // begin example "FSM::Night.example"
        class Night :
            public LightControlInterface::StateInterface
        {
        public:
            Night(LightControlInterface* lci) :
                LightControlInterface::StateInterface(lci, "wns_fsm_tests_Night")
            {
            }

            virtual StateInterface*
            movementDetected();

            virtual StateInterface*
            timeElapsed();

            virtual StateInterface*
            light();

            virtual StateInterface*
            dark();
        };
        // end example

        // begin example "FSM::LampOn.example"
        class LampOn :
            public LightControlInterface::StateInterface
        {
        public:
            LampOn(LightControlInterface* lci) :
                LightControlInterface::StateInterface(lci, "wns_fsm_tests_LampOn")
            {
            }

            virtual StateInterface*
            movementDetected();

            virtual StateInterface*
            timeElapsed();

            virtual StateInterface*
            light();

            virtual StateInterface*
            dark();
        };
        // end example

        ///////////////////////////////////////////////////////////////
        // Test setup
        void prepare();
        void cleanup();
        void sendSignals();
        void multipleStateCreations();

        CPPUNIT_TEST_SUITE( FSMTest );
        CPPUNIT_TEST( sendSignals );
        CPPUNIT_TEST( multipleStateCreations );
        CPPUNIT_TEST_SUITE_END();

        Lamp* l;
        LightControl* lc;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( FSMTest );

    ///////////////////////////////////////////////////////////////
    // Testing
    void
    FSMTest::prepare()
    {
        l = new Lamp();
        lc = new LightControl(*l);
    }

    void
    FSMTest::cleanup()
    {
        delete lc;
        delete l;
    }

    void
    FSMTest::sendSignals()
    {
        CPPUNIT_ASSERT_EQUAL( std::string("wns_fsm_tests_Night"), lc->getStateName() );

        lc->dark();
        CPPUNIT_ASSERT( !lc->lamp().on );
        CPPUNIT_ASSERT_EQUAL( std::string("wns_fsm_tests_Night"), lc->getStateName() );

        lc->movementDetected();
        CPPUNIT_ASSERT( lc->lamp().on );
        CPPUNIT_ASSERT_EQUAL( std::string("wns_fsm_tests_LampOn"), lc->getStateName() );

        lc->light();
        CPPUNIT_ASSERT( !lc->lamp().on );
        CPPUNIT_ASSERT_EQUAL( std::string("wns_fsm_tests_Day"), lc->getStateName() );

        CPPUNIT_ASSERT_THROW( lc->timeElapsed(), Exception );
        CPPUNIT_ASSERT_EQUAL( std::string("wns_fsm_tests_Day"), lc->getStateName() );

        lc->dark();
        CPPUNIT_ASSERT( !lc->lamp().on );
        CPPUNIT_ASSERT_EQUAL( std::string("wns_fsm_tests_Night"), lc->getStateName() );

        lc->movementDetected();
        CPPUNIT_ASSERT( lc->lamp().on );
        CPPUNIT_ASSERT_EQUAL( std::string("wns_fsm_tests_LampOn"), lc->getStateName() );

        lc->timeElapsed();
        CPPUNIT_ASSERT( !lc->lamp().on );
        CPPUNIT_ASSERT_EQUAL( std::string("wns_fsm_tests_Night"), lc->getStateName() );
    }

    void
    FSMTest::multipleStateCreations()
    {
        lc->createState<LampOn>();
        WNS_ASSERT_ASSURE_EXCEPTION( lc->createState<LampOn>() );
    }

    // begin example "FSM::LightControlMethods.example"
    FSMTest::LightControl::LightControl(const FSMTest::LightControlInterface::VariablesType& v) :
        FSMTest::LightControlInterface(v)
    {
        changeState(createState<Night>());
    }

    void
    FSMTest::LightControl::movementDetected()
    {
        changeState(getState()->movementDetected());
    }

    void
    FSMTest::LightControl::timeElapsed()
    {
        changeState(getState()->timeElapsed());
    }

    void
    FSMTest::LightControl::light()
    {
        changeState(getState()->light());
    }

    void
    FSMTest::LightControl::dark()
    {
        changeState(getState()->dark());
    }
    // end example

    // begin example "FSM::DayMethods.example"
STATIC_FACTORY_REGISTER_WITH_CREATOR(FSMTest::Day,
                        FSMTest::LightControlInterface::StateInterface,
                        "wns_fsm_tests_Day",
                        FSMConfigCreator);

    FSMTest::Day::StateInterface*
    FSMTest::Day::movementDetected()
    {
        // ignore
        return this;
    }

    FSMTest::Day::StateInterface*
    FSMTest::Day::timeElapsed()
    {
        throw(Exception("Signal only valid in state LampOn"));
        return this;
    }

    FSMTest::Day::StateInterface*
    FSMTest::Day::light()
    {
        // ignore
        return this;
    }

    FSMTest::Day::StateInterface*
    FSMTest::Day::dark()
    {
        return getFSM()->createState<Night>();
    }
    // end example

    // begin example "FSM::NightMethods.example"
STATIC_FACTORY_REGISTER_WITH_CREATOR(FSMTest::Night,
                     FSMTest::LightControlInterface::StateInterface,
                     "wns_fsm_tests_Night",
                     FSMConfigCreator);

    FSMTest::Night::StateInterface*
    FSMTest::Night::movementDetected()
    {
        vars().on = true;
        return getFSM()->createState<LampOn>();
    }

    FSMTest::Night::StateInterface*
    FSMTest::Night::timeElapsed()
    {
        throw(Exception("Signal only valid in state LampOn"));
        return this;
    }

    FSMTest::Night::StateInterface*
    FSMTest::Night::light()
    {
        return getFSM()->createState<Day>();
    }

    FSMTest::Night::StateInterface*
    FSMTest::Night::dark()
    {
        // ignore
        return this;
    }
    // end example

    // begin example "FSM::LampOnMethods.example"
STATIC_FACTORY_REGISTER_WITH_CREATOR(FSMTest::LampOn,
                     FSMTest::LightControlInterface::StateInterface,
                     "wns_fsm_tests_LampOn",
                     FSMConfigCreator);

    FSMTest::LampOn::StateInterface*
    FSMTest::LampOn::movementDetected()
    {
        // ignore
        return this;
    }

    FSMTest::LampOn::StateInterface*
    FSMTest::LampOn::timeElapsed()
    {
        vars().on = false;
        return getFSM()->createState<Night>();
    }

    FSMTest::LampOn::StateInterface*
    FSMTest::LampOn::light()
    {
        vars().on = false;
        return getFSM()->createState<Day>();
    }

    FSMTest::LampOn::StateInterface*
    FSMTest::LampOn::dark()
    {
        // ignore
        return this;
    }
    // end example

} // tests
} // fsm
} // wsn


