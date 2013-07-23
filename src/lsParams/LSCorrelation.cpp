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

#include <IMTAPHY/lsParams//LSCorrelation.hpp>
#include <WNS/evaluation/statistics/moments.hpp>



#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/ChannelModuleCreator.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

#include <iostream>
#include<itpp/itbase.h>
#include <itpp/signal/transforms.h>
#include <itpp/signal/freq_filt.h>

// For opt versions disable range checking in the multi_array wrapper
// WNS_NDEBUG means: assures are disabled
#ifdef  WNS_NDEBUG
#define BOOST_DISABLE_ASSERTS
#endif
#include <boost/multi_array.hpp>


#ifdef MKL
#include <mkl_service.h>
#endif

#include <algorithm>
#include <math.h>
#include <algorithm>

#ifndef __APPLE__
#include <omp.h>
#endif

using namespace imtaphy::lsparams;

// this could be indpendent from M2135 but currently it is M2135-specific

LSCorrelation::LSCorrelation(imtaphy::LinkVector _links, LinkManager* _linkManager, RandomMatrix* _rnGen):
    links(_links), 
    linkManager(_linkManager)
{
    rnGen = _rnGen;
    // find out which base stations are at the same site
    for (imtaphy::LinkVector::const_iterator iter = links.begin(); iter != links.end(); iter++)
        siteMap[(*iter)->getBS()->getPosition()].insert((*iter)->getBS());

}

LSCorrelation::~LSCorrelation()
{

}


LSmap* 
LSCorrelation::generateLSCorrelation()
{
    
//    std::cout << "\nGenerating Large Scale parameters for "<< links.size() << " links ..." << std::endl;

    lsParams = new LSmap();
    

    // Idea:
    // for each site/propCond/scenario combinations find all links and mobiles
    // the grids created are in no special order but the randomgrids loaded from matlab are
    // grid[site][prop][scenario]
    
            
    // LScorrelation has to be done separately for each scenario/propagation/site
    // combination. We loop over scenarios and then over propagation starting with LoS
    // because that is the order of values for the unittest from Matlab
            
    for (int scenarioId = 0; scenarioId <= 4; scenarioId++) // InH = 0, RMa = 4
    {
        imtaphy::Link::Scenario scenario = imtaphy::Link::Scenario(scenarioId);
        
        for (int propagationId = 2; propagationId >= 0; propagationId--)
        {
            imtaphy::Link::Propagation propagation = imtaphy::Link::Propagation(propagationId);

            
            // iterate over all base station sites
            for (StationMap::const_iterator iter = siteMap.begin(); iter != siteMap.end(); iter++)
            {
                // filter out those links to this site that have the current 
                // scenario/propagation combination
                imtaphy::LinkVector linksToCorrelateThisSite = this->getLinksWithSameScenarioPropagationThisSite(iter->second, scenario, propagation);
            
                //                 std::cout << linksToCorrelateThisSite.size() << " links for propagation "
                //                           << propagation << " and scenario type " << scenario << " go to site at position "
                //                           << iter->first << "\n";
                
                
                // if no links with this combination at this site, try next scenario/propagation
                if (linksToCorrelateThisSite.size() == 0)
                    continue;
                else
                    performCorrelation(linksToCorrelateThisSite, imtaphy::scm::m2135::FixParSingleton::Instance()(scenario, propagation));
             
            } // end of loop over multiple sites
        } // end propagation condition loop
    } // end scenarios loop

    return lsParams;
}

