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

#ifndef PATHLOSSMODELINTERFACE_HPP
#define PATHLOSSMODELINTERFACE_HPP

#include <WNS/Position.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/pyconfig/View.hpp>

namespace imtaphy { 
    class Channel;
    class Link;
    
    namespace pathloss {
    
        class PathlossModelInterface
        {
        public:
            PathlossModelInterface(Channel* channel, wns::pyconfig::View config) {};
    
    
            /**
             * @brief To be called by TheChannel after all stations have registered, use for initialization 
             */
            virtual
            void onWorldCreated() = 0;
    
            /**
             * @brief Compute pathloss between source and destination. 
             * Frequency must be in MHz. The link object itself is not modified. 
             */
            virtual 
            wns::Ratio getPathloss(Link* link, double frequency) = 0;
        };

    }}

#endif // PATHLOSSMODELINTERFACE_HPP
