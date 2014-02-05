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

#include <WNS/ldk/tests/DelayedInterfaceTest.hpp>
#include <WNS/ldk/arq/CumulativeACK.hpp>
#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/tools/Bridge.hpp>
#include <WNS/ldk/tools/Producer.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/fun/Main.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <vector>

namespace wns { namespace ldk { namespace arq { namespace tests {

    class CumulativeACKTest :
        public wns::ldk::tests::DelayedInterfaceTest
    {
        CPPUNIT_TEST_SUB_SUITE( CumulativeACKTest, wns::ldk::tests::DelayedInterfaceTest );
        CPPUNIT_TEST( fillWindow );
        CPPUNIT_TEST( sendOnAck );
        CPPUNIT_TEST( sendOnAck2 );
        CPPUNIT_TEST( sendAck );
        CPPUNIT_TEST( sendAck2 );
        CPPUNIT_TEST( retransmission );
        CPPUNIT_TEST( speed );
        CPPUNIT_TEST( speedWithLoss );
        CPPUNIT_TEST_SUITE_END();
        
        public:
            ///@missing test on not expected Ack
        
            void
            fillWindow();

            void
            sendOnAck();

            void
            sendOnAck2();

            void
            sendAck();

            void
            sendAck2();

            void
            retransmission();

            void
            speed();

            void
            speedWithLoss();

        private:
            virtual void
            prepare();

            virtual void
            setUpTestFUs();

            virtual CumulativeACK*
            newTestee();

            virtual void
            tearDownTestee(DelayedInterface*)
            {
            }

            virtual FunctionalUnit*
            getUpperTestFU() const;

            buffer::Bounded* buffer;

            static const unsigned long int windowSize;
    };



    CPPUNIT_TEST_SUITE_REGISTRATION( CumulativeACKTest );

    const unsigned long int
    CumulativeACKTest::windowSize = 6;

    void
    CumulativeACKTest::prepare()
    {
        wns::ldk::CommandProxy::clearRegistries();

        wns::ldk::tests::DelayedInterfaceTest::prepare();

        wns::simulator::getEventScheduler()->reset();
    } // prepare

    void
    CumulativeACKTest::setUpTestFUs()
    {
        wns::ldk::tests::DelayedInterfaceTest::setUpTestFUs();

        wns::pyconfig::Parser all;
        all.loadString("from openwns.Buffer import *\n"
                       "foo = Bounded(size = 100, probingEnabled = False)\n");
        wns::pyconfig::View config(all, "foo");
        buffer = new buffer::Bounded(getFUN(), config);
        getFUN()->addFunctionalUnit("buffer", buffer);
        buffer->connect(getTestee<FunctionalUnit>());
    } // setUpTestFUs

    CumulativeACK*
    CumulativeACKTest::newTestee()
    {
        std::stringstream ss;
        ss << "from openwns.ARQ import CumulativeACK\n"
           << "foo = CumulativeACK(\n"
           << "  windowSize = " << (2*windowSize) << ",\n"
           << "  resendTimeout = 1.0)\n";
        wns::pyconfig::Parser all;
        all.loadString(ss.str());

        pyconfig::View config(all, "foo");
        return new CumulativeACK(getFUN(), config);
    } // newTestee

    wns::ldk::FunctionalUnit*
    CumulativeACKTest::getUpperTestFU() const
    {
        return buffer;
    } // getUpperTestFU


    ///@brief tests if srACK stops accepting packets if windowSize is reached
    void
    CumulativeACKTest::fillWindow()
    {
        for( unsigned long int i = 1; i <= ( (windowSize) + 1 ); i++ )
        {
            getUpperStub()->sendData(getFUN()->createCompound());
        }
        CPPUNIT_ASSERT( buffer->getSize() == 1 );
        CPPUNIT_ASSERT( compoundsSent() == ( windowSize ));
    } // fillQWindow


