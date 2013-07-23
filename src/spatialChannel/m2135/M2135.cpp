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

#include <IMTAPHY/spatialChannel/m2135/M2135.hpp>

#include <WNS/evaluation/statistics/moments.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/ChannelModuleCreator.hpp>
#include <IMTAPHY/linkManagement/LinkManager.hpp>

#include <IMTAPHY/spatialChannel/m2135/ClusterPowers.hpp>
#include <IMTAPHY/spatialChannel/m2135/RayAngles.hpp>
#include <IMTAPHY/spatialChannel/m2135/Delays.hpp>


#include <iostream>
#include <itpp/signal/filter.h>
#include <algorithm>

#include <math.h>
#include <vector>

#ifndef __APPLE__
#include <omp.h>
#endif



using namespace imtaphy::scm::m2135;

// Note that the implementation of the templated class M2135 is possible inside
// this C++ file because we explicitely instantiate the M2135<float> and M2135<double>
// specializations in the StaticFactory macros below:
STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::scm::m2135::M2135<double>,
    imtaphy::scm::SpatialChannelModelInterface<double>,
    "imtaphy.SCM.M2135",
    imtaphy::ChannelModuleCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    imtaphy::scm::m2135::M2135<float>,
    imtaphy::scm::SpatialChannelModelInterface<float>,
    "imtaphy.SCM.M2135SinglePrecision",
    imtaphy::ChannelModuleCreator);

namespace imtaphy { namespace scm { namespace m2135 {
            // Template specialization for MKL's CIS function: 
            // Computes complex exponent of real vector elements 
            // (cosine and sine of real vector elements combined to complex value)
            // http://software.intel.com/sites/products/documentation/hpc/mkl/vml/functions/CIS_c.html
            template <> void MKLCis<float>::execute(const int n, 
                                                    const float* a, 
                                                    std::complex<float>* y 
                ) 
            { 
                vmlClearErrStatus();

                // low accuracy  / enhanced performance versions of VML functions
                vmlSetMode( VML_EP );
                vcCIS(n, a, y);  // c stands for MKL_COMPLEX8 / std::complex<float>
        
                assure(vmlGetErrStatus() == 0, "MKL CIS gave an error. vmlGetErrStatus()=" << vmlGetErrStatus());
            } 
            template <> void MKLCis<double>::execute(const int n, 
                                                     const double* a, 
                                                     std::complex<double>* y 
                )  
            { 
                vmlClearErrStatus();
                
                // high accuracy
                vmlSetMode( VML_HA );
                vzCIS(n, a, y); // z stands for MKL_COMPLEX16 / std::complex<double>
        
                assure(vmlGetErrStatus() == 0, "MKL CIS gave an error. vmlGetErrStatus()=" << vmlGetErrStatus());

            }
            // explicit instantiation
            template class MKLCis<float>;
            template class MKLCis<double>;
    
    
            // same thing for cblas_?scal functions:
            template <> void MKLscal<float>::execute(const int N, const float alpha, float *X, const int incX)
            {
                cblas_sscal(N, alpha, X, incX);
            }
            template <> void MKLscal<double>::execute(const int N, const double alpha, double *X, const int incX)
            {
                cblas_dscal(N, alpha, X, incX);
            }
            // explicit instatiation
            template class MKLscal<float>;
            template class MKLscal<double>;

            // ?gemm matrix-matrix multiplication routines
            template <> void MKLgemm<float>::execute(const CBLAS_ORDER Order, const CBLAS_TRANSPOSE TransA, 
                                                     const CBLAS_TRANSPOSE TransB, const int M, const int N, 
                                                     const int K, const void *alpha, const void *A, const int lda, 
                                                     const void *B, const int ldb, const void *beta, void *C, const int ldc)
            {   // cblas_cgemm works on MKL_COMPLEX8, i.e., std::complex<float> values
                cblas_cgemm(Order, TransA, TransB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc);
            }
            // ?gemm matrix-matrix multiplication routines
            template <> void MKLgemm<double>::execute(const CBLAS_ORDER Order, const CBLAS_TRANSPOSE TransA, 
                                                      const  CBLAS_TRANSPOSE TransB, const int M, const int N, 
                                                      const int K, const void *alpha, const void *A, const int lda, 
                                                      const void *B, const int ldb, const void *beta, void *C, const int ldc)
            {   // cblas_zgemm works on MKL_COMPLEX16, i.e., std::complex<double> values
                cblas_zgemm(Order, TransA, TransB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc);
            }
            template class MKLgemm<float>;
            template class MKLgemm<double>;
    
            template <> void MKLdotu<float>::execute(const int N, const void *X, const int incX, 
                                                     const void *Y, const int incY, void *dotu)
            {
                // cblas_cdotu_sub works on MKL_COMPLEX8, i.e., std::complex<float> values
                cblas_cdotu_sub(N, X, incX, Y, incY, dotu);
            }
            template <> void MKLdotu<double>::execute(const int N, const void *X, const int incX, 
                                                      const void *Y, const int incY, void *dotu)
            {
                // cblas_zdotu_sub works on MKL_COMPLEX16, i.e., std::complex<double> values
                cblas_zdotu_sub(N, X, incX, Y, incY, dotu);
            }
            template class MKLdotu<float>;
            template class MKLdotu<double>;
        }}}


template <typename PRECISION>    
M2135<PRECISION>::M2135(imtaphy::Channel* _channel, wns::pyconfig::View config) :
    SpatialChannelModelInterface<PRECISION>(_channel, config),
    channel(_channel),
    tVariantCoeff(2),   // these vectors store ptrs to the UL/DL multi-dim arrays -> length 2
    tInvariantFactor(2),
    frequencyCoeff(2),
    H(2),
    T(2),
    angles(NULL),
    logger(config.get("logger")),
    dumpCalibrationData(config.get<bool>("dumpCalibrationData")),
    calibrationOutputFileName(config.get<std::string>("calibrationOutputFileName")),
    directionsEnabled(2),
    computeEffectiveAntennaGains(config.get<bool>("computeEffectiveAntennaGains")),
    allSpeedsZero(true)
{
}

template <typename PRECISION>    
M2135<PRECISION>::~M2135()
{
    // release dynamically allocated memory according to how it was allocated (mkl_malloc or new)
    // we don't need to check for null ptrs before deleting according to the c++ FAQ
    mkl_free(tVariantCoeff[0]);
    mkl_free(tVariantCoeff[1]);

    mkl_free(tInvariantFactor[0]);
    mkl_free(tInvariantFactor[1]);

    mkl_free(frequencyCoeff[0]);
    mkl_free(frequencyCoeff[1]);
   
    mkl_free(H[0]);
    mkl_free(H[1]);
    
    mkl_free(T[0]);
    mkl_free(T[1]);
}

