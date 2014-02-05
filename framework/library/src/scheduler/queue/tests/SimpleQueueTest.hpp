/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
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

#ifndef WNS_SCHEDULER_QUEUE_TESTS_SIMPLEQUEUETEST_HPP
#define WNS_SCHEDULER_QUEUE_TESTS_SIMPLEQUEUETEST_HPP

#include <cppunit/extensions/HelperMacros.h>
#include <WNS/ldk/Classifier.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/ldk.hpp>
#include <WNS/scheduler/queue/SimpleQueue.hpp>
#include <WNS/scheduler/tests/ClassifierPolicyDropIn.hpp>
#include <WNS/scheduler/tests/RegistryProxyStub.hpp>

namespace wns { namespace scheduler { namespace queue { namespace tests {

	class SimpleQueueTest :
		public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE( SimpleQueueTest );
		CPPUNIT_TEST( testSimpleQueue );
		CPPUNIT_TEST( testReset );
		CPPUNIT_TEST( testSizes );
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp();
		void tearDown();

		void testSimpleQueue();
		void testReset();
		void testSizes();

	private:
		wns::ldk::CompoundPtr createPDUwithCID(wns::scheduler::ConnectionID cid);
		wns::ldk::Classifier<wns::scheduler::tests::ClassifierPolicyDropIn>* classifier;
		wns::scheduler::tests::RegistryProxyStub* registry;
		SimpleQueue* queue;

		wns::ldk::fun::FUN* fuNet;
		wns::ldk::tools::Stub* lower;

	};

}}}} // namespace wns::scheduler::queue::tests
#endif // WNS_SCHEDULER_QUEUE_TESTS_SIMPLEQUEUETEST_HPP


