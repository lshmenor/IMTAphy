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

#include <WNS/osi/PCI.hpp>
#include <WNS/TestFixture.hpp>

namespace wns { namespace osi { namespace tests {

    /**
     * @brief Test for PCI without a SDU. PCI with SDU is tested in PDU test.
     * @author Maciej Muehleisen <mue@comnets.rwth-aachen.de>
     */
	class PCITest :
		public wns::TestFixture
	{
		CPPUNIT_TEST_SUITE( PCITest );
        CPPUNIT_TEST( Constructor );
        CPPUNIT_TEST( CopyConstructor );
        CPPUNIT_TEST( ID );
		CPPUNIT_TEST_SUITE_END();

	public:
 		void prepare();
		void cleanup();

        void Constructor();
        void CopyConstructor();
        void ID();

	};

	CPPUNIT_TEST_SUITE_REGISTRATION( PCITest );

	void
	PCITest::prepare()
	{
	}

	void
	PCITest::cleanup()
	{
	}

	void
    PCITest::Constructor()
    {
        /*Using default parameters*/
        PCI testPCI1 = PCI();
        pduType dontKnow = PCI::unknown;
        CPPUNIT_ASSERT_EQUAL(dontKnow, testPCI1.getPDUType());
        CPPUNIT_ASSERT_EQUAL(Bit(0), testPCI1.getPDULength());

        /*With explicit type*/
        pduType type = 123;
        PCI testPCI2 = PCI(type);
        CPPUNIT_ASSERT_EQUAL(pduType(123), testPCI2.getPDUType());
        CPPUNIT_ASSERT_EQUAL(Bit(0), testPCI2.getPDULength());
        
        /* With type and length*/     
        Bit length = 1024;
        PCI testPCI3 = PCI(type, length);
        CPPUNIT_ASSERT_EQUAL(pduType(123), testPCI3.getPDUType());
        CPPUNIT_ASSERT_EQUAL(Bit(1024), testPCI3.getPDULength());
    }

    void
    PCITest::CopyConstructor()
    {
        PCI testPCI = PCI();
        PCI copyPCI = PCI(testPCI);

        CPPUNIT_ASSERT_EQUAL(testPCI.getPDUType(), copyPCI.getPDUType());
        CPPUNIT_ASSERT_EQUAL(testPCI.getPDULength(), copyPCI.getPDULength());
        CPPUNIT_ASSERT_EQUAL(testPCI.getPDUId(), copyPCI.getPDUId());
        CPPUNIT_ASSERT_EQUAL(testPCI.getSDU(), copyPCI.getSDU());
    }

    void
    PCITest::ID()
    {
        PCI testPCI1 = PCI();
        unsigned long int id = testPCI1.getPDUId();
        PCI testPCI2 = PCI();

        /* Next ID should be greater by 1*/     
        CPPUNIT_ASSERT_EQUAL(id + 1, testPCI2.getPDUId());
    }

} // tests
} // osi
} // wns
