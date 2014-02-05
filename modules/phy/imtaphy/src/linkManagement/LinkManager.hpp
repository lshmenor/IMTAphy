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

#ifndef IMTAPHY_LINKMANAGER
#define IMTAPHY_LINKMANAGER

#include <IMTAPHY/Link.hpp>
#include <IMTAPHY/Spectrum.hpp>
#include <IMTAPHY/linkManagement/classifier/LinkClassifierInterface.hpp>
#include <WNS/Position.hpp>
#include <WNS/PowerRatio.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/pyconfig/Parser.hpp>
#include <IMTAPHY/spatialChannel/SpatialChannelModelInterface.hpp>
#include <WNS/logger/Logger.hpp>

namespace imtaphy { 
        
    class LinkManager
    {
    public:
        LinkManager() :
            config(wns::pyconfig::Parser()) 
        {}; //dummy
        
        /**
         * Constructor: Does wrap-around but no other computations  
         */
        LinkManager(Channel* channel, imtaphy::StationList bsList, imtaphy::StationList msList, wns::pyconfig::View config);
        
        void onPathlossAndShadowingReady();
        void determineServingLinks();

        void doBeforeSCMinit();
        void doAfterSCMinit(scm::SpatialChannelModelInterface<float>* scm, imtaphy::Spectrum* spectrum);
        
        Link* getLink(StationPhy* station1, StationPhy* station2);
        
        Link* getServingLinkForMobileStation(StationPhy* station);        
        LinkMap getServedLinksForBaseStation(StationPhy* station);
        
        LinkMap getAllLinksForStation(StationPhy* station);
            
        LinkVector getAllLinks();
        LinkVector getSCMLinks();
        
        wns::Power getRSRPReferenceTxPower() const {return referenceTxPower;}

        imtaphy::linkclassify::LinkClassifierInterface* getClassifier() {return linkClassifier;}
              
    protected:
        void computeRSRPbasedOnWBL();
        void computeRSRPbasedOnSCM(scm::SpatialChannelModelInterface<float>* scmm, imtaphy::Spectrum* spectrum);

        
        LinkVector links;
        LinkVector scmLinks;

        StationList baseStations;
        StationList mobileStations;

        LinksPerStation linksPerStation;

        // for each BS a map containing the MSs served by this BS and the corresponding links
        std::map<StationPhy*, LinkMap, StationPhyPtrCompare> servedLinks;
        LinkMap servingLinks; // for each MS the link to its serving BS

        wns::Power referenceTxPower;
        Channel* channel;

        
        linkclassify::LinkClassifierInterface* linkClassifier;
    private:
        wns::pyconfig::View config;
        
        
        
        wns::Position getWrapAroundPosition(wns::Position bsPosition, wns::Position originalMSposition);
        std::vector<wns::geometry::Vector> wraparoundShiftVectors;
               
        wns::logger::Logger logger;
        bool useSCMforRSRP;
    };
    
    
    class LinkManagerStub :
            public LinkManager 
    {
    public:
        LinkManagerStub()
        {
            channel = NULL;
            baseStations = imtaphy::StationList();
            mobileStations = imtaphy::StationList();
        };
            
        void addLink(Link* link, bool isSCM);
        void setServingLink(Link* link, StationPhy* station)
        {
            servingLinks[station] = link;

            LinkMap map = linksPerStation[station];
            map[station] = link;
            linksPerStation[station] = map;
        }
                            
    private:
            
    };
}
#endif