template <typename PRECISION> 
void
M2135<PRECISION>::onWorldCreated(imtaphy::LinkManager* linkManager, imtaphy::lsparams::LSmap* lsParams, bool keepIntermediates)
{
    // Do the initialization and pre-computation of all relevant spatial
    // channel model properties here

    // Step 1: Set environment, network layout, and antenna array parameters
    // some of these things the Channel knows, some can be retrieved from the
    // StationPhy (some mechanisms have to be implemented still)

    // Step 2: Assign propagation condition (LoS/NLoS)
    // Channel has a model for that that should be used for the specified link

    // Step 3: Calculate path loss
    // Not implemented here, is done in Channel

    // Step 4: Generate correlated large scale parameters, i.e. delay spread, angular spreads, 
    // Ricean K factor and shadow fading term like explained in § 3.3.1 
    // this is done externally by the Large Scale Correlation module and the results for all
    // links are passed in lsParams.
    
    if (channel->getSpectrum()->getNumberOfPRBs(imtaphy::Downlink) > 0)
    {
        directionsEnabled[Downlink] = true;
    }
    else
    {
        directionsEnabled[Downlink] = false;
    }

    if (channel->getSpectrum()->getNumberOfPRBs(imtaphy::Uplink) > 0)
    {
        directionsEnabled[Uplink] = true;
    }
    else
    {
        directionsEnabled[Uplink] = false;
    }
    
    // only get the links flagged as "SCM" for use in the spatial channel model
    scmLinks = linkManager->getSCMLinks();
    unsigned int K = scmLinks.size();
        
    // TODO: change the rest of the code to work on LSMap
    // for compatibility with legacy code, we have to build a itpp::mat sigmas 
    // matrix that contains the LS params in 5 cols and scmLinks.size() rows
    
    itpp::mat* sigmas = new itpp::mat(K, 5);
    
    for (unsigned int k = 0; k < K; k++)
    {
        assure(lsParams->find(scmLinks[k]) != lsParams->end(), "SCM link without large scale paramters");
        
        sigmas->set(k, int(DSpread), (*lsParams)[scmLinks[k]].getDelaySpread());
        sigmas->set(k, int(ASDeparture), (*lsParams)[scmLinks[k]].getAngularSpreadDeparture());
        sigmas->set(k, int(ASArrival), (*lsParams)[scmLinks[k]].getAngularSpreadArrival());
        sigmas->set(k, int(SFading), (*lsParams)[scmLinks[k]].getShadowFading());
        sigmas->set(k, int(RiceanK), (*lsParams)[scmLinks[k]].getRicanK());

        // There is no need to evolve the channel if all MS speeds are set to zero
        if(scmLinks[k]->getMS()->getSpeed() != 0.0)
            allSpeedsZero = false;
    }

    // TODO: adapt to work on LSmap
    setRiceanKterms(sigmas->get_col(RiceanK));

    // Step 5: Generate delays
    MESSAGE_SINGLE(NORMAL, logger,"Generating Delays for " << scmLinks.size() << " links");
    itpp::Real_Timer timer;

    timer.reset();
    timer.tic();
        
    
    delays = new  imtaphy::scm::m2135::Delays(sigmas, scmLinks, MaxClusters);
    delays->init();
    delays->generateDelays();
        
    timer.toc();
        
    MESSAGE_SINGLE(VERBOSE, logger,"Took "<<timer.get_time()<<" seconds for generating Delays for "<<scmLinks.size()<<" links");
//    std::cout << "Took "<<timer.get_time()<<" seconds for generating Delays for "<<scmLinks.size()<<" links" << std::endl;
        
    // Step 6: Generate cluster powers, P
    MESSAGE_SINGLE(NORMAL, logger,"Generating Cluster powers for "<< scmLinks.size() <<" links");
    timer.reset(); 
    timer.tic();

    clusterPowers = new ClusterPowers(linkManager, MaxClusters);
    clusterPowers->init();
    clusterPowers->computeClusterPowers(*sigmas, delays->getDelays());
    
    timer.toc();
    MESSAGE_SINGLE(VERBOSE, logger,"Took "<<timer.get_time()<<" seconds for generating Cluster Powers for "<<scmLinks.size()<<" links");
//    std::cout << "Took "<<timer.get_time()<<" seconds for generating Cluster Powers for "<<scmLinks.size()<<" links" << std::endl;

    // Step 7: Generate arrival angles φ and departure angles φ.
    // this should be a 3D matrix [links k][clusters n][rays m]


    /*
      H_{k,u,s,n} is the channel impulse response / complex coeff. of the n-th cluster/path

      We always assume 24 clusters/paths

      n=1..10 is always be used
      n=11..20 is be used for scenarios that have so many clusters
      n=21..24 are the additional subclusters

      In the H and delay vectors, there would be "(0, 0)" for non-used clusters/paths
      This also means that the delay vector would not be sorted, at least partially.


    */

    MESSAGE_SINGLE(NORMAL, logger,"Generating AoA and AoDs " << scmLinks.size() <<" links");
    timer.reset();
    timer.tic();
        
    angles = new imtaphy::scm::m2135::RayAngles(scmLinks, MaxClusters, NumRays, channel);
    angles->generateAoAandAoDs(*(clusterPowers->getScaledClusterPowers()), sigmas);
    
    timer.toc();
    MESSAGE_SINGLE(VERBOSE, logger,"Took "<<timer.get_time()<<" seconds for generating AoA and AoDs for "<<scmLinks.size()<<" links\n");
//    std::cout << "Took "<<timer.get_time()<<" seconds for generating AoA and AoDs for "<<scmLinks.size()<<" links\n" << std::endl;
    RayAngles3DArray* aoas = angles->getAoAs();
    RayAngles3DArray* aods = angles->getAoDs();

        
    if (dumpCalibrationData)
        dumpSmallScaleCalibration(*aoas, *aods, sigmas, delays->getScaledDelays(), clusterPowers->getClusterPowers(), clusterPowers->getScaledClusterPowers());
        
    // Step 8: Random coupling of rays within clusters

    clusterPowers->determineTwoStrongestClusters();
    strongestIndices = *(clusterPowers->getStrongestIndices());
    secondStrongestIndices = *(clusterPowers->getSecondStrongestIndices());
    
    assure(clusterPowers->getStrongestIndices()->size() == clusterPowers->getSecondStrongestIndices()->size(), "length mismatch");

    for (unsigned int k = 0; k < clusterPowers->getStrongestIndices()->size(); k++)
        assure(clusterPowers->getStrongestIndices()->at(k) != clusterPowers->getSecondStrongestIndices()->at(k), 
               "cluster can't be both strongest and second-strongest");

    //addPowersforSubClusters(clusterPowers);
    clusterPowers->addPowersforSubClusters();
    
    delays->addSubclustersToDelays(*(clusterPowers->getStrongestIndices()), *(clusterPowers->getSecondStrongestIndices()));

    MESSAGE_SINGLE(NORMAL, logger,"Creating sub-clusters and permuting AoDs "<<scmLinks.size()<<" links");
    
    timer.reset();
    timer.tic();
    angles->createSubclustersAndPermutateAngles(*(clusterPowers->getStrongestIndices()), *(clusterPowers->getSecondStrongestIndices()), scmLinks);
    timer.toc();

    MESSAGE_SINGLE(VERBOSE, logger,"Took "<<timer.get_time()<<" seconds for creating sub-clusters and permuting AoDs for "<<scmLinks.size()<<" links");
//    std::cout << "Took "<<timer.get_time()<<" seconds for creating sub-clusters and permuting AoDs for "<<scmLinks.size()<<" links" << std::endl;
    
    
    // Step 9: Draw initial phases
    // They are independently chosen for UL and DL according to Winner II D1.1.2 V1.2
    // (section 5.4.3 "FDD modeling"). One could argue that there is a systematic shift 
    // related to the DL/UL frequencies though.
    std::vector<Phases*> initialPhases(2, static_cast<Phases*>(NULL));
    if (directionsEnabled[Downlink])
    {
        initialPhases[Downlink] = new Phases(scmLinks.size(), MaxClusters, NumRays);
        initialPhases[Downlink]->initPhasesUniformlyRandomly(-itpp::pi, itpp::pi);
    }
    if (directionsEnabled[Uplink])
    {
        initialPhases[Uplink] = new Phases(scmLinks.size(), MaxClusters, NumRays);
        initialPhases[Uplink]->initPhasesUniformlyRandomly(-itpp::pi, itpp::pi);
    }
            
 
    // Step 9.5: initialize cross-polarization power ratios, K x N X M matrix
    itpp::vec* XPRs = generateXPRs(scmLinks);

    maxBsAntennas = 0;
    maxMsAntennas = 0;
    setMaxAntennas();
    

    // before we can compute the t-invariant things, we have to allocate memory and setup the wrappers:
    sizeMultiDimArrays();

    MESSAGE_SINGLE(NORMAL, logger,"Computing time-invariant coefficients for "<<scmLinks.size()<<" links");
    timer.reset();
    timer.tic();

    computeTimeInvariantCoefficients(*aoas, *aods, 
                                     *(clusterPowers->getStrongestIndices()), *(clusterPowers->getSecondStrongestIndices()),
                                     scmLinks, initialPhases, delays->getScaledDelays(), 
                                     clusterPowers->getClusterPowers(), XPRs);

    timer.toc();
    MESSAGE_SINGLE(VERBOSE, logger,"Took "<<timer.get_time()<<" seconds for computing time-invariant coefficients for "<<scmLinks.size()<<" links");

    if (!keepIntermediates)
    {
        // if we don't need them (e.g. for testing), we can throw all intermediate variables 
        // away to free up memory for main simulation
        delete angles;
        delete delays;
        delete clusterPowers;
    
        // Clean-up of all temporary objects:
        delete XPRs;
        delete sigmas;
        
        delete initialPhases[Downlink];
        delete initialPhases[Uplink];
    }
}




