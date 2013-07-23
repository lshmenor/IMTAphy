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

// By default, the array access methods operator() and operator[] perform range checking. If a supplied index is out of the range defined for an array, an assertion will abort the program. To disable range checking (for performance reasons in production releases), define the BOOST_DISABLE_ASSERTS preprocessor macro prior to including multi_array.hpp in an application.

// For opt versions disable range checking in the multi_array wrapper
// WNS_NDEBUG means: assures are disabled
#ifdef  WNS_NDEBUG
#define BOOST_DISABLE_ASSERTS
#endif
#include <boost/multi_array.hpp>

#include <itpp/itbase.h>
#include <algorithm>
#include <iostream>

#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <IMTAPHY/receivers/LteRel8Codebook.hpp>
#include <WNS/Singleton.hpp>

namespace imtaphy { namespace receivers { namespace tests {
    
    
            class LteRel8CodebookTest :
            public CppUnit::TestFixture
                {
                
                    CPPUNIT_TEST_SUITE( LteRel8CodebookTest );
                    CPPUNIT_TEST( test4TxCodebook );
                    CPPUNIT_TEST_SUITE_END();

                public:

                    void setUp();
                    void tearDown();
                    void test4TxCodebook();
                    
        
                private:
                };
    
                CPPUNIT_TEST_SUITE_REGISTRATION( LteRel8CodebookTest);
                 


                void
                LteRel8CodebookTest::setUp()
                {       
    
                }

                void 
                LteRel8CodebookTest::tearDown()
                {
    
                }
                
