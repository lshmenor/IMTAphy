/******************************************************************************* 
* This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2011
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

#ifndef IMTAPHY_INTERFACE_TRANSMISSIONSTATUS_HPP
#define IMTAPHY_INTERFACE_TRANSMISSIONSTATUS_HPP

#include <WNS/RefCountable.hpp>
#include <IMTAPHY/interface/IMTAphyObserver.hpp>
#include <WNS/PowerRatio.hpp>
#include <map>

#ifdef  WNS_NDEBUG
#define BOOST_DISABLE_ASSERTS
#endif
#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>

namespace imtaphy { namespace interface {
        typedef unsigned int PRB;
        typedef std::vector<PRB> PRBVector;
        typedef std::map<PRB, wns::Ratio> PRBMapdB;
        typedef std::vector<wns::Ratio> SINRVector;
        typedef std::vector<wns::Ratio> IoTVector;
        typedef boost::shared_ptr<SINRVector> SINRVectorPtr;

        
        class TransmissionStatus :
            public wns::RefCountable
        {
        public:
            
            TransmissionStatus(unsigned int _numberOfLayers, const PRBVector &_prbs) :
                numberOfLayers(_numberOfLayers),
                numberOfPRBs(_prbs.size()),
                prbs(_prbs),
                sinrs(boost::extents[numberOfLayers][numberOfPRBs]),
                iots(boost::extents[numberOfLayers][numberOfPRBs])
#ifndef WNS_NDEBUG
               ,checkSINR(boost::extents[numberOfLayers][numberOfPRBs]),
                checkIoT(boost::extents[numberOfLayers][numberOfPRBs])
#endif

            {
                assure(numberOfLayers > 0, "Invalid number of layers");
                
                for (unsigned int i = 0; i < numberOfPRBs; i++)
                {
                    assure(prbMap.find(prbs[i]) == prbMap.end(), "Duplicate PRB entry");
                    prbMap[prbs[i]] = i;
                }
#ifndef WNS_NDEBUG
                for (unsigned int i = 0; i < numberOfPRBs; i++)
                    for (unsigned int j = 0; j < numberOfLayers; j++)
                    {
                        checkSINR[j][i] = false;
                        checkIoT[j][i] = false;
                    }
#endif
            }
            
            void setSINRsForPRB(PRB prb, const SINRVector& sinrsPerLayer)
            {
                assure(sinrsPerLayer.size() == numberOfLayers, "Need to provide vector with as many SINRs as we have layers");
                assure(prbMap.find(prb) != prbMap.end(), "Unknown PRB requested"); 
                
                for (unsigned int i = 0; i < numberOfLayers; i++)
                {
                    sinrs[i][prbMap[prb]] = sinrsPerLayer[i];
#ifndef WNS_NDEBUG
                    checkSINR[i][prbMap[prb]] = true;
#endif
                }
            }

            void setIoTsForPRB(PRB prb, const IoTVector& iotsPerLayer)
            {
                assure(iotsPerLayer.size() == numberOfLayers, "Need to provide vector with as many IoTs as we have layers");
                assure(prbMap.find(prb) != prbMap.end(), "Unknown PRB requested"); 
                
                for (unsigned int i = 0; i < numberOfLayers; i++)
                {
                    iots[i][prbMap[prb]] = iotsPerLayer[i];
#ifndef WNS_NDEBUG
                    checkIoT[i][prbMap[prb]] = true;
#endif
                }
            }
            
            
            void setSINR(PRB prb, unsigned int layer, wns::Ratio sinr)
            {
                assure(prbMap.find(prb) != prbMap.end(), "Unknown PRB requested");
                assure((layer > 0) && (layer <= numberOfLayers), "Invalid layer index, must be from 1..numberOfLayers");
                
                sinrs[layer-1][prbMap[prb-1]] = sinr;
                
#ifndef WNS_NDEBUG
                checkSINR[layer-1][prbMap[prb-1]] = true;
#endif
            }
            
            PRBVector getPRBs()
            {
                return prbs;
            }
            
            wns::Ratio getSINR(PRB prb, unsigned int layer)
            {
                assure(prbMap.find(prb) != prbMap.end(), "Unknown PRB requested");
                assure((layer > 0) && (layer <= numberOfLayers), "Invalid layer index, must be from 1..numberOfLayers");

#ifndef WNS_NDEBUG
                assure(checkSINR[layer-1][prbMap[prb]], "Trying to read a SINR entry that had not been set");
#endif
                return sinrs[layer-1][prbMap[prb]];
            }
            
            SINRVector getSINRsForLayer(unsigned int layer)
            {
                assure((layer > 0) && (layer <= numberOfLayers), "Invalid layer index, must be from 1..numberOfLayers");

                SINRVector result(numberOfPRBs);
                
                for (unsigned int i = 0; i < numberOfPRBs; i++)
                {
#ifndef WNS_NDEBUG
                    assure(checkSINR[layer-1][prbMap[i]], "Trying to read a SINR entry that had not been set");
#endif
                    result[i] = sinrs[layer-1][i];
                }
                
                return result;
            }
            
            IoTVector getIoTsForLayer(unsigned int layer)
            {
                assure((layer > 0) && (layer <= numberOfLayers), "Invalid layer index, must be from 1..numberOfLayers");

                IoTVector result(numberOfPRBs);
                
                for (unsigned int i = 0; i < numberOfPRBs; i++)
                {
#ifndef WNS_NDEBUG
                    assure(checkIoT[layer-1][prbMap[i]], "Trying to read an IoT entry that had not been set");
#endif
                    result[i] = iots[layer-1][i];
                }
                
                return result;
            }

            
            unsigned int getNumberOfLayers()
            {
               return numberOfLayers;
            }
            
            unsigned int getNumberOfPRBs()
            {
                return numberOfPRBs;
            }
            
            unsigned int getPRBid(unsigned int prbIndex)
            {
                assure((prbIndex >= 0) && (prbIndex < numberOfPRBs), "Invalid PRB index");
            
                return prbs[prbIndex];
            }
            
        private:
            typedef boost::multi_array<wns::Ratio, 2> SINRarrayType;
            typedef boost::multi_array<wns::Ratio, 2> IoTarrayType;
            typedef boost::multi_array<bool, 2> ControlArrayType;
            
            
            unsigned int numberOfLayers;
            unsigned int numberOfPRBs;
            
            std::vector<PRB> prbs;
            SINRarrayType sinrs;
            IoTarrayType iots;
#ifndef WNS_NDEBUG
            ControlArrayType checkSINR;
            ControlArrayType checkIoT;
#endif
            
            std::map<PRB, unsigned int> prbMap;
            
        };

    typedef boost::shared_ptr<TransmissionStatus> TransmissionStatusPtr;


}}

#endif