    ///@brief checks if after ACK the srACK is sending another packet
    void
    CumulativeACKTest::sendOnAck()
    {
        for( unsigned int i = 1; i <= ( windowSize + 1 ); i++ )
        {
            getUpperStub()->sendData(getFUN()->createCompound());
        }
        CPPUNIT_ASSERT(compoundsSent() == windowSize);
        CumulativeACKCommand* receivedPCI = getTestee<CumulativeACK>()->getCommand(getLowerStub()->sent[0]->getCommandPool());
        CompoundPtr compound(getFUN()->createCompound());
        CumulativeACKCommand* ackPCI = getTestee<CumulativeACK>()->activateCommand(compound->getCommandPool());
        ackPCI->peer.type = CumulativeACKCommand::RR;
        ackPCI->peer.NR = (receivedPCI->peer.NS + 1) % (2 * windowSize);
        ackPCI->peer.NS = ackPCI->peer.NR;
        getLowerStub()->onData(compound);
        CPPUNIT_ASSERT(buffer->getSize() == 0);
        CPPUNIT_ASSERT(compoundsSent() == ( windowSize + 1 ) );
    } // sendOnAck
    ///@todo testSendOnACK3 like 1 but after one iteration through the windowSize


    // Ack for a PDU, which has been sent after a PDU which has not been acknowledged.
    void
    CumulativeACKTest::sendOnAck2()
    {
        for( unsigned int i = 1; i <= ( windowSize + 2 ); i++ )
        {
            getUpperStub()->sendData(getFUN()->createCompound());
        }
        CPPUNIT_ASSERT(compoundsSent() == windowSize);
        CumulativeACKCommand* receivedPCI = getTestee<CumulativeACK>()->getCommand(getLowerStub()->sent[1]->getCommandPool());
        CompoundPtr compound(getFUN()->createCompound());
        CumulativeACKCommand* ackPCI = getTestee<CumulativeACK>()->activateCommand(compound->getCommandPool());
        ackPCI->peer.type = CumulativeACKCommand::RR;
        ackPCI->peer.NR = (receivedPCI->peer.NS + 1) % (2 * windowSize);
        ackPCI->peer.NS = ackPCI->peer.NR;
        getLowerStub()->onData(compound);
        CPPUNIT_ASSERT(buffer->getSize() == 0);
        CPPUNIT_ASSERT(compoundsSent() == ( windowSize + 2 ) );
    } // sendOnAck


    //tests if an ACK is sent after a PDU was received
    void
    CumulativeACKTest::sendAck()
    {
        CompoundPtr compound(getFUN()->createCompound());
        CumulativeACKCommand* command = getTestee<CumulativeACK>()->activateCommand(compound->getCommandPool());
        command->peer.type = CumulativeACKCommand::I;
        command->peer.NS = 0;
        getLowerStub()->onData(compound);
        CPPUNIT_ASSERT(compoundsSent() == 1);
        CPPUNIT_ASSERT(compoundsDelivered() == 1);
        CPPUNIT_ASSERT(getUpperStub()->received[0] == compound);
        compound = getLowerStub()->sent[0];
        command = getTestee<CumulativeACK>()->getCommand(compound->getCommandPool());
        CPPUNIT_ASSERT(command->peer.type == CumulativeACKCommand::RR);
        CPPUNIT_ASSERT(command->peer.NS == 1);
    } // sendAck


    // tests in the case of packets being received in a wrong order or again, if only the last packet is acknowleged
    void
    CumulativeACKTest::sendAck2()
    {
        CompoundPtr compound(getFUN()->createCompound());
        CumulativeACKCommand* command = getTestee<CumulativeACK>()->activateCommand(compound->getCommandPool());
        command->peer.type = CumulativeACKCommand::I;
        command->peer.NS = 1;

        //second PDU
        getLowerStub()->onData( compound );
        CPPUNIT_ASSERT(compoundsSent() == 0 );
        CPPUNIT_ASSERT(compoundsDelivered() == 0 );
        CompoundPtr compound2(getFUN()->createCompound());
        CumulativeACKCommand* command2 = getTestee<CumulativeACK>()->activateCommand(compound2->getCommandPool());
        command2->peer.type = CumulativeACKCommand::I;
        command2->peer.NS = 0;

        //first PDU
        getLowerStub()->onData(compound2);
        CPPUNIT_ASSERT(compoundsSent() == 1);
        CPPUNIT_ASSERT(compoundsDelivered() == 2);
        CPPUNIT_ASSERT(getUpperStub()->received[0] == compound2);
        CPPUNIT_ASSERT(getUpperStub()->received[1] == compound);
        compound = getLowerStub()->sent[0];

        command = getTestee<CumulativeACK>()->getCommand(compound->getCommandPool());
        CPPUNIT_ASSERT(command->peer.type == CumulativeACKCommand::RR);
        CPPUNIT_ASSERT(command->peer.NS == 2);

        //first PDU again
        CompoundPtr compound3(getFUN()->createCompound());
        CumulativeACKCommand* command3 = getTestee<CumulativeACK>()->activateCommand(compound3->getCommandPool());
        command3->peer.type = CumulativeACKCommand::I;
        command3->peer.NS = 0;
        getLowerStub()->onData( compound3 );

        CPPUNIT_ASSERT(compoundsSent() == 2);
        CPPUNIT_ASSERT(compoundsDelivered() == 2);
        compound = getLowerStub()->sent[1];
        command = getTestee<CumulativeACK>()->getCommand(compound->getCommandPool());
        CPPUNIT_ASSERT(command->peer.type == CumulativeACKCommand::RR);
        CPPUNIT_ASSERT(command->peer.NS == 2);
    } // sendAck


