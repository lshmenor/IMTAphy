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

#ifndef L2S_MMSEFDE_HPP
#define L2S_MMSEFDE_HPP

#include <IMTAPHY/link2System/EffectiveSINRModelInterface.hpp>

namespace imtaphy { namespace l2s {

        class MMSEFrequencyDomainEqualization :
            public EffectiveSINRModelInterface
        {
        public:
            MMSEFrequencyDomainEqualization() :
                EffectiveSINRModelInterface() {};
            
            wns::Ratio getEffectiveSINR(std::vector<wns::Ratio> sinrs,
                                        ModulationScheme modulation) 
            {
                // Effect of Frequency Domain MMSE Equalization for the SC-FDMA uplink
                // according to R1-050718 and R1-051352
                
                double lambda = 0.0;
                double K = sinrs.size();
                
                for (unsigned int k = 0; k < sinrs.size(); k++)
                {
                    lambda += sinrs[k].get_factor() / (sinrs[k].get_factor() + 1.0);
                }
                
                double gamma = pow(lambda, 2.0) / (K*lambda - pow(lambda, 2.0));
                
                return wns::Ratio::from_factor(gamma);
            }
        };
    }}
#endif
