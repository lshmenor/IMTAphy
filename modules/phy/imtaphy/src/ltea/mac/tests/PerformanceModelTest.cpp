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

#include <IMTAPHY/ltea/mac/tests/PerformanceModelTest.hpp>

#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tests/LayerStub.hpp>
#include <WNS/node/tests/Stub.hpp>
#include <WNS/ldk/helper/FakePDU.hpp>
#include <WNS/CppUnit.hpp>
#include <queue>

#include <IMTAPHY/ltea/mac/harq/HARQentity.hpp>
#include <IMTAPHY/ltea/mac/ModulationAndCodingSchemes.hpp>
#include <IMTAPHY/receivers/LteRel8Codebook.hpp>
#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <IMTAPHY/link2System/LteCQIs.hpp>
#include <IMTAPHY/link2System/BlockErrorModel.hpp>

#include <iostream>
#include <fstream>
#include <iomanip>

using namespace ltea::mac::tests;


/*
 * WARNING: this code is partly outdated
 * 
 */


//CPPUNIT_TEST_SUITE_REGISTRATION( PerformanceModelTest );
//CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( PerformanceModelTest, "PerformanceModelTest");

// run with ./openwns -tvT N4ltea3mac5tests20PerformanceModelTestE::getBLERsAndThroughputsForMCSs
// run with ./openwns -tvT N4ltea3mac5tests20PerformanceModelTestE::getBLERsAndThroughputs
void PerformanceModelTest::setUp() {
    // see ClassifierTest from ldk

    wns::ldk::ILayer* layer = new wns::ldk::tests::LayerStub();
    wns::pyconfig::Parser pycoParser;
    pycoParser.loadString("import openwns.logger\n"
                        "class LinkHandler:\n"
                        "  type = \"wns.ldk.SimpleLinkHandler\"\n"
                        "  isAcceptingLogger = openwns.logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
                        "  sendDataLogger = openwns.logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
                        "  wakeupLogger = openwns.logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
                        "  onDataLogger = openwns.logger.Logger(\"W-NS\", \"LinkHandler\", True)\n"
                        "  traceCompoundJourney = True\n"
                        "linkHandler = LinkHandler()\n"
                        "class FUNConfig:\n"
                        "  logger = openwns.logger.Logger(\"W-NS\",\"TestFUN\",True)\n"
                        "  commandProxy = openwns.FUN.CommandProxy(logger)\n"
                        "fun = FUNConfig()");
    wns::ldk::fun::FUN* fun = new wns::ldk::fun::Main(layer, pycoParser);
    proxy = fun->getProxy();
    fu = new A(fun);

    proxy->addFunctionalUnit("Scheduler", fu);

    wns::pyconfig::Parser emptyConfig;

 //   fuNet->getProxy()->addFunctionalUnit( "classifier", classifier );

 //   lower = new tools::Stub(fuNet, emptyConfig);

    std::stringstream ss;
    ss << "from openwns.node import Node\n"
       << "import ltea\n"
        << "import ltea.dll.harq\n"
        << "harqEntity = ltea.dll.harq.HARQEntity()\n"
        << "node1 = Node('Tx')\n"
        << "node2 = Node('Rx')\n";
    pyConfig.loadString(ss.str());

}

void PerformanceModelTest::tearDown()
{

}

double
getSinr(unsigned int index, unsigned int numSINRs, double minSINR, double maxSINR)
{
    return (maxSINR - minSINR) * static_cast<double>(index) / static_cast<double>(numSINRs) + minSINR;
}