template <typename PRECISION>
itpp::vec*
M2135<PRECISION>::generateXPRs(imtaphy::LinkVector links)
{
    itpp::vec* xprsVector = new itpp::vec(links.size() * MaxClusters * NumRays);
    typedef boost::multi_array_ref<double, 3> Double3DArray;
    Double3DArray xprs(xprsVector->_data(), boost::extents[links.size()][MaxClusters][NumRays]);

    // According to table 4.5 in D1.1.2 V1.2 page 57
    double sigma, mu;
    for(unsigned int k = 0; k < scmLinks.size(); k++)
    {
        sigma = FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->XPR_sigma;
        mu = FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->XPR_mu;

        itpp::mat randKappa = itpp::pow10((((itpp::randn(MaxClusters, NumRays)) * sqrt(sigma)) + mu) / 10.0);
        for (int n = 0; n < MaxClusters; n++)
            for (int m = 0; m < NumRays; m++)
                xprs[k][n][m] = randKappa(n,m);

    }
    return xprsVector;
}

template <typename PRECISION>
void
M2135<PRECISION>::sizeMultiDimArrays()
{
    // These are all the arrays that we want to keep for evolving the CIR/CTF in time
    
    // Allocate memory aligned on 32-bit boundaries to assure identical results in 
    // case of parallelization (see MKL user guide)
    
    if (directionsEnabled[Downlink])
    {
        tVariantCoeff[Downlink] = new boost::multi_array_ref<PRECISION, 5>(static_cast<PRECISION*>(mkl_malloc(sizeof(PRECISION) * scmLinks.size() * maxMsAntennas * maxBsAntennas * MaxClusters * (NumRays + 1), 32)),
                                                                           boost::extents[scmLinks.size()][maxMsAntennas][maxBsAntennas][MaxClusters][NumRays + 1]);
                                                                           
        tInvariantFactor[Downlink] = new boost::multi_array_ref<std::complex<PRECISION>, 5>(static_cast<std::complex<PRECISION>*>(mkl_malloc(sizeof(std::complex<PRECISION>) * scmLinks.size() * maxMsAntennas * maxBsAntennas * MaxClusters * (NumRays + 1), 32)),
                                                                                            boost::extents[scmLinks.size()][maxMsAntennas][maxBsAntennas][MaxClusters][NumRays + 1]);
                                                                                            
        frequencyCoeff[Downlink] = new boost::multi_array_ref<std::complex<PRECISION>, 3>(static_cast<std::complex<PRECISION>*>(mkl_malloc(sizeof(std::complex<PRECISION>) * scmLinks.size() * channel->getSpectrum()->getNumberOfPRBs(imtaphy::Downlink) * MaxClusters, 32)),
                                                                                          boost::extents[scmLinks.size()][channel->getSpectrum()->getNumberOfPRBs(imtaphy::Downlink)][MaxClusters]);
                                                                                          
        H[Downlink] = new boost::multi_array_ref<std::complex<PRECISION>, 4>(static_cast<std::complex<PRECISION>*>(mkl_malloc(sizeof(std::complex<PRECISION>) * scmLinks.size() * maxMsAntennas * maxBsAntennas * MaxClusters , 32)), 
                                                                             boost::extents[scmLinks.size()][maxMsAntennas][maxBsAntennas][MaxClusters]);
                                                                             
        T[Downlink] = new boost::multi_array_ref<std::complex<PRECISION>, 4>(static_cast<std::complex<PRECISION>*>(mkl_malloc(sizeof(std::complex<PRECISION>) * scmLinks.size() *  channel->getSpectrum()->getNumberOfPRBs(imtaphy::Downlink) * maxMsAntennas * maxBsAntennas , 32)), 
                                                                             boost::extents[scmLinks.size()][channel->getSpectrum()->getNumberOfPRBs(imtaphy::Downlink)][maxMsAntennas][maxBsAntennas]);
    }
    
    // channel matrices for Uplink are transposed compared to Donwlink -> exchange Ms and Bs indices:
    if (directionsEnabled[Uplink])
    {
        tVariantCoeff[Uplink] = new boost::multi_array_ref<PRECISION, 5>(static_cast<PRECISION*>(mkl_malloc(sizeof(PRECISION) * scmLinks.size() * maxMsAntennas * maxBsAntennas * MaxClusters * (NumRays + 1), 32)),
                                                                         boost::extents[scmLinks.size()][maxMsAntennas][maxBsAntennas][MaxClusters][NumRays + 1]);
        tInvariantFactor[Uplink] = new boost::multi_array_ref<std::complex<PRECISION>, 5>(static_cast<std::complex<PRECISION>*>(mkl_malloc(sizeof(std::complex<PRECISION>) * scmLinks.size() * maxMsAntennas * maxBsAntennas * MaxClusters * (NumRays + 1), 32)),
                                                                                          boost::extents[scmLinks.size()][maxMsAntennas][maxBsAntennas][MaxClusters][NumRays + 1]);

        frequencyCoeff[Uplink] = new boost::multi_array_ref<std::complex<PRECISION>, 3>(static_cast<std::complex<PRECISION>*>(mkl_malloc(sizeof(std::complex<PRECISION>) * scmLinks.size() * channel->getSpectrum()->getNumberOfPRBs(imtaphy::Uplink) * MaxClusters, 32)),
                                                                                          boost::extents[scmLinks.size()][channel->getSpectrum()->getNumberOfPRBs(imtaphy::Uplink)][MaxClusters]);
                                                                                          
        H[Uplink] = new boost::multi_array_ref<std::complex<PRECISION>, 4>(static_cast<std::complex<PRECISION>*>(mkl_malloc(sizeof(std::complex<PRECISION>) * scmLinks.size() * maxMsAntennas * maxBsAntennas * MaxClusters , 32)), 
                                                                             boost::extents[scmLinks.size()][maxBsAntennas][maxMsAntennas][MaxClusters]);

        T[Uplink] = new boost::multi_array_ref<std::complex<PRECISION>, 4>(static_cast<std::complex<PRECISION>*>(mkl_malloc(sizeof(std::complex<PRECISION>) * scmLinks.size() *  channel->getSpectrum()->getNumberOfPRBs(imtaphy::Uplink) * maxMsAntennas * maxBsAntennas , 32)), 
                                                                           boost::extents[scmLinks.size()][channel->getSpectrum()->getNumberOfPRBs(imtaphy::Uplink)][maxBsAntennas][maxMsAntennas]);
    }
}

