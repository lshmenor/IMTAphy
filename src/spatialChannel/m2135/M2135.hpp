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

#ifndef SPATIALCHANNEL_M2135_HPP
#define SPATIALCHANNEL_M2135_HPP

#include <IMTAPHY/spatialChannel/SpatialChannelModelInterface.hpp>
#include <IMTAPHY/spatialChannel/m2135/LinkPar.hpp>
#include <IMTAPHY/spatialChannel/m2135/FixPar.hpp>
#include <IMTAPHY/spatialChannel/m2135/ClusterPowers.hpp>
#include <IMTAPHY/spatialChannel/m2135/RayAngles.hpp>
#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/spatialChannel/m2135/Phases.hpp>

#include <WNS/logger/Logger.hpp>

// For opt versions disable range checking in the multi_array wrapper
// WNS_NDEBUG means: assures are disabled
#ifdef  WNS_NDEBUG
#define BOOST_DISABLE_ASSERTS
#endif
#include <boost/multi_array.hpp>


#define MKL_Complex8 std::complex<float>
#define MKL_Complex16 std::complex<double>
#include <mkl_vml_functions.h>
#include <mkl_service.h>
#include <mkl_cblas.h>
#include <mkl_vml_defines.h>

namespace imtaphy { namespace scm { namespace m2135 {
    
            class RayAngles;
            class ClusterPowers;
            class Delays;
    
            namespace tests {
                class M2135Test;
                class M2135CalibrationTest;
                class M2135TestCoherenceTime;
                class M2135TapsStatisticsTest;
            }

            template <typename T> 
            class MKLCis 
            {
            public:
                void execute(const int n, // Specifies the number of elements to be calculated.
                             const T* a, // Pointer to an array that contains the input vector a
                             std::complex<T>* y // Pointer to an array that contains the output vector y.
                    );
            };
    
            template <typename T>
            class MKLscal // the ?scal routines perform a vector operation defined as x = a*x
            {
            public:
                void execute(const int N, // Specifies the number of elements in vector x.
                             const T alpha, // Specifies the scalar a.
                             T *X, // points to the vector x to be scaled
                             const int incX // Specifies the increment for the elements of x
                    );
        
            };
    
            template <typename T>
            class MKLgemm // The ?gemm routines perform a matrix-matrix operation with general matrices. The operation is defined as C := alpha*op(A)*op(B) + beta*C
            {
            public:
                void execute(const CBLAS_ORDER Order, 
                             const CBLAS_TRANSPOSE TransA, 
                             const CBLAS_TRANSPOSE TransB, 
                             const int M, 
                             const int N, 
                             const int K, 
                             const void *alpha, 
                             const void *A, 
                             const int lda, 
                             const void *B, 
                             const int ldb, 
                             const void *beta, 
                             void *C, 
                             const int ldc); 
            };
    
            template <typename T>
            class MKLdotu // The ?dotu_sub routines compute a vector-vector dot product
            {
            public:
                void execute(const int N, // Specifies the number of elements in vectors x and y.
                             const void *X, // points to vector X
                             const int incX, // specifies the increment for vector X
                             const void *Y,  // points to vector Y
                             const int incY, // specifies the increment for vector Y
                             void *dotu      // points to the memory for the result
                    );
            };
    
            // Note: Even though M2135 is templated, we do not have to put the implementation into the 
            // header file. The reason is that inside the cpp file we explicitely instantiate the 
            // M2135<double> and the M2135<float> version in the StaticFactory macro.
            // These two specializations are then not only available from the StaticFactory but also
            // in unit tests, for example.
    