imtaphy::LinkVector
LSCorrelation::getLinksWithSameScenarioPropagationThisSite(StationSet colocatedBSs, Link::Scenario scenario, Link::Propagation propagation)
{
    imtaphy::LinkVector linksToCorrelateThisSite;
    linksToCorrelateThisSite.clear();
    
    // iterate over all base stations at this site
    for (StationSet::const_iterator bsIter = colocatedBSs.begin(); bsIter != colocatedBSs.end(); bsIter++)
    {
        imtaphy::LinkMap allLinksThisBS = linkManager->getAllLinksForStation(*bsIter);
        
        // only consider those links that are  scm Links and have the right scenario/propagation
        for (imtaphy::LinkMap::const_iterator linkIter = allLinksThisBS.begin();
             linkIter != allLinksThisBS.end(); linkIter++)
        {
            imtaphy::Link* link = linkIter->second;

            // treat UMI O2I separately
            if ((scenario == imtaphy::Link::UMi) && (propagation == imtaphy::Link::UMiO2I))
            {
                if ((link->getScenario() == imtaphy::Link::UMi) &&
                    (link->getUserLocation() == imtaphy::Link::Indoor))
                {
                    linksToCorrelateThisSite.push_back(link);
                }
                    
            }   
            else
            {
                if ((link->getScenario() == scenario) &&
                    (link->getPropagation() == propagation))
                {
                    linksToCorrelateThisSite.push_back(link);
                }
            }
        }
    }
    
    return linksToCorrelateThisSite;
}
void
LSCorrelation::performCorrelation(LinkVector linksToCorrelate, const imtaphy::scm::m2135::Parameters* scenarioPropagationParams)
{
// float should be just fine for performing the grid-based filtering process
// because the values are random anyway and should not be that sensitive to 
// numerical accuracy. The grids have a 1m x 1m resolution and can thus be quite big
// especially in large scenarios (e.g. Rural Macro) with wraparound.
// The output values will always be double precision.

    // we have 5 rows, one for each LS parameter, and as many columns as links to correlate
    itpp::mat ksi;
    ksi.set_size(5, linksToCorrelate.size());
    ksi.zeros();
    
    if (linksToCorrelate.size() == 1)
        ksi = rnGen->getNormalDistribution(5,1);
    else
    { // more than one link, so we actually have to do correlation
        
        // init correlation distances for Large Scale parameters
        std::vector<double> delta;
        delta.resize(5);
        delta[0] = scenarioPropagationParams->CD_DS; 
        delta[1] = scenarioPropagationParams->CD_ASD; 
        delta[2] = scenarioPropagationParams->CD_ASA; 
        delta[3] = scenarioPropagationParams->CD_SF; 
        delta[4] = scenarioPropagationParams->CD_K;
        
        // first let's find the corners of the grid
        std::vector<double> xCoords, yCoords;
        
        xCoords.resize(linksToCorrelate.size());
        yCoords.resize(linksToCorrelate.size());
        
        for (unsigned int j = 0; j < linksToCorrelate.size(); j++)
        {
            xCoords[j] = linksToCorrelate[j]->getWrappedMSposition().getX();
            yCoords[j] = linksToCorrelate[j]->getWrappedMSposition().getY();
        }
        
        double Xmax = *std::max_element(xCoords.begin(), xCoords.end());
        double Xmin = *std::min_element(xCoords.begin(), xCoords.end());
        double Ymax = *std::max_element(yCoords.begin(), yCoords.end());
        double Ymin = *std::min_element(yCoords.begin(), yCoords.end());
        
        double D = 100; // for adding some extra samples in grid
      
#ifdef MKL
        unsigned int cols = Xmax-Xmin+2*D+1;
        unsigned int rows = Ymax-Ymin+2*D+1;

        float* gridMem = NULL;
        float* tempMem = NULL;
        gridMem = static_cast<float*>(mkl_malloc(sizeof(float) * cols * rows , 16));
        tempMem = static_cast<float*>(mkl_malloc(sizeof(float) * cols * rows , 16));
        
        assure(gridMem, "Could not get memory for grid");
        assure(tempMem, "Could not get memory for temporary grid");

        typedef boost::multi_array_ref<float, 2> Grid2DArray;
        Grid2DArray grid(gridMem, boost::extents[rows][cols]);  
        Grid2DArray temp(tempMem, boost::extents[rows][cols]);
        
        unsigned int FilterLength = 101;
        float* filter = static_cast<float*>(mkl_malloc(sizeof(float) * FilterLength , 16));

#endif        
        // iterate over all 5 large scale parameters
        for (int i = 0; i < 5; i++) 
        {
            // each LS paramter gets its own grid

#ifdef MKL
            rnGen->fillNormalDistributionWithMKL(gridMem, rows, cols);
            
            // prepare the filter vector:
            double sum = 0.0;
            for (unsigned int n = 0; n < FilterLength; n++)
            {
                filter[n] = exp(-1.0 * double(n) / delta[i]);
                sum += filter[n];
            }
            for (unsigned int n = 0; n < FilterLength; n++)
                filter[n] = filter[n] / sum;

            // First, we setup a convolution task in MKL, then we filter each row and column.
            // As the filter is always the same, we can use the same task for each column and row
            // of a grid. This saves internal computation time. Different sites might have different
            // grid sizes so we have to create a new task for each grid. The filters per LS parameter 
            // i are also different.
            
            int status;
            VSLConvTaskPtr task;
            // create the row task. Here, we want to feed one row after the other and the convolution operation
            // is performed on all the entries (columns) of that row. So the size of input2 and output is cols.
            status = vslsConvNewTaskX1D(&task, // to store the task ptr - single (float) precision
                                        VSL_CONV_MODE_AUTO, 
                                        //                                          VSL_CONV_MODE_AUTO, // mode
                                        FilterLength, // xshape: the length of the filter used for all subsequent calls
                                        cols,  // yshape: the length of the second input
                                        cols, // zshape: the lenght of the output (=input)
                                        filter, // the pointer to the input x
                                        1); // the stride for the input x
            
            assure(status == VSL_STATUS_OK, "MKL Convolution gave an error message");
            
            int y; // signed int for OpenMP parallel for loop
#pragma omp parallel for            
            for (y = 0; y < static_cast<int>(rows); y++)
            {
                status = vslsConvExecX1D(task,
                                         &(grid[y][0]), // pointer where row starts in input y
                                         1, // ystride, skip over the columns to the next x
                                         &(temp[y][0]), // pointer to output z
                                         1); // zstride
                
                                
                assure(status == VSL_STATUS_OK, "MKL Convolution gave an error message");
            }
            
            // Free this task, the number of columns might be different
            
            status = vslConvDeleteTask(&task);
            assure(status == VSL_STATUS_OK, "MKL Convolution gave an error message");
            
            // create the column task
            status = vslsConvNewTaskX1D(&task, // to store the task ptr - single (float) precision
                                        VSL_CONV_MODE_AUTO, 
                                        //                                          VSL_CONV_MODE_AUTO, // mode
                                        FilterLength, // xshape: the length of the filter used for all subsequent calls
                                        rows,  // yshape: the length of the second input
                                        rows, // zshape: the lenght of the output (=input)
                                        filter, // the pointer to the input x
                                        1); // the stride for the input x
            assure(status == VSL_STATUS_OK, "MKL Convolution gave an error message");
            
            // now filter column-wise, use the row-filtered matrix temp as input and store back to the grid
            int x; // signed int for OpenMP parallel for loop
#pragma omp parallel for            
            for (x = 0; x < static_cast<int>(cols); x++)
            {
                status = vslsConvExecX1D(task,
                                         &(temp[0][x]), // pointer to the start of row (colum_major!)
                                         cols, // stride (no stride due to column_major)
                                         &(grid[0][x]), // same for the result matrix grid
                                         cols); // zstride: for the grid
                                        
                assure(status == VSL_STATUS_OK, "MKL Convolution gave an error message");
            }
            
            // Free this task, the next one  might be different
            status = vslConvDeleteTask(&task);
            assure(status == VSL_STATUS_OK, "MKL Convolution gave an error message");
            
            // compute the std. dev. could also be done using MKL VSL functions available in MKL v.10.3
            sum = 0.0;
            double sq_sum = 0.0;

            float* p = gridMem;
            for (unsigned int ii = 0; ii < cols*rows; ii++, p++) 
            {
                sum += *p;
                sq_sum += *p * *p;
            }

            double stdDev =sqrt((sq_sum - sum*sum / (cols*rows)) / double((cols*rows) - 1));
            
            // we don't want to divide by zeros
            if( stdDev == 0 )
                stdDev =1.0;
                
            // finally set the correlated random entry for each considered link
            // according to the position of the mobile on the grid
            // for the current LS parameter i
            for(unsigned int j =0 ; j < linksToCorrelate.size(); j++)
            {                
                ksi(i, j) = 
                    grid[linksToCorrelate[j]->getWrappedMSposition().getY() - Ymin + D]
                        [linksToCorrelate[j]->getWrappedMSposition().getX() - Xmin + D] / stdDev;

            }
#else  // ifdef MKL
            // No MKL, just use itpp functionality
            itpp::mat grid = rnGen->getNormalDistribution((Ymax-Ymin+2*D+1), (Xmax-Xmin+2*D+1));
            
            // prepare the filter vector:
            itpp::vec d = "0:1:100";
            itpp::vec filter = itpp::exp((-d)/delta[i]);
            filter = filter / itpp::sum(filter);

            itpp::mat tmpGrid;
            tmpGrid.set_size(grid.rows(), grid.cols());

            // column-wise filtering :
            for(int j = 0; j < grid.cols(); j++)
            {
                
                itpp::Freq_Filt<double> FF(filter,grid.get_col(0).length());
                // Filter the data
                tmpGrid.set_col( j, FF.filter(grid.get_col(j), 0) );
            }
            
            // row-wise filtering
            for(int j = 0; j < tmpGrid.rows(); j++)
            {
                itpp::Freq_Filt<double> FF2(filter,tmpGrid.get_row(0).length());
                // Filter the data
                grid.set_row( j, FF2.filter(tmpGrid.get_row(j), 0) );
            }

            // find the std. dev. and divide each entry of the gride by the std. dev.
            wns::evaluation::statistics::Moments mnts;
            mnts.reset();
            
            for (int k=0; k<grid.rows(); k++)
                for(int m=0; m<grid.cols(); m++)
                    mnts.put(grid.get(k,m));
                
            double stdDev = sqrt(mnts.variance());

            // we don't want to divide by zeros
            if( stdDev == 0 )
                stdDev =1.0;
                
            // finally set the correlated random entry for each considered link
            // according to the position of the mobile on the grid
            // for the current LS parameter i
            for(int j =0 ; j < linksToCorrelate.size(); j++)
            {                
                ksi(i, j) = 
                    grid(linksToCorrelate[j]->getWrappedMSposition().getY() - Ymin + D, 
                             linksToCorrelate[j]->getWrappedMSposition().getX() - Xmin + D) / stdDev;
            }
#endif // non-MKL version            
        } // enf o loop over all LS parameters i
        
#ifdef MKL
    mkl_free(filter);
    mkl_free(gridMem);
    mkl_free(tempMem);
#endif
    } // end of handling the case of multiple links

    // now introduce cross-correlation by multiplying with the corresponding R_sqrt matrix:
    itpp::mat crossCorrelationRsqrt = scenarioPropagationParams->R_sqrt;
    assure(crossCorrelationRsqrt.cols() == ksi.rows(), "matrices don't match");

    // cross-correlation: matrix multiplication
    ksi = crossCorrelationRsqrt * ksi;

    // Now we can finally compute and save the large scale parameters
    // The order in which we loop over the entries is the same as above
    
    for(unsigned int j =0 ; j < linksToCorrelate.size(); j++)
    {
        imtaphy::Link* link = linksToCorrelate[j];
        
        (*lsParams)[link] = LargeScaleParameters();
        
        // delay spread Large Scale parameter
        (*lsParams)[link].setDelaySpread(itpp::pow10( (scenarioPropagationParams->DS_sigma) * ksi(0, j) +
                                                      scenarioPropagationParams->DS_mu));

        // angular spread (AoD) Large Scale parameter
        (*lsParams)[link].setAngularSpreadDeparture(itpp::pow10( (scenarioPropagationParams->ASD_sigma) * ksi(1, j) + 
                                                                 scenarioPropagationParams->ASD_mu ));
    
        // angular spread (AoA) Large Scale parameter
        (*lsParams)[link].setAngularSpreadArrival(itpp::pow10( (scenarioPropagationParams->ASA_sigma) * ksi(2, j) + 
                                                               scenarioPropagationParams->ASA_mu ));
    
        // shadow fading large scale parameter
        (*lsParams)[link].setShadowFading(itpp::pow10( 0.1 * (scenarioPropagationParams->SF_sigma) * ksi(3, j)));
                                                            
        // Ricean K factor large scale parameter
        (*lsParams)[link].setRiceanK(itpp::pow10( 0.1 * ((scenarioPropagationParams->KF_sigma) * ksi(4, j) + 
                                                         scenarioPropagationParams->KF_mu)));                                                 

    }    
}

