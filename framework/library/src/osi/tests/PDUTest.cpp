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

#include <WNS/osi/PDU.hpp>
#include <WNS/osi/PCI.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace osi { namespace tests {

    /**
     * @brief Test for PDU.
     * @author Maciej Muehleisen <mue@comnets.rwth-aachen.de>
     */
	class PDUTest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( PDUTest );
        CPPUNIT_TEST( Constructor );
        CPPUNIT_TEST( CopyConstructor );
        CPPUNIT_TEST( GetSetPCI );
        CPPUNIT_TEST( GetSetUserData );
        CPPUNIT_TEST( ID );
        CPPUNIT_TEST( GetSetType );
        CPPUNIT_TEST( GetLength );
        CPPUNIT_TEST( Clone );
#ifndef NDEBUG
        CPPUNIT_TEST( DebugInfo );
#endif
		CPPUNIT_TEST_SUITE_END();

	public:
 		void prepare();
		void cleanup();

        void Constructor();
        void CopyConstructor();
        void GetSetPCI();
        void GetSetUserData();
        void ID();
        void GetSetType();
        void GetLength();
        void Clone();
#ifndef NDEBUG
        void DebugInfo();
#endif

	};

	CPPUNIT_TEST_SUITE_REGISTRATION( PDUTest );

	void
	PDUTest::prepare()
	{
	}

	void
	PDUTest::cleanup()
	{
	}

	void
    PDUTest::Constructor()
    {
        PDU emptyPDU = PDU();
        CPPUNIT_ASSERT(NULL == emptyPDU.getPCI());
        CPPUNIT_ASSERT(NULL == emptyPDU.getUserData());

        PCI* header = new PCI();
        PDU* headerPDU = new PDU(header);
        CPPUNIT_ASSERT_EQUAL(header->getPDUId(), headerPDU->getPCI()->getPDUId());
        CPPUNIT_ASSERT(NULL == headerPDU->getUserData());

        PDU* userDataNoHeaderPDU = new PDU(NULL, headerPDU);
        CPPUNIT_ASSERT(NULL == userDataNoHeaderPDU->getPCI());
        /* We can't compare IDs since the ID is in the PCI we don't have*/
        CPPUNIT_ASSERT(headerPDU == userDataNoHeaderPDU->getUserData());

        PDU testPDU = PDU(header, userDataNoHeaderPDU);
        CPPUNIT_ASSERT_EQUAL(header->getPDUId(), testPDU.getPCI()->getPDUId());
        /* We can't compare IDs since the ID is in the PCI we don't have*/
        CPPUNIT_ASSERT(userDataNoHeaderPDU == testPDU.getUserData());
    }

    void
    PDUTest::CopyConstructor()
    {
        PCI* header = new PCI();
        PDU* userData = new PDU();

        PDU testPDU = PDU(header, userData);
        PDU copyPDU = PDU(testPDU);
        CPPUNIT_ASSERT_EQUAL(testPDU.getPDUId(), copyPDU.getPDUId());

        PDU emptyPDU = PDU();
        PDU emptyCopyPDU = PDU(emptyPDU);
        CPPUNIT_ASSERT(NULL == emptyCopyPDU.getPCI());
        CPPUNIT_ASSERT(NULL == emptyCopyPDU.getUserData());

#ifndef NDEBUG
        long int cnt = PDU::getExistingPDUs();
        long int maxCnt = PDU::getMaxExistingPDUs();

        /*Does a new PDU increase maxCnt*/
        long int inc = 0;
        if(maxCnt == cnt)
            inc = 1;

        PDU debugCopy = PDU(emptyPDU);

        CPPUNIT_ASSERT_EQUAL(cnt + 1, PDU::getExistingPDUs());
        CPPUNIT_ASSERT_EQUAL(maxCnt + inc, PDU::getMaxExistingPDUs());
#endif        
    }

    void
    PDUTest::GetSetPCI()
    {
        PCI* testPCI = new PCI();
        PDU testPDU = PDU();

        testPDU.setPCI(testPCI);
        CPPUNIT_ASSERT_EQUAL(testPCI->getPDUId(), testPDU.getPDUId());
        CPPUNIT_ASSERT_EQUAL(testPCI->getPDUId(), testPDU.getPCI()->getPDUId());
    }

    void
    PDUTest::GetSetUserData()
    {
        PCI* testPCI = new PCI();
        PCI* userDataPCI = new PCI();
        PDU* userData = new PDU(userDataPCI);
        PDU testPDU = PDU(testPCI);

        testPDU.setUserData(userData);
        CPPUNIT_ASSERT_EQUAL(userData->getPDUId(), testPDU.getUserData()->getPDUId());
    }

    void
    PDUTest::ID()
    {
        PCI* testPCI = new PCI();
        PDU testPDU = PDU();

        WNS_ASSERT_ASSURE_EXCEPTION(testPDU.getPDUId());
        testPDU.setPCI(testPCI);
        CPPUNIT_ASSERT_EQUAL(testPCI->getPDUId(), testPDU.getPDUId());
        
        unsigned long int id = testPDU.getPDUId();
        PDU nextPDU = PDU(new PCI());
        CPPUNIT_ASSERT_EQUAL(id + 1, nextPDU.getPDUId());
    }

    void
    PDUTest::GetSetType()
    {
        PCI* testPCI = new PCI(pduType(123));
        PDU testPDU = PDU();

        WNS_ASSERT_ASSURE_EXCEPTION(testPDU.getPDUId());

        testPDU.setPCI(testPCI);
        CPPUNIT_ASSERT_EQUAL(pduType(123), testPDU.getPDUType());
    }

    void
    PDUTest::GetLength()
    {
        PDU testPDU = PDU();

        WNS_ASSERT_ASSURE_EXCEPTION(testPDU.getLengthInBits());

        class FixedSizePDU :
            public PDU
        {
            public:
                FixedSizePDU(Bit length) :
                    PDU(),
                    length_(length)
                {}

            private:
                Bit length_;

                virtual Bit 
                doGetLengthInBits() const
                {return length_;}
        };

        FixedSizePDU negativePDU = FixedSizePDU(-10);
        WNS_ASSERT_ASSURE_EXCEPTION(testPDU.getLengthInBits());

        FixedSizePDU goodPDU = FixedSizePDU(10);
        CPPUNIT_ASSERT_EQUAL(Bit(10), goodPDU.getLengthInBits());
    }

    void
    PDUTest::Clone()
    {
        PCI* header = new PCI();
        PDU* userData = new PDU();

        PDU testPDU = PDU(header, userData);
        PDU* copyPDU = testPDU.clone();
        CPPUNIT_ASSERT_EQUAL(testPDU.getPDUId(), copyPDU->getPDUId());

        PDU emptyPDU = PDU();
        PDU* emptyCopyPDU = emptyPDU.clone();
        CPPUNIT_ASSERT(NULL == emptyCopyPDU->getPCI());
        CPPUNIT_ASSERT(NULL == emptyCopyPDU->getUserData());
    }