template <typename PRECISION>
void
M2135<PRECISION>::setRiceanKterms(itpp::Vec<double> sigmaK)
{
    riceanKterm1.resize(sigmaK.size());
    riceanKterm2.resize(sigmaK.size());

    for (int k = 0; k < sigmaK.size(); k++)
    {
        double sigma = sigmaK[k];

        riceanKterm1[k] = sqrt(1.0 / (sigma + 1.0));
        riceanKterm2[k] = sqrt(sigma / (sigma + 1.0));
    }

}

template <typename PRECISION>
void
M2135<PRECISION>::computeTimeInvariantCoefficients(RayAngles3DArray& aoas, RayAngles3DArray& aods,
                                                   std::vector<int> &strongestIndices,
                                                   std::vector<int> &secondStrongestIndices, 
                                                   imtaphy::LinkVector links,
                                                   std::vector<Phases*> initialPhases,
                                                   itpp::mat* scaledDelays,
                                                   itpp::mat* clusterPowers,
                                                   itpp::vec* XPRs)

{
    Double3DArray xprs(XPRs->_data(), boost::extents[links.size()][MaxClusters][NumRays]);


    int K = links.size();
    int k; // signed int for OpenMP parallel for loop
#pragma omp parallel for
    for (k = 0; k < K ; k++)
    {
        // we get the electrical (!) field pattern of the antenna depending on the azimuth angle of the
        // ray coming from one of the clusters (LoS for LoS) and depending on the elevation angle on the
        // LoS connection from BS to MS, i.e., all rays coming at arbitrary azimuth angles from all clusters
        // get the same elevation angle assigned because the M.2135 channel model does not specify any elevations
        // for the clusters. Otherwise, we would not be able to represent the elevation pattern (and downtilt)
        // of the antenna. 
        double arrivalElevation = channel->getElevation(links[k]->getWrappedMSposition(), links[k]->getBS()->getPosition());
        double departureElevation = channel->getElevation(links[k]->getBS()->getPosition(), links[k]->getWrappedMSposition());
        
        int nClusters = FixParSingleton::Instance()(links[k]->getScenario(), links[k]->getPropagation())->NumClusters;
        
        double effectiveAntennaGains = 0.0;
        double totalPower = 0.0;
        
        
        for (unsigned int u = 0; u < maxMsAntennas; u++)
        {
            for (unsigned int s = 0; s < maxBsAntennas; s++)
            {
                for (int n = 0; n < MaxClusters; n++)
                {
                    // this ray (ray 21) is only used to store the additional LOS comonent
                    if (directionsEnabled[Downlink])
                    {
                        (*tInvariantFactor[Downlink])[k][u][s][n][NumRays] = std::complex<double>(0.0, 0.0);
                        (*tVariantCoeff[Downlink])[k][u][s][n][NumRays] = 0.0;
                    }
                    if (directionsEnabled[Uplink])
                    {
                        (*tInvariantFactor[Uplink])[k][u][s][n][NumRays] = std::complex<double>(0.0, 0.0);
                        (*tVariantCoeff[Uplink])[k][u][s][n][NumRays] = 0.0;
                    }
                    
                    // skip clusters which do not exist in the link's scenario
                    // but set all their coefficients/invariant parts to 0
                    // subclusters in positions 20..23 (clusters 21..24) always exist
                    if ((n >= nClusters) && (n < 20))
                    {
                        for (int m = 0; m < NumRays; m++)
                        {
                            if (directionsEnabled[Downlink])
                            {
                                (*tInvariantFactor[Downlink])[k][u][s][n][m] = std::complex<double>(0., 0.);
                                (*tVariantCoeff[Downlink])[k][u][s][n][m] = 0.;
                            }
                            if (directionsEnabled[Uplink])
                            {
                                (*tInvariantFactor[Uplink])[k][u][s][n][m] = std::complex<double>(0., 0.);
                                (*tVariantCoeff[Uplink])[k][u][s][n][m] = 0.;
                            }
                        }
                        continue;
                    }

                    // The powers are per cluster and are distributed equally among the 20 rays of the
                    // cluster. Here we check for the subdivided clusters which have less rays.
                    double powerScaling;
                    if ( n == strongestIndices[k] || n == secondStrongestIndices[k] )
                        powerScaling = 10.0;
                    else if ( n == 20 || n == 22)
                        powerScaling = 6.0;
                    else if ( n == 21 || n == 23)
                        powerScaling = 4.0;
                    else
                        powerScaling = 20.0;


                    for (int m = 0; m < NumRays; m++) // fill the "real" rays, LOS case handled below
                    {
                        // Also clean and skip the coefficients for the non-existing rays due to the subclusters
                        // this is achieved in the matlab code in wim_core.m at lines 342..372 by only adding the rays according to the subclusters
                        // the matlab comments state that splitting the two strongest clusters is currently only supported in the Matlab implementation
                        // and not in the C-Mex implementation (at least as far as the Wim2 version available on the ITU website is concerned)
                        if ((n == 20) || (n == 22)) // sub-cluster 2 of the two strongest paths, have to skip some rays:
                            if ( !( (m == 8) || (m == 9) || (m == 10) || (m == 11) || (m == 16) || (m == 17) ) )
                            {
                                 if (directionsEnabled[Downlink])
                                 {       
                                    (*tInvariantFactor[Downlink])[k][u][s][n][m] = std::complex<double>(0., 0.);
                                    (*tVariantCoeff[Downlink])[k][u][s][n][m] = 0.;
                                 }
                                 if (directionsEnabled[Uplink])
                                 {       
                                    (*tInvariantFactor[Uplink])[k][u][s][n][m] = std::complex<double>(0., 0.);
                                    (*tVariantCoeff[Uplink])[k][u][s][n][m] = 0.;
                                 }
                                continue;
                            }

                        if ((n == 21) || (n == 23)) // sub-cluster 3 of the two strongest paths, have to skip some:
                            if ( !( (m == 12) || (m == 13) || (m == 14) || (m == 15) ) )
                            {
                                 if (directionsEnabled[Downlink])
                                 {       
                                    (*tInvariantFactor[Downlink])[k][u][s][n][m] = std::complex<double>(0., 0.);
                                    (*tVariantCoeff[Downlink])[k][u][s][n][m] = 0.;
                                 }
                                 if (directionsEnabled[Uplink])
                                 {       
                                    (*tInvariantFactor[Uplink])[k][u][s][n][m] = std::complex<double>(0., 0.);
                                    (*tVariantCoeff[Uplink])[k][u][s][n][m] = 0.;
                                 }
                                continue;
                            }

                        if ((n == strongestIndices[k]) || (n == secondStrongestIndices[k])) // skip some rays for the sub-cluster 1 of the two strongest paths
                            if (!((m < 8) || (m > 17))) // clean positions inside the range from 8 to 17 including
                            {
                                 if (directionsEnabled[Downlink])
                                 {       
                                    (*tInvariantFactor[Downlink])[k][u][s][n][m] = std::complex<double>(0., 0.);
                                    (*tVariantCoeff[Downlink])[k][u][s][n][m] = 0.;
                                 }
                                 if (directionsEnabled[Uplink])
                                 {       
                                    (*tInvariantFactor[Uplink])[k][u][s][n][m] = std::complex<double>(0., 0.);
                                    (*tVariantCoeff[Uplink])[k][u][s][n][m] = 0.;
                                 }
                                continue;
                            }
                            
                        // we only execute the following if we did not jump back to the top of the loop with the continue statements above
                        // i.e., the clusters/rays n,m do exist

                        double aoa = aoas[k][n][m];
                        double aod = aods[k][n][m];

                        // Sender's (base station's) tx antenna element s path difference from element 0
                        // Note that this replaces d * sin(phi) in eq. (20) in M.2135 because the angle is already considered
                        double ds = links[k]->getBS()->getAntenna()->getPathDifferenceMeters(s,aod);
                        // Receiver's (mobile station's) rx antenna element u path difference from element 0
                        // Note that this replaces d * sin(phi) in eq. (20) in M.2135 because the angle is already considered
                        double du = links[k]->getMS()->getAntenna()->getPathDifferenceMeters(u,aoa);
                        
                        double XPRcoeff = sqrt(1.0/xprs[k][n][m]);
                        
                        // there have been situations where aoa/aod have been almost exactly pi so that the assure failed
                        // even though it was probably just a precision issue
                        assure((aoa>=-3.1416 && aoa<=3.1416 && aod>=-3.1416 && aod<=3.1416), "Angles should be b/w -pi tp pi");

                        // see above for comment on using fixed LoS arrival and departure elevations even for rays coming from clusters at 
                        // arbitrary azimuth angles
                        std::complex<double> fieldRxV = links[k]->getMS()->getAntenna()->getVerticalFieldPattern(aoa, arrivalElevation, u);
                        std::complex<double> fieldRxH = links[k]->getMS()->getAntenna()->getHorizontalFieldPattern(aoa,arrivalElevation, u);
                        std::complex<double> fieldTxV = links[k]->getBS()->getAntenna()->getVerticalFieldPattern(aod, departureElevation, s);
                        std::complex<double> fieldTxH = links[k]->getBS()->getAntenna()->getHorizontalFieldPattern(aod, departureElevation, s);

                        // to compute effective antenna patterns
                        if ((s==0) && (u==0))
                        {
                            effectiveAntennaGains +=   (*clusterPowers)(k,n) / powerScaling
                                                   * links[k]->getMS()->getAntenna()->getGain(aoa, arrivalElevation).get_factor()  
                                                   * links[k]->getBS()->getAntenna()->getGain(aod, departureElevation).get_factor();
                            totalPower += (*clusterPowers)(k,n) / powerScaling;
                        }
                        
                        for (unsigned int d = 0; d <= 1; d++)
                        {
                            imtaphy::Direction direction = static_cast<imtaphy::Direction>(d);
                            if (!directionsEnabled[direction])
                                continue;

                            std::complex<double> expPhiVV = exp(std::complex<double>(0., initialPhases[direction]->vv[k][n][m]));
                            std::complex<double> expPhiVH = exp(std::complex<double>(0., initialPhases[direction]->vh[k][n][m]));
                            std::complex<double> expPhiHV = exp(std::complex<double>(0., initialPhases[direction]->hv[k][n][m]));
                            std::complex<double> expPhiHH = exp(std::complex<double>(0., initialPhases[direction]->hh[k][n][m]));

                            double lambda = channel->getSpectrum()->getSystemCenterFrequencyWavelenghtMeters(direction);
                            
                            std::complex<double> expTxArray = exp(std::complex<double>(0., ds * (2.0 * itpp::pi / lambda)));
                            std::complex<double> expRxArray = exp(std::complex<double>(0., du * (2.0 * itpp::pi / lambda)));
                            
                            // This is the time invariant left part of the summation from equation (20) in M.2135
                            (*tInvariantFactor[direction])[k][u][s][n][m] = sqrt((*clusterPowers)(k,n) / powerScaling) *
                                                                        ( fieldRxV * (fieldTxV * expPhiVV + XPRcoeff * fieldTxH * expPhiVH)  +
                                                                          fieldRxH * (XPRcoeff * fieldTxV * expPhiHV + fieldTxH * expPhiHH) ) *
                                                                        expTxArray * expRxArray;
                            // equation (21) in M.2135
                            double v = links[k]->getMS()->getSpeed() * cos(aoa - links[k]->getMS()->getDirectionOfTravel()) / lambda;

                            // this will be multiplied with the current time t when evolving:
                            (*tVariantCoeff[direction])[k][u][s][n][m] = 2.0 * itpp::pi * v;
                        }
                    }
                }

                // now also the LOS part which is not dependent on clusters n or rays m:
                if (links[k]->getPropagation() == imtaphy::Link::LoS)
                {
                    for (unsigned int d = 0; d <= 1; d++)
                    {
                        imtaphy::Direction direction = static_cast<imtaphy::Direction>(d);
                        if (!directionsEnabled[direction])
                            continue;
                    
                        // scale down all paths/rays according to Ricean k
                        for (int n = 0; n < MaxClusters; n++)
                            for (int m = 0; m < NumRays; m++)
                                (*tInvariantFactor[direction])[k][u][s][n][m] *= riceanKterm1[k];
                    }
                    
                    // azimuth from mobile (station2) looking towards the base station (station1)
                    double aoaLOS = channel->getAzimuth(links[k]->getWrappedMSposition(), links[k]->getBS()->getPosition());
                    // azimuth from base station (station1) looking towards the mobile station (station2)
                    double aodLOS = channel->getAzimuth(links[k]->getBS()->getPosition(), links[k]->getWrappedMSposition());

                    // Sender's (base station's) tx antenna element s path difference from element 0
                    // Note that this replaces d * sin(phi) in eq. (20) in M.2135 because the angle is already considered
                    double ds = links[k]->getBS()->getAntenna()->getPathDifferenceMeters(s,aodLOS);
                    // Receiver's (mobile station's) rx antenna element u path difference  from element 0
                    // Note that this replaces d * sin(phi) in eq. (20) in M.2135 because the angle is already considered
                    double du = links[k]->getMS()->getAntenna()->getPathDifferenceMeters(u,aoaLOS);

                    // see above for comment on using fixed LoS arrival and departure elevations even for rays coming from clusters at 
                    // arbitrary azimuth angles
                    std::complex<double> fieldRxVLOS = links[k]->getMS()->getAntenna()->getVerticalFieldPattern(aoaLOS, arrivalElevation, u);
                    std::complex<double> fieldRxHLOS = links[k]->getMS()->getAntenna()->getHorizontalFieldPattern(aoaLOS, arrivalElevation, u);
                    std::complex<double> fieldTxVLOS = links[k]->getBS()->getAntenna()->getVerticalFieldPattern(aodLOS, departureElevation, s);
                    std::complex<double> fieldTxHLOS = links[k]->getBS()->getAntenna()->getHorizontalFieldPattern(aodLOS, departureElevation, s);

                    if ((s==0) && (u==0))
                    {
                        effectiveAntennaGains *= pow(riceanKterm1[k], 2.0);
                        effectiveAntennaGains += pow(riceanKterm2[k], 2.0) 
                                                * links[k]->getBS()->getAntenna()->getGain(aodLOS, departureElevation).get_factor() 
                                                * links[k]->getMS()->getAntenna()->getGain(aoaLOS, arrivalElevation).get_factor();
                        totalPower = totalPower*pow(riceanKterm1[k], 2.0) + pow(riceanKterm2[k], 2.0);
                    }
                    
                    for (unsigned int d = 0; d <= 1; d++)
                    {
                        imtaphy::Direction direction = static_cast<imtaphy::Direction>(d);
                        if (!directionsEnabled[direction])
                            continue;
                        
                        std::complex<double> expPhiVVLOS = exp(std::complex<double>(0., initialPhases[direction]->LOSvv[k]));
                        std::complex<double> expPhiHHLOS = exp(std::complex<double>(0., initialPhases[direction]->LOShh[k]));

                        
                        double lambda = channel->getSpectrum()->getSystemCenterFrequencyWavelenghtMeters(direction);
                    
                        std::complex<double> expTxArrayLOS = exp(std::complex<double>(0., ds * (2.0 * itpp::pi / lambda)));
                        std::complex<double> expRxArrayLOS = exp(std::complex<double>(0., du * (2.0 * itpp::pi / lambda)));

                        // add a LOS component to the first cluster (n=0) and use the 21st "pseudo" ray (NumRays) for storing it
                        // the multiplier already contains the ricean K scaling
                        (*tInvariantFactor[direction])[k][u][s][0][NumRays] = riceanKterm2[k] * (fieldRxVLOS * fieldTxVLOS * expPhiVVLOS  + fieldRxHLOS * fieldTxHLOS * expPhiHHLOS) *
                            expTxArrayLOS * expRxArrayLOS;

                        // equation (21) in M.2135
                        double vLOS = links[k]->getMS()->getSpeed() * cos(aoaLOS - links[k]->getMS()->getDirectionOfTravel()) / lambda;

                        // this will be multiplied with the current time t when evolving:
                        (*tVariantCoeff[direction])[k][u][s][0][NumRays] = 2.0 * itpp::pi * vLOS;
                    }
                }
            }
        }
        if (computeEffectiveAntennaGains)
        {
            // tell the link to update its wideband loss value to reflect the effective antenna gains:
            // the multipath propagation between MS and BS means that the gains from the antenna patterns 
            // depend on the arrival and departure angles of each reflection cluster
            scmLinks[k]->updateEffectiveAntennaGains(wns::Ratio::from_factor(effectiveAntennaGains / totalPower));
        }
    }
    
    itpp::Real_Timer timer;
    timer.tic();

#pragma omp parallel for
    for (k = 0; k < K ; k++)
    {
        for (unsigned int d = 0; d <= 1; d++)
        {
            imtaphy::Direction direction = static_cast<imtaphy::Direction>(d);
            if (!directionsEnabled[direction])
                continue;
            
            for (unsigned int f = 0; f < channel->getSpectrum()->getNumberOfPRBs(direction); f++)
            {
                for (int n = 0; n < MaxClusters; n++)
                {
                    // Set the delay to zero if it is a NAN
                    // Delay will be NAN for the non existing clusters/paths and should be taken as yero
                    // for computing the transform and the channel tap will be zero for this cluster/path
                    double tau;
                    if (std::isnan((*scaledDelays)(k,n)))
                        tau = 0.0;
                    else
                        tau = (*scaledDelays)(k,n);

                    (*frequencyCoeff[direction])[k][f][n] = exp(std::complex<double>(0.0,
                                                                          -2.0 * itpp::pi * tau * 
                                                                          channel->getSpectrum()->getPRBcenterFrequencyHz(direction, f)));
      
                    (*frequencyCoeff[direction])[k][f][n] *= sqrt((scmLinks[k]->getShadowing() - scmLinks[k]->getPathloss()).get_factor());
                }
            }
        }                
    } // end parallel for
    timer.toc();
//    std::cout << "frequency Coeff computation takes " << timer.get_time() << " seconds\n";
}