void PerformanceModelTest::printCodeRateTables()
{
    unsigned int antennaPorts[3] ={1, 2, 4};
    unsigned int ttis[3] = {1, 5, 10}; // different subframes with and without sync and BCH

    ltea::mac::ModulationAndCodingSchemes* mcsLookup = &(ltea::mac::TheMCSLookup::Instance());

    std::ofstream codeRates;
    codeRates.open("codeRateTables.txt");
    codeRates.setf(std::ios::fixed, std::ios::floatfield);
    codeRates.setf(std::ios::showpoint);
    
    for (unsigned int numSixCenterPRBs = 0; numSixCenterPRBs <=6; numSixCenterPRBs++)
    {
        for (unsigned int pdcchLength = 3; pdcchLength > 0; pdcchLength--)
        {
            for (int i = 0; i < 3; i++)
            {
                unsigned int tti = ttis[i];
                
                for (int j = 0; j < 3; j++)
                {
                    unsigned int antennas = antennaPorts[j];

                    for (unsigned int numLayers = 1; numLayers <= antennas; numLayers++)
                    {
                        codeRates << "\n\nCenter PRBs=" << numSixCenterPRBs << " PDCCH control region size=" << pdcchLength << " TTI=" << tti << " antenna Ports=" << antennas << " numLayers=" << numLayers << ":\n";
                        codeRates << "MCS/PRB\t";
                        
                        for (unsigned int mcs = 0; mcs < 29; mcs++)
                        {
                            codeRates << mcs << "\t";
                        }
                        for (unsigned int numPRBs = 1; numPRBs <= 110; numPRBs++)
                        {
                            codeRates << "\n" << numPRBs << "\t";
                            for (unsigned int mcs = 0; mcs < 29; mcs++)
                            {
                                
                                codeRates <<  std::setprecision(4) << mcsLookup->getDownlinkCodeRate(mcs, pdcchLength, antennas, numPRBs, numLayers, numSixCenterPRBs, tti, false, 0, 0) << "\t";
                            }

                        }
                    }
                }
            }
        }
    }

    codeRates.close();
}