    // checks if the compounds are retransmitted in case of not receiving ACKs
    void
    CumulativeACKTest::retransmission()
    {
        wns::events::scheduler::Interface *scheduler = wns::simulator::getEventScheduler();
        CPPUNIT_ASSERT(!scheduler->processOneEvent()); // assure clean scheduler
        getUpperStub()->sendData(getFUN()->createCompound());
        CPPUNIT_ASSERT(compoundsSent() == 1);

        // there should be only one event: the retransmission timer
        scheduler->processOneEvent();
        CPPUNIT_ASSERT(compoundsSent() == 2);

        // just to be sure... :)
        scheduler->processOneEvent();
        CPPUNIT_ASSERT(compoundsSent() == 3);

        // send an ACK
        CumulativeACKCommand* receivedPCI = getTestee<CumulativeACK>()->getCommand(getLowerStub()->sent[0]->getCommandPool());
        CompoundPtr compound(getFUN()->createCompound());
        CumulativeACKCommand* command = getTestee<CumulativeACK>()->activateCommand(compound->getCommandPool());

        command->peer.type = CumulativeACKCommand::RR;
        command->peer.NR = ((receivedPCI->peer.NS) + 1) % windowSize;
        getLowerStub()->onData(compound);

        // now the scheduler shouldn't contain any events and there
        // shouldn't be any new compounds in the getLowerStub() layer.
        CPPUNIT_ASSERT(compoundsSent() == 3);
        CPPUNIT_ASSERT(!scheduler->processOneEvent());

    } // retransmission


    void
    CumulativeACKTest::speed()
    {
        const unsigned int speedIterations = 10; // 1000000;

        class LaLayer :
            public wns::ldk::tests::LayerStub
        {
            public:
                LaLayer() :
                    wns::ldk::tests::LayerStub()
            {
            }
            std::string getName() const {return "la";};
        };

        LaLayer* leftLayer = new LaLayer;
        fun::FUN* leftFUN = new fun::Main(leftLayer);
        ILayer* rightLayer = new wns::ldk::tests::LayerStub();
        fun::FUN* rightFUN = new fun::Main(rightLayer);

        wns::pyconfig::Parser all;
        all.loadString( "from openwns.ARQ import CumulativeACK\n"
                        "arq = CumulativeACK(\n"
                        "  windowSize = 6,\n"
                        "  resendTimeout = 1.0\n"
                        ")\n");
                        
        wns::pyconfig::View arqConfig(all, "arq");
        arq::CumulativeACK *leftARQ = new arq::CumulativeACK(leftFUN, arqConfig);
        arq::CumulativeACK *rightARQ = new arq::CumulativeACK(rightFUN, arqConfig);

        wns::pyconfig::Parser emptyConfig;
        tools::Stub* stepper = new tools::Stub(rightFUN, emptyConfig);
        stepper->close();

        leftFUN->addFunctionalUnit("ernie", leftARQ);
        rightFUN->addFunctionalUnit("ernie", rightARQ);

        tools::Bridge* bridge = new tools::Bridge(leftFUN, rightFUN);
        leftARQ
            ->connect(stepper)
            ->connect(bridge->getLeft());
        rightARQ
            ->connect(bridge->getRight());

        FunctionalUnit* leftUpper = new tools::Producer(leftFUN);
        tools::Stub* rightUpper = new tools::Stub(rightFUN, emptyConfig);
        //rightUpper->consumeInUplink();

        leftUpper
            ->connect(leftARQ);

        rightUpper
            ->connect(rightARQ);

        CPPUNIT_ASSERT(stepper->received.size() == 0);
        leftUpper->wakeup();

        // to enable measuring, uncomment the following lines and link
        // with librt...
        // struct timespec t_start, t_end;
        // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_start);
        for(unsigned int  l = 0; l < speedIterations; ++l)
        {
            //stepper->flush();
            stepper->step();
            CPPUNIT_ASSERT(rightUpper->received.size() == l+1);
            //std::cout<<" Stepper->sent.size(): "<<stepper->sent.size()<<" l: "<<l<<std::endl;
            CPPUNIT_ASSERT(stepper->sent.size() == l + 1);
        }
        // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_end);
        // cerr << t_start.tv_sec << " " << t_start.tv_nsec << " "
        //	    << t_end.tv_sec << " " << t_end.tv_nsec << endl;

        delete rightUpper;
        delete leftUpper;
        delete bridge;
        delete stepper;
        delete rightFUN;
        delete leftFUN;
        delete rightLayer;
        delete leftLayer;
    } // speed


