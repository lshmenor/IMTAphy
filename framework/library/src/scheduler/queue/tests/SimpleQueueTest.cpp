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

#include <WNS/scheduler/queue/tests/SimpleQueueTest.hpp>
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/scheduler/tests/ClassifierPolicyDropIn.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/node/tests/Stub.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/CppUnit.hpp>

using namespace wns;
using namespace wns::ldk;
using namespace wns::scheduler;
using namespace wns::scheduler::queue;
using namespace wns::scheduler::queue::tests;

CPPUNIT_TEST_SUITE_REGISTRATION( SimpleQueueTest );
//CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SimpleQueueTest, "SimpleQueueTest");

void SimpleQueueTest::setUp() {
    // see ClassifierTest from ldk

    fuNet = new wns::ldk::fun::Main(new wns::ldk::tests::LayerStub());


    wns::pyconfig::Parser emptyConfig;
    classifier =
        new Classifier<wns::scheduler::tests::ClassifierPolicyDropIn>(fuNet, emptyConfig);

    std::vector<ConnectionID> vektor;
    vektor.resize(3);
    vektor[0] = ConnectionID(1);
    vektor[1] = ConnectionID(2);
    vektor[2] = ConnectionID(3);

    classifier->setCIDsVector(vektor);

    fuNet->getProxy()->addFunctionalUnit( "classifier", classifier );

    lower = new tools::Stub(fuNet, emptyConfig);

    classifier->connect(lower);

    //
    registry = new wns::scheduler::tests::RegistryProxyStub();
    registry->setFUN(fuNet);

    wns::pyconfig::Parser queueConfig;

    std::stringstream ss;

    ss << "from openwns.Scheduler import SimpleQueue\n"
       << "queue = SimpleQueue()\n";

    queueConfig.loadString(ss.str());
    pyconfig::View pcv = queueConfig.getView("queue");

    queue = new SimpleQueue(NULL, pcv);
    registry->setFriends(dynamic_cast<wns::ldk::CommandTypeSpecifier<wns::ldk::ClassifierCommand>* >(classifier));
    queue->setColleagues(dynamic_cast<RegistryProxyInterface*>(registry));
}

void SimpleQueueTest::tearDown() {
    delete queue;
    delete registry;
    delete classifier;
    delete lower;
    delete fuNet;
}