void PerformanceModelTest::getBLERsAndThroughputsForMCSs()
{
    unsigned int numPRBs = 20;
    unsigned int numLayers = 1;
    unsigned int numSINRs = 350; // use 350 for finer resolution
    unsigned int numMCSs = 29;
    double minSINR = -10.0;
    double maxSINR = 25.0;
    unsigned int pdcchLength = 3; // 3 symbols control region
    unsigned int numAntennaPorts = 1;
    unsigned int numSixCenterPRBs = 0; //6;
    unsigned int tti = 0;
    //unsigned int numTrials = 100; // use more trials for better resolution

    
    ltea::mac::ModulationAndCodingSchemes* mcsLookup = &(ltea::mac::TheMCSLookup::Instance());
    imtaphy::l2s::BlockErrorModel* blerModel = imtaphy::l2s::TheLTEBlockErrorModel::getInstance();
    
    
    boost::multi_array<double, 2> blers(boost::extents[numSINRs][numMCSs]);
    boost::multi_array<double, 2> throughput(boost::extents[numSINRs][numMCSs]);
    
    for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
    {
        std::cout << sinr*100/numSINRs << "%\n";
        for (unsigned int mcs = 0; mcs < numMCSs; mcs++)
        {
            
            imtaphy::l2s::ModulationScheme modulation = mcsLookup->getModulation(mcs);
            double codeRate = mcsLookup->getDownlinkCodeRate(mcs, pdcchLength, numAntennaPorts, numPRBs, numLayers, numSixCenterPRBs, tti, false, 0, 0);
            unsigned int blockSize = mcsLookup->getSize(mcs, numPRBs, numLayers);

            codeRate = std::min(1.0, codeRate);
            codeRate = std::max(0.0, codeRate);
    
            if (sinr == numSINRs -1)
            {
                std::cout << "MCS " << mcs << " codeRate=" << codeRate << " blockSize=" << blockSize << " modulation=" << modulation.getName() << "\n";
            }
            
//             Result result = performEvaluation(getSinr(sinr, numSINRs, minSINR, maxSINR), numPRBs,
//                                               numLayers, blockSize, codeRate, modulation, numTrials);
            blers[sinr][mcs] = blerModel->getBlockErrorRate(wns::Ratio::from_dB(getSinr(sinr, numSINRs, minSINR, maxSINR)), 
                                                                                      modulation,
                                                                                      codeRate,
                                                                                      blockSize);
            throughput[sinr][mcs] = (1.0 - blers[sinr][mcs]) *  static_cast<double>(blockSize) / (1e7) * 1000.0; // 10 MHz and 1ms TTIs
        }
    }

    std::ofstream blersMCS;
    blersMCS.open("blersMCS.txt");
    blersMCS.setf(std::ios::fixed, std::ios::floatfield);
    blersMCS.setf(std::ios::showpoint);
    
    std::cout <<"Writing block error rates to file\n";
    
    blersMCS << "#SINR\t";
    for (unsigned int mcs = 0; mcs < numMCSs; mcs++)
        blersMCS << mcs << "\t";
    blersMCS << "\n";

    for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
    {
        blersMCS << std::setw(8) << getSinr(sinr, numSINRs, minSINR, maxSINR) << "\t";
        for (unsigned int mcs = 0; mcs < numMCSs; mcs++)
        {
            blersMCS << std::setw(8) << blers[sinr][mcs] << "\t";
        }
        blersMCS << "\n";
    }
    blersMCS.close();


    std::ofstream efficienciesMCS;
    efficienciesMCS.open("efficienciesMCS.txt");
    efficienciesMCS.setf(std::ios::fixed, std::ios::floatfield);
    efficienciesMCS.setf(std::ios::showpoint);
    
    std::cout <<"Writing spectral efficiencies to file\n";
    
    efficienciesMCS << "#SINR\t";
    for (unsigned int mcs = 0; mcs < numMCSs; mcs++)
        efficienciesMCS << mcs << "\t";
    efficienciesMCS << "\n";
    
    for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
    {
        efficienciesMCS << std::setw(8) << getSinr(sinr, numSINRs, minSINR, maxSINR) << "\t";
        for (unsigned int mcs = 0; mcs < numMCSs; mcs++)
        {
            double modulationBits = mcsLookup->getModulation(mcs).getBitsPerSymbol();
            double codeRate = mcsLookup->getDownlinkCodeRate(mcs, pdcchLength, numAntennaPorts, numPRBs, numLayers, numSixCenterPRBs, tti, false, 0, 0);
            efficienciesMCS << std::setw(8) << modulationBits * codeRate * (1.0 - blers[sinr][mcs]) << "\t";
        }
        efficienciesMCS << "\n";
    }
    efficienciesMCS.close();
    

    
    std::ofstream throughputsMCS;
    throughputsMCS.open("throughputsMCS.txt");
    throughputsMCS.setf(std::ios::fixed, std::ios::floatfield);
    throughputsMCS.setf(std::ios::showpoint);
    
    std::cout <<"Writing throughputs to file:\n";

    throughputsMCS << "#SINR\t";
    for (unsigned int mcs = 0; mcs < numMCSs; mcs++)
        throughputsMCS << mcs << "\t";
    throughputsMCS << "\n";

    
    for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
    {
        throughputsMCS << getSinr(sinr, numSINRs, minSINR, maxSINR) << "\t";
        for (unsigned int mcs = 0; mcs < numMCSs; mcs++)
        {
            throughputsMCS << throughput[sinr][mcs] << "\t";
        }
        throughputsMCS << "\n";
    }
    throughputsMCS.close();

    for (unsigned int mcs = 0; mcs < numMCSs; mcs++)
    {
        for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
        {
            if (blers[sinr][mcs] < 0.1)
            {
                std::cout << "MCS " << mcs << " best switching point at " << getSinr(sinr, numSINRs, minSINR, maxSINR) << "\n";
                break;
            }
        }
    }
}