template <typename PRECISION>
bool
M2135<PRECISION>::checkZeroRays(std::complex<double> c, int cluster, int ray, int link) const
{
    //std::cout << "Running module ------->  checkZeroRays" << std::endl;
    // if (cluster < 20) // must be the strongest ones
    if ( cluster == strongestIndices[link] || cluster == secondStrongestIndices[link])
        if ((ray > 7) && (ray < 18)) // these should be zeros
            if (c != std::complex<double>(0., 0.))
            {
                assure(0, "problem with strongest clusters");
            }

    if ((cluster == 20) || (cluster == 22)) // sub-cluster 2
    {
        if ( !((ray == 8) || (ray == 9) || (ray == 10) || (ray == 11) || (ray == 16) || (ray == 17))) //all others should be zero
            if (c != std::complex<double>(0., 0.))
            {
                assure(0, "problem with subcluster2");
            }
    }

    if ((cluster == 21) || (cluster == 23)) // sub-cluster 3
    {
        if ( !((ray == 12) || (ray == 13) || (ray == 14) || (ray == 15))) //all others should be zero
            if (c != std::complex<double>(0., 0.))
            {
                assure(0, "problem with subcluster3");
            }
    }

    return true;
}

template <typename PRECISION>
void
M2135<PRECISION>::evolve(double t)
{
    // Nothing will happen if all speeds are zero except for t = 0.0
    if(allSpeedsZero && t > 0.0)
        return;

    itpp::Real_Timer timer;
    timer.tic();

    // TODO: choose suitable value
    const unsigned int chunkSize = 1000;
    unsigned int thisChunkSize;

    unsigned int K = scmLinks.size();
    unsigned int elementsPerLink = maxMsAntennas * maxBsAntennas * MaxClusters * (NumRays + 1);

    PRECISION* scaled = static_cast<PRECISION*>(mkl_malloc(sizeof(PRECISION) *
                                                           chunkSize * elementsPerLink, 32));
                                    
    // we need a vector to store the intermediate step (tVariantVec)
    std::complex<PRECISION>* tVariantVec = static_cast<std::complex<PRECISION>* >(mkl_malloc(sizeof(std::complex<PRECISION>) * chunkSize * elementsPerLink, 32));
    
    for (unsigned int d = 0; d <= 1; d++)
    {
        imtaphy::Direction direction = static_cast<imtaphy::Direction>(d);
        if (!directionsEnabled[direction])
            continue;
    
        // Here, we only want to do the minimum number of computations that are required when time t changes
        // In order not to use too much temporary memory, we operate on large chunks of links at once.

        for (unsigned int chunk = 0; chunk < (K / chunkSize) + 1; chunk++)
        {
            // determine how many links and elements are in this chunk
            if (chunk == (K / chunkSize))
                thisChunkSize = K % chunkSize;
            else
                thisChunkSize = chunkSize;

            // in case that the number of links is a multiple of chunkSize we would get an emptyChunk
            // so we skip the "last" iteration:
            if (thisChunkSize == 0)
                break;
            
            // copy the unscaled tVariantCoefficients into "scaled" to later scale them
            // using memcpy should be  safe because it is a plain-old-data type (array of floats/doubles)
            // with current time t; this is the exponent
            memcpy(scaled, tVariantCoeff[direction]->data() + chunk * chunkSize * elementsPerLink,
                   sizeof(PRECISION) * thisChunkSize * elementsPerLink);
                
            assure(scaled[0] == tVariantCoeff[direction]->data()[chunk * chunkSize * elementsPerLink], "mid not working correctly");
            assure(scaled[thisChunkSize * elementsPerLink - 1] ==
                   tVariantCoeff[direction]->data()[chunk * chunkSize * elementsPerLink + thisChunkSize * elementsPerLink - 1],
                   "mid not working correctly");

            // here we scale the exponent (scaled from tVariantCoeffVector) with the current time t and compute the complex exponential:
            // exp(0 + j*tVariant*t)
            
            MKLscal<PRECISION> mklscal;
            mklscal.execute(thisChunkSize * elementsPerLink, t, scaled, 1);
            
            MKLCis<PRECISION> mklcis;
            mklcis.execute(thisChunkSize * elementsPerLink, scaled, tVariantVec);
            
            // to easily acces the result
            boost::multi_array_ref<std::complex<PRECISION>, 5> result(tVariantVec, boost::extents[thisChunkSize][maxMsAntennas][maxBsAntennas][MaxClusters][NumRays+1]);

            MKLdotu<PRECISION> mkldotu;
            
            int kOffset; // signed int for OpenMP parallel for loop
#pragma omp parallel for
            for (kOffset = 0; kOffset < static_cast<int>(thisChunkSize); kOffset++)
            {
                // compute the absolute k
                unsigned int k = chunk * chunkSize + kOffset;
                int nClusters = FixParSingleton::Instance()(scmLinks[k]->getScenario(), scmLinks[k]->getPropagation())->NumClusters;

                if (direction == imtaphy::Downlink)
                {                
                    for (unsigned int u = 0; u < maxMsAntennas; u++)
                    {
                        for (unsigned int s = 0; s < maxBsAntennas; s++)
                        {
                            for (int n = 0; n < MaxClusters; n++)
                            {
                                (*H[imtaphy::Downlink])[k][u][s][n] = std::complex<PRECISION>(0.0, 0.0);

                                // only compute coefficients for clusters that exist in this link's scenario
                                if ((n < nClusters) || (n >= 20))
                                    mkldotu.execute(NumRays+1, // number of elements in vectors
                                                    &(result[kOffset][u][s][n][0]), // pointer to beginning of vector x
                                                    1, // incx
                                                    &((*tInvariantFactor[imtaphy::Downlink])[k][u][s][n][0]), // pointer to beginning of vector y
                                                    1, // incy
                                                    &((*H[imtaphy::Downlink])[k][u][s][n]));
                                
                            }   // LoS case is handled by looping over NumRays+1
                        }
                    }
                }
                else // uplink -> we flip the order of u and s for the H matrix
                {                
                    for (unsigned int s = 0; s < maxBsAntennas; s++)
                    {
                        for (unsigned int u = 0; u < maxMsAntennas; u++)
                        {
                            for (int n = 0; n < MaxClusters; n++)
                            {
                                (*H[imtaphy::Uplink])[k][s][u][n] = std::complex<PRECISION>(0.0, 0.0);

                                // only compute coefficients for clusters that exist in this link's scenario
                                if ((n < nClusters) || (n >= 20))
                                    mkldotu.execute(NumRays+1, // number of elements in vectors
                                                    &(result[kOffset][u][s][n][0]), // pointer to beginning of vector x
                                                    1, // incx
                                                    &((*tInvariantFactor[imtaphy::Uplink])[k][u][s][n][0]), // pointer to beginning of vector y
                                                    1, // incy
                                                    &((*H[imtaphy::Uplink])[k][s][u][n]));
                                
                            }   // LoS case is handled by looping over NumRays+1
                        }
                    }
                }
                    
            } // end of parallel for loop
        } // end of the loop over all chunks
    }
    
    timer.toc();
    MESSAGE_SINGLE(VERBOSE, logger, "Took " << timer.get_time() << "seconds for evolving CIR at t=" << t);
//    std::cout << "Took " << timer.get_time() << "seconds for evolving CIR at t=" << t << "\n";

    mkl_free(scaled);
    mkl_free(tVariantVec);
    
    // now, compute frequency response:
    transformChannel();
}

