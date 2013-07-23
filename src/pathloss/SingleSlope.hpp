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

#ifndef IMTAPHY_PATHLOSS_SINGLESLOPE
#define IMTAPHY_PATHLOSS_SINGLESLOPE

#include <IMTAPHY/pathloss/PathlossModelInterface.hpp>
#include <IMTAPHY/Link.hpp>

namespace imtaphy { 
    
    class Channel;
    
    namespace pathloss {

        class SingleSlope :
                public PathlossModelInterface
        {
        public:
            SingleSlope(Channel* _channel, wns::pyconfig::View config);
        
            virtual void onWorldCreated() {};
        
            /**
             * @brief Return the pathloss according to a single-slope pathloss model
             * pl = offset + distFactor * log10(distance/m) + freqFactor * log10(frequency/MHz)
             * where offset, distfactor, and freqFactor are configured from PyConfig
             * for both LOS and NLOS conditions.
             * 
             */
        
            virtual wns::Ratio getPathloss(Link* link, double frequencyMHz);
        
        private:
            double frequencyFactorLOS;
            double frequencyFactorNLOS;
        
            double distanceFactorLOS;
            double distanceFactorNLOS;

            double offsetLOS;
            double offsetNLOS;
        
            double minPathloss;
        };

    }}

#endif
