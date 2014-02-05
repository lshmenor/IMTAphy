/******************************************************************************
 * WNS (Wireless Network Simulator)                                           *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2004-2006                                                    *
 * Chair of Communication Networks (ComNets)                                  *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#ifndef GLUE_ARQFSM_STOPANDWAIT_TEST_HPP
#define GLUE_ARQFSM_STOPANDWAIT_TEST_HPP

#include <GLUE/arqfsm/stopandwait/FSMFU.hpp>

#include <WNS/ldk/buffer/Bounded.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/CppUnit.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>

namespace glue { namespace arqfsm { namespace stopandwait {

	using namespace wns::ldk;

	class StopAndWaitFSMTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( StopAndWaitFSMTest );
		CPPUNIT_TEST( testFillWindow );
		CPPUNIT_TEST( testSendOnAck );
		CPPUNIT_TEST( testSendAck );
		CPPUNIT_TEST( testRetransmission );
		CPPUNIT_TEST( testIFrameSize );
		CPPUNIT_TEST( testRRFrameSize );
		CPPUNIT_TEST_SUITE_END();
	public:
		void prepare();
		void cleanup();

		void testFillWindow();
		void testSendOnAck();
		void testSendAck();
 		void testRetransmission();
		void testIFrameSize();
		void testRRFrameSize();
	private:
		Layer* layer;
		fun::FUN* fuNet;

		tools::Stub* upper;
		buffer::Bounded* buffer;
		FSMFU* arq;
		tools::Stub* lower;

		static const int bitsPerIFrame;
		static const int bitsPerRRFrame;
	};

} // stopandwait
} // arqfsm
} // glue

#endif	// NOT defined GLUE_ARQFSM_STOPANDWAIT_TEST_HPP


