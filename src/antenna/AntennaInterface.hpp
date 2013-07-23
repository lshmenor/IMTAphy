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

#ifndef IMTAPHY_ANTENNA_ANTENNAINTERFACE
#define IMTAPHY_ANTENNA_ANTENNAINTERFACE

#include <WNS/PowerRatio.hpp>
#include <WNS/Position.hpp>
#include <WNS/pyconfig/View.hpp>
#include <complex>

namespace imtaphy { namespace antenna {
    
    
        class AntennaInterface {
        public:
    
            AntennaInterface(const wns::pyconfig::View& pyConfigView) {
            }
            /**
             * @brief Return the power gain of the antenna pattern due to the 
             * azimuth angle only (absolute azimuth between -Pi and Pi 
             * clockwise with north being 0) from which the destination is seen.
             * Note that the combined method is preferred because, e.g., to avoid
             * adding max attenuations from vertical and horizontal patterns.
             */
            virtual wns::Ratio getAzimuthGain(double azimuth) const =0;

            /**
             * @brief Return the power gain of the antenna pattern due to the 
             * elevation angle only (from -Pi/2 to Pi/2 with horizontal being 0 and 
             * downwards being positive) from which the destination is seen.
             * Note that the combined method is preferred because, e.g., to avoid
             * adding max attenuations from vertical and horizontal patterns.
             */
            virtual wns::Ratio getElevationGain(double elevation) const =0;

    
            /**
             * @brief Returns the power gain towards azimuth and elevation angles in 
             * radians (absolute azimuth between -Pi and Pi clockwise with north being 0, 
             * elevation from -Pi/2 to
             * Pi/2 with horizontal being 0 and downwards being positive).
             * 
             * This can be used when no gains are included in e.g. the fast fading 
             * values provided by the spatialChannel model
             */ 
            virtual wns::Ratio getGain(double azimuth, double elevation) const =0;

        
            /**
             * @brief Horizontal field (not power gain!) pattern towards the absolute
             * azimuth angle phi (in radians -Pi..Pi, clockwise orientation with north being 0)
             * elevation from -Pi/2 to
             * Pi/2 with horizontal being 0 and downwards being positive
             */
            virtual std::complex<double> getHorizontalFieldPattern(double azimuth, double elevation, unsigned int elementNumber) const = 0;
    
            /**
             * @brief Vertical field (not power gain!) pattern towards the absolute
             * azimuth angle phi (in radians -Pi..Pi, clockwise orientation with north being 0)
             * elevation from -Pi/2 to
             * Pi/2 with horizontal being 0 and downwards being positive
             */
            virtual std::complex<double> getVerticalFieldPattern(double azimuth, double elevation, unsigned int elementNumber) const = 0;
    
            /**
             * @brief The number of antenna elements. Counting starts with
             * antenna element 0 (which is called the reference element in 
             * case of an antenna array).
             */
            virtual unsigned int getNumberOfElements() const = 0;
    
            /**
             * @brief This is the path difference a wavefront arriving/departing at the
             * array at an absoulte azimuth angle phi_n_m experiences between element "elementNumber" and
             * the reference element 0. This takes the element spacings as well as the antenna array
             * broadsize azimuth orientation into account. phi_n_m
             * is the absolute azimuth angle (in radians -Pi..Pi, clockwise orientation with
             * north being 0) from which the array is seen.
             * 
             * For arbitrary array layouts, this is computed as described in
             * Annex 1 of Rep. ITU-R M.2135 equation (24) and Fig. 10.
             */
            
            virtual double getPathDifferenceMeters(unsigned int elementNumber,
                                                   double phi_n_m) const = 0;
    
            /**
             * @brief Computes the relative azimuth angle (azimuth - arrayBroadsideAzimuth)
             * between the absolute azimuth of the direction in question and the arrays
             * broadside orientation.
             * 
             */
    
            virtual double getAzimuthRelativeToBroadside(double absoluteAzimuth) const = 0;
        };
    }}
#endif
