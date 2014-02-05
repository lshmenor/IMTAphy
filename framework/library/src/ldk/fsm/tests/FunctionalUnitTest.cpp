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

#include <WNS/ldk/fsm/FunctionalUnit.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>

#include <WNS/Cloneable.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk { namespace fsm { namespace tests {

    class FunctionalUnitTest :
        public wns::TestFixture
    {
    private:
        /**
         * Variables available in FU with FSM support
         */
        struct Variables
        {
        };

        /**
         * Handy typedef to Interface of FU with FSM support and
         * respective Variables
         */
        typedef ldk::fsm::FunctionalUnit<Variables> MyFUInterface;

    public:
        /**
         * Handy typedef to StateInterface of the basic FSM; used as
         * return type of signals in state definitions/implementations
         */
        typedef MyFUInterface::BaseFSM::StateInterface MyStateInterface;

        /**
         * Special implementation of MyFUInterface (you can have several
         * implementations, working with the same implementation of the
         * states)
         */
        class Testee :
            public MyFUInterface,
            public CommandTypeSpecifier<>,
            public HasReceptor<>,
            public HasConnector<>,
            public HasDeliverer<>,
            public Cloneable<Testee>
        {
        public:
            Testee(fun::FUN* fun) :
                MyFUInterface(Variables()),
                CommandTypeSpecifier<>(fun),
                HasReceptor<>(),
                HasConnector<>(),
                HasDeliverer<>(),
                Cloneable<Testee>()
            {
                changeState(createState<ReadyForTransmission>());
            }

            virtual
            ~Testee()
            {
            }
        };

        /**
         * Implementation of state WaitingForAck
         *
         * Signals/Functions not defined lead to an exception if called
         * in this state (therefor derived from Unhandled signals).
         */
        class WaitingForACK :
            public MyFUInterface::UnhandledSignals
        {
        public :
            WaitingForACK(MyFUInterface::BaseFSM* t) :
                MyFUInterface::UnhandledSignals(t, "wns_ldk_fsm_tests_WaitingForACK")
            {
            }
        private:
            virtual MyStateInterface*
            doOnData(const CompoundPtr& compound);

            virtual MyStateInterface*
            doWakeup();

            virtual void
            doIsAccepting(const CompoundPtr& compound, bool& accepting) const;
        };

        /**
         * Implementation of state ReadyForTransmission
         *
         * Signals/Functions not defined lead to an exception if called
         * in this state (therefor derived from Unhandled signals).
         */
        class ReadyForTransmission :
            public MyFUInterface::UnhandledSignals
        {
        public :
            ReadyForTransmission(MyFUInterface::BaseFSM* t) :
                MyFUInterface::UnhandledSignals(t, "wns_ldk_fsm_tests_ReadyForTransmission")
            {
            }
        private:
            virtual MyStateInterface*
            doSendData(const CompoundPtr& compound);

            virtual MyStateInterface*
            doWakeup();

            virtual void
            doIsAccepting(const CompoundPtr& compound, bool& accepting) const;
        };

        /**
         * Implementation of state WaitingForAckReconfiguration
         *
         * Signals/Functions not defined lead to an exception if called
         * in this state (therefor derived from Unhandled signals).
         */
        class WaitingForACKReconfiguration :
            public MyFUInterface::UnhandledSignals
        {
        public :
            WaitingForACKReconfiguration(MyFUInterface::BaseFSM* t) :
                MyFUInterface::UnhandledSignals(t, "wns_ldk_fsm_tests_WaitingForACKReconfiguration")
            {
            }
        private:
            virtual MyStateInterface*
            doOnData(const CompoundPtr& compound);

            virtual MyStateInterface*
            doWakeup();

            virtual void
            doIsAccepting(const CompoundPtr& compound, bool& accepting) const;
        };

        /**
         * Implementation of state ReadyForTransmissionReconfiguration
         *
         * Signals/Functions not defined lead to an exception if called
         * in this state (therefor derived from Unhandled signals).
         */
        class ReadyForTransmissionReconfiguration :
            public MyFUInterface::UnhandledSignals
        {
        public :
            ReadyForTransmissionReconfiguration(MyFUInterface::BaseFSM* t) :
                MyFUInterface::UnhandledSignals(t, "wns_ldk_fsm_tests_ReadyForTransmissionReconfiguration")
            {
            }
        private:
            virtual MyStateInterface*
            doSendData(const CompoundPtr& compound);

            virtual MyStateInterface*
            doWakeup();

            virtual void
            doIsAccepting(const CompoundPtr& compound, bool& accepting) const;
        };

    private:
        CPPUNIT_TEST_SUITE( FunctionalUnitTest );
        CPPUNIT_TEST( simple );
        CPPUNIT_TEST( stateReplacement );
        CPPUNIT_TEST_SUITE_END();

        virtual void
        prepare();

        virtual void
        cleanup();

        void
        simple();

        /*
         * @todo: ksw,msg
         * - move test of function replaceState(...) of FSM template to FSMTest.cpp
         */
        void
        stateReplacement();

        pyconfig::Parser emptyConfig;

        ldk::ILayer* layer;
        fun::FUN* fuNet;
        tools::Stub* lower;
        tools::Stub* upper;
        Testee* testee;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( FunctionalUnitTest );

STATIC_FACTORY_REGISTER_WITH_CREATOR(FunctionalUnitTest::WaitingForACK,
                     FunctionalUnitTest::MyStateInterface,
                     "wns_ldk_fsm_tests_WaitingForACK",
                     wns::fsm::FSMConfigCreator);

    FunctionalUnitTest::MyStateInterface*
    FunctionalUnitTest::WaitingForACK::doOnData(const CompoundPtr&)
    {
        // if this is called we assume this is the ack and throw it away
        // we need to wakeup the other FUs
        getFU()->wakeup();
        return getFSM()->createState<ReadyForTransmission>();
    }

    FunctionalUnitTest::MyStateInterface*
    FunctionalUnitTest::WaitingForACK::doWakeup()
    {
        // we can't wakeup, we're waiting for an ACK
        return this;
    }

    void
    FunctionalUnitTest::WaitingForACK::doIsAccepting(const CompoundPtr&, bool& accepting) const
    {
        accepting = false;
    }

STATIC_FACTORY_REGISTER_WITH_CREATOR(FunctionalUnitTest::ReadyForTransmission,
                     FunctionalUnitTest::MyStateInterface,
                     "wns_ldk_fsm_tests_ReadyForTransmission",
                     wns::fsm::FSMConfigCreator);

    FunctionalUnitTest::MyStateInterface*
    FunctionalUnitTest::ReadyForTransmission::doSendData(const CompoundPtr& compound)
    {
        getFU()->getConnector()->getAcceptor(compound)->sendData(compound);
        return getFSM()->createState<WaitingForACK>();
    }

    FunctionalUnitTest::MyStateInterface*
    FunctionalUnitTest::ReadyForTransmission::doWakeup()
    {
        getFU()->getReceptor()->wakeup();
        return this;
    }

    void
    FunctionalUnitTest::ReadyForTransmission::doIsAccepting(const CompoundPtr&, bool& accepting) const
    {
        accepting = true;
    }


STATIC_FACTORY_REGISTER_WITH_CREATOR(FunctionalUnitTest::WaitingForACKReconfiguration,
                     FunctionalUnitTest::MyStateInterface,
                     "wns_ldk_fsm_tests_WaitingForACKReconfiguration",
                     wns::fsm::FSMConfigCreator);

    FunctionalUnitTest::MyStateInterface*
    FunctionalUnitTest::WaitingForACKReconfiguration::doOnData(const CompoundPtr&)
    {
        // if this is called we assume this is the ack and throw it away
        // we need to wakeup the other FUs
        getFU()->wakeup();
        return getFSM()->createState<ReadyForTransmissionReconfiguration>();
    }

    FunctionalUnitTest::MyStateInterface*
    FunctionalUnitTest::WaitingForACKReconfiguration::doWakeup()
    {
        // we can't wakeup, we're waiting for an ACK
        return this;
    }

    void
    FunctionalUnitTest::WaitingForACKReconfiguration::doIsAccepting(const CompoundPtr&, bool& accepting) const
    {
        accepting = false;
    }

STATIC_FACTORY_REGISTER_WITH_CREATOR(FunctionalUnitTest::ReadyForTransmissionReconfiguration,
                     FunctionalUnitTest::MyStateInterface,
                     "wns_ldk_fsm_tests_ReadyForTransmissionReconfiguration",
                     wns::fsm::FSMConfigCreator);

    FunctionalUnitTest::MyStateInterface*
    FunctionalUnitTest::ReadyForTransmissionReconfiguration::doSendData(const CompoundPtr& compound)
    {
        getFU()->getConnector()->getAcceptor(compound)->sendData(compound);
        return this;
    }

    FunctionalUnitTest::MyStateInterface*
    FunctionalUnitTest::ReadyForTransmissionReconfiguration::doWakeup()
    {
        getFU()->getReceptor()->wakeup();
        return this;
    }

    void
    FunctionalUnitTest::ReadyForTransmissionReconfiguration::doIsAccepting(const CompoundPtr&, bool& accepting) const
    {
        accepting = false;
    }

    void
    FunctionalUnitTest::prepare()
    {
        emptyConfig = pyconfig::Parser();

        layer = new wns::ldk::tests::LayerStub();
        fuNet = new fun::Main(layer);

        lower = new tools::Stub(fuNet, emptyConfig);
        upper = new tools::Stub(fuNet, emptyConfig);
        testee = new Testee(fuNet);

        upper
            ->connect(testee)
            ->connect(lower);
    }

    void
    FunctionalUnitTest::cleanup()
    {
        delete testee;
        delete upper;
        delete lower;

        delete fuNet;
        delete layer;
    }

    void
    FunctionalUnitTest::simple()
    {
        CompoundPtr compound1(fuNet->createCompound(wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(2))));
        CompoundPtr compound2(fuNet->createCompound(wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(2))));

        upper->sendData(compound1);
        CPPUNIT_ASSERT_EQUAL(std::string("wns_ldk_fsm_tests_WaitingForACK"), testee->getStateName());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), lower->sent.size());
        CPPUNIT_ASSERT( !upper->isAccepting(compound2) );

        lower->onData(compound1);
        CPPUNIT_ASSERT_EQUAL(std::string("wns_ldk_fsm_tests_ReadyForTransmission"), testee->getStateName());
        CPPUNIT_ASSERT( upper->isAccepting(compound2) );

        upper->sendData(compound2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lower->sent.size());
    }

    void
    FunctionalUnitTest::stateReplacement()
    {
        CompoundPtr compound1(fuNet->createCompound(wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(2))));
        CompoundPtr compound2(fuNet->createCompound(wns::ldk::helper::FakePDUPtr(new wns::ldk::helper::FakePDU(2))));

        upper->sendData(compound1);
        CPPUNIT_ASSERT_EQUAL(std::string("wns_ldk_fsm_tests_WaitingForACK"), testee->getStateName());

        testee->replaceState(testee->createState("wns_ldk_fsm_tests_WaitingForACKReconfiguration"));
        CPPUNIT_ASSERT_EQUAL(std::string("wns_ldk_fsm_tests_WaitingForACKReconfiguration"), testee->getStateName());
        CPPUNIT_ASSERT( !upper->isAccepting(compound2) );

        lower->onData(compound1);
        CPPUNIT_ASSERT_EQUAL(std::string("wns_ldk_fsm_tests_ReadyForTransmissionReconfiguration"), testee->getStateName());
        CPPUNIT_ASSERT( !upper->isAccepting(compound2) );
    }

} // tests
} // fsm
} // ldk
} // wns