    void
    CumulativeACKTest::speedWithLoss()
    {
        const long maxIterations = 100000000;

        class LaLayer :
            public wns::ldk::tests::LayerStub
        {
            public:
                LaLayer() :
                    wns::ldk::tests::LayerStub()
            {
            }
            
            std::string getName() const
            {
                return "lala";
            }
        };

        LaLayer *leftLayer = new LaLayer;
        fun::FUN* leftFUN = new fun::Main(leftLayer);
        ILayer *rightLayer = new wns::ldk::tests::LayerStub();
        fun::FUN* rightFUN = new fun::Main(rightLayer);

        wns::pyconfig::Parser all;
        all.loadString( "from openwns.ARQ import CumulativeACK\n"
                        "arq = CumulativeACK(\n"
                        "  windowSize = 6,\n"
                        "  resendTimeout = 1.0\n"
                        ")\n");
                        
        wns::pyconfig::View arqConfig(all, "arq");
        arq::CumulativeACK *leftARQ = new arq::CumulativeACK(leftFUN, arqConfig);
        arq::CumulativeACK *rightARQ = new arq::CumulativeACK(rightFUN, arqConfig);

        wns::pyconfig::Parser emptyConfig;
        tools::Stub* stepper = new tools::Stub(rightFUN, emptyConfig);
        stepper->close();

        leftFUN->addFunctionalUnit("ernie", leftARQ);
        rightFUN->addFunctionalUnit("ernie", rightARQ);

        tools::Bridge* bridge = new tools::Bridge(leftFUN, rightFUN, 0.50);
        leftARQ
            ->connect(stepper)
            ->connect(bridge->getLeft());
        rightARQ
            ->connect(bridge->getRight()) ;

        FunctionalUnit* leftUpper = new tools::Producer(leftFUN);
        tools::Stub* rightUpper = new tools::Stub(rightFUN, emptyConfig);
        //rightUpper->consumeInUplink();

        leftUpper
            ->connect(leftARQ);

        rightUpper
            ->connect(rightARQ);

        CPPUNIT_ASSERT(stepper->received.size() == 0);
        leftUpper->wakeup();

        long l;
        for(l = 0; l < maxIterations; ++l)
        {
            stepper->step();
            if(rightUpper->received.size() > 9)
                break;
            //std::cout<<".";
            wns::simulator::getEventScheduler()->processOneEvent();
        }
        // std::cout<<" iterations needed: "<<l<<std::endl;
        CPPUNIT_ASSERT(l < maxIterations);

        delete rightUpper;
        delete leftUpper;
        delete bridge;
        delete stepper;
        delete rightFUN;
        delete leftFUN;
        delete rightLayer;
        delete leftLayer;
    } // speedWithLoss

/*
  --- Stucture of testSpeed*:

  [ leftUpper = Producer    ]      [  rightUpper = Consumer    ]
  |                                 |
  [ leftARQ = CumulativeAck ]      [  rightARQ = CumulativeACK ]
  |                                 |
  [ stepper = Stub          ]                    |
  |                                 |
  [ bride_left ------------ bridge -------- bridge_right       ]

*/

}
}
}
}