void SimpleQueueTest::testSimpleQueue() {

    // first test the classifier

    CompoundPtr compound1(new Compound(fuNet->getProxy()->createCommandPool()));
    classifier->sendData(compound1);
    CPPUNIT_ASSERT(lower->sent.size() == 1);
    ClassifierCommand* command1 =
        classifier->getCommand( compound1->getCommandPool() );
    CPPUNIT_ASSERT( command1->peer.id == 1 );

    CompoundPtr compound2(new Compound(fuNet->getProxy()->createCommandPool()));
    classifier->sendData(compound2);
    CPPUNIT_ASSERT(lower->sent.size() == 2);
    ClassifierCommand* command2 =
        classifier->getCommand( compound2->getCommandPool() );
    CPPUNIT_ASSERT( command2->peer.id == 2 );

    CompoundPtr compound3(new Compound(fuNet->getProxy()->createCommandPool()));
    classifier->sendData(compound3);
    CPPUNIT_ASSERT(lower->sent.size() == 3);
    ClassifierCommand* command3 =
        classifier->getCommand( compound3->getCommandPool() );
    CPPUNIT_ASSERT( command3->peer.id == 3 );

    CompoundPtr compound4(new Compound(fuNet->getProxy()->createCommandPool()));
    classifier->sendData(compound4);
    CPPUNIT_ASSERT(lower->sent.size() == 4);
    ClassifierCommand* command4 =
        classifier->getCommand( compound4->getCommandPool() );
    CPPUNIT_ASSERT( command4->peer.id == 1 );

    // tell the RegistryProxyDropIn which users and cids we have
    UserID user1(new wns::node::tests::Stub());
    UserID user2(new wns::node::tests::Stub());

    registry->associateCIDandUser(ConnectionID(1), user1);
    registry->associateCIDandUser(ConnectionID(2), user2);
    registry->associateCIDandUser(ConnectionID(3), user2);

    // test the RegistryProxyDropIn

    CPPUNIT_ASSERT_EQUAL( user1, registry->getUserForCID(ConnectionID(1)) );
    CPPUNIT_ASSERT_EQUAL( user2, registry->getUserForCID(ConnectionID(2)) );
    CPPUNIT_ASSERT_EQUAL( user2, registry->getUserForCID(ConnectionID(3)) );


    // now the queue itself

    // queues should be non-existent and empty!
    CPPUNIT_ASSERT( !queue->queueHasPDUs(ConnectionID(0)) );
    CPPUNIT_ASSERT( !queue->queueHasPDUs(ConnectionID(1)) );
    CPPUNIT_ASSERT( !queue->queueHasPDUs(ConnectionID(2)) );
    CPPUNIT_ASSERT( !queue->queueHasPDUs(ConnectionID(3)) );

    // and no user be active

    UserSet noUser = queue->getQueuedUsers();
    CPPUNIT_ASSERT_EQUAL(size_t(0), noUser.size());


    // now fill them

    CPPUNIT_ASSERT( queue->isAccepting(compound1) );
    queue->put(compound1);
    CPPUNIT_ASSERT( queue->isAccepting(compound2) );
    queue->put(compound2);
    CPPUNIT_ASSERT( queue->isAccepting(compound3) );
    queue->put(compound3);
    CPPUNIT_ASSERT( queue->isAccepting(compound4) );
    queue->put(compound4);

    // now all but the first and non-existent should be filled

    CPPUNIT_ASSERT( !queue->queueHasPDUs(ConnectionID(0)) );
    CPPUNIT_ASSERT( queue->queueHasPDUs(ConnectionID(1)) );
    CPPUNIT_ASSERT( queue->queueHasPDUs(ConnectionID(2)) );
    CPPUNIT_ASSERT( queue->queueHasPDUs(ConnectionID(3)) );

    // and both users should be active
    UserSet twoUsers = queue->getQueuedUsers();
    CPPUNIT_ASSERT_EQUAL(size_t(2), twoUsers.size());
    CPPUNIT_ASSERT( twoUsers.find(user1) != twoUsers.end() );
    CPPUNIT_ASSERT( twoUsers.find(user2) != twoUsers.end() );

    // now get/remove the PDUs from the queue

    // 2 pdus in queue with cid 1
    CPPUNIT_ASSERT_EQUAL( compound1, queue->getHeadOfLinePDU(ConnectionID(1)) );
    CPPUNIT_ASSERT( queue->queueHasPDUs(ConnectionID(1)) );
    CPPUNIT_ASSERT_EQUAL( compound4, queue->getHeadOfLinePDU(ConnectionID(1)) );
    CPPUNIT_ASSERT( !queue->queueHasPDUs(ConnectionID(1)) );

    // now only user2 should be active
    UserSet oneUser = queue->getQueuedUsers();
    CPPUNIT_ASSERT_EQUAL(size_t(1), oneUser.size());
    CPPUNIT_ASSERT( oneUser.find(user2) != twoUsers.end() );

    // the other only have one pdu

    CPPUNIT_ASSERT_EQUAL( compound2, queue->getHeadOfLinePDU(ConnectionID(2)) );
    CPPUNIT_ASSERT( !queue->queueHasPDUs(ConnectionID(2)) );

    CPPUNIT_ASSERT_EQUAL( compound3, queue->getHeadOfLinePDU(ConnectionID(3)) );
    CPPUNIT_ASSERT( !queue->queueHasPDUs(ConnectionID(3)) );

    // no user should be active anymore

    noUser = queue->getQueuedUsers();
    CPPUNIT_ASSERT_EQUAL(size_t(0), noUser.size());

    delete user1.getNode();
    delete user2.getNode();
}

wns::ldk::CompoundPtr
SimpleQueueTest::createPDUwithCID(ConnectionID cid)
{
    CompoundPtr compound(new Compound(fuNet->getProxy()->createCommandPool()));

    classifier->setNextCID(cid);
    classifier->sendData(compound);

    return compound;
}

