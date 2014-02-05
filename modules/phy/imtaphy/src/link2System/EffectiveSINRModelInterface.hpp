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

#ifndef L2S_EFFECTIVESINRMODELINTERFACE_HPP
#define L2S_EFFECTIVESINRMODELINTERFACE_HPP

#include <IMTAPHY/link2System/Modulations.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/pyconfig/View.hpp>

namespace imtaphy { namespace l2s {

        class EffectiveSINRModelInterface
        {
        public:
            EffectiveSINRModelInterface() {};
            
            virtual wns::Ratio getEffectiveSINR(std::vector<wns::Ratio> sinrs,
                                                ModulationScheme modulation)  = 0;
            
        };
        
        class EffectiveSINRModelBase :
            public EffectiveSINRModelInterface
        {
        public:
            EffectiveSINRModelBase() :
                EffectiveSINRModelInterface() {};
            
            wns::Ratio getEffectiveSINR(std::vector<wns::Ratio> sinrs,
                                        ModulationScheme modulation) 
            {
                // transform sinrs into a domain that allows taking the arithmetic mean
                // by using the forwardMapping (e.g. compute Mean Mutual Information per Bit)
                // and use the reverse mapping to compute the effective SINR from the mean
                
                unsigned int size = sinrs.size();
                double average = 0;
                
                assure(size, "effective SINRs called with empty vector");

                
                for (unsigned int i = 0; i < size; i++)
                {
                    average += forwardMapping(sinrs[i], modulation);
                }
                
                average /= static_cast<double>(size);
                
                return reverseMapping(average, modulation);
            };
                
        protected:
            virtual double forwardMapping(wns::Ratio sinr,
                                          ModulationScheme modulation)  = 0;
            virtual wns::Ratio reverseMapping(double average,
                                              ModulationScheme modulation) = 0;
        };
        
    }}
#endif
