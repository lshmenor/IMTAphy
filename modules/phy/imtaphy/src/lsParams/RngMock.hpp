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

#ifndef SPATIALCHANNEL_RNGMOCK_HPP
#define SPATIALCHANNEL_RNGMOCK_HPP

#include <WNS/distribution/Uniform.hpp>

#ifdef MKL
#include <mkl_vsl.h>
#include <mkl_service.h>
#endif

#include <boost/multi_array.hpp>

#include <itpp/base/random.h>

namespace imtaphy { namespace lsparams {
        
        class RandomMatrix
        {
        public:
            RandomMatrix()
            {
#ifdef MKL      
                wns::distribution::StandardUniform rnd = wns::distribution::StandardUniform();
                unsigned int mklSeed = rnd() * UINT_MAX;
                
                int errcode = vslNewStream( &stream, VSL_BRNG_SFMT19937,  mklSeed );
                assure(errcode == VSL_STATUS_OK , "VSL error");
#endif   
            }
            
            ~RandomMatrix()
            {
#ifdef MKL                
                int errcode = vslDeleteStream( &stream );
                assure(errcode == VSL_STATUS_OK , "VSL error");   
#endif                
            }
            
            virtual itpp::Mat<double> getNormalDistribution(int rows, int cols)
            {
                return itpp::randn(rows, cols);
            };

#ifdef MKL
            virtual void fillNormalDistributionWithMKL(float* ptr, int rows, int cols)
            {
                // for performance data, see here: 
                // http://software.intel.com/sites/products/documentation/hpc/mkl/vsl/vsl_performance_data.htm
                unsigned int METHOD = VSL_METHOD_SGAUSSIAN_ICDF;
                int errcode = vsRngGaussian( METHOD, stream, rows * cols, ptr, 0.0, 1.0 );
                assure(errcode == VSL_STATUS_OK , "VSL error");
           };
#endif            
        private:
#ifdef MKL
            VSLStreamStatePtr stream;
#endif        

                        
        };
        class RandomMatrixMock: public RandomMatrix
        {
        public:
            itpp::Mat<double> getNormalDistribution(int rows, int cols)
            {
                if (randomGrids.size() != 0)
                {
                    itpp::mat temp = randomGrids.front();
                    randomGrids.pop_front();
                    assure((rows==temp.rows() && cols==temp.cols()), "Invalid dimensions for randomGrids");
                    return temp;
                }
                else
                    assure(0, "randomGrids empty, cannot generate more mocked_random matrices");
                return 0;
            }
            
#ifdef MKL
            virtual void fillNormalDistributionWithMKL(float* ptr, int rows, int cols)
            {
                if (randomGrids.size() != 0)
                {
                    itpp::mat m = randomGrids.front();
                    randomGrids.pop_front();
                    assure((rows == m.rows() && cols == m.cols()), "Invalid dimensions for randomGrids");
                    
                    typedef boost::multi_array_ref<float, 2> Grid2DArray;
                    Grid2DArray grid(ptr, boost::extents[rows][cols]);
                    
                    for (int i = 0; i < m.rows(); ++i)
                        for (int j = 0; j < m.cols(); ++j) 
                            grid[i][j] = static_cast<float>(m(i, j));
                }
                else
                    assure(0, "randomGrids empty, cannot generate more mocked_random matrices");
           };
#endif      
            
                        
            void feedRandomMatrices(std::list<itpp::mat>& randlistofMats) {randomGrids=randlistofMats;}
        private:
            std::list<itpp::mat> randomGrids;
        };
    }}
#endif
