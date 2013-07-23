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

#ifndef IMTAPHY_RECEIVERS_LTEREL8CODEBOOK_HPP
#define IMTAPHY_RECEIVERS_LTEREL8CODEBOOK_HPP

#include <WNS/Singleton.hpp>

#ifdef  WNS_NDEBUG
#define BOOST_DISABLE_ASSERTS
#endif
#include <boost/multi_array.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <IMTAPHY/receivers/feedback/LteFeedback.hpp>
#include <map>

namespace imtaphy {

    namespace receivers {
        
        template <typename PRECISION>
        class AntennaSelection
        {
        public:
            AntennaSelection() :
                matrices(boost::extents[17][17])
            {
                for (int length = 1; length < 17; length++)
                {
                    for (int pos = 0; pos < length; pos++)
                    {
                        imtaphy::detail::ComplexFloatMatrixPtr precoding(new imtaphy::detail::ComplexFloatMatrix(length, 1));
                        
                        for (int i = 0; i < length; i++)
                        {
                            if (i == pos)
                            {
                                (*precoding)[pos][0] = std::complex<PRECISION>(1.0, 0.0);
                            }
                            else
                            {
                                (*precoding)[i][0] = std::complex<PRECISION>(0.0, 0.0);
                            }
                        }
                        
                        matrices[length][pos] = precoding;
                    }
                }
            }
            
            boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > > getPrecodingVector(unsigned int numAntennas, unsigned int chooseAntenna) 
            {
                assure((numAntennas > 0) && (numAntennas < 17), "Only 1..16 antennas supported");
                assure(chooseAntenna < numAntennas, "Non-existing antenna chosen, counting starts with 0");
                
                return matrices[numAntennas][chooseAntenna];
            }
        private:
            boost::multi_array<boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >, 2> matrices;
        };

        
        typedef struct CC {
            
            CC (unsigned int pmi_, unsigned int column_) :
                pmi(pmi_),
                column(column_)
            {}
            
            bool operator<(const CC& other) const
            {
                return (pmi < other.pmi ||
                        ((pmi == other.pmi) && (column < other.column)));
            }
            
            unsigned int pmi;
            unsigned int column;
        } CodebookColumn;
        
        typedef std::set<CodebookColumn> CodebookColumnSet;
        
        /**
         * Hard-coded LTE Downlink Codebook according to 3GPP TS 36.211 Version 10.0
         * section 6.3.4.2.3
         * 
         * Currently only for transmissions on antenna ports {0, 1} and {0, 1, 2, 3}
         * 
         * run the LteRel8CodebookTest spike to get all matrices displayed:
         * openwns-sdk/tests/unit/unitTests$ ./openwns -tvT N7imtaphy9receivers5tests19LteRel8CodebookTestE
         * 
         * */
        
        template <typename PRECISION>
        class LteRel8Codebook 
        {
            static const unsigned int NoPrecodingMaxS = 8;
            
            typedef boost::multi_array<boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >, 2> CodeBookType;
            
