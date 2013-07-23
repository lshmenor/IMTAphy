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

#ifndef L2S_MMIBEFFECTIVESINR_HPP
#define L2S_MMIBEFFECTIVESINR_HPP

#include <IMTAPHY/link2System/Modulations.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/pyconfig/View.hpp>
#include <IMTAPHY/link2System/EffectiveSINRModelInterface.hpp>
#include <IMTAPHY/detail/LookupTable.hpp>
#include <IMTAPHY/detail/Interpolation.hpp>
#include <WNS/Singleton.hpp>

namespace imtaphy { namespace l2s {

        /* Mean Mutual Information per Bit effective SINR computation
         * according to WiMAX 802.16m Evaluation Methodology Document
         * IEEE 802.16m-08/004r5, Section 4.3.2.1 
         * 
         * */
    
        class MMIBeffectiveSINR :
            public EffectiveSINRModelBase
        {
        public:
            MMIBeffectiveSINR();
            
            double forwardMapping(wns::Ratio sinr,
                                  ModulationScheme modulation) ;
            wns::Ratio reverseMapping(double average,
                                      ModulationScheme modulation);
        private:
            typedef imtaphy::detail::LookupTable<double, double, 1> MiSinrLutType;

            double J(double x) const;
            double Jinv(double y) const;

            std::vector<double> lookForKey;
            MiSinrLutType::HyperCubeType result;
            imtaphy::detail::Interpolation<double, double, 1> interpolation;
            
            MiSinrLutType* qam16Lut;
            MiSinrLutType* qam64Lut;

            double minSINR, maxSINR;
            double minMIqam16, maxMIqam16;
            double minMIqam64, maxMIqam64;
        };
        
        typedef wns::SingletonHolder<MMIBeffectiveSINR> TheMMIBEffectiveSINRModel;
        
    }}
#endif