#ifndef NDEBUG
    void
    PDUTest::DebugInfo()
    {
        long int cnt = PDU::getExistingPDUs();
        long int maxCnt = PDU::getMaxExistingPDUs();

        /*Does a new PDU increase maxCnt*/
        long int inc = 0;
        if(maxCnt == cnt)
            inc = 1;
                        

        PDU newPDU = PDU();
        CPPUNIT_ASSERT_EQUAL(cnt + 1, PDU::getExistingPDUs());
        CPPUNIT_ASSERT_EQUAL(maxCnt + inc, PDU::getMaxExistingPDUs());

        cnt = PDU::getExistingPDUs();
        maxCnt = PDU::getMaxExistingPDUs();

        /*Does a new PDU increase maxCnt*/
        inc = 0;
        if(maxCnt == cnt)
            inc = 1;

        do
        {
            PDU deleteSoonPDU = PDU();
            CPPUNIT_ASSERT_EQUAL(cnt + 1, PDU::getExistingPDUs());
            CPPUNIT_ASSERT_EQUAL(maxCnt + inc, PDU::getMaxExistingPDUs());
        }
        while(false);
        CPPUNIT_ASSERT_EQUAL(cnt, PDU::getExistingPDUs());
        CPPUNIT_ASSERT_EQUAL(maxCnt + inc, PDU::getMaxExistingPDUs());

        /*Create PDUs until maxCount increases*/
        std::list<PDU*> delPDUs;
        inc = 0;
        maxCnt = PDU::getMaxExistingPDUs();
        cnt = PDU::getExistingPDUs();

        while(maxCnt >= PDU::getExistingPDUs())
        {
            delPDUs.push_back(new PDU());
            inc++;
        }
        CPPUNIT_ASSERT_EQUAL(cnt + inc, PDU::getExistingPDUs());
        CPPUNIT_ASSERT_EQUAL(maxCnt + 1, PDU::getMaxExistingPDUs());

        /*And delete them again*/
        while(!delPDUs.empty())
        {
            delete delPDUs.back();
            delPDUs.pop_back();
        }
        CPPUNIT_ASSERT_EQUAL(cnt, PDU::getExistingPDUs());
        CPPUNIT_ASSERT_EQUAL(maxCnt + 1, PDU::getMaxExistingPDUs());
    }
#endif //NDEBUG
} // tests
} // osi
} // wns
