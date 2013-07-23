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

#ifndef IMTAPHY_PATHLOSS_NO
#define IMTAPHY_PATHLOSS_NO

#include <IMTAPHY/pathloss/PathlossModelInterface.hpp>
#include <IMTAPHY/Link.hpp>

namespace imtaphy { namespace pathloss {
    
        class No :
            public PathlossModelInterface
        {
        public:
            No(Channel* _channel, wns::pyconfig::View config) :
                PathlossModelInterface(_channel, config)
            {};
        
            void onWorldCreated() {};
        
            wns::Ratio getPathloss(Link* link, double frequency);
        
        private:
        };
    
    }}

#endif