                void
                LteRel8CodebookTest::test4TxCodebook()
                {
                    // see 36.211 (V10.0) Table 6.3.4.2.3-2
                    // unfortunately, it does not accept complex numbers in init string
                    std::vector<itpp::cmat> uVectors(16);
                    uVectors[0] = itpp::cmat("1;-1;-1;-1");
                    
                    uVectors[1] = itpp::cmat("1;1;1;1");
                    uVectors[1](1,0) = std::complex<double>(0,-1);
                    uVectors[1](3,0) = std::complex<double>(0,1);
                    
                    uVectors[2] = itpp::cmat("1;1;-1;1");
                    
                    uVectors[3] = itpp::cmat("1; 1;1;-1");
                    uVectors[3](1,0) = std::complex<double>(0,1);
                    uVectors[3](3,0) = std::complex<double>(0,-1);
                    
                    uVectors[4] = itpp::cmat("1;1;1;1"); 
                    uVectors[4](1,0) = std::complex<double>(-1,-1)/sqrt(2); 
                    uVectors[4](2,0) = std::complex<double>(0,-1);
                    uVectors[4](3,0) = std::complex<double>(1,-1)/sqrt(2);
                    
                    uVectors[5] = itpp::cmat("1;1;1;1"); 
                    uVectors[5](1,0) = std::complex<double>(1,-1) / sqrt(2); 
                    uVectors[5](2,0) = std::complex<double>(0,1); 
                    uVectors[5](3,0) = std::complex<double>(-1,-1) / sqrt(2); 
                    
                    uVectors[6] = itpp::cmat("1;1;1;1");
                    uVectors[6](1,0) = std::complex<double>(1,1) / sqrt(2); 
                    uVectors[6](2,0) = std::complex<double>(0,-1); 
                    uVectors[6](3,0) = std::complex<double>(-1,1) / sqrt(2); 

                    uVectors[7] = itpp::cmat("1;1;1;1");
                    uVectors[7](1,0) = std::complex<double>(-1,1) / sqrt(2); 
                    uVectors[7](2,0) = std::complex<double>(0,1); 
                    uVectors[7](3,0) = std::complex<double>(1,1) / sqrt(2); 

                    uVectors[8] = itpp::cmat("1;-1;1;1");
                    
                    uVectors[9] = itpp::cmat("1;1;-1;1");
                    uVectors[9](1,0) = std::complex<double>(0,-1); 
                    uVectors[9](3,0) = std::complex<double>(0,-1); 
                    
                    
                    uVectors[10] = itpp::cmat("1;1;1;-1");
                    
                    uVectors[11] = itpp::cmat("1; 1;-1;1");
                    uVectors[11](1,0) = std::complex<double>(0,1); 
                    uVectors[11](3,0) = std::complex<double>(0,1); 

                    uVectors[12] = itpp::cmat("1;-1;-1;1");
                    uVectors[13] = itpp::cmat("1;-1;1;-1");
                    uVectors[14] = itpp::cmat("1;1;-1;-1");
                    uVectors[15] = itpp::cmat("1;1;1;1");
                    
                    
                    std::vector<itpp::mat> permutations(16);
                    permutations[0] = itpp::mat("1 0 0 0; 1 4 0 0; 1 2 4 0; 1 2 3 4");
                    permutations[1] = itpp::mat("1 0 0 0; 1 2 0 0; 1 2 3 0; 1 2 3 4");
                    permutations[2] = itpp::mat("1 0 0 0; 1 2 0 0; 1 2 3 0; 3 2 1 4");
                    permutations[3] = itpp::mat("1 0 0 0; 1 2 0 0; 1 2 3 0; 3 2 1 4");
                    permutations[4] = itpp::mat("1 0 0 0; 1 4 0 0; 1 2 4 0; 1 2 3 4");
                    permutations[5] = itpp::mat("1 0 0 0; 1 4 0 0; 1 2 4 0; 1 2 3 4");
                    permutations[6] = itpp::mat("1 0 0 0; 1 3 0 0; 1 3 4 0; 1 3 2 4");
                    permutations[7] = itpp::mat("1 0 0 0; 1 3 0 0; 1 3 4 0; 1 3 2 4");
                    permutations[8] = itpp::mat("1 0 0 0; 1 2 0 0; 1 2 4 0; 1 2 3 4");
                    permutations[9] = itpp::mat("1 0 0 0; 1 4 0 0; 1 3 4 0; 1 2 3 4");
                    permutations[10] = itpp::mat("1 0 0 0; 1 3 0 0; 1 2 3 0; 1 3 2 4");
                    permutations[11] = itpp::mat("1 0 0 0; 1 3 0 0; 1 3 4 0; 1 3 2 4");
                    permutations[12] = itpp::mat("1 0 0 0; 1 2 0 0; 1 2 3 0; 1 2 3 4");
                    permutations[13] = itpp::mat("1 0 0 0; 1 3 0 0; 1 2 3 0; 1 3 2 4");
                    permutations[14] = itpp::mat("1 0 0 0; 1 3 0 0; 1 2 3 0; 3 2 1 4");
                    permutations[15] = itpp::mat("1 0 0 0; 1 2 0 0; 1 2 3 0; 1 2 3 4");
                    itpp::mat scaling("1 1 1 1"); scaling(0,1) = 1.0/sqrt(2); scaling(0,2) = 1.0/sqrt(3); scaling(0,3) = 1.0/2.0;
                    
                    imtaphy::receivers::LteRel8Codebook<float>* fourTxCodebookFloat = &(wns::SingletonHolder<imtaphy::receivers::LteRel8Codebook<float> >::Instance());

                    for (unsigned int pmi = 0; pmi < 16; pmi++)
                    {
                        // For computation, see 36.211 section 6.3.4.2.3
//                        std::cout << "u-vector u_"<<pmi<<"=\n" << uVectors[pmi] << "\n";
                        itpp::cmat W = itpp::eye_c(4) - 2*uVectors[pmi]*itpp::hermitian_transpose(uVectors[pmi]) / (itpp::hermitian_transpose(uVectors[pmi]) * uVectors[pmi])(0,0);
//                        std::cout << "W_" << pmi << "=\n" << W << "\n";

                        
                        for (unsigned int rank = 1; rank <= 4; rank ++)
                        {
                            imtaphy::detail::ComplexFloatMatrixPtr matrix = fourTxCodebookFloat->getPrecodingMatrix(4, rank, pmi);
//                            imtaphy::detail::displayMatrix(*matrix);
                            
                            CPPUNIT_ASSERT_EQUAL(rank, matrix->getColumns());
                            CPPUNIT_ASSERT_EQUAL(4u, matrix->getRows());

//                            std::cout << "\nrank= " << rank << ", pmi= " << pmi << std::endl;
                            for (unsigned int i = 0; i < 4; i++)
                            {
                                for (unsigned int j = 0; j < rank; j++)
                                {
//                                     std::cout << "Comparing " << W(i,permutations[pmi](rank-1,j)-1) * scaling(0,rank-1) << " against codebook " << (*matrix)[i][j] << "\n";
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(W(i,permutations[pmi](rank-1,j)-1).real() * scaling(0,rank-1), (*matrix)[i][j].real(), 1e-5);
                                    CPPUNIT_ASSERT_DOUBLES_EQUAL(W(i,permutations[pmi](rank-1,j)-1).imag() * scaling(0,rank-1), (*matrix)[i][j].imag(), 1e-5);
                                }
                            }
                        }
                    }   
                }
    
    
        class LteRel8CodebookTestSpike :
            public CppUnit::TestFixture
                {
                
                    CPPUNIT_TEST_SUITE( LteRel8CodebookTestSpike );
                    CPPUNIT_TEST( display2TxCodebook );
                    CPPUNIT_TEST( display4TxCodebook );
                    CPPUNIT_TEST( displayNoPrecoding );
                    CPPUNIT_TEST( displayUniqueColumns4TxCodebook );
                    CPPUNIT_TEST_SUITE_END();

                public:

                    void setUp();
                    void tearDown();
                    void display2TxCodebook();
                    void display4TxCodebook();
                    void displayNoPrecoding();
                    void displayUniqueColumns4TxCodebook();
                    
        
                private:
                };
    
