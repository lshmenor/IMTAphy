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

#ifndef SPATIALCHANNEL_LSCORRELATION_HPP
#define SPATIALCHANNEL_LSCORRELATION_HPP

#include <IMTAPHY/spatialChannel/SpatialChannelModelInterface.hpp>
#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/lsParams/LargeScaleParameters.hpp>
#include <IMTAPHY/lsParams/RngMock.hpp>
#include <IMTAPHY/spatialChannel/m2135/FixPar.hpp>
#include <set>



namespace imtaphy { namespace lsparams {
        
        typedef std::set< StationPhy*, StationPhyPtrCompare> StationSet;
        typedef std::map< wns::Position, StationSet > StationMap;

        class LSCorrelation
        {
        public:
            LSCorrelation(imtaphy::LinkVector _links, LinkManager* _linkManager, RandomMatrix* _rnGen);
                        
            virtual ~LSCorrelation();
            /**
             * @brief Generate correlated Large Scale parameters
             */
            LSmap* generateLSCorrelation();

            wns::logger::Logger logger;

        private:
            imtaphy::LinkVector getLinksWithSameScenarioPropagationThisSite(StationSet colocatedBSs, Link::Scenario scenario, Link::Propagation propagation);
            void performCorrelation(LinkVector linksToCorrelate, const imtaphy::scm::m2135::Parameters* scenarioPropagationParams);
            unsigned int findNextPower(unsigned int size);
            
            imtaphy::LinkVector links;
            LinkManager* linkManager;
            std::list<itpp::mat> randomGrids;
            StationMap siteMap;
            LSmap* lsParams;
            
            RandomMatrix* rnGen;
            
            
        };


    }}
#endif