void SimpleQueueTest::testReset()
{
    ConnectionID cid1 = ConnectionID(1);
    ConnectionID cid2 = ConnectionID(2);
    ConnectionID cid3 = ConnectionID(3);
    ConnectionID cid4 = ConnectionID(4);
    ConnectionID cid5 = ConnectionID(5);

    // tell the RegistryProxyDropIn which users and cids we have
    UserID user1(new wns::node::tests::Stub());
    UserID user2(new wns::node::tests::Stub());

    registry->associateCIDandUser(cid1, user1);
    registry->associateCIDandUser(cid2, user2);
    registry->associateCIDandUser(cid3, user1);
    registry->associateCIDandUser(cid4, user1);
    registry->associateCIDandUser(cid5, user1);

    // test the RegistryProxyDropIn
    CPPUNIT_ASSERT_EQUAL( user1, registry->getUserForCID(cid1) );
    CPPUNIT_ASSERT_EQUAL( user2, registry->getUserForCID(cid2) );
    CPPUNIT_ASSERT_EQUAL( user1, registry->getUserForCID(cid3) );
    CPPUNIT_ASSERT_EQUAL( user1, registry->getUserForCID(cid4) );
    CPPUNIT_ASSERT_EQUAL( user1, registry->getUserForCID(cid5) );

    // all queues should be empty
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid1));
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid2));
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid3));
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid4));
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid5));

    // now fill the queues
    queue->put(createPDUwithCID(1));
    queue->put(createPDUwithCID(2));
    queue->put(createPDUwithCID(3));
    queue->put(createPDUwithCID(4));
    queue->put(createPDUwithCID(5));
    queue->put(createPDUwithCID(1));
    queue->put(createPDUwithCID(1));
    queue->put(createPDUwithCID(5));
    queue->put(createPDUwithCID(5));
    queue->put(createPDUwithCID(5));
    queue->put(createPDUwithCID(5));

    // all queues should be filled
    CPPUNIT_ASSERT(queue->queueHasPDUs(cid1));
    CPPUNIT_ASSERT(queue->queueHasPDUs(cid2));
    CPPUNIT_ASSERT(queue->queueHasPDUs(cid3));
    CPPUNIT_ASSERT(queue->queueHasPDUs(cid4));
    CPPUNIT_ASSERT(queue->queueHasPDUs(cid5));

    // reset queue belonging to cid4, then it should not exist anymore
    queue->resetQueue(cid4);
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid4));
    WNS_ASSERT_ASSURE_EXCEPTION(queue->getHeadOfLinePDUbits(cid4));

    // fill cid4-queue again
    queue->put(createPDUwithCID(cid4));
    CPPUNIT_ASSERT(queue->queueHasPDUs(cid4));

    // reset all queues (cid1, cid3, cid4, cid5) belonging to user1
    queue->resetQueues(user1);
    // the users queues should be gone now
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid1));
    WNS_ASSERT_ASSURE_EXCEPTION(queue->getHeadOfLinePDUbits(cid1));
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid3));
    WNS_ASSERT_ASSURE_EXCEPTION(queue->getHeadOfLinePDUbits(cid3));
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid4));
    WNS_ASSERT_ASSURE_EXCEPTION(queue->getHeadOfLinePDUbits(cid4));
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid5));
    WNS_ASSERT_ASSURE_EXCEPTION(queue->getHeadOfLinePDUbits(cid5));
    // queue for user2 (cid2) should still be there
    CPPUNIT_ASSERT(queue->queueHasPDUs(cid2));

    // fill cid4 and cid1-queue again
    queue->put(createPDUwithCID(cid4));
    CPPUNIT_ASSERT(queue->queueHasPDUs(cid4));
    queue->put(createPDUwithCID(cid1));
    CPPUNIT_ASSERT(queue->queueHasPDUs(cid1));

    // reset all queues and check if everything is gone
    queue->resetAllQueues();
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid1));
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid2));
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid3));
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid4));
    CPPUNIT_ASSERT(!queue->queueHasPDUs(cid5));

    // can I fill some of them again?
    queue->put(createPDUwithCID(cid4));
    queue->put(createPDUwithCID(cid4));
    CPPUNIT_ASSERT(queue->queueHasPDUs(cid4));
    queue->put(createPDUwithCID(cid1));
    CPPUNIT_ASSERT(queue->queueHasPDUs(cid1));


    delete user1.getNode();
    delete user2.getNode();
}

