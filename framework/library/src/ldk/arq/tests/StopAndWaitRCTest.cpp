/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2008                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <WNS/ldk/arq/StopAndWaitRC.hpp>
#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/ldk/fun/Main.hpp>

#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/tools/Bridge.hpp>
#include <WNS/ldk/tools/Consumer.hpp>
#include <WNS/ldk/tools/Producer.hpp>

#include <WNS/pyconfig/Parser.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <vector>

namespace wns { namespace ldk { namespace arq { namespace tests {

    class StopAndWaitRCTest
        : public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( StopAndWaitRCTest );
        CPPUNIT_TEST( fillWindow );
        CPPUNIT_TEST( sendOnAck );
        CPPUNIT_TEST( sendAck );
        CPPUNIT_TEST( retransmission );
        CPPUNIT_TEST( IFrameSize );
        CPPUNIT_TEST( RRFrameSize );
        CPPUNIT_TEST_SUITE_END();

    public:
        void
        fillWindow();

        void
        sendOnAck();

        void
        sendAck();

        void
        retransmission();

        void
        IFrameSize();

        void
        RRFrameSize();

    private:
        virtual void
        prepare();

        virtual void
        cleanup();

        ILayer* layer;
        fun::Main* fun;

        tools::Stub* upperStub;
        tools::Stub* lowerDataStub;
        tools::Stub* lowerAckStub;

        buffer::Bounded* buffer;
        StopAndWaitRC* saw;

        static const int bitsPerIFrame;
        static const int bitsPerRRFrame;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( StopAndWaitRCTest );


    const int
    StopAndWaitRCTest::bitsPerIFrame = 2;


    const int
    StopAndWaitRCTest::bitsPerRRFrame = 3;


    void
    StopAndWaitRCTest::prepare()
    {
        wns::ldk::CommandProxy::clearRegistries();

        layer = new wns::ldk::tests::LayerStub();
        fun = new wns::ldk::fun::Main(layer);


        wns::pyconfig::Parser emptyConfig;
        upperStub = new tools::Stub(fun, emptyConfig);
        lowerDataStub = new tools::Stub(fun, emptyConfig);
        lowerAckStub = new tools::Stub(fun, emptyConfig);

        fun->addFunctionalUnit("upperStub", upperStub);
        fun->addFunctionalUnit("lowerDataStub", lowerDataStub);
        fun->addFunctionalUnit("lowerAckStub", lowerAckStub);


        wns::pyconfig::Parser pyco;
        pyco.loadString("from openwns.Buffer import Bounded\n"
                        "buffer = Bounded(size = 100, probingEnabled = False)\n"
            );
        wns::pyconfig::View view(pyco, "buffer");
        buffer = new buffer::Bounded(fun, view);
        fun->addFunctionalUnit("buffer", buffer);


        std::stringstream ss;

        ss << "from openwns.ARQ import StopAndWaitRC\n"
           << "arq = StopAndWaitRC(\n"
           << "  bitsPerIFrame = " << bitsPerIFrame << ",\n"
           << "  bitsPerRRFrame = " << bitsPerRRFrame << ",\n"
           << "  resendTimeout = 1.0\n"
           << ")\n";

        wns::pyconfig::Parser all;
        all.loadString(ss.str());
        wns::pyconfig::View config(all, "arq");
        saw = new StopAndWaitRC(fun, config);
        fun->addFunctionalUnit("saw", saw);


        upperStub->connect(buffer);
        buffer->connect(saw);
        saw->connect(lowerDataStub, "Data");
        saw->connect(lowerAckStub, "Ack");


        wns::simulator::getEventScheduler()->reset();
    } // prepare


    void
    StopAndWaitRCTest::cleanup()
    {
        delete fun;
        delete layer;
    } // cleanup


    void
    StopAndWaitRCTest::fillWindow()
    {
        upperStub->sendData(fun->createCompound());
        upperStub->sendData(fun->createCompound());

        CPPUNIT_ASSERT(buffer->getSize() == 1);
        CPPUNIT_ASSERT(lowerDataStub->sent.size() == 1);
        CPPUNIT_ASSERT(lowerAckStub->sent.size() == 0);
    } // fillWindow


    void
    StopAndWaitRCTest::sendOnAck()
    {

    } // sendOnAck


    void
    StopAndWaitRCTest::sendAck()
    {

    } // sendAck


    void
    StopAndWaitRCTest::retransmission()
    {

    } // retransmission


    void
    StopAndWaitRCTest::IFrameSize()
    {

    } // IFrameSize


    void
    StopAndWaitRCTest::RRFrameSize()
    {

    } // RRFrameSize

   } // tests
  } // arq
 } // ldk
} // wns
