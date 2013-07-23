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

#ifndef IMTAPHY_ANTENNA_ULAANTENNAS
#define IMTAPHY_ANTENNA_ULAANTENNAS

#include <IMTAPHY/antenna/AntennaInterface.hpp>
#include <WNS/logger/Logger.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>

namespace imtaphy { namespace antenna {
    
        /**
         * @brief
         * Base class for all antennas that can form an Uniform Linear Array.
         * 
         * Provides some basic functionality to derived classes (currently Omnidirectional
         * and AntennaITU). Could be separated further if different array geometries 
         * should be supported. 
         */
    
        class LinearAntennaArray :
            public AntennaInterface 
        {
        public:
            LinearAntennaArray(const wns::pyconfig::View& pyConfigView);
        
            /**
             * @brief Horizontal field (not power gain!) pattern towards the absolute
             * azimuth angle phi (in radians -Pi..Pi, clockwise orientation with north being 0)
             * elevation from -Pi/2 to
             * Pi/2 with horizontal being 0 and downwards being positive
             */
            virtual std::complex<double> getHorizontalFieldPattern(double azimuth, double elevation, unsigned int elementNumber) const;
        
            /**
             * @brief Vertical field (not power gain!) pattern towards the absolute
             * azimuth angle phi (in radians -Pi..Pi, clockwise orientation with north being 0)
             * elevation from -Pi/2 to
             * Pi/2 with horizontal being 0 and downwards being positive
             */
            virtual std::complex<double> getVerticalFieldPattern(double azimuth, double elevation, unsigned int elementNumber) const;
        
            /**
             * @brief The number of antenna elements. Counting starts with
             * antenna element 0 (which is called the reference element in 
             * case of an antenna array).
             */
            virtual unsigned int getNumberOfElements() const;

            /**
             * @brief Computes the relative azimuth angle (azimuth - arrayBroadsideAzimuth)
             * between the absolute azimuth of the direction in question and the arrays
             * broadside orientation.
             * 
             */
        
            virtual double getAzimuthRelativeToBroadside(double absoluteAzimuth) const;
                
                
            /**
             * @brief This is the path difference a wavefront arriving/departing at the
             * array at an absoulte azimuth angle phi_n_m experiences between element "elementNumber" and
             * the reference element 0. This takes the element spacings as well as the antenna array
             * broadsize azimuth orientation into account. phi_n_m
             * is the absolute azimuth angle (in radians -Pi..Pi, clockwise orientation with
             * north being 0) from which the array is seen.
         
             * For arbitrary array layouts, this is computed as described in
             * Annex 1 of Rep. ITU-R M.2135 equation (24) and Fig. 10.
            */
        
            virtual double getPathDifferenceMeters(unsigned int elementNumber,
                                                   double phi_n_m) const;
                                                   
#ifndef WNS_NDEBUG                                                   
            void computeAntennaPattern(imtaphy::detail::ComplexFloatMatrix& beamformingVector, std::vector< wns::Ratio >& pattern360Degrees, double wavelengthMeters);
#endif            
    
        protected:
            double broadsideAzimuth;
            unsigned int numElements;
            std::vector<double> distancesToFirstElement;
            std::vector<double> slantAngle;
            wns::logger::Logger logger;

        };
    }}
#endif

