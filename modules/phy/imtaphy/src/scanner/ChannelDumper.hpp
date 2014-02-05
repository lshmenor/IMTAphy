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

#ifndef CHANNELDUMPER_HPP
#define CHANNELDUMPER_HPP

#include <WNS/node/component/Component.hpp>
#include <IMTAPHY/interface/DataTransmission.hpp>
#include <IMTAPHY/interface/DataReception.hpp>
#include <IMTAPHY/interface/IMTAphyObserver.hpp>

#include <WNS/Observer.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

#include <IMTAPHY/Channel.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/scanner/LteScanner.hpp>
#include <WNS/evaluation/statistics/moments.hpp>

#include <IMTAPHY/spatialChannel/SpatialChannelModelInterface.hpp>

namespace imtaphy { namespace scanner {
    
        class ChannelDumper :
            public LteScanner
        {
        public:
            ChannelDumper(wns::node::Interface* _node, const wns::pyconfig::View& _pyco);
                
            // LTEPhyObserver interface
	    void onNewTTI(unsigned int ttiNumber) {};
            void beforeTTIover(unsigned int ttiNumber);
        
        private:
            void initProbes() {}; // no probes for dumper
            std::ofstream ff;
            imtaphy::scm::ChannelLayout layout;
        };
    
    
    }}

#endif