void SimpleQueueTest::testSizes()
{
    ConnectionID cid1 = ConnectionID(1);
    ConnectionID cid2 = ConnectionID(2);

    // 1st compound: size=3000bit, cid1
    helper::FakePDUPtr inner(new  helper::FakePDU(3000));
    CompoundPtr compound1(fuNet->createCompound(inner));
    classifier->setNextCID(cid1);
    classifier->sendData(compound1);
    ClassifierCommand* command1 =
        classifier->getCommand( compound1->getCommandPool() );
    CPPUNIT_ASSERT( command1->peer.id == 1 );

    // 2nd compound: size=2000bit, cid2
    CompoundPtr compound2(fuNet->createCompound(helper::FakePDUPtr(new  helper::FakePDU(2000))));
    classifier->setNextCID(cid2);
    classifier->sendData(compound2);
    ClassifierCommand* command2 =
        classifier->getCommand( compound2->getCommandPool() );
    CPPUNIT_ASSERT( command2->peer.id == 2 );

    // 3rd compound: size=4000bit, cid1
    CompoundPtr compound3(fuNet->createCompound(helper::FakePDUPtr(new  helper::FakePDU(4000))));
    classifier->setNextCID(cid1);
    classifier->sendData(compound3);
    ClassifierCommand* command3 =
        classifier->getCommand( compound3->getCommandPool() );
    CPPUNIT_ASSERT( command3->peer.id == 1 );

    // tell the RegistryProxyDropIn which users and cids we have
    UserID user1(new wns::node::tests::Stub());
    UserID user2(new wns::node::tests::Stub());
    registry->associateCIDandUser(ConnectionID(1), user1);
    registry->associateCIDandUser(ConnectionID(2), user2);

    // first check, that the queues are empty and the assure mechanism works
    UserSet noUser = queue->getQueuedUsers();
    CPPUNIT_ASSERT_EQUAL(size_t(0), noUser.size());

    WNS_ASSERT_ASSURE_EXCEPTION(queue->getHeadOfLinePDUbits(cid1));
    WNS_ASSERT_ASSURE_EXCEPTION(queue->getHeadOfLinePDUbits(cid2));

    // now insert pdus and check how the sizes behave

    queue->put(compound1);
    CPPUNIT_ASSERT_EQUAL(3000, queue->getHeadOfLinePDUbits(cid1));

    queue->put(compound2);
    CPPUNIT_ASSERT_EQUAL(2000, queue->getHeadOfLinePDUbits(cid2));

    queue->put(compound3);
    CPPUNIT_ASSERT_EQUAL(3000, queue->getHeadOfLinePDUbits(cid1));
    CPPUNIT_ASSERT_EQUAL(2000, queue->getHeadOfLinePDUbits(cid2));

    // test for number of compounds in queues
    // obsolete. No more user-related questions
    //CPPUNIT_ASSERT_EQUAL(static_cast<unsigned long int>(2), queue->numCompoundsForUser(user1));
    //CPPUNIT_ASSERT_EQUAL(static_cast<unsigned long int>(1), queue->numCompoundsForUser(user2));

    // now remove them and check the sizes
    wns::ldk::CompoundPtr dummy = queue->getHeadOfLinePDU(cid1);
    CPPUNIT_ASSERT_EQUAL(4000, queue->getHeadOfLinePDUbits(cid1));

    dummy = queue->getHeadOfLinePDU(cid1);
    WNS_ASSERT_ASSURE_EXCEPTION(queue->getHeadOfLinePDUbits(cid1));
    dummy = queue->getHeadOfLinePDU(cid2);
    WNS_ASSERT_ASSURE_EXCEPTION(queue->getHeadOfLinePDUbits(cid2));

    delete user1.getNode();
    delete user2.getNode();
}