void PerformanceModelTest::getBLERsAndThroughputsForCQIs()
{
    unsigned int numPRBs = 50;
    unsigned int numSINRs = 700; // use 350 for finer resolution
    unsigned int numCQIs = 15;
    double minSINR = -10.0;
    double maxSINR = 25.0;

    
    boost::multi_array<double, 2> blers(boost::extents[numSINRs][numCQIs]);
    boost::multi_array<double, 2> throughput(boost::extents[numSINRs][numCQIs]);
    
    imtaphy::l2s::BlockErrorModel* blerModel = imtaphy::l2s::TheLTEBlockErrorModel::getInstance();
    
    for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
    {
        for (unsigned int cqi = 0; cqi < numCQIs; cqi++)
        {
            imtaphy::l2s::ModulationScheme modulation = imtaphy::l2s::CQITable[cqi+1].modulation;
            double codeRate = imtaphy::l2s::CQITable[cqi+1].codeRate;
            unsigned int blockSize = numPRBs * modulation.getBitsPerSymbol() * (12 * (14 - 3) - 6) * codeRate; // assume 1 antenna port (6 RS)
            blockSize = 5000;
//             Result result = performEvaluation(getSinr(sinr, numSINRs, minSINR, maxSINR), numPRBs,
//                                               numLayers, blockSize, codeRate, modulation, numTrials);
            blers[sinr][cqi] = blerModel->getBlockErrorRate(wns::Ratio::from_dB(getSinr(sinr, numSINRs, minSINR, maxSINR)), 
                                                                                      modulation,
                                                                                      codeRate,
                                                                                      blockSize);
            throughput[sinr][cqi] = static_cast<double>(modulation.getBitsPerSymbol()) * (1.0 - blers[sinr][cqi]) * codeRate ; 
        }
    }
    
    std::ofstream blersCQI;
    blersCQI.open("blersCQI.txt");
    blersCQI.setf(std::ios::fixed, std::ios::floatfield);
    blersCQI.setf(std::ios::showpoint);
    
    std::cout <<"Block error rates:\n";
    
    std::cout <<"Writing block error rates to file\n";
    
    blersCQI << "#SINR\t";
    for (unsigned int cqi = 0; cqi < numCQIs; cqi++)
        blersCQI << cqi << "\t";
    blersCQI << "\n";
    
    
    for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
    {
        blersCQI << std::setw(8) << getSinr(sinr, numSINRs, minSINR, maxSINR) << "\t";
        for (unsigned int cqi = 0; cqi < numCQIs; cqi++)
        {
            blersCQI << std::setw(8) << blers[sinr][cqi] << "\t";
        }
        blersCQI << "\n";
    }
    blersCQI.close();

    std::ofstream efficienciesCQI;
    efficienciesCQI.open("efficienciesCQI.txt");
    efficienciesCQI.setf(std::ios::fixed, std::ios::floatfield);
    efficienciesCQI.setf(std::ios::showpoint);
    
    std::cout <<"Writing spectral efficiencies to file\n";
    
    efficienciesCQI << "#SINR\t";
    for (unsigned int cqi = 0; cqi < numCQIs; cqi++)
        efficienciesCQI << cqi+1 << "\t";
    efficienciesCQI << "\n";
    
    for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
    {
        efficienciesCQI << std::setw(8) << getSinr(sinr, numSINRs, minSINR, maxSINR) << "\t";
        for (unsigned int cqi = 0; cqi < numCQIs; cqi++)
        {
            double modulationBits = imtaphy::l2s::CQITable[cqi+1].modulation.getBitsPerSymbol();
            double codeRate = imtaphy::l2s::CQITable[cqi+1].codeRate;
            efficienciesCQI << std::setw(8) << modulationBits * codeRate * (1.0 - blers[sinr][cqi]) << "\t";
        }
        efficienciesCQI << "\n";
    }
    efficienciesCQI.close();
    

    std::ofstream throughputsCQI;
    throughputsCQI.open("throughputsCQI.txt");
    throughputsCQI.setf(std::ios::fixed, std::ios::floatfield);
    throughputsCQI.setf(std::ios::showpoint);
    
    std::cout <<"Writing throughputs to file:\n";
    
    throughputsCQI << "#SINR\t";
    for (unsigned int cqi = 0; cqi < numCQIs; cqi++)
        throughputsCQI << cqi + 1 << "\t";
    throughputsCQI << "\n";
    
    
    for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
    {
        throughputsCQI << getSinr(sinr, numSINRs, minSINR, maxSINR) << "\t";
        for (unsigned int cqi = 0; cqi < numCQIs; cqi++)
        {
            throughputsCQI << throughput[sinr][cqi] << "\t";
        }
        throughputsCQI << "\n";
    }
    throughputsCQI.close();
    
    for (unsigned int cqi = 0; cqi < numCQIs; cqi++)
    {
        for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
        {
            if (blers[sinr][cqi] < 0.1)
            {
                std::cout << "CQI " << cqi + 1 << " best switching point at " << getSinr(sinr, numSINRs, minSINR, maxSINR) << "\n";
                break;
            }
        }
    }
}


