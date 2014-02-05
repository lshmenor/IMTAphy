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

#ifndef IMTAPHY_LINK
#define IMTAPHY_LINK

#include <WNS/Position.hpp>
#include <WNS/PowerRatio.hpp>
#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/spatialChannel/SpatialChannelModelInterface.hpp>
#include <IMTAPHY/Spectrum.hpp>

namespace imtaphy { 
    class Channel;
    class LinkManager;
    class LinkManagerStub;
    
    class Link
    {
        friend class LinkManager;
        friend class LinkManagerStub;

    public:
        typedef enum {
            InH, UMi, SMa, UMa, RMa
        } Scenario;
        
        typedef enum{
            INVALID = 99, NLoS = 0, LoS = 1, UMiO2I = 2 // O2I is only valid for UMi scenarios in M.2135 Table A1-7
        } Propagation;
      
        typedef enum {
            Outdoor, Indoor, InVehicle, NotApplicable
        } UserLocation;
                
        static const int NonSCMLink = -1;

        Link() {};
        
        Link(StationPhy* bs, StationPhy* ms, 
             Scenario scenario, Propagation propagation, Propagation outdoorPropagation,
             UserLocation userLocation);
        
        
        StationPhy* getMS() const {return ms;}
        StationPhy* getBS() const {return bs;}
                
        Scenario getScenario() const {return scenario; }
        Propagation getPropagation() const {return propagation;}
        Propagation getOutdoorPropagation() const {return outdoorPropagation;}
        UserLocation getUserLocation() const {return userLocation;}
        wns::Position getWrappedMSposition() const {return wrappedMSposition;}
        
        /**
         * @brief Returns cached wideband loss (pathloss + shadowing) value or 
         computes if called for the first time. Returns loss >= 0dB
        */
        wns::Ratio getWidebandLoss() const
        {
            return widebandLoss;
        }

        wns::Power getRSRP() const
        {
            return RSRP;
        }
        
        
        void updateEffectiveAntennaGains(wns::Ratio effectiveGains)
        {
            widebandLoss = pathloss - shadowing - effectiveGains;
        }

        bool isSCM() const
        {
            if (scmLinkId == -1)
                return false;
            else
                return true;
        }
        
        unsigned int getSCMlinkId()
        {
            assure(scmLinkId != -1, "Not a SCM link");
            
            return scmLinkId;
        }
        
        wns::Ratio getPathloss() { return pathloss;}
        wns::Ratio getShadowing() { return shadowing;}
        
        template <typename PRECISION>
        void initComplexFloatChannelMatrices(imtaphy::Spectrum* spectrum, imtaphy::scm::SpatialChannelModelInterface<PRECISION>*);

        std::complex<float>* getRawChannelMatrix(imtaphy::Direction direction, unsigned int prb) const
        {
            assure(prb < channelMatrices[direction].size(), "Invalid PRB requested");

            return channelMatrices[direction][prb]->data();
        }

        imtaphy::detail::ComplexFloatMatrixPtr getChannelMatrix(imtaphy::Direction direction, unsigned int prb) const
        {
            assure(prb < channelMatrices[direction].size(), "Invalid PRB requested");

            return channelMatrices[direction][prb];
        }

        
    protected:
        StationPhy* bs;
        StationPhy* ms;
        int scmLinkId;
        
        Scenario scenario;
        Propagation propagation;
        UserLocation userLocation;
        Propagation outdoorPropagation;
        
        wns::Position wrappedMSposition;

        
        // reference symbol received power, see 36.214 5.1.1
        // defined as linear average over power contributions of
        // the resource elements carrying cell-specific reference signals
        // the reference point is the antenna connector of the UE
        // if receiver diversity is used by the UE, the reported value shall
        // not be lower than the corresponding RSRP of any of the individual
        // branches.
        wns::Power RSRP; 
        
        // serves as a cache to hold pathloss and shadowing losses
        wns::Ratio widebandLoss;
        wns::Ratio pathloss;
        wns::Ratio shadowing;

        std::vector<std::vector<imtaphy::detail::ComplexFloatMatrixPtr> > channelMatrices;
    };
    
    
    typedef std::vector<Link*> LinkVector;
    typedef std::map<StationPhy*, Link*, StationPhyPtrCompare> LinkMap;
    typedef std::map<StationPhy*, LinkMap, StationPhyPtrCompare> LinksPerStation;
        
    
    // TODO: move stubs into test directory
    class LinkStub:
            public imtaphy::Link
    {
    public:
        LinkStub(StationPhy* bs_, StationPhy* ms_, 
                 Scenario scenario_, Propagation propagation_, Propagation outdoorPropagation_,  UserLocation userLocation_, 
                 wns::Position wrappedMSposition_, wns::Ratio widebandLoss_, int scmLinkId_)
        {
            // init base class members
            bs = bs_;
            ms = ms_;
            scenario = scenario_;
            propagation = propagation_;
            outdoorPropagation = outdoorPropagation_;
            userLocation = userLocation_;
            wrappedMSposition = wrappedMSposition_;
            widebandLoss = widebandLoss_;
            scmLinkId = scmLinkId_;
                        
        }           
    };
    
    class LinkStub2 :
        public imtaphy::Link
    {
        public:
            LinkStub2(wns::Ratio wbLoss)
            {
                widebandLoss = wbLoss;
            }
    };
}
#endif