        public:
            LteRel8Codebook() :
                twoTxCodebook(boost::extents[2][4]),  // 4 pmis and 2 layers
                fourTxCodebook(boost::extents[4][16]), // 16 pmis and 4 layers
                noPrecoding(boost::extents[NoPrecodingMaxS][NoPrecodingMaxS]),
                antennaSelection()
            {
                // init no precoding "codebook". This stores S-by-m identity matrices (with #Tx antennas S >= #layers m)
                // that are scaled by 1 / sqrt(m)
                // note that the matrix contains exactly m non-zero entries so that the total power is unchanged
                for (unsigned int s = 1; s <= NoPrecodingMaxS; s++)
                {
                    for (unsigned int m = 1; m <= NoPrecodingMaxS; m++)
                    {
                        noPrecoding[s-1][m-1] = boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >(new imtaphy::detail::MKLMatrix<std::complex<PRECISION> >(s, m));
                        for (unsigned int i = 0; i < s; i++)
                        {
                            for (unsigned int j = 0; j < m; j++)
                            {
                                if (i == j)
                                    (*noPrecoding[s-1][m-1])[i][j] = std::complex<PRECISION>(1.0 / sqrt(static_cast<double>(m)), 0.0);
                                else
                                    (*noPrecoding[s-1][m-1])[i][j] = std::complex<PRECISION>(0.0, 0.0);
                            }
                        }
                    }
                }
                
                
                // init 2x2 codebook
                for (int layers = 0; layers < 2; layers++)
                    for (int pmi = 0; pmi < 4; pmi++)
                    {
                        // the precoding matrices are S-by-m matrices with S being the number of Tx antennas and m being the number of layers
                        twoTxCodebook[layers][pmi] = boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >(new imtaphy::detail::MKLMatrix<std::complex<PRECISION> >(2, layers + 1));
                    }
                    
                // See TS 36.211 (Version 10.0.0), Table 6.3.4.2.3-1: Codebook for transmission on antenna ports {0,1}
                //             #m pmi  S  m
                (*twoTxCodebook[0][0])[0][0] = std::complex<PRECISION>( 0.707106781186547, 0); // 1/sqrt(2) = 0.70...
                (*twoTxCodebook[0][0])[1][0] = std::complex<PRECISION>( 0.707106781186547, 0);
                
                (*twoTxCodebook[0][1])[0][0] = std::complex<PRECISION>( 0.707106781186547, 0);
                (*twoTxCodebook[0][1])[1][0] = std::complex<PRECISION>( -0.707106781186547, 0);
                
                (*twoTxCodebook[0][2])[0][0] = std::complex<PRECISION>( 0.707106781186547, 0);
                (*twoTxCodebook[0][2])[1][0] = std::complex<PRECISION>( 0, 0.707106781186547);
                
                (*twoTxCodebook[0][3])[0][0] = std::complex<PRECISION>( 0.707106781186547, 0);
                (*twoTxCodebook[0][3])[1][0] = std::complex<PRECISION>( 0, -0.707106781186547);
                
                
                // 36.211 defines a 2x2 precoding matrix for PMI 0 but this is only used in 
				// open-loop operation
                (*twoTxCodebook[1][1])[0][0] = 0.5;
                (*twoTxCodebook[1][1])[0][1] = 0.5;
                (*twoTxCodebook[1][1])[1][0] = 0.5;
                (*twoTxCodebook[1][1])[1][1] = -0.5;
                


                (*twoTxCodebook[1][2])[0][0] = std::complex<PRECISION>( 0.5, 0);
                (*twoTxCodebook[1][2])[0][1] = std::complex<PRECISION>( 0.5, 0);
                (*twoTxCodebook[1][2])[1][0] = std::complex<PRECISION>( 0, 0.5);
                (*twoTxCodebook[1][2])[1][1] = std::complex<PRECISION>( 0, -0.5);
                
                
                for (int layers = 0; layers < 4; layers++)
                    for (int pmi = 0; pmi < 16; pmi++)
                    {
                        fourTxCodebook[layers][pmi] = boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >(new imtaphy::detail::MKLMatrix<std::complex<PRECISION> >(4, layers + 1));
                    }

                // See TS 36.211 (Version 10.0.0), Table 6.3.4.2.3-2: Codebook for transmission on antenna ports {0,1,2,3}

                // 4 Tx antennas, 2 layers precoding matrices -> 4 rows, 1 column
                //              #m pmi  S  m
                (*fourTxCodebook[0][0])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][0])[1][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][0])[2][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][0])[3][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);


                (*fourTxCodebook[0][1])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][1])[1][0] = std::complex<PRECISION>( 0.000000000000000, 0.500000000000000);
                (*fourTxCodebook[0][1])[2][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][1])[3][0] = std::complex<PRECISION>( 0.000000000000000, -0.500000000000000);


                (*fourTxCodebook[0][2])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][2])[1][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][2])[2][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][2])[3][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);


                (*fourTxCodebook[0][3])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][3])[1][0] = std::complex<PRECISION>( 0.000000000000000, -0.500000000000000);
                (*fourTxCodebook[0][3])[2][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][3])[3][0] = std::complex<PRECISION>( 0.000000000000000, 0.500000000000000);


                (*fourTxCodebook[0][4])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][4])[1][0] = std::complex<PRECISION>( 0.353553390593274, 0.353553390593274);
                (*fourTxCodebook[0][4])[2][0] = std::complex<PRECISION>( 0.000000000000000, 0.500000000000000);
                (*fourTxCodebook[0][4])[3][0] = std::complex<PRECISION>( -0.353553390593274, 0.353553390593274);


                (*fourTxCodebook[0][5])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][5])[1][0] = std::complex<PRECISION>( -0.353553390593274, 0.353553390593274);
                (*fourTxCodebook[0][5])[2][0] = std::complex<PRECISION>( 0.000000000000000, -0.500000000000000);
                (*fourTxCodebook[0][5])[3][0] = std::complex<PRECISION>( 0.353553390593274, 0.353553390593274);


                (*fourTxCodebook[0][6])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][6])[1][0] = std::complex<PRECISION>( -0.353553390593274, -0.353553390593274);
                (*fourTxCodebook[0][6])[2][0] = std::complex<PRECISION>( 0.000000000000000, 0.500000000000000);
                (*fourTxCodebook[0][6])[3][0] = std::complex<PRECISION>( 0.353553390593274, -0.353553390593274);


                (*fourTxCodebook[0][7])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][7])[1][0] = std::complex<PRECISION>( 0.353553390593274, -0.353553390593274);
                (*fourTxCodebook[0][7])[2][0] = std::complex<PRECISION>( 0.000000000000000, -0.500000000000000);
                (*fourTxCodebook[0][7])[3][0] = std::complex<PRECISION>( -0.353553390593274, -0.353553390593274);


                (*fourTxCodebook[0][8])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][8])[1][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][8])[2][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][8])[3][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);


                (*fourTxCodebook[0][9])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][9])[1][0] = std::complex<PRECISION>( 0.000000000000000, 0.500000000000000);
                (*fourTxCodebook[0][9])[2][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][9])[3][0] = std::complex<PRECISION>( 0.000000000000000, 0.500000000000000);


                (*fourTxCodebook[0][10])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][10])[1][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][10])[2][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][10])[3][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);


                (*fourTxCodebook[0][11])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][11])[1][0] = std::complex<PRECISION>( 0.000000000000000, -0.500000000000000);
                (*fourTxCodebook[0][11])[2][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][11])[3][0] = std::complex<PRECISION>( 0.000000000000000, -0.500000000000000);


                (*fourTxCodebook[0][12])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][12])[1][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][12])[2][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][12])[3][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);


                (*fourTxCodebook[0][13])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][13])[1][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][13])[2][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][13])[3][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);


                (*fourTxCodebook[0][14])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][14])[1][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][14])[2][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][14])[3][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);


                (*fourTxCodebook[0][15])[0][0] = std::complex<PRECISION>( 0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][15])[1][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][15])[2][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);
                (*fourTxCodebook[0][15])[3][0] = std::complex<PRECISION>( -0.500000000000000, 0.000000000000000);

                // 4 Tx antennas, 2 layers precoding matrices -> 4 rows, 2 columns

                (*fourTxCodebook[1][0])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][0])[0][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][0])[1][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][0])[1][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][0])[2][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][0])[2][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][0])[3][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][0])[3][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);


                (*fourTxCodebook[1][1])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][1])[0][1] = std::complex<PRECISION>( 0.000000000000000, -0.353553390593274);
                (*fourTxCodebook[1][1])[1][0] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);
                (*fourTxCodebook[1][1])[1][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][1])[2][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][1])[2][1] = std::complex<PRECISION>( 0.000000000000000, -0.353553390593274);
                (*fourTxCodebook[1][1])[3][0] = std::complex<PRECISION>( 0.000000000000000, -0.353553390593274);
                (*fourTxCodebook[1][1])[3][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);


                (*fourTxCodebook[1][2])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][2])[0][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][2])[1][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][2])[1][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][2])[2][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][2])[2][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][2])[3][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][2])[3][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);


                (*fourTxCodebook[1][3])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][3])[0][1] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);
                (*fourTxCodebook[1][3])[1][0] = std::complex<PRECISION>( 0.000000000000000, -0.353553390593274);
                (*fourTxCodebook[1][3])[1][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][3])[2][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][3])[2][1] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);
                (*fourTxCodebook[1][3])[3][0] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);
                (*fourTxCodebook[1][3])[3][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);


                (*fourTxCodebook[1][4])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][4])[0][1] = std::complex<PRECISION>( -0.250000000000000, -0.250000000000000);
                (*fourTxCodebook[1][4])[1][0] = std::complex<PRECISION>( 0.250000000000000, 0.250000000000000);
                (*fourTxCodebook[1][4])[1][1] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);
                (*fourTxCodebook[1][4])[2][0] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);
                (*fourTxCodebook[1][4])[2][1] = std::complex<PRECISION>( -0.250000000000000, 0.250000000000000);
                (*fourTxCodebook[1][4])[3][0] = std::complex<PRECISION>( -0.250000000000000, 0.250000000000000);
                (*fourTxCodebook[1][4])[3][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);


                (*fourTxCodebook[1][5])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][5])[0][1] = std::complex<PRECISION>( 0.250000000000000, -0.250000000000000);
                (*fourTxCodebook[1][5])[1][0] = std::complex<PRECISION>( -0.250000000000000, 0.250000000000000);
                (*fourTxCodebook[1][5])[1][1] = std::complex<PRECISION>( 0.000000000000000, -0.353553390593274);
                (*fourTxCodebook[1][5])[2][0] = std::complex<PRECISION>( 0.000000000000000, -0.353553390593274);
                (*fourTxCodebook[1][5])[2][1] = std::complex<PRECISION>( 0.250000000000000, 0.250000000000000);
                (*fourTxCodebook[1][5])[3][0] = std::complex<PRECISION>( 0.250000000000000, 0.250000000000000);
                (*fourTxCodebook[1][5])[3][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);


                (*fourTxCodebook[1][6])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][6])[0][1] = std::complex<PRECISION>( 0.000000000000000, -0.353553390593274);
                (*fourTxCodebook[1][6])[1][0] = std::complex<PRECISION>( -0.250000000000000, -0.250000000000000);
                (*fourTxCodebook[1][6])[1][1] = std::complex<PRECISION>( 0.250000000000000, -0.250000000000000);
                (*fourTxCodebook[1][6])[2][0] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);
                (*fourTxCodebook[1][6])[2][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][6])[3][0] = std::complex<PRECISION>( 0.250000000000000, -0.250000000000000);
                (*fourTxCodebook[1][6])[3][1] = std::complex<PRECISION>( 0.250000000000000, 0.250000000000000);


                (*fourTxCodebook[1][7])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][7])[0][1] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);
                (*fourTxCodebook[1][7])[1][0] = std::complex<PRECISION>( 0.250000000000000, -0.250000000000000);
                (*fourTxCodebook[1][7])[1][1] = std::complex<PRECISION>( -0.250000000000000, -0.250000000000000);
                (*fourTxCodebook[1][7])[2][0] = std::complex<PRECISION>( 0.000000000000000, -0.353553390593274);
                (*fourTxCodebook[1][7])[2][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][7])[3][0] = std::complex<PRECISION>( -0.250000000000000, -0.250000000000000);
                (*fourTxCodebook[1][7])[3][1] = std::complex<PRECISION>( -0.250000000000000, 0.250000000000000);


                (*fourTxCodebook[1][8])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][8])[0][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][8])[1][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][8])[1][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][8])[2][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][8])[2][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][8])[3][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][8])[3][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);


                (*fourTxCodebook[1][9])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][9])[0][1] = std::complex<PRECISION>( 0.000000000000000, -0.353553390593274);
                (*fourTxCodebook[1][9])[1][0] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);
                (*fourTxCodebook[1][9])[1][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][9])[2][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][9])[2][1] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);
                (*fourTxCodebook[1][9])[3][0] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);
                (*fourTxCodebook[1][9])[3][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);


                (*fourTxCodebook[1][10])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][10])[0][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][10])[1][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][10])[1][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][10])[2][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][10])[2][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][10])[3][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][10])[3][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);


                (*fourTxCodebook[1][11])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][11])[0][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][11])[1][0] = std::complex<PRECISION>( 0.000000000000000, -0.353553390593274);
                (*fourTxCodebook[1][11])[1][1] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);
                (*fourTxCodebook[1][11])[2][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][11])[2][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][11])[3][0] = std::complex<PRECISION>( 0.000000000000000, -0.353553390593274);
                (*fourTxCodebook[1][11])[3][1] = std::complex<PRECISION>( 0.000000000000000, 0.353553390593274);


                (*fourTxCodebook[1][12])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][12])[0][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][12])[1][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][12])[1][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][12])[2][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][12])[2][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][12])[3][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][12])[3][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);


                (*fourTxCodebook[1][13])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][13])[0][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][13])[1][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][13])[1][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][13])[2][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][13])[2][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][13])[3][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][13])[3][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);


                (*fourTxCodebook[1][14])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][14])[0][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][14])[1][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][14])[1][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][14])[2][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][14])[2][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][14])[3][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][14])[3][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);


                (*fourTxCodebook[1][15])[0][0] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][15])[0][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][15])[1][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][15])[1][1] = std::complex<PRECISION>( 0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][15])[2][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][15])[2][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][15])[3][0] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);
                (*fourTxCodebook[1][15])[3][1] = std::complex<PRECISION>( -0.353553390593274, 0.000000000000000);

                // 4 Tx antennas, 3 layers precoding matrices -> 4 rows, 3 columns

                (*fourTxCodebook[2][0])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][0])[0][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][0])[0][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][0])[1][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][0])[1][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][0])[1][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][0])[2][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][0])[2][1] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][0])[2][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][0])[3][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][0])[3][1] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][0])[3][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
				      

                (*fourTxCodebook[2][1])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][1])[0][1] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][1])[0][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][1])[1][0] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][1])[1][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][1])[1][2] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][1])[2][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][1])[2][1] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][1])[2][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][1])[3][0] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][1])[3][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][1])[3][2] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);


                (*fourTxCodebook[2][2])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][2])[0][1] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][2])[0][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][2])[1][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][2])[1][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][2])[1][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][2])[2][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][2])[2][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][2])[2][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][2])[3][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][2])[3][1] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][2])[3][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);


                (*fourTxCodebook[2][3])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][3])[0][1] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][3])[0][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][3])[1][0] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][3])[1][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][3])[1][2] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][3])[2][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][3])[2][1] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][3])[2][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][3])[3][0] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][3])[3][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][3])[3][2] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);


                (*fourTxCodebook[2][4])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][4])[0][1] = std::complex<PRECISION>( 0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][4])[0][2] = std::complex<PRECISION>( -0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][4])[1][0] = std::complex<PRECISION>( 0.204124145231932, 0.204124145231932);
                (*fourTxCodebook[2][4])[1][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][4])[1][2] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][4])[2][0] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][4])[2][1] = std::complex<PRECISION>( -0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][4])[2][2] = std::complex<PRECISION>( -0.204124145231932, 0.204124145231932);
                (*fourTxCodebook[2][4])[3][0] = std::complex<PRECISION>( -0.204124145231932, 0.204124145231932);
                (*fourTxCodebook[2][4])[3][1] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][4])[3][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);


                (*fourTxCodebook[2][5])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][5])[0][1] = std::complex<PRECISION>( -0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][5])[0][2] = std::complex<PRECISION>( 0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][5])[1][0] = std::complex<PRECISION>( -0.204124145231932, 0.204124145231932);
                (*fourTxCodebook[2][5])[1][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][5])[1][2] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][5])[2][0] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][5])[2][1] = std::complex<PRECISION>( 0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][5])[2][2] = std::complex<PRECISION>( 0.204124145231932, 0.204124145231932);
                (*fourTxCodebook[2][5])[3][0] = std::complex<PRECISION>( 0.204124145231932, 0.204124145231932);
                (*fourTxCodebook[2][5])[3][1] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][5])[3][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);


                (*fourTxCodebook[2][6])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][6])[0][1] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][6])[0][2] = std::complex<PRECISION>( 0.204124145231932, 0.204124145231932);
                (*fourTxCodebook[2][6])[1][0] = std::complex<PRECISION>( -0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][6])[1][1] = std::complex<PRECISION>( 0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][6])[1][2] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][6])[2][0] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][6])[2][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][6])[2][2] = std::complex<PRECISION>( 0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][6])[3][0] = std::complex<PRECISION>( 0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][6])[3][1] = std::complex<PRECISION>( 0.204124145231932, 0.204124145231932);
                (*fourTxCodebook[2][6])[3][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);


                (*fourTxCodebook[2][7])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][7])[0][1] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][7])[0][2] = std::complex<PRECISION>( -0.204124145231932, 0.204124145231932);
                (*fourTxCodebook[2][7])[1][0] = std::complex<PRECISION>( 0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][7])[1][1] = std::complex<PRECISION>( -0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][7])[1][2] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][7])[2][0] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][7])[2][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][7])[2][2] = std::complex<PRECISION>( -0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][7])[3][0] = std::complex<PRECISION>( -0.204124145231932, -0.204124145231932);
                (*fourTxCodebook[2][7])[3][1] = std::complex<PRECISION>( -0.204124145231932, 0.204124145231932);
                (*fourTxCodebook[2][7])[3][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);


                (*fourTxCodebook[2][8])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][8])[0][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][8])[0][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][8])[1][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][8])[1][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][8])[1][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][8])[2][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][8])[2][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][8])[2][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][8])[3][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][8])[3][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][8])[3][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);


                (*fourTxCodebook[2][9])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][9])[0][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][9])[0][2] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][9])[1][0] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][9])[1][1] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][9])[1][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][9])[2][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][9])[2][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][9])[2][2] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][9])[3][0] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][9])[3][1] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][9])[3][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);


                (*fourTxCodebook[2][10])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][10])[0][1] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][10])[0][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][10])[1][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][10])[1][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][10])[1][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][10])[2][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][10])[2][1] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][10])[2][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][10])[3][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][10])[3][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][10])[3][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);


                (*fourTxCodebook[2][11])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][11])[0][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][11])[0][2] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][11])[1][0] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][11])[1][1] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][11])[1][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][11])[2][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][11])[2][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][11])[2][2] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][11])[3][0] = std::complex<PRECISION>( 0.000000000000000, -0.288675134594813);
                (*fourTxCodebook[2][11])[3][1] = std::complex<PRECISION>( 0.000000000000000, 0.288675134594813);
                (*fourTxCodebook[2][11])[3][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);


                (*fourTxCodebook[2][12])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][12])[0][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][12])[0][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][12])[1][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][12])[1][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][12])[1][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][12])[2][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][12])[2][1] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][12])[2][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][12])[3][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][12])[3][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][12])[3][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);


                (*fourTxCodebook[2][13])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][13])[0][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][13])[0][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][13])[1][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][13])[1][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][13])[1][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][13])[2][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][13])[2][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][13])[2][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][13])[3][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][13])[3][1] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][13])[3][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);


                (*fourTxCodebook[2][14])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][14])[0][1] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][14])[0][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][14])[1][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][14])[1][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][14])[1][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][14])[2][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][14])[2][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][14])[2][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][14])[3][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][14])[3][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][14])[3][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);


                (*fourTxCodebook[2][15])[0][0] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][15])[0][1] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][15])[0][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][15])[1][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][15])[1][1] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][15])[1][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][15])[2][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][15])[2][1] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][15])[2][2] = std::complex<PRECISION>( 0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][15])[3][0] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][15])[3][1] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);
                (*fourTxCodebook[2][15])[3][2] = std::complex<PRECISION>( -0.288675134594813, 0.000000000000000);

                // 4 Tx antennas, 4 layers precoding matrices -> 4 rows, 4 columns

                (*fourTxCodebook[3][0])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[0][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[0][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[0][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[1][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[1][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[1][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[1][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[2][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[2][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[2][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[2][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[3][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[3][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[3][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][0])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][1])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][1])[0][1] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][1])[0][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][1])[0][3] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][1])[1][0] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][1])[1][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][1])[1][2] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][1])[1][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][1])[2][0] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][1])[2][1] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][1])[2][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][1])[2][3] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][1])[3][0] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][1])[3][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][1])[3][2] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][1])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][2])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[0][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[0][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[0][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[1][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[1][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[1][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[1][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[2][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[2][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[2][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[2][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[3][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[3][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[3][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][2])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][3])[0][0] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][3])[0][1] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][3])[0][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][3])[0][3] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][3])[1][0] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][3])[1][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][3])[1][2] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][3])[1][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][3])[2][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][3])[2][1] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][3])[2][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][3])[2][3] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][3])[3][0] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][3])[3][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][3])[3][2] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][3])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][4])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][4])[0][1] = std::complex<PRECISION>( 0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][4])[0][2] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][4])[0][3] = std::complex<PRECISION>( -0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][4])[1][0] = std::complex<PRECISION>( 0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][4])[1][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][4])[1][2] = std::complex<PRECISION>( -0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][4])[1][3] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][4])[2][0] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][4])[2][1] = std::complex<PRECISION>( -0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][4])[2][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][4])[2][3] = std::complex<PRECISION>( -0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][4])[3][0] = std::complex<PRECISION>( -0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][4])[3][1] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][4])[3][2] = std::complex<PRECISION>( -0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][4])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][5])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][5])[0][1] = std::complex<PRECISION>( -0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][5])[0][2] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][5])[0][3] = std::complex<PRECISION>( 0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][5])[1][0] = std::complex<PRECISION>( -0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][5])[1][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][5])[1][2] = std::complex<PRECISION>( 0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][5])[1][3] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][5])[2][0] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][5])[2][1] = std::complex<PRECISION>( 0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][5])[2][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][5])[2][3] = std::complex<PRECISION>( 0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][5])[3][0] = std::complex<PRECISION>( 0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][5])[3][1] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][5])[3][2] = std::complex<PRECISION>( 0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][5])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][6])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][6])[0][1] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][6])[0][2] = std::complex<PRECISION>( -0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][6])[0][3] = std::complex<PRECISION>( 0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][6])[1][0] = std::complex<PRECISION>( -0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][6])[1][1] = std::complex<PRECISION>( 0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][6])[1][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][6])[1][3] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][6])[2][0] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][6])[2][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][6])[2][2] = std::complex<PRECISION>( 0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][6])[2][3] = std::complex<PRECISION>( 0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][6])[3][0] = std::complex<PRECISION>( 0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][6])[3][1] = std::complex<PRECISION>( 0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][6])[3][2] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][6])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][7])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][7])[0][1] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][7])[0][2] = std::complex<PRECISION>( 0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][7])[0][3] = std::complex<PRECISION>( -0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][7])[1][0] = std::complex<PRECISION>( 0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][7])[1][1] = std::complex<PRECISION>( -0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][7])[1][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][7])[1][3] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][7])[2][0] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][7])[2][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][7])[2][2] = std::complex<PRECISION>( -0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][7])[2][3] = std::complex<PRECISION>( -0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][7])[3][0] = std::complex<PRECISION>( -0.176776695296637, -0.176776695296637);
                (*fourTxCodebook[3][7])[3][1] = std::complex<PRECISION>( -0.176776695296637, 0.176776695296637);
                (*fourTxCodebook[3][7])[3][2] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][7])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][8])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[0][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[0][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[0][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[1][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[1][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[1][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[1][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[2][0] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[2][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[2][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[2][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[3][0] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[3][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[3][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][8])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][9])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][9])[0][1] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][9])[0][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][9])[0][3] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][9])[1][0] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][9])[1][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][9])[1][2] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][9])[1][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][9])[2][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][9])[2][1] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][9])[2][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][9])[2][3] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][9])[3][0] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][9])[3][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][9])[3][2] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][9])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][10])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[0][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[0][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[0][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[1][0] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[1][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[1][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[1][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[2][0] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[2][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[2][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[2][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[3][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[3][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[3][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][10])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][11])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][11])[0][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][11])[0][2] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][11])[0][3] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][11])[1][0] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][11])[1][1] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][11])[1][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][11])[1][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][11])[2][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][11])[2][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][11])[2][2] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][11])[2][3] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][11])[3][0] = std::complex<PRECISION>( 0.000000000000000, -0.250000000000000);
                (*fourTxCodebook[3][11])[3][1] = std::complex<PRECISION>( 0.000000000000000, 0.250000000000000);
                (*fourTxCodebook[3][11])[3][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][11])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][12])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[0][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[0][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[0][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[1][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[1][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[1][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[1][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[2][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[2][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[2][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[2][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[3][0] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[3][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[3][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][12])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][13])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[0][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[0][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[0][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[1][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[1][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[1][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[1][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[2][0] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[2][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[2][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[2][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[3][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[3][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[3][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][13])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][14])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[0][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[0][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[0][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[1][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[1][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[1][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[1][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[2][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[2][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[2][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[2][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[3][0] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[3][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[3][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][14])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);


                (*fourTxCodebook[3][15])[0][0] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[0][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[0][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[0][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[1][0] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[1][1] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[1][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[1][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[2][0] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[2][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[2][2] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[2][3] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[3][0] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[3][1] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[3][2] = std::complex<PRECISION>( -0.250000000000000, 0.000000000000000);
                (*fourTxCodebook[3][15])[3][3] = std::complex<PRECISION>( 0.250000000000000, 0.000000000000000);
                
                build4x4CodebookIndex();
            }

            // TODO: make const and make it return pre-build Matrix pointers
            
            boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >
            getPrecodingMatrix(unsigned int numTxAntennas, unsigned int layers, int pmi)
            {
                assure(layers > 0, "Number of layers must be positive");

                if (pmi < 0)
                {  // negative PMIs denote SingleAntenna transmission on the abs(pmi)-th antenna
                    unsigned int antenna = -pmi;
                    assure(antenna <= numTxAntennas, "PMI suggests single antenna transmission on non-existing antenna");
                    assure(layers == 1, "Single Antenna transmission only compatible with single layer transmission");
                    
                    return antennaSelection.getPrecodingVector(numTxAntennas, antenna - 1);
                }
                else if (pmi == imtaphy::receivers::feedback::EqualPower)
                {
                    assure(numTxAntennas <= NoPrecodingMaxS, "This class supports a maximum of " << NoPrecodingMaxS << " Tx antennas without precoding");
                    assure(layers <= numTxAntennas, "Number of layers cannot be bigger than number of Tx antennas");
                    
                    return noPrecoding[numTxAntennas-1][layers-1];
                }
                else // a non-equal power distribution precoding codebook entry was requested
                {
                    assure((numTxAntennas == 2) || (numTxAntennas == 4), "Codebook only contains entries for 2 or 4 Tx antennas");
                    // check if PMI and RANK are in correct range
#ifndef  WNS_NDEBUG
                    if (numTxAntennas == 2)
                    {
                        assure((layers == 1) || (layers == 2), "Invalid rank for 2Tx Antennas (possible values: 1, 2)");
                        assure((pmi < 4), "Invalid PMI value for 2Tx Codebook (pmi >= 4)");
                        if (layers == 2)
                        {
                            assure((pmi == 1 || pmi == 2), "Invalid PMI value for rank-2, 2Tx Codebook (possible values: 1, 2)");
                        }
                    }
                    if (numTxAntennas == 4)
                    {
                        assure( (layers <= 4) && (pmi < 16), "Invalied PMI or number of layer for 4Tx Codebook");
                    }
#endif
                    if (numTxAntennas == 2)
                        return twoTxCodebook[layers - 1][pmi];
                    else
                        return fourTxCodebook[layers - 1][pmi];
                }
            }

            imtaphy::detail::ComplexFloatMatrixPtr get4TxCodebookColumn(unsigned int pmi, unsigned int column)
            {
                assure(pmi < 16, "Invalid PMI");
                assure(column < 4, "Invalid column");
                
                return individual4TxColumns[pmi][column];
            }
            
            CodebookColumnSet getCodebookColumns(unsigned int index)
            {
                return columnLookup[index];
            }
            
         

            unsigned int getIndex(unsigned int pmi, unsigned int column)
            {
                return indexLookup[pmi][column];
            }
            
            std::map<unsigned int, CodebookColumnSet> columnLookup;

        private:
           CodeBookType twoTxCodebook;
           CodeBookType fourTxCodebook;
           CodeBookType noPrecoding;    // equal power per layer and 1:1 layer to tx antenna mapping
           AntennaSelection<float> antennaSelection;
           
           unsigned int indexLookup[16][4];
           imtaphy::detail::ComplexFloatMatrixPtr individual4TxColumns[16][4];
           
           bool identicalColumns(unsigned int firstPMI, unsigned int firstColumn, unsigned int secondPMI, unsigned int secondColumn)
           {
               if (std::abs(imtaphy::detail::dotProductOfAColumnAndConjugateBColumn(*fourTxCodebook[3][firstPMI], firstColumn,
                                                                                              *fourTxCodebook[3][firstPMI], firstColumn))
                  == std::abs(imtaphy::detail::dotProductOfAColumnAndConjugateBColumn(*fourTxCodebook[3][firstPMI], firstColumn,
                                                                                              *fourTxCodebook[3][secondPMI], secondColumn)))
                   return true;
               else
               {
                   return false;
               }
           }
           
           void build4x4CodebookIndex()
           {
               unsigned int index = 0;
               for (unsigned int pmi = 0; pmi < 16; pmi++)
                   for (unsigned int column = 0; column < 4; column++)
                   {
                       individual4TxColumns[pmi][column] = imtaphy::detail::ComplexFloatMatrixPtr(new imtaphy::detail::ComplexFloatMatrix(4, 1));
                       for (unsigned int row = 0; row < 4; row++)
                       {
                            (*individual4TxColumns[pmi][column])[row][0] = (*fourTxCodebook[3][pmi])[row][column];
                       }
                       
                       bool found = false;
                       for (std::map<unsigned int, CodebookColumnSet>::iterator iter = columnLookup.begin(); iter != columnLookup.end(); iter++)
                       {
                            if (identicalColumns(pmi, column, iter->second.begin()->pmi, iter->second.begin()->column))
                            {
                                found = true;
                                indexLookup[pmi][column] = iter->first;
                                iter->second.insert(CodebookColumn(pmi, column));
                                break;
                            }
                       }
                       if (!found)
                       {
                           indexLookup[pmi][column] = index;
                           columnLookup[index].insert(CodebookColumn(pmi, column));
                           index++;
                       }
                   }
           }
        };
        
        typedef  wns::SingletonHolder<LteRel8Codebook<float> > TheLteRel8CodebookFloat;
    }}

#endif //