//  ./openwns -tvT  N4ltea3mac5tests20PerformanceModelTestE::determineMCSLookupTable
// change test registration to normal registration if not found with named registration
void 
PerformanceModelTest::determineMCSLookupTable()
{
    unsigned int maxAvailableSymbols = 150;
    unsigned int numPRBs = 110; //110;
    unsigned int numSINRs = 350; // increase for higher resolution, e.g. 350
    unsigned int numMCSs = 29;
    unsigned int numLayers = 1;
    double minSINR = -10.0;
    double maxSINR = 25.0;
//    unsigned int numTrials = 100; // increase for higher resolution e.g. 200
    
    imtaphy::l2s::BlockErrorModel* blerModel = imtaphy::l2s::TheLTEBlockErrorModel::getInstance();
    
    ltea::mac::ModulationAndCodingSchemes* mcsLookup = &(ltea::mac::TheMCSLookup::Instance());
    
    boost::multi_array<double, 4> blers(boost::extents[numPRBs][maxAvailableSymbols][numSINRs][numMCSs]);
    
    for (unsigned int nPRBs = 0; nPRBs < numPRBs; nPRBs++)
    {
        for (unsigned int nSymbols = 0; nSymbols < maxAvailableSymbols; nSymbols++)
        {    
            std::cout << "Now running for nPRBs= " << nPRBs << " and nSymbols = " << nSymbols+1 << "\n";
            for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
            {
                for (unsigned int mcs = 0; mcs < numMCSs; mcs++)
                {
                    imtaphy::l2s::ModulationScheme modulation = mcsLookup->getModulation(mcs);
                    unsigned int blockSize = mcsLookup->getSize(mcs, (nPRBs+1), numLayers);
                    double codeRate = mcsLookup->getEffectiveCodeRate(blockSize, modulation.getBitsPerSymbol() * (nPRBs+1) * numLayers * (nSymbols+1));

                    codeRate = std::min(1.0, codeRate);
                    codeRate = std::max(0.0, codeRate);
                    
                    if (sinr == numSINRs -1)
                    {
                        std::cout << "MCS " << mcs << " codeRate=" << codeRate << " blockSize=" << blockSize << " modulation=" << modulation.getName() << "\n";
                    }
                    
//                     Result result = performEvaluation(getSinr(sinr, numSINRs, minSINR, maxSINR), (nPRBs+1),
//                                                     numLayers, blockSize, codeRate, modulation, numTrials);
//                    blers[nPRBs][nSymbols][sinr][mcs] = result.firstAttemptBLER;
                    blers[nPRBs][nSymbols][sinr][mcs] =  blerModel->getBlockErrorRate(wns::Ratio::from_dB(getSinr(sinr, numSINRs, minSINR, maxSINR)), 
                                                                                      modulation,
                                                                                      codeRate,
                                                                                      blockSize);

                    
                }
            }
        }
    }
   
    std::ofstream MCSthresholds;
    MCSthresholds.open("MCSthresholds.txt");
    
    MCSthresholds << "double mcsThresholds[" << numPRBs << "][" << maxAvailableSymbols << "][ "<< numMCSs  << "] = {\n";
    
    for (unsigned int nPRBs = 0; nPRBs < numPRBs; nPRBs++)
    {
        for (unsigned int nSymbols = 0; nSymbols < maxAvailableSymbols; nSymbols++)
        {   
            if (nSymbols == 0)
                MCSthresholds << "{";
            for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
            {
                if (blers[nPRBs][nSymbols][sinr][0] < 0.1)
                {
                    imtaphy::l2s::ModulationScheme modulation = mcsLookup->getModulation(0);
                    unsigned int blockSize = mcsLookup->getSize(0, numPRBs, numLayers);
                    double codeRate = mcsLookup->getEffectiveCodeRate(blockSize, modulation.getBitsPerSymbol() * (nPRBs+1) * numLayers * (nSymbols+1));

                    if (codeRate >= 1.0)
                        MCSthresholds << "{" << 10000.0;
                    else    
                        MCSthresholds << "{" << getSinr(sinr, numSINRs, minSINR, maxSINR);
                    break;
                }
                if (sinr == numSINRs - 1)
                    MCSthresholds << "{" << 10000.0;
            }

            for (unsigned int mcs = 1; mcs < numMCSs; mcs++)
            {
                for (unsigned int sinr = 0; sinr < numSINRs; sinr++)
                {
                    if (blers[nPRBs][nSymbols][sinr][mcs] < 0.1)
                    {                
                        imtaphy::l2s::ModulationScheme modulation = mcsLookup->getModulation(mcs);
                        unsigned int blockSize = mcsLookup->getSize(mcs, numPRBs, numLayers);
                        double codeRate = mcsLookup->getEffectiveCodeRate(blockSize, modulation.getBitsPerSymbol() * (nPRBs+1) * numLayers * (nSymbols+1));

                        if (codeRate >= 1.0)
                            MCSthresholds << "," << 10000.0;
                        else    
                            MCSthresholds << "," << getSinr(sinr, numSINRs, minSINR, maxSINR);

                        break;
                    }
                    if (sinr == numSINRs - 1)
                        MCSthresholds << "," << 10000.0;
                }
            }
            if (nSymbols == maxAvailableSymbols - 1)
                MCSthresholds << "}\n";
            else
                MCSthresholds << "},\n";
        }
        if (nPRBs == numPRBs - 1)
            MCSthresholds << "}\n";
        else
            MCSthresholds << "},\n";
    }
    MCSthresholds << "};\n";
    
    MCSthresholds.close();
}



