/*******************************************************************************
* This file is part of IMTAphy
* _____________________________________________________________________________
*
* Copyright (C) 2010
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

#include <WNS/CppUnit.hpp>
#include <cppunit/extensions/HelperMacros.h>

#include <IMTAPHY/link2System/BlockErrorModel.hpp>
#include <IMTAPHY/link2System/MMIBeffectiveSINR.hpp>
#include <IMTAPHY/link2System/Modulations.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

namespace imtaphy { namespace l2s { namespace tests {


    class BlockErrorModelTest :
        public CppUnit::TestFixture
            {

                CPPUNIT_TEST_SUITE( BlockErrorModelTest );
                CPPUNIT_TEST( outputCurves );
                CPPUNIT_TEST_SUITE_END();

            public:

                void setUp();
                void tearDown();
                void outputCurves();

            private:
            };

            CPPUNIT_TEST_SUITE_REGISTRATION( BlockErrorModelTest);



            void
            BlockErrorModelTest::setUp()
            {

            }

            void
            BlockErrorModelTest::tearDown()
            {

            }

            void
            BlockErrorModelTest::outputCurves()
            {
                MMIBeffectiveSINR mmib;
                imtaphy::l2s::BlockErrorModel blerModel;

                // some existing and some non-existing (interpolated between entries or
                // just the min/max available entries) block sizes
                unsigned int blockSizes[7] = {40, 100, 300, 500, 2000, 5000, 7000};

                for (unsigned int blockSizeIndex = 0; blockSizeIndex < 7; blockSizeIndex++)
                {
                    unsigned int blockSize = blockSizes[blockSizeIndex];

                    std::stringstream ss;
                    ss << "BLERs_LTEcqis_BL" << blockSize << ".txt";

                    std::ofstream file(ss.str().c_str());

                    file << "#SINR\t";

                    for (unsigned int cqi = 1; cqi < 16; cqi++)
                        file << "CQI " << cqi << "\t";
                    file << "\n";

                    for (int sinrIdx = -100.01; sinrIdx < 250; sinrIdx++)
                    {
                        file << sinrIdx/10. << "\t";
                        wns::Ratio sinr = wns::Ratio::from_dB(sinrIdx / 10.);

                        // QPSK values
                        file << blerModel.getBlockErrorRate(sinr, QPSK(), 78./1024., blockSize) << "\t";
                        file << blerModel.getBlockErrorRate(sinr, QPSK(), 120./1024., blockSize) << "\t";
                        file << blerModel.getBlockErrorRate(sinr, QPSK(), 193./1024., blockSize) << "\t";
                        file << blerModel.getBlockErrorRate(sinr, QPSK(), 308./1024., blockSize) << "\t";
                        file << blerModel.getBlockErrorRate(sinr, QPSK(), 449./1024., blockSize) << "\t";
                        file << blerModel.getBlockErrorRate(sinr, QPSK(), 602./1024., blockSize) << "\t";

                        // QAM16
                        file << blerModel.getBlockErrorRate(sinr, QAM16(), 378./1024., blockSize) << "\t";
                        file << blerModel.getBlockErrorRate(sinr, QAM16(), 490./1024., blockSize) << "\t";
                        file << blerModel.getBlockErrorRate(sinr, QAM16(), 616./1024., blockSize) << "\t";

                        // QAM64
                        file << blerModel.getBlockErrorRate(sinr, QAM64(), 466./1024., blockSize) << "\t";
                        file << blerModel.getBlockErrorRate(sinr, QAM64(), 567./1024., blockSize) << "\t";
                        file << blerModel.getBlockErrorRate(sinr, QAM64(), 666./1024., blockSize) << "\t";
                        file << blerModel.getBlockErrorRate(sinr, QAM64(), 772./1024., blockSize) << "\t";
                        file << blerModel.getBlockErrorRate(sinr, QAM64(), 873./1024., blockSize) << "\t";
                        file << blerModel.getBlockErrorRate(sinr, QAM64(), 948./1024., blockSize) << "\t";

                        file << "\n";
                    }
                    file.close();

                }

                std::ofstream file("BLERs_differentCR_QPSK_BL00.txt");

                file << "#SINR\t";

                for (int codeRate = 1; codeRate < 50; codeRate++)
                    file << "CR " << float(codeRate)/50.0 << "\t";
                file << "\n";

                for (int sinrIdx = -100; sinrIdx < 250; sinrIdx++)
                {
                    file << sinrIdx/10. << "\t";

                    for (int codeRate = 1; codeRate < 50; codeRate++)
                    {
                        float cr = float(codeRate) / 50.0;
                        wns::Ratio sinr = wns::Ratio::from_dB(sinrIdx / 10.);
                        file << std::scientific << blerModel.getBlockErrorRate(sinr, QAM16(), cr, 50) << "\t";
                    }
                    file << "\n";
                }
                file.close();


                std::ofstream file2("BLERs_differentBL_QAM64_CR03.txt");

                file2 << "#SINR\t";

                for (int blockSize = 40; blockSize < 6144; blockSize += 100)
                    file2 << "BL " << blockSize << "\t";
                file2 << "\n";

                for (int sinrIdx = -100.01; sinrIdx < 250; sinrIdx++)
                {
                    file2 << sinrIdx/10. << "\t";

                    for (int blockSize = 40; blockSize < 6144; blockSize += 100)
                    {
                        wns::Ratio sinr = wns::Ratio::from_dB(sinrIdx / 10.);
                        file2 << std::scientific << blerModel.getBlockErrorRate(sinr, QAM64(), 0.3, blockSize) << "\t";
                    }
                    file2 << "\n";
                }
                file2.close();

            }


}}}

