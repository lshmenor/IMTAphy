/******************************************************************************
 * IPModule   Internet Protocol Implementation                                *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005                                                         *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnetsrwth-aachen.de, www: http://wns.comnets.rwth-aachen.de/  *
 ******************************************************************************/

#include <WNS/TestFixture.hpp>
#include <IP/iptables/Chain.hpp>
#include <IP/iptables/PreroutingChain.hpp>
#include <IP/iptables/filters/SourceDestinationFilter.hpp>
#include <IP/iptables/targets/LoggingTarget.hpp>
#include <IP/iptables/Rule.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/pyconfig/helper/Functions.hpp>

#include <iostream>

namespace ip { namespace iptables { namespace tests {

	class RuleContainerTest:
		public wns::TestFixture
    {
        CPPUNIT_TEST_SUITE( RuleContainerTest );
        CPPUNIT_TEST( addRuleTest );
// 		CPPUNIT_TEST(hasRulesTest);
// 		CPPUNIT_TEST(deleteRulesTest);
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
        addRuleTest();

// 		void
// 		hasRulesTest();

// 		void
// 		deleteRulesTest();

		void
		setUp();

		void
		tearDown();

	private:
		wns::ldk::ILayer* layer;
		wns::ldk::fun::Main* fuNet;
	};

} // tests
} // iptables
} // ip

using namespace ip::iptables::tests;
CPPUNIT_TEST_SUITE_REGISTRATION( RuleContainerTest );

void
RuleContainerTest::setUp()
{
	layer = new wns::ldk::tests::LayerStub();
	fuNet = new wns::ldk::fun::Main(layer);
}

void
RuleContainerTest::tearDown()
{
	delete layer;
	delete fuNet;
}

void
RuleContainerTest::addRuleTest()
{
	std::stringstream ss;
	ss << "import ip\n"
	   << "from ip.IPTables import PreroutingChain\n"
	   << "chain = ip.IPTables.PreroutingChain()\n";

	wns::pyconfig::Parser all;
	all.loadString(ss.str());

	wns::pyconfig::View config(all, "chain");

	ip::iptables::PreroutingChain* testee = new ip::iptables::PreroutingChain(fuNet, config);

	wns::service::nl::Address addr0 = wns::service::nl::Address("192.168.0.1");
	wns::service::nl::Address addr1 = wns::service::nl::Address("192.168.0.1");
	wns::service::nl::Address addr2 = wns::service::nl::Address("192.168.0.34");
	wns::service::nl::Address addr3 = wns::service::nl::Address("192.168.2.12");
	wns::service::nl::Address addr4 = wns::service::nl::Address("192.168.1.106");
	wns::service::nl::Address netMaskA = wns::service::nl::Address("255.255.255.255");
	wns::service::nl::Address netMaskB = wns::service::nl::Address("255.255.0.0");
	wns::service::nl::Address netMaskC = wns::service::nl::Address("255.255.15.0");

	ip::iptables::filters::SourceDestinationFilter* filter0 = new ip::iptables::filters::SourceDestinationFilter(addr0,
																												 netMaskA,
																												 addr1,
																												 netMaskB);

	ip::iptables::filters::SourceDestinationFilter* filter1 = new  ip::iptables::filters::SourceDestinationFilter(addr1,
																												  netMaskA,
																												  addr2,
																												  netMaskB);

	ip::iptables::filters::SourceDestinationFilter* filter2 = new  ip::iptables::filters::SourceDestinationFilter(addr4,
																												  netMaskB,
																												  addr2,
																												  netMaskA);

	ip::iptables::filters::SourceDestinationFilter* filter3 = new  ip::iptables::filters::SourceDestinationFilter(addr2,
																												  netMaskB,
																												  addr3,
																												  netMaskC);

	ip::iptables::filters::SourceDestinationFilter* filter4 = new  ip::iptables::filters::SourceDestinationFilter(addr4,
																												  netMaskA,
																												  addr4,
																												  netMaskC);

	targets::TargetInterface* target = new ip::iptables::targets::LoggingTarget(config);

	ip::iptables::Rule rule1 = ip::iptables::Rule(filter0, target, 0);
	ip::iptables::Rule rule2 = ip::iptables::Rule(filter1, target, 1);
	ip::iptables::Rule rule3 = ip::iptables::Rule(filter2, target, 1);
	ip::iptables::Rule rule4 = ip::iptables::Rule(filter3, target, 2);
	ip::iptables::Rule rule5 = ip::iptables::Rule(filter4, target, 2);
	ip::iptables::Rule rule6 = ip::iptables::Rule(filter4, target, 3);
	ip::iptables::Rule rule7 = ip::iptables::Rule(filter1, target, 2);

 	testee->addRule(rule1);
  	testee->addRule(rule2);
	testee->addRule(rule3);
	testee->addRule(rule4);
	testee->addRule(rule5);
	testee->addRule(rule6);
	testee->addRule(rule7);

  	CPPUNIT_ASSERT(testee->hasRules(0));
   	CPPUNIT_ASSERT(testee->hasRules(1));
  	CPPUNIT_ASSERT(testee->hasRules(2));
   	CPPUNIT_ASSERT(testee->hasRules(3));
   	CPPUNIT_ASSERT(!testee->hasRules(4));

	testee->removeRules(0);
  	CPPUNIT_ASSERT(!testee->hasRules(0));
	testee->removeRules(1);
  	CPPUNIT_ASSERT(!testee->hasRules(1));
	testee->removeRules(2);
  	CPPUNIT_ASSERT(!testee->hasRules(2));
	testee->removeRules(3);
	CPPUNIT_ASSERT(!testee->hasRules(3));
	testee->removeRules(3);
	CPPUNIT_ASSERT(!testee->hasRules(3));
	CPPUNIT_ASSERT(!testee->hasRules(2));

}

// void
// RuleContainerTest::hasRulesTest()
// {
// 	CPPUNIT_ASSERT(!testee->hasRules(2));
// }

// void
// RuleContainerTest::deleteRulesTest()
// {
// }
