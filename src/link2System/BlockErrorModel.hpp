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

#ifndef L2S_BLOCKERRORMODEL_HPP
#define L2S_BLOCKERRORMODEL_HPP

#include <IMTAPHY/link2System/Modulations.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/pyconfig/View.hpp>
#include <IMTAPHY/detail/LookupTable.hpp>
#include <IMTAPHY/detail/Interpolation.hpp>
#include <WNS/Singleton.hpp>
#include <IMTAPHY/link2System/LteCQIs.hpp>

namespace imtaphy { namespace l2s {

        class BlockErrorModel
        {
            typedef imtaphy::detail::LookupTable<double, unsigned int, 1> TableType;
            
        public:
            BlockErrorModel();

            double getBlockErrorRate(wns::Ratio sinr, ModulationScheme modulation, float codeRate, unsigned int blockSize) const;
            
            wns::Ratio getSINRthreshold(unsigned int cqi)
            {
                assure((cqi >= 0) && (cqi < 16), "Invalid CQI");
                return sinrThresholds[cqi];
            }
            
            unsigned int getCQI(wns::Ratio sinr)
            {
                // The requested SINR will usually lie between two SINR thresholds. We want to return the 
                // lower threshold because for the higher the BLER would be > 10% and could be quite bad
                unsigned int cqi = lut->getHyperCube(std::vector<double>(1, sinr.get_factor()))[imtaphy::detail::Lower].value;
                            
                return std::max(1u, cqi);
            }
            
        private:
            typedef imtaphy::detail::LookupTable<float, float, 3> BLERLookUpTable;
            
            std::vector<BLERLookUpTable*> blerLUTs;
       
            imtaphy::detail::Interpolation<float, float, 3> tri;

            std::vector<float> minCR;
            std::vector<float> maxCR;

            float minSINR, maxSINR;
            float minBL, maxBL;
            std::vector<std::vector<std::vector<float> > > keys;
            std::vector<std::vector<float> > offsets;
            
            wns::Ratio sinrOffsetQPSK;
            wns::Ratio sinrOffsetQAM16;
            wns::Ratio sinrOffsetQAM64;
            
            std::vector<wns::Ratio> sinrThresholds;
            TableType* lut;
        };
        
        typedef wns::SingletonHolder<BlockErrorModel> TheLTEBlockErrorModel;
    }}
#endif
