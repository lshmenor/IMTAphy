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

#ifndef IMTAPHY_ANTENNA_OMNIDIRECTIONAL
#define IMTAPHY_ANTENNA_OMNIDIRECTIONAL

#include <IMTAPHY/antenna/AntennaInterface.hpp>
#include <IMTAPHY/antenna/LinearAntennaArray.hpp>
#include <WNS/logger/Logger.hpp>

namespace imtaphy { namespace antenna {
    
        class Omnidirectional :
            public LinearAntennaArray 
        {
        public:
            Omnidirectional(const wns::pyconfig::View& pyConfigView);
            /**
             * @brief Return the power gain of the antenna pattern due to the 
             * azimuth angle only (azimuth azimuth between -Pi and Pi 
             * clockwise with north being 0) from which the destination is seen.
             * Note that the combined method is preferred because, e.g., to avoid
             * adding max attenuations from vertical and horizontal patterns.
             */
            wns::Ratio getAzimuthGain(double azimuth) const;
        
            /**
             * @brief Return the power gain of the antenna pattern due to the 
             * elevation angle only (from -Pi/2 to Pi/2 with horizontal being 0 and 
             * downwards being positive) from which the destination is seen.
             * Note that the combined method is preferred because, e.g., to avoid
             * adding max attenuations from vertical and horizontal patterns.
             */
            wns::Ratio getElevationGain(double elevation) const;
        
        
            /**
             * @brief Returns the power gain towards azimuth and elevation angles in 
             * radians (azimuth between -Pi and Pi clockwise with north being 0, 
             * elevation from -Pi/2 to
             * Pi/2 with horizontal being 0 and downwards being positive).
             * 
             * This can be used when no gains are included in e.g. the fast fading 
             * values provided by the spatialChannel model
             */
            wns::Ratio getGain(double azimuth, double elevation) const;
        
        private:
            wns::Ratio dBiGain;
        
        };
    }}
#endif