Result
ltea::mac::tests::PerformanceModelTest::performEvaluation(double sinrdB, unsigned int numPRBs, unsigned int numLayers, unsigned int blockSize,
                                                          double codeRate, imtaphy::l2s::ModulationScheme modulation, unsigned int trials)
{
    int numSenderProcesses = 8;
    int numReceiverProcesses = 8;
    int numRVs = 1;
    int retransmissionLimit = 3;
    double decodingDelay = 0.0;
    wns::logger::Logger logger;
    
    ltea::mac::harq::HARQEntity* rxEntity = new ltea::mac::harq::HARQEntity( pyConfig.getView("harqEntity"),
                                                                          numSenderProcesses,
                                                                          numReceiverProcesses,
                                                                          numRVs,
                                                                          retransmissionLimit,
                                                                          decodingDelay,
                                                                          logger);

    ltea::mac::harq::HARQEntity* txEntity = new ltea::mac::harq::HARQEntity( pyConfig.getView("harqEntity"),
                                                                            numSenderProcesses,
                                                                            numReceiverProcesses,
                                                                            numRVs,
                                                                            retransmissionLimit,
                                                                            decodingDelay,
                                                                            logger);
    
    
    wns::ldk::CommandReaderInterface* dciReader = proxy->getCommandReader("Scheduler");
    assure(dciReader, "did not get dcireader");
    rxEntity->setDCIReader(dciReader);
    txEntity->setDCIReader(dciReader);
    
//     imtaphy::receivers::LteRel8Codebook<float>* codebook = &(wns::SingletonHolder<imtaphy::receivers::LteRel8Codebook<float> >::Instance());

    std::queue<wns::ldk::CompoundPtr> queue;
    imtaphy::interface::PrbPowerPrecodingMap prbPowerPrecoders;
    imtaphy::interface::PRBVector prbs(numPRBs);
    for (unsigned int prb = 0; prb < numPRBs; prb++)
    {
        prbPowerPrecoders[prb] = imtaphy::interface::PowerAndPrecoding();
        prbs[prb] = prb; 
    }
    for (unsigned int n = 0; n < trials; n++)
    {
        
/*
        unsigned int mcsIndex = tbsLookup->getSuitableMCSindex(imtaphy::receivers::feedback::CQITable[cqi].codeRate,
                                                               imtaphy::receivers::feedback::CQITable[cqi].modulation,
                                                               numPRBs,
                                                               numLayers);
        unsigned int tbSize = tbsLookup->getSize(mcsIndex, numPRBs, numLayers);
        */
        
        wns::ldk::CompoundPtr compound(new wns::ldk::Compound(proxy->createCommandPool()));
        proxy->activateCommand(compound->getCommandPool(), fu);
        ltea::mac::DownlinkControlInformation* dci = dciReader->readCommand<ltea::mac::DownlinkControlInformation>(compound->getCommandPool());
        dci->peer.assignedToLayers = std::vector<unsigned int>(1,1);
        dci->peer.blockSize = blockSize;
        dci->peer.codeRate = codeRate;
        dci->peer.modulation = modulation;
        dci->local.prbPowerPrecoders =  prbPowerPrecoders;
    
        queue.push(compound);
    }
    
    wns::Ratio sinr = wns::Ratio::from_dB(sinrdB);

    
    
    imtaphy::interface::TransmissionStatusPtr status(new imtaphy::interface::TransmissionStatus(numLayers, prbs));
    imtaphy::interface::SINRVector sinrs;
    sinrs.push_back(sinr);
    for (unsigned int prb = 0; prb < numPRBs; prb++)
        status->setSINRsForPRB(prb, sinrs);
    
    unsigned int ttisNeeded = 0;
    unsigned int totalThroughput = 0;
    unsigned int firstAttemptBlockErrors = 0;
    
    while (!queue.empty() || txEntity->hasRetransmissions())
    {
        ttisNeeded++;
        
        if (txEntity->hasRetransmissions())
        {
            std::list<int> ids = txEntity->getProcessesWithRetransmissions();
            wns::ldk::CompoundPtr compound = txEntity->getRetransmission(ids.front(), 0);
            ltea::mac::DownlinkControlInformation* dci = dciReader->readCommand<ltea::mac::DownlinkControlInformation>(compound->getCommandPool());
            txEntity->retransmissionStarted(ids.front(), 0);
            
            if (rxEntity->decodeReceivedTransportBlock(compound, status, 0))
                totalThroughput += dci->peer.blockSize;
                        
            continue;
        }
        else
        {
            wns::ldk::CompoundPtr compound = queue.front();
            queue.pop();
            ltea::mac::DownlinkControlInformation* dci = dciReader->readCommand<ltea::mac::DownlinkControlInformation>(compound->getCommandPool());
            

            txEntity->storeScheduledTransportBlock(compound, 0);
            

            
            if (rxEntity->decodeReceivedTransportBlock(compound, status, 0))
                totalThroughput += dci->peer.blockSize;
            else
                firstAttemptBlockErrors++;
            
            continue;
        }
    }

    Result result;
    result.firstAttemptBLER = static_cast<double>(firstAttemptBlockErrors) / static_cast<double>(trials);
    // bits / tti (of 1ms)
    result.spectralEfficiency = static_cast<double>(totalThroughput) / static_cast<double>(ttisNeeded) / (200.0 * static_cast<double>(numPRBs));

    delete txEntity;
    delete rxEntity;
    
    return result;
}    