template <typename PRECISION>
void
M2135<PRECISION>::transformChannel()
{
    itpp::Real_Timer timer;
    timer.reset();
    timer.tic();
    
    MKLgemm<PRECISION> mklgemm;

    for (unsigned int d = 0; d <= 1; d++)
    {
        imtaphy::Direction direction = static_cast<imtaphy::Direction>(d);
        if (!directionsEnabled[direction])
            continue;
    
        MESSAGE_SINGLE(NORMAL, logger, "Now transforming channel on " << scmLinks.size() << " links and " << channel->getSpectrum()->getNumberOfPRBs(direction) << " frequency channels");

        std::complex<PRECISION> one = std::complex<PRECISION>(1.0, 0.0);
        std::complex<PRECISION> zero = std::complex<PRECISION>(0.0, 0.0);
        
        int k; // signed int for OpenMP parallel for loop
#pragma omp parallel for
        for (k = 0; k < static_cast<int>(scmLinks.size()); k++)
        {
            // compute the Fourier transformation by doing a matrix multiplication of the
            // channel impulse response matrix H (matrix A) and the precomputed frequrencyCoefficient 
            // matrix (matrix B): C = 1A*B + 0*C
            // A is the k=k submatrix of H (rows contain antenna pairs, cols contain clusters)
            // B is the k=k submatrix of the freqCoeff matrix (rows contain clusters and cols contain frequency bins (PRBs))
            // C is the sub-matrix T[k][0][0][0] that contains #antenna pairs rows and #PRBs columns
            // Note that the U and S dimensions are treated as if they are one dimension of antenna pairs. The chosen
            // storage order allows us to do that.
            
            // We want T to hold links X frequency bins X rx antennas U X tx antennas S so that a U-by-S channel
            // matrix for a given link and frequency bin would be stored adjacent in memory.
            // Because T is stored in row-major-mode, we need u*s to be the number of columns and k*PRBs the 
            // number of rows. So for this matrix multiplication we have the result T as a (k*PRBs)X(u*s) matrix
            // in memory.
            // H is stored k X u X s X n and frequencyCoeff is stored as k X f X n
            // Because in each loop iteration we fix k to k=1..K, we work with submatrices
            // H is then u X s X n and frequencyCoeff is f X n
            // To get (for a fixed k) T f X u X s, we multiply 
            // frequency coeff f X n with H^T n X (u*s)
            // So we have C = A * B with
            // A is frequency coeff as an f X n matrix
            // B is H^T as n X (u*s) matrix
            // The resulting C will then be f X (u X s) so M==f and N==u*s with the shared dimension K==MaxClusters
            
            mklgemm.execute(CblasRowMajor, // const enum CBLAS_ORDER Order, matrix storage mode
                            CblasNoTrans, // CBLAS_TRANSPOSE TransA: (no) transpose mode for matrix A
                            CblasTrans,  // transpose for B: should be transpose because it is multiplied via "clusters"
                            channel->getSpectrum()->getNumberOfPRBs(direction), // M 
                            maxMsAntennas * maxBsAntennas, // N: 
                            MaxClusters, // K: number of columns to multiply over: Number of Clusters
                            &one, // alpha = (1,0), i.e. no scaling 
                            &((*frequencyCoeff[direction])[k][0][0]), //*A pointer to matrix A
                            MaxClusters, // lda: size of leading dimension of A: noTrans and row major-> number of columns of A, thus MaxClusters
                            &((*H[direction])[k][0][0][0]), // *B pointer to matrix B
                            MaxClusters, // ldb: size of leading dimension of B: Transpose and row major-> number of rows of B, thus MaxClusters
                            &zero, // *beta 0 because C should not be added and does not need to be zeroed then
                            &((*T[direction])[k][0][0][0]), // *C pointer to matrix C
                            maxMsAntennas * maxBsAntennas // ldc: size of leading dimension of C: NoTrans and row major-> number of cols of C, thus u*s
                ); 
        } // end of parallel for
    } // uplink / downlink
    
    timer.toc();
    MESSAGE_SINGLE(VERBOSE, logger, "transformChannel via Matrix Multiplication takes " << timer.get_time() << " seconds");
//    std::cout << "transformChannel via Matrix Multiplication takes " << timer.get_time() << " seconds\n";    

//#else // we don't have MKL
//    timer.tic();
//
//    // initialize the vector that will finally hold the frequency response to (0.0, 0.0)
//    TVector.set_size(scmLinks.size() * maxMsAntennas * maxBsAntennas * channel->getSpectrum()->getNumberOfPRBs());
//    TVector.zeros();
//    
//#pragma omp parallel for
//    for (int k = 0; k < scmLinks.size(); k++)
//        for (unsigned int f = 0; f < channel->getSpectrum()->getNumberOfPRBs(); f++)
//            for (unsigned int u = 0; u < maxMsAntennas; u++)
//                for (unsigned int s = 0; s < maxBsAntennas; s++)
//                    for (unsigned int n = 0; n < MaxClusters; n++)
//                        (*T)[k][f][u][s] += (*H)[k][u][s][n] * (*frequencyCoeff)[k][f][n];
//    
//    timer.toc();
//    std::cout << "transformChannel loop-based takes " << timer.get_time() << " seconds\n";
//
//#endif
}
                



