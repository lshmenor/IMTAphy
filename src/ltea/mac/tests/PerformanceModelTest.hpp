/*******************************************************************************
* This file is part of IMTAphy
* _____________________________________________________________________________
*
* Copyright (C) 2011
* Institute of Communication Networks (LKN)
* Department of Electrical Engineering and Information Technology (EE & IT)
* Technische Universitaet Muenchen
* Arcisstr. 21
* 80333 Muenchen - Germany
* http://www.lkn.ei.tum.de/~jan/imtaphy/index.html
*
* _____________________________________________________________________________
*
*   IMTAphy is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   IMTAphy is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with IMTAphy.  If not, see <http://www.gnu.org/licenses/>.
*
******************************************************************************/

#ifndef LTEA_MAC_TESTS_PERFORMANCEMODELTEST_HPP
#define LTEA_MAC_TESTS_PERFORMANCEMODELTEST_HPP

#include <cppunit/extensions/HelperMacros.h>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/ldk/ldk.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>
#include <IMTAPHY/ltea/mac/DCI.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/HasReceptor.hpp>
#include <WNS/ldk/HasConnector.hpp>
#include <WNS/ldk/HasDeliverer.hpp>
#include <WNS/Cloneable.hpp>
#include <WNS/ldk/Command.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/pyconfig/Parser.hpp>


namespace ltea { namespace mac {  namespace tests {


    struct Result {
        double firstAttemptBLER;
        double spectralEfficiency;
    };
    
    // see commandproxy test
    struct A :
        public virtual wns::ldk::FunctionalUnit,
        public wns::ldk::HasReceptor<>, // see ldk/tests/CommandProxyTest
        public wns::ldk::HasConnector<>,
        public wns::ldk::HasDeliverer<>,
        public wns::Cloneable<A>,
        public wns::ldk::CommandTypeSpecifier<ltea::mac::DownlinkControlInformation>
    {
        A(wns::ldk::fun::FUN* fun) : wns::ldk::CommandTypeSpecifier<ltea::mac::DownlinkControlInformation>(fun) {};
        void doOnData(const wns::ldk::CompoundPtr&){}
        void doSendData(const wns::ldk::CompoundPtr&){}
        bool doIsAccepting(const wns::ldk::CompoundPtr&) const { return true; }
        void doWakeup(){}
    };
    
    // ./openwns -tvT N4ltea3mac5tests20PerformanceModelTestE
    // ./openwns -tvT N4ltea3mac5tests20PerformanceModelTestE::getBLERsAndThroughputsForCQIs
    class PerformanceModelTest :
        public CppUnit::TestFixture
    {
        CPPUNIT_TEST_SUITE( PerformanceModelTest );
        CPPUNIT_TEST( getBLERsAndThroughputsForMCSs );
        CPPUNIT_TEST( getBLERsAndThroughputsForCQIs );
        CPPUNIT_TEST( printCodeRateTables );
        CPPUNIT_TEST( determineMCSLookupTable );
        CPPUNIT_TEST_SUITE_END();
    public:
        void setUp();
        void tearDown();

        void getBLERsAndThroughputsForMCSs();
        void getBLERsAndThroughputsForCQIs();
        void printCodeRateTables();
        void determineMCSLookupTable();

    private:
        wns::ldk::CommandProxy* proxy;
        A* fu;
        wns::pyconfig::Parser pyConfig;

        Result performEvaluation(double sinrdB, unsigned int numPRBs, unsigned int numLayers, unsigned int blockSize,
                                 double codeRate, imtaphy::l2s::ModulationScheme modulation, unsigned int numTrials);
        
    };

}}}
#endif