            template <typename PRECISION>
            class M2135 :
                public imtaphy::scm::SpatialChannelModelInterface<PRECISION>
            {
    
            public:
        
                friend class imtaphy::scm::m2135::tests::M2135Test;
                friend class imtaphy::scm::m2135::tests::M2135CalibrationTest;
                friend class imtaphy::scm::m2135::tests::M2135TestCoherenceTime;
                friend class imtaphy::scm::m2135::tests::M2135TapsStatisticsTest;
        
                M2135(imtaphy::Channel* channel, wns::pyconfig::View config);
                virtual ~M2135();
            
                /**
                 * @brief To be called by TheChannel after all stations have registered, use for initialization 
                 */
                void onWorldCreated(LinkManager* linkManager, lsparams::LSmap* lsParams, bool keepIntermediates);
        
                void evolve(double t);
        

            protected:
        
                /**
                 * @brief Computes the time invariant parts 
                 */
        
                void computeTimeInvariantCoefficients(RayAngles3DArray& aoas, RayAngles3DArray& aods,
                                                      std::vector<int> &strongestIndices,
                                                      std::vector<int> &secondStrongestIndices, 
                                                      imtaphy::LinkVector links, 
                                                      std::vector<Phases*> initialPhases,
                                                      itpp::mat* scaledDelays,
                                                      itpp::mat* clusterPowers,
                                                      itpp::vec* XPRs);
        
        
                /**
                 * @brief Generate  initial cross-polarization power ratios, K x N X M matrix
                 */
                itpp::vec* generateXPRs(imtaphy::LinkVector links);
        
                void addPowersforSubClusters();
        
                void setMaxAntennas();
        
        
                scm::ChannelLayout getChannelLayout();
        
                std::complex<PRECISION> getCurrentCIR( unsigned int k, // SCM link ID
                                                       imtaphy::Direction d, // Uplink or Downlink
                                                       unsigned int rxAntenna, // Rx antenna ID
                                                       unsigned int txAntenna, // Tx antenna ID
                                                       int n  // cluster/path ID
                    )
                {
                    assure(k < scmLinks.size(), "invalid link ID");
                    if (d == imtaphy::Downlink)
                    {
                        assure(rxAntenna < maxMsAntennas, "invalid Rx antenna ID u");
                        assure(txAntenna < maxBsAntennas, "invalid Tx antenna ID s");
                    }
                    else // uplink
                    {
                        assure(rxAntenna < maxBsAntennas, "invalid Rx antenna ID u");
                        assure(txAntenna < maxMsAntennas, "invalid Tx antenna ID s");
                    }

                    assure(n < MaxClusters, "invalid cluster/path ID n");
            
                    return (*H[d])[k][rxAntenna][txAntenna][n];
                }

                std::complex<PRECISION> getCurrentCTF( unsigned int k, // SCM link ID
                                                       imtaphy::Direction d, // Uplink or Downlink
                                                       unsigned int rxAntenna, // Rx antenna ID
                                                       unsigned int txAntenna, // Tx antenna ID
                                                       unsigned int f  // frequency bin (PRB) index
                    )
                {
                    assure(k < scmLinks.size(), "invalid link ID");
                    if (d == imtaphy::Downlink)
                    {
                        assure(rxAntenna < maxMsAntennas, "invalid Rx antenna ID u");
                        assure(txAntenna < maxBsAntennas, "invalid Tx antenna ID s");
                    }
                    else // uplink
                    {
                        assure(rxAntenna < maxBsAntennas, "invalid Rx antenna ID u");
                        assure(txAntenna < maxMsAntennas, "invalid Tx antenna ID s");
                    }
                    assure(f < channel->getSpectrum()->getNumberOfPRBs(d), "invalid PRB ID f");

                    return (*T[d])[k][f][rxAntenna][txAntenna];
                }

                boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >
                getChannelMatrix(imtaphy::Link* link,
                                 imtaphy::Direction direction,        
                                 unsigned int f)  // frequency bin (PRB) index
                {
                    // reconsider whether the advantages of having shared_ptrs of MKLMatrix objects
                    // are a reasonable trade-off between memory consumption and safety
                    // Pros:
                    // - dimension info is stored with the object
                    // - more consistent because ComplexFloatMatrices are used everywhere
                    //
                    // Cons:
                    // - extra size compared to pure channel matrices (see below)
                    // - ownership management not necessary; points to external memory anyway
                    // - sizes can be managed by Link, which knows it anyway
                    // - 
                    // 
                    // size of ComplexFloatMatrix:      112 Bytes
                    // size of ComplexFloatMatrixPtr:   16  Bytes
                    // size of std::complex<float>:     8   Bytes
                    
                    
                    unsigned int u = link->getMS()->getAntenna()->getNumberOfElements();
                    unsigned int s = link->getBS()->getAntenna()->getNumberOfElements();

                    assure(f < channel->getSpectrum()->getNumberOfPRBs(direction), "Invalid PRB index");
                    
                    if (link->isSCM())
                    {
                        unsigned int k = link->getSCMlinkId();
                        
                        if ((maxMsAntennas == u) &&
                            (maxBsAntennas == s))
                        {
                            // return a matrix whose data resides where we store and evolve the channel matrix
                            // for this link
                            if (direction == imtaphy::Downlink)
                            {
                                return boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >
                                    (new imtaphy::detail::MKLMatrix<std::complex<PRECISION> >(maxMsAntennas, maxBsAntennas, &((*T[imtaphy::Downlink])[k][f][0][0])));
                            }
                            else // uplink
                            {
                                return boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >
                                    (new imtaphy::detail::MKLMatrix<std::complex<PRECISION> >(maxBsAntennas, maxMsAntennas, &((*T[imtaphy::Uplink])[k][f][0][0])));
                            }
                        }
                        else 
                        {
                            // we had computed the channel matrix for a maxMsAntennas-by-maxBsAntennas configuration but 
                            // the actual channel has less antenna pairs, so construct a smaller one

                            assure( u <= maxMsAntennas, "maxMsAntennas ist not the maximum");
                            assure( s <= maxBsAntennas, "maxBsAntennas ist not the maximum");
                            
                            if (((s == maxBsAntennas) && (direction == imtaphy::Downlink)) ||
                                ((u == maxMsAntennas) && (direction == imtaphy::Uplink)))
                            { // we store matrices in row-major mode so we can easily support channel matrices that 
                              // have less rx antennas than the maximum rx antennas we computed because then we can 
                              // simply take the first u rows of the computed bigger matrix
                              
                                if (direction == imtaphy::Downlink)
                                {
                                    return boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >
                                        (new imtaphy::detail::MKLMatrix<std::complex<PRECISION> >(u, maxBsAntennas, &((*T[imtaphy::Downlink])[k][f][0][0])));
                                }
                                else // uplink
                                {
                                    return boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >
                                        (new imtaphy::detail::MKLMatrix<std::complex<PRECISION> >(s, maxMsAntennas, &((*T[imtaphy::Uplink])[k][f][0][0])));
                                }

                            }
                            else
                            {   // the channel has both less tx as well as rx antennas compared to the maximum
                                // we could copy them here but they would not get evolved properly
                                assure(0, "We currently support different channel sizes because the channel matrices would not be evolved automatically");
                            }
                        }                            
                    }
                    else // link is not SCM so return a static channel matrix
                    {
                        imtaphy::detail::MKLMatrix<std::complex<PRECISION> >* matrix;
                        if (direction == imtaphy::Downlink)
                        {
                             matrix = new imtaphy::detail::MKLMatrix<std::complex<PRECISION> >(u, s);
                        }
                        else // uplink
                        {
                             matrix = new imtaphy::detail::MKLMatrix<std::complex<PRECISION> >(s, u);
                        }
                             
                        for (unsigned int i = 0; i < u; i++)
                            for (unsigned int j = 0; j < s; j++)
                            {
                                if (direction == imtaphy::Downlink)
                                {
                                    (*matrix)[i][j] = std::complex<PRECISION>(1.0 / sqrt(link->getWidebandLoss().get_factor()), 0.0);
                                }
                                else // uplink
                                {
                                    (*matrix)[j][i] = std::complex<PRECISION>(1.0 / sqrt(link->getWidebandLoss().get_factor()), 0.0);
                                }
                            }
                                    
                        return boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >(matrix);
                    }
                    assure(0, "We should never reach this point");
                    return boost::shared_ptr<imtaphy::detail::MKLMatrix<std::complex<PRECISION> > >(new imtaphy::detail::MKLMatrix<std::complex<PRECISION> >(*(wns::SingletonHolder<imtaphy::detail::Identity<std::complex<PRECISION> > >::Instance().get(u))));
                }


            private:
                void dumpSmallScaleCalibration(RayAngles3DArray& aoas, RayAngles3DArray& aods, itpp::mat* sigmas, 
                                            itpp::mat* scaledDelays, itpp::mat* clusterPowers, itpp::mat* scaledClusterPowers);
                void sizeMultiDimArrays();
                void setRiceanKterms(itpp::Vec<double> sigmaK);
                void transformChannel();
                bool checkZeroRays(std::complex<double> c, int cluster, int ray, int link) const;
        
                imtaphy::Channel* channel;
                imtaphy::LinkVector scmLinks;
        
                static const int MaxClusters = 24; // includes the subclusters for the 2 strongest paths
                static const int NumRays = 20; 
        
                unsigned int maxBsAntennas; // U
                unsigned int maxMsAntennas; // S
        
                unsigned int numTTIs; // T: how many ttis to compute when evolving
        
                // uplink and downlink versions of these multi-dimensional arrays:
                std::vector<boost::multi_array_ref<PRECISION, 5>*> tVariantCoeff;
                std::vector<boost::multi_array_ref<std::complex<PRECISION>, 5>*> tInvariantFactor;
                std::vector<boost::multi_array_ref<std::complex<PRECISION>, 3>*> frequencyCoeff;

                std::vector<boost::multi_array_ref<std::complex<PRECISION>, 4>*> H;
                std::vector<boost::multi_array_ref<std::complex<PRECISION>, 4>*> T;

                std::vector<double> riceanKterm1;
                std::vector<double> riceanKterm2;
        
                // The class to compute the delays of each multi paths
                Delays* delays;

                // The class to compute the (normalized) powers per multipath cluster
                ClusterPowers* clusterPowers;
                
                // we keep the indices for asserting correct computations during evolve
                std::vector<int> strongestIndices;
                std::vector<int> secondStrongestIndices;

                // The class to compute the rays' angles
                RayAngles* angles;
        
                wns::logger::Logger logger;
                bool dumpCalibrationData;
                std::string calibrationOutputFileName;
                
                std::vector<bool> directionsEnabled;
                bool computeEffectiveAntennaGains;

                bool allSpeedsZero;
            };
        
        }}}
#endif