template <typename PRECISION>
void
M2135<PRECISION>::setMaxAntennas()
{
    imtaphy::StationList bsList = channel->getAllBaseStations();
    imtaphy::StationList msList = channel->getAllMobileStations();
    for (imtaphy::StationList::const_iterator bsIter=bsList.begin(); bsIter!=bsList.end() ; bsIter++)
    {
        if ((*bsIter)->getAntenna()->getNumberOfElements() > maxBsAntennas)
            maxBsAntennas = (*bsIter)->getAntenna()->getNumberOfElements();
    }
    for (imtaphy::StationList::const_iterator msIter=msList.begin(); msIter!=msList.end() ; msIter++)
    {
        if ((*msIter)->getAntenna()->getNumberOfElements() > maxMsAntennas)
            maxMsAntennas = (*msIter)->getAntenna()->getNumberOfElements();
    }
}

template <typename PRECISION>
imtaphy::scm::ChannelLayout 
M2135<PRECISION>::getChannelLayout()
{
    ChannelLayout layout;
    
    layout.F[imtaphy::Uplink] = channel->getSpectrum()->getNumberOfPRBs(imtaphy::Uplink);
    layout.F[imtaphy::Downlink] = channel->getSpectrum()->getNumberOfPRBs(imtaphy::Downlink);
    layout.K = scmLinks.size();
    layout.N = MaxClusters;
    layout.U = maxMsAntennas;
    layout.S = maxBsAntennas;
    
    return layout;
}