                // register as a spike, so it does not get executed with the other tests but hast to be
                // called like this: ./openwns -tvT N7imtaphy9receivers5tests19LteRel8CodebookTestE
                CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( LteRel8CodebookTestSpike, wns::testsuite::Spike());
                //CPPUNIT_TEST_SUITE_REGISTRATION( LteRel8CodebookTestSpike);
                 


                void
                LteRel8CodebookTestSpike::setUp()
                {       
    
                }

                void 
                LteRel8CodebookTestSpike::tearDown()
                {
    
                }

                void 
                LteRel8CodebookTestSpike::displayNoPrecoding()
                {
                    imtaphy::receivers::LteRel8Codebook<float>* noPrecodingFloat = &(wns::SingletonHolder<imtaphy::receivers::LteRel8Codebook<float> >::Instance());
    
                    std::cout << "Now printing no-precoding \"precoding\" matrices:\n";
                    for (int s = 1; s <=8; s++)
                        for (int m = 1; m <= s; m++)
                        {
                            std::cout << "Now printing equal power precoding matrix for " << s << " Tx antennas and " << m << " layers\n";
                            imtaphy::detail::displayMatrix(*(noPrecodingFloat->getPrecodingMatrix(s, m, imtaphy::receivers::feedback::EqualPower)));
                        }   
                }

                void
                LteRel8CodebookTestSpike::display2TxCodebook()
                {
                    imtaphy::receivers::LteRel8Codebook<float>* twoTxCodebookFloat = &(wns::SingletonHolder<imtaphy::receivers::LteRel8Codebook<float> >::Instance());

                    std::cout << "Now printing Codebooks for 2Tx Antennas\n";
                    for (int rank = 1; rank <= 2; rank ++)
                        for (int pmi = 0; pmi < 4; pmi++)
                        {
                            std::cout << "rank= " << rank << ", pmi= " << pmi << std::endl;
                            if ( (rank == 2) && (pmi == 0 || pmi == 3))
                                continue;
                            imtaphy::detail::displayMatrix(*(twoTxCodebookFloat->getPrecodingMatrix(2, rank, pmi)));
                        }

                }
            

                void
                LteRel8CodebookTestSpike::display4TxCodebook()
                {
                    imtaphy::receivers::LteRel8Codebook<float>* fourTxCodebookFloat = &(wns::SingletonHolder<imtaphy::receivers::LteRel8Codebook<float> >::Instance());

                    std::cout << "Now printing Codebooks for 4Tx Antennas\n";
                    for (int rank = 1; rank <= 4; rank ++)
                        for (int pmi = 0; pmi < 16; pmi++)
                        {
                            std::cout << "rank= " << rank << ", pmi= " << pmi << std::endl;
                            imtaphy::detail::displayMatrix(*(fourTxCodebookFloat->getPrecodingMatrix(4, rank, pmi)));
                        }
                }
                
                void
                LteRel8CodebookTestSpike::displayUniqueColumns4TxCodebook()
                {
                    imtaphy::receivers::LteRel8Codebook<float>* fourTxCodebookFloat = &(wns::SingletonHolder<imtaphy::receivers::LteRel8Codebook<float> >::Instance());

                    std::cout << "\nNow printing " << fourTxCodebookFloat->columnLookup.size() << " unique column ids and their occurences in the 4 Tx codebook";

                    
                    for (std::map<unsigned int, CodebookColumnSet>::iterator iter = fourTxCodebookFloat->columnLookup.begin(); iter != fourTxCodebookFloat->columnLookup.end(); iter++)
                    {
                        std::cout << "\nIndex " << iter->first << " occurs for these (pmi, column) combinations:\n";
                        for (imtaphy::receivers::CodebookColumnSet::iterator coordinateIter = iter->second.begin(); coordinateIter != iter->second.end(); coordinateIter++)
                        {
                            std::cout << "(" << coordinateIter->pmi << ", " << coordinateIter->column << ") ";
                        }
                    }
                    
                    std::cout << "\nBuilt the following index table:\n";
                    std::cout << "PMI\tCol1\tCol2\tCol3\tCol4";
                    for (unsigned int pmi = 0; pmi < 16; pmi++)
                    {
                        std::cout << "\n" << pmi << "\t";
                        for (unsigned int column = 0; column < 4; column++)
                        {
                            std::cout << fourTxCodebookFloat->getIndex(pmi, column) << "\t";
                        }
                    }
                    std::cout << "\n";
                }
    
}}}

