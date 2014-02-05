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
#include <WNS/ldk/concatenation/Concatenation.hpp>

#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/ldk/buffer/Dropping.hpp>

#include <WNS/pyconfig/Parser.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace wns { namespace ldk { namespace concatenation { namespace tests {

    class ConcatenationTest :
        public wns::ldk::tests::DelayedInterfaceTest
    {
        CPPUNIT_TEST_SUB_SUITE( ConcatenationTest, wns::ldk::tests::DelayedInterfaceTest );
        //Concatenation Tests
        CPPUNIT_TEST( noConcatenationOnlyOnePDU );
        CPPUNIT_TEST( concatenationEasy );
        CPPUNIT_TEST( noConcatenationByOverfillBuffer );
        CPPUNIT_TEST( concatenationByOverfillBuffer1 );
        CPPUNIT_TEST( concatenationByOverfillBuffer2 );
        CPPUNIT_TEST( concatenationByOverfillBuffer3 );
        CPPUNIT_TEST( maxFragments );

        //Fragmentation Tests
        CPPUNIT_TEST( noFragmentationOnlyOnePDU );
        CPPUNIT_TEST( fragmentationEasy );
        CPPUNIT_TEST( noFragmentationByOverfillBuffer );
        CPPUNIT_TEST( fragmentationByOverfillBuffer1 );
        CPPUNIT_TEST( fragmentationByOverfillBuffer2 );
        CPPUNIT_TEST( fragmentationByOverfillBuffer3 );
        CPPUNIT_TEST_SUITE_END();
    public:
        //Concatenation Tests
        void
        noConcatenationOnlyOnePDU();

        void
        concatenationEasy();

        void
        noConcatenationByOverfillBuffer();

        void
        concatenationByOverfillBuffer1();

        void
        concatenationByOverfillBuffer2();

        void
        concatenationByOverfillBuffer3();

        void
        maxFragments();

        //Fragmentation Tests
        void
        noFragmentationOnlyOnePDU();

        void
        fragmentationEasy();

        void
        noFragmentationByOverfillBuffer();

        void
        fragmentationByOverfillBuffer1();

        void
        fragmentationByOverfillBuffer2();

        void
        fragmentationByOverfillBuffer3();

    private:
        virtual void
        prepare();

        virtual void
        cleanup();

        virtual void
        setUpTestFUs();

        virtual Concatenation*
        newTestee();

        virtual void
        tearDownTestee(DelayedInterface*)
        {
        }

        virtual FunctionalUnit*
        getUpperTestFU() const;

        buffer::Dropping* buffer;

        helper::FakePDUPtr innerPDU1;
        helper::FakePDUPtr innerPDU2;
        helper::FakePDUPtr innerPDU3;
        helper::FakePDUPtr innerPDU4;
        helper::FakePDUPtr innerPDU5;
        helper::FakePDUPtr innerPDU6;

        CommandPool* command1;
        CommandPool* command2;
        CommandPool* command3;
        CommandPool* command4;
        CommandPool* command5;
        CommandPool* command6;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( ConcatenationTest );

    void
    ConcatenationTest::prepare()
    {
        wns::ldk::tests::DelayedInterfaceTest::prepare();

        innerPDU1 = helper::FakePDUPtr(new helper::FakePDU());
        innerPDU2 = helper::FakePDUPtr(new helper::FakePDU());
        innerPDU3 = helper::FakePDUPtr(new helper::FakePDU());
        innerPDU4 = helper::FakePDUPtr(new helper::FakePDU());
        innerPDU5 = helper::FakePDUPtr(new helper::FakePDU());
        innerPDU6 = helper::FakePDUPtr(new helper::FakePDU());

        command1 = IncRef(getFUN()->createCommandPool());
        command2 = IncRef(getFUN()->createCommandPool());
        command3 = IncRef(getFUN()->createCommandPool());
        command4 = IncRef(getFUN()->createCommandPool());
        command5 = IncRef(getFUN()->createCommandPool());
        command6 = IncRef(getFUN()->createCommandPool());
    } // prepare

    void
    ConcatenationTest::setUpTestFUs()
    {
        wns::ldk::tests::DelayedInterfaceTest::setUpTestFUs();

        wns::pyconfig::Parser layerConfig = wns::pyconfig::Parser();
        layerConfig.loadString("from openwns.Buffer import Dropping\n"
                       "dropping = Dropping(size = 100)\n"
                       "dropping.name = 'buffer'\n");

        buffer = new buffer::Dropping(getFUN(), layerConfig.get("dropping"));
        getFUN()->addFunctionalUnit("buffer", buffer);
        buffer->connect(getTestee<FunctionalUnit>());
    } // setUpTestFUs

    Concatenation*
    ConcatenationTest::newTestee()
    {
        wns::pyconfig::Parser all = wns::pyconfig::Parser();
        all.loadString(
                       "from openwns.Concatenation import Concatenation\n"
                       "cat = Concatenation(\n"
                       "  maxSize = 77,\n"
                       "  numBitsIfConcatenated = 3,\n"
                       "  numBitsIfNotConcatenated = 1,\n"
                       "  numBitsPerEntry = 16,\n"
                       "  maxFragments = 4,\n"
                       "  logging = True\n"
                       ")"
                       );
        wns::pyconfig::View pyco(all, "cat");

        return new Concatenation(getFUN(), pyco);
    } // newTestee

    void
    ConcatenationTest::cleanup()
    {
        innerPDU1 = helper::FakePDUPtr();
        innerPDU2 = helper::FakePDUPtr();
        innerPDU3 = helper::FakePDUPtr();
        innerPDU4 = helper::FakePDUPtr();
        innerPDU5 = helper::FakePDUPtr();
        innerPDU6 = helper::FakePDUPtr();

        ::Object::derefObj(command1);
        ::Object::derefObj(command2);
        ::Object::derefObj(command3);
        ::Object::derefObj(command4);
        ::Object::derefObj(command5);
        ::Object::derefObj(command6);

        wns::ldk::tests::DelayedInterfaceTest::cleanup();
    } // cleanup


    wns::ldk::FunctionalUnit*
    ConcatenationTest::getUpperTestFU() const
    {
        return buffer;
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Concatenation Tests
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

    void
    ConcatenationTest::noConcatenationOnlyOnePDU()
    {
        innerPDU1->setLengthInBits(1);
        CompoundPtr compound1(new Compound(command1, innerPDU1));

        CPPUNIT_ASSERT(compound1->getLengthInBits() == 1);

        CPPUNIT_ASSERT(compoundsSent() == 0);
        getUpperStub()->sendData(compound1);
        CPPUNIT_ASSERT(compoundsSent() == 1);

        //The size of the PDU
        CPPUNIT_ASSERT(compound1->getLengthInBits() == 2);

    } //noConcatenationOnlyOnePDU



    void
    ConcatenationTest::concatenationEasy()
    {
        innerPDU1->setLengthInBits(21);
        innerPDU2->setLengthInBits(21);

        CompoundPtr compound1(new Compound(command1, innerPDU1));
        CompoundPtr compound2(new Compound(command2, innerPDU2));

        CPPUNIT_ASSERT(compound1->getLengthInBits() == 21);
        CPPUNIT_ASSERT(compound2->getLengthInBits() == 21);

        //Before sending
        CPPUNIT_ASSERT(compoundsSent() == 0);
        //Two compounds come simultaneously
        getLowerStub()->close();
        getUpperStub()->sendData(compound1);
        getUpperStub()->sendData(compound2);
        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->open();
        //After concatenation only one PDU is outgoing
        CPPUNIT_ASSERT(compoundsSent() == 1);

        //The size of the PDU
        //3(numBitsIfConcatenated) + 16(numBitsPerEntry) * 2(compounds.Size()) + 21(compound1) + 21(compound2)
        CPPUNIT_ASSERT(getLowerStub()->sent[0]->getLengthInBits() == 77);
    } //concatenationEasy


    //First Outgoing PDU: NotConcatenated	Second Outgoing PDU: NotConcatenated
    void
    ConcatenationTest::noConcatenationByOverfillBuffer()
    {
        innerPDU1->setLengthInBits(21);
        innerPDU2->setLengthInBits(22);

        CompoundPtr compound1(new Compound(command1, innerPDU1));
        CompoundPtr compound2(new Compound(command2, innerPDU2));

        CPPUNIT_ASSERT(compound1->getLengthInBits() == 21);
        CPPUNIT_ASSERT(compound2->getLengthInBits() == 22);

        //Before sending
        CPPUNIT_ASSERT(compoundsSent() == 0);
        //Two compounds come simultaneously
        getLowerStub()->close();
        getUpperStub()->sendData(compound1);
        getUpperStub()->sendData(compound2);
        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->open();
        //No Concatenation is done because the maximum allowed size is exceeded
        CPPUNIT_ASSERT(compoundsSent() == 2);

        //The sizes of the compounds
        //1(numBitsIfNotConcatenated) + 21(compound1)
        CPPUNIT_ASSERT(getLowerStub()->sent[0]->getLengthInBits() == 22);
        //1(numBitsIfNotConcatenated) + 22(compound2)
        CPPUNIT_ASSERT(getLowerStub()->sent[1]->getLengthInBits() == 23);
    } //noConcatenationByOverfillBuffer


    //First Outgoing PDU: Concatenated	Second Outgoing PDU: Concatenated
    void
    ConcatenationTest::concatenationByOverfillBuffer1()
    {
        innerPDU1->setLengthInBits(6);
        innerPDU2->setLengthInBits(6);
        innerPDU3->setLengthInBits(14);
        innerPDU4->setLengthInBits(10);
        innerPDU5->setLengthInBits(10);

        CompoundPtr compound1(new Compound(command1, innerPDU1));
        CompoundPtr compound2(new Compound(command2, innerPDU2));
        CompoundPtr compound3(new Compound(command3, innerPDU3));
        CompoundPtr compound4(new Compound(command4, innerPDU4));
        CompoundPtr compound5(new Compound(command5, innerPDU5));

        CPPUNIT_ASSERT(compound1->getLengthInBits() == 6);
        CPPUNIT_ASSERT(compound2->getLengthInBits() == 6);
        CPPUNIT_ASSERT(compound3->getLengthInBits() == 14);
        CPPUNIT_ASSERT(compound4->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound5->getLengthInBits() == 10);


        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->close();
        getUpperStub()->sendData(compound1);
        getUpperStub()->sendData(compound2);
        getUpperStub()->sendData(compound3);
        getUpperStub()->sendData(compound4);
        getLowerStub()->step();	//Only one PDU (compound1,compound2,compound3) will pass. For compound4 is not enough bufferspace
        getUpperStub()->sendData(compound5);
        CPPUNIT_ASSERT(compoundsSent() == 1);
        getLowerStub()->open(); //compound4 and compound 5 will pass together
        CPPUNIT_ASSERT(compoundsSent() == 2);

        //The sizes of the compounds
        //3(numBitsIfConcatenated) + [ 16(numBitsPerEntry) * 3(compounds.Size()) ] + 26(compound1,compound2,compound3)
        CPPUNIT_ASSERT(getLowerStub()->sent[0]->getLengthInBits() == 77);
        //3(numBitsIfConcatenated) + [ 16(numBitsPerEntry) * 2(compounds.Size()) ]  + 20(compound4,compound5)
        CPPUNIT_ASSERT(getLowerStub()->sent[1]->getLengthInBits() == 55);
    } //concatenationByOverfillBuffer1



    //First Outgoing PDU: Concatenated	Second Outgoing PDU: NotConcatenated
    void
    ConcatenationTest::concatenationByOverfillBuffer2()
    {
        innerPDU1->setLengthInBits(10);
        innerPDU2->setLengthInBits(10);
        innerPDU3->setLengthInBits(10);

        CompoundPtr compound1(new Compound(command1, innerPDU1));
        CompoundPtr compound2(new Compound(command2, innerPDU2));
        CompoundPtr compound3(new Compound(command3, innerPDU3));

        CPPUNIT_ASSERT(compound1->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound2->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound3->getLengthInBits() == 10);

        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->close();
        getUpperStub()->sendData(compound1);
        getUpperStub()->sendData(compound2);
        getUpperStub()->sendData(compound3);
        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->open();
        CPPUNIT_ASSERT(compoundsSent() == 2);

        //The sizes of the compounds
        //3(numBitsIfConcatenated) + [ 16(numBitsPerEntry) * 2(compounds.Size()) ] + 20(compound1,compound2)
        CPPUNIT_ASSERT(getLowerStub()->sent[0]->getLengthInBits() == 55);
        //1(numBitsIfNotConcatenated)  + 10(compound5)
        CPPUNIT_ASSERT(getLowerStub()->sent[1]->getLengthInBits() == 11);
    } //concatenationByOverfillBuffer2()



    //First Outgoing PDU: NotConcatenated	Second Outgoing PDU: Concatenated   Third Outgoing PDU: NotConcatenated
    void
    ConcatenationTest::concatenationByOverfillBuffer3()
    {
        innerPDU1->setLengthInBits(40);
        innerPDU2->setLengthInBits(10);
        innerPDU3->setLengthInBits(10);
        innerPDU4->setLengthInBits(10);

        CompoundPtr compound1(new Compound(command1, innerPDU1));
        CompoundPtr compound2(new Compound(command2, innerPDU2));
        CompoundPtr compound3(new Compound(command3, innerPDU3));
        CompoundPtr compound4(new Compound(command4, innerPDU4));

        CPPUNIT_ASSERT(compound1->getLengthInBits() == 40);
        CPPUNIT_ASSERT(compound2->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound3->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound4->getLengthInBits() == 10);

        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->close();
        getUpperStub()->sendData(compound1);
        getUpperStub()->sendData(compound2);
        getLowerStub()->step(); // Only one PDU (compound1) will pass. For compound2 the frame would be
        // too big
        CPPUNIT_ASSERT(compoundsSent() == 1);
        getUpperStub()->sendData(compound3);
        getUpperStub()->sendData(compound4);
        getLowerStub()->open(); //The remaining compounds (compound2,compound3) (compound4) will pass in two packets
        CPPUNIT_ASSERT(compoundsSent() == 3);

        //The sizes of the compounds
        //1(numBitsIfNotConcatenated) + 40(compound1)
        CPPUNIT_ASSERT(getLowerStub()->sent[0]->getLengthInBits() == 41);
        //3(numBitsIfConcatenated) + [ 16(numBitsPerEntry) * 2(compounds.Size()) ]  + 20(compound2,compound3)
        CPPUNIT_ASSERT(getLowerStub()->sent[1]->getLengthInBits() == 55);
        //1(numBitsIfNotConcatenated) + 10(compound4)
        CPPUNIT_ASSERT(getLowerStub()->sent[2]->getLengthInBits() == 11);
    } //concatenationByOverfillBuffer3



    void
    ConcatenationTest::maxFragments()
    {
        innerPDU1->setLengthInBits(1);
        innerPDU2->setLengthInBits(1);
        innerPDU3->setLengthInBits(1);
        innerPDU4->setLengthInBits(1);
        innerPDU5->setLengthInBits(1);

        CompoundPtr compound1(new Compound(command1, innerPDU1));
        CompoundPtr compound2(new Compound(command2, innerPDU2));
        CompoundPtr compound3(new Compound(command3, innerPDU3));
        CompoundPtr compound4(new Compound(command4, innerPDU4));
        CompoundPtr compound5(new Compound(command5, innerPDU5));

        CPPUNIT_ASSERT(compound1->getLengthInBits() == 1);
        CPPUNIT_ASSERT(compound2->getLengthInBits() == 1);
        CPPUNIT_ASSERT(compound3->getLengthInBits() == 1);
        CPPUNIT_ASSERT(compound4->getLengthInBits() == 1);
        CPPUNIT_ASSERT(compound5->getLengthInBits() == 1);

        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->close();
        getUpperStub()->sendData(compound1);
        getUpperStub()->sendData(compound2);
        getUpperStub()->sendData(compound3);
        getUpperStub()->sendData(compound4);
        //The next PDU (compound5) exceed the maxFragments border
        getUpperStub()->sendData(compound5);
        CPPUNIT_ASSERT(buffer->getSize() == 1);
        getLowerStub()->open(); //The first compounds (compound1,compound2,compound3,compound4) will pass
        //together, compound5 will go alone
        CPPUNIT_ASSERT(compoundsSent() == 2);

        //The sizes of the compounds
        //3(numBitsIfConcatenated) + [ 16(numBitsPerEntry) * 4(compounds.Size()) ] + 4(compound1,compound2,compound3,compound4)
        CPPUNIT_ASSERT(getLowerStub()->sent[0]->getLengthInBits() == 71);
        //1(numBitsIfNotConcatenated) + 1(compound5)
        CPPUNIT_ASSERT(getLowerStub()->sent[1]->getLengthInBits() == 2);
    } //maxFragments







/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Fragmentation Tests
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

    void
    ConcatenationTest::noFragmentationOnlyOnePDU()
    {
        innerPDU1->setLengthInBits(1);
        CompoundPtr compound1(new Compound(command1, innerPDU1));
        getUpperStub()->sendData(compound1);

        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->sent.clear();
        CPPUNIT_ASSERT_EQUAL( static_cast<unsigned int>(1), compoundsDelivered() );
        CPPUNIT_ASSERT_EQUAL( 2, (int)getUpperStub()->received[0]->getLengthInBits() );
    } // noFragmentationOnlyOnePDU


    void
    ConcatenationTest::fragmentationEasy()
    {
        innerPDU1->setLengthInBits(21);
        innerPDU2->setLengthInBits(21);

        CompoundPtr compound1(new Compound(command1, innerPDU1));
        CompoundPtr compound2(new Compound(command2, innerPDU2));

        CPPUNIT_ASSERT(compound1->getLengthInBits() == 21);
        CPPUNIT_ASSERT(compound2->getLengthInBits() == 21);

        //Before sending
        CPPUNIT_ASSERT(compoundsSent() == 0);
        //Two compounds come simultaneously
        getLowerStub()->close();
        getUpperStub()->sendData(compound1);
        getUpperStub()->sendData(compound2);
        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->open();
        //After concatenation only one PDU is outgoing with size 77
        CPPUNIT_ASSERT(compoundsSent() == 1);

        //Only one PDU is passed to the upper Layer
        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->sent.clear();
        //The number  sizes of the fragmented compounds
        CPPUNIT_ASSERT(compoundsDelivered() == 2);
        CPPUNIT_ASSERT_EQUAL( 22 , (int)getUpperStub()->received[0]->getLengthInBits() );
        CPPUNIT_ASSERT_EQUAL( 22 , (int)getUpperStub()->received[1]->getLengthInBits() );
    } //fragmentationEasy



    //First Outgoing PDU: NotConcatenated	Second Outgoing PDU: NotConcatenated
    void
    ConcatenationTest::noFragmentationByOverfillBuffer()
    {
        innerPDU1->setLengthInBits(21);
        innerPDU2->setLengthInBits(22);

        CompoundPtr compound1(new Compound(command1, innerPDU1));
        CompoundPtr compound2(new Compound(command2, innerPDU2));

        CPPUNIT_ASSERT(compound1->getLengthInBits() == 21);
        CPPUNIT_ASSERT(compound2->getLengthInBits() == 22);

        //Before sending
        CPPUNIT_ASSERT(compoundsSent() == 0);
        //Two compounds come simultaneously
        getLowerStub()->close();
        getUpperStub()->sendData(compound1);
        getUpperStub()->sendData(compound2);
        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->open();
        //No Concatenation is done because the buffersize is not enough
        CPPUNIT_ASSERT(compoundsSent() == 2);

        //Two PDUa are passed to the upper Layer
        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->onData(getLowerStub()->sent[1]);
        getLowerStub()->sent.clear();
        //The sizes of the fragmented compounds
        CPPUNIT_ASSERT(compoundsDelivered() == 2);
        CPPUNIT_ASSERT_EQUAL(22, (int)getUpperStub()->received[0]->getLengthInBits() );
        CPPUNIT_ASSERT_EQUAL(23, (int)getUpperStub()->received[1]->getLengthInBits() );

    } //fragmentationAndOverfillBuffer


    //First Outgoing PDU: Concatenated	Second Outgoing PDU: Concatenated
    void
    ConcatenationTest::fragmentationByOverfillBuffer1()
    {
        innerPDU1->setLengthInBits(10);
        innerPDU2->setLengthInBits(10);
        innerPDU3->setLengthInBits(22);
        innerPDU4->setLengthInBits(10);

        CompoundPtr compound1(new Compound(command1, innerPDU1));
        CompoundPtr compound2(new Compound(command2, innerPDU2));
        CompoundPtr compound3(new Compound(command3, innerPDU3));
        CompoundPtr compound4(new Compound(command4, innerPDU4));

        CPPUNIT_ASSERT(compound1->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound2->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound3->getLengthInBits() == 22);
        CPPUNIT_ASSERT(compound4->getLengthInBits() == 10);

        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->close();
        getUpperStub()->sendData(compound1);
        getUpperStub()->sendData(compound2);
        getUpperStub()->sendData(compound3);
        getLowerStub()->step();	//Only one PDU (compound1,compound2) will pass. compound3 would exceed the maximum
        getUpperStub()->sendData(compound4);

        CPPUNIT_ASSERT(compoundsSent() == 1);
        getLowerStub()->open(); //compound3 and compound 4 will pass together
        CPPUNIT_ASSERT(compoundsSent() == 2);

        //Two PDUa are passed to the upper Layer
        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->onData(getLowerStub()->sent[1]);
        getLowerStub()->sent.clear();
        //The number and sizes of the fragmented compounds
        CPPUNIT_ASSERT_EQUAL(4, (int) compoundsDelivered() );

        CPPUNIT_ASSERT_EQUAL(11, (int) getUpperStub()->received[0]->getLengthInBits());
        CPPUNIT_ASSERT_EQUAL(11, (int) getUpperStub()->received[1]->getLengthInBits());
        CPPUNIT_ASSERT_EQUAL(23, (int) getUpperStub()->received[2]->getLengthInBits());
        CPPUNIT_ASSERT_EQUAL(11, (int) getUpperStub()->received[3]->getLengthInBits());
        // CPPUNIT_ASSERT(getUpperStub()->received[4]->getLengthInBits() == 11);
    } //fragmentationByOverfillBuffer1

    //First Outgoing PDU: Concatenated	   Second Outgoing PDU: NotConcatenated
    //Third Outgoing PDU: NotConcatenated  Fourth Outgoing PDU: NotConcatenated
    void
    ConcatenationTest::fragmentationByOverfillBuffer2()
    {
        innerPDU1->setLengthInBits(10);
        innerPDU2->setLengthInBits(10);
        innerPDU3->setLengthInBits(10);
        innerPDU4->setLengthInBits(10);
        innerPDU5->setLengthInBits(10);

        CompoundPtr compound1(new Compound(command1, innerPDU1));
        CompoundPtr compound2(new Compound(command2, innerPDU2));
        CompoundPtr compound3(new Compound(command3, innerPDU3));
        CompoundPtr compound4(new Compound(command4, innerPDU4));
        CompoundPtr compound5(new Compound(command5, innerPDU5));

        CPPUNIT_ASSERT(compound1->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound2->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound3->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound4->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound5->getLengthInBits() == 10);


        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->close();
        getUpperStub()->sendData(compound1); // first concatenated
        getUpperStub()->sendData(compound2); // second concatenated
        getUpperStub()->sendData(compound3); // third gooes into "fulll"
        getUpperStub()->sendData(compound4); // fourth stays in upper buffer
        getUpperStub()->sendData(compound5); // fifth stays in upper buffer
        CPPUNIT_ASSERT_EQUAL(2, (int)buffer->getSize());
        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->open();
        CPPUNIT_ASSERT_EQUAL(4, (int)compoundsSent());

        //Four compounds are passed to the upper Layer
        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->onData(getLowerStub()->sent[1]);
        getLowerStub()->onData(getLowerStub()->sent[2]);
        getLowerStub()->onData(getLowerStub()->sent[3]);
        getLowerStub()->sent.clear();
        //The number and sizes of fragmented compounds
        CPPUNIT_ASSERT(compoundsDelivered() == 5);

        CPPUNIT_ASSERT_EQUAL(11, (int)getUpperStub()->received[0]->getLengthInBits());
        CPPUNIT_ASSERT_EQUAL(11, (int)getUpperStub()->received[1]->getLengthInBits());
        CPPUNIT_ASSERT_EQUAL(11, (int)getUpperStub()->received[2]->getLengthInBits());
        CPPUNIT_ASSERT_EQUAL(11, (int)getUpperStub()->received[3]->getLengthInBits());
        CPPUNIT_ASSERT_EQUAL(11, (int)getUpperStub()->received[4]->getLengthInBits());
    } //fragmentationByOverfillBuffer2()


    //First Outgoing PDU: NotConcatenated	Second Outgoing PDU: Concatenated
    void
    ConcatenationTest::fragmentationByOverfillBuffer3()
    {
        innerPDU1->setLengthInBits(40);
        innerPDU2->setLengthInBits(10);
        innerPDU3->setLengthInBits(10);
        innerPDU4->setLengthInBits(10);
        innerPDU5->setLengthInBits(10);

        CompoundPtr compound1(new Compound(command1, innerPDU1));
        CompoundPtr compound2(new Compound(command2, innerPDU2));
        CompoundPtr compound3(new Compound(command3, innerPDU3));
        CompoundPtr compound4(new Compound(command4, innerPDU4));
        CompoundPtr compound5(new Compound(command5, innerPDU5));

        CPPUNIT_ASSERT(compound1->getLengthInBits() == 40);
        CPPUNIT_ASSERT(compound2->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound3->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound4->getLengthInBits() == 10);
        CPPUNIT_ASSERT(compound5->getLengthInBits() == 10);


        CPPUNIT_ASSERT(compoundsSent() == 0);
        getLowerStub()->close();
        getUpperStub()->sendData(compound1); // start concatenating
        getUpperStub()->sendData(compound2); // exceeds maxSize, goes into "full"
        getLowerStub()->step(); //Only one PDU (compound1) will pass. compound2 would exceed the maxsize
        getUpperStub()->sendData(compound3); // concatenated with compound2
        getUpperStub()->sendData(compound4); // exceeds maxSize, goes into "full"
        getUpperStub()->sendData(compound5); // stays in upper buffer
        CPPUNIT_ASSERT_EQUAL(1,  (int)compoundsSent());
        getLowerStub()->open(); //The remaining compounds (compound2,compound3) (compound4) (compound5) will pass
        //three containers, because (compound5) was still in the buffer
        //when we started accepting
        CPPUNIT_ASSERT_EQUAL(4,  (int)compoundsSent());

        //Two PDUs are passed to the upper Layer
        getLowerStub()->onData(getLowerStub()->sent[0]);
        getLowerStub()->onData(getLowerStub()->sent[1]);
        getLowerStub()->onData(getLowerStub()->sent[2]);
        getLowerStub()->onData(getLowerStub()->sent[3]);
        getLowerStub()->sent.clear();
        //The number and sizes of the fragmented compounds
        CPPUNIT_ASSERT(compoundsDelivered() == 5);
        CPPUNIT_ASSERT_EQUAL(41, (int)getUpperStub()->received[0]->getLengthInBits());
        CPPUNIT_ASSERT_EQUAL(11, getUpperStub()->received[1]->getLengthInBits());
        CPPUNIT_ASSERT_EQUAL(11, getUpperStub()->received[2]->getLengthInBits());
        CPPUNIT_ASSERT_EQUAL(11, getUpperStub()->received[3]->getLengthInBits());
        CPPUNIT_ASSERT_EQUAL(11, getUpperStub()->received[4]->getLengthInBits());
    } //fragmentationByOverfillBuffer3

}
}
}
}