template <typename PRECISION>
void 
M2135<PRECISION>::dumpSmallScaleCalibration(RayAngles3DArray& aoas, RayAngles3DArray& aods, itpp::mat* sigmas, 
                                            itpp::mat* scaledDelays, itpp::mat* clusterPowers, itpp::mat* scaledClusterPowers)
{
    // Make sure to make all links LoS or NLoS in the configuration for calibration
    // We are exporting the calibration data before splitting into sub-clusters

    itpp:: it_file ff;
    ff.open(calibrationOutputFileName);

    // Export the delays
    ff << itpp::Name("taus")   << *scaledDelays;

    // Export the cluster powers
    // Note that scaled powers has to be used to calibration of AoAs and AoDs
    // We can use the clusterPowers and scale them later with the K-factors
    // as done in clusterPowers class (lines 83 and 84 in clusterPowers.cpp)
    ff << itpp::Name("Powers") << *clusterPowers;
    ff << itpp::Name("PowersScaled") << *scaledClusterPowers;

    // Export the correlated LSPs (from which we will get the K-Factors)
    ff << itpp::Name("sigmas") << *sigmas;


    // Export the LoS angles
    itpp::vec aoasLOS, aodsLOS;
    aoasLOS.set_size(scmLinks.size());
    aodsLOS.set_size(scmLinks.size());

    for(unsigned int k = 0; k<scmLinks.size(); k++)
    {
        aoasLOS(k) = channel->getAzimuth(scmLinks[k]->getWrappedMSposition(), scmLinks[k]->getBS()->getPosition());
        aodsLOS(k) = channel->getAzimuth(scmLinks[k]->getBS()->getPosition(), scmLinks[k]->getWrappedMSposition());
    }

    ff << itpp::Name("AoAsLOS") << aoasLOS;
    ff << itpp::Name("AoDsLOS") << aodsLOS;

    // Export the AoAs and AoDs after collapsing them to a (k x N*M) matrix
    itpp::mat AoAsForItpp,AoDsForItpp;
    AoAsForItpp.set_size(scmLinks.size(), MaxClusters*NumRays);
    AoDsForItpp.set_size(scmLinks.size(), MaxClusters*NumRays);
    for(unsigned int k=0; k<scmLinks.size(); k++)
    {
        unsigned int nm = 0;
        for(int m=0; m<NumRays; m++)
            for(int n=0; n<MaxClusters; n++)
            {
                AoAsForItpp(k,nm) = aoas[k][n][m];
                AoDsForItpp(k,nm) = aods[k][n][m];
                nm++;
            }
    }

    ff << itpp::Name("AoAs") << AoAsForItpp;
    ff << itpp::Name("AoDs") << AoDsForItpp;

    ff.close();
}
