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
#include <WNS/ldk/arq/None.hpp>
#include <WNS/pyconfig/Parser.hpp>

namespace wns { namespace ldk { namespace arq { namespace tests {

    class NoneTest :
        public wns::ldk::tests::DelayedInterfaceTest
    {
        CPPUNIT_TEST_SUB_SUITE( NoneTest, wns::ldk::tests::DelayedInterfaceTest );
        CPPUNIT_TEST( factory );
        CPPUNIT_TEST( forwarding );
        CPPUNIT_TEST_SUITE_END();
    public:
        void
        forwarding();

        void
        factory();

    private:
        virtual None*
        newTestee();

        virtual void
        tearDownTestee(DelayedInterface*)
        {
        }

    };

    CPPUNIT_TEST_SUITE_REGISTRATION( NoneTest );

    None*
    NoneTest::newTestee()
    {
        wns::pyconfig::Parser all;
        all.loadString(
                       "from openwns.ARQ import NoARQ\n"
                       "arq = NoARQ()\n"
                           );
        wns::pyconfig::View arqConfig(all, "arq");
        return new None(getFUN(), arqConfig);
    } // newTestee


    void
    NoneTest::forwarding()
    {
        CompoundPtr compound(getFUN()->createCompound());

        CPPUNIT_ASSERT(compoundsSent() == 0);

        getUpperStub()->sendData(compound);
        CPPUNIT_ASSERT(compoundsSent() == 1);
        CPPUNIT_ASSERT(getLowerStub()->sent[0] == compound);
    } // forwarding


    void
    NoneTest::factory()
    {
        wns::pyconfig::Parser all;
        all.loadString(
                       "from openwns.ARQ import NoARQ\n"
                       "arq = NoARQ()\n"
                           );
        wns::pyconfig::View arqConfig(all, "arq");

        ARQ* arq = ARQFactory::creator("wns.arq.None")->create(getFUN(), arqConfig);

        CPPUNIT_ASSERT( NULL != dynamic_cast<None*>(arq) );

        delete arq;
    } // factory

}
}
}
}


