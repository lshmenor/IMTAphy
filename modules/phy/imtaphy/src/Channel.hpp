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

#ifndef IMTAPHY_CHANNEL_HPP
#define IMTAPHY_CHANNEL_HPP

#include <WNS/Singleton.hpp>
#include <WNS/PowerRatio.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/events/PeriodicTimeout.hpp>
#include <WNS/Subject.hpp>
#include <IMTAPHY/interface/IMTAphyObserver.hpp>
#include <WNS/geometry/AABoundingBox.hpp>

#include <IMTAPHY/StationPhy.hpp>
#include <IMTAPHY/Transmission.hpp>
#include <IMTAPHY/pathloss/PathlossModelInterface.hpp>
#include <IMTAPHY/linkManagement/classifier/LinkClassifierInterface.hpp>
#include <IMTAPHY/spatialChannel/SpatialChannelModelInterface.hpp>
#include <IMTAPHY/Spectrum.hpp>
#include <IMTAPHY/lsParams/LSCorrelation.hpp>

#include <WNS/pyconfig/View.hpp>

#include <vector>

// this could be templated...
#define SCMPRECISION float

namespace imtaphy {
  
    
    class LinkManager;
    //class StationPhy;
  
    class Channel :
            public wns::events::PeriodicTimeout,
            // IMTAphyObserver
            public wns::Subject<imtaphy::interface::IMTAphyObserver>
    {
    public:
        Channel();
        Channel(int dummy);// For unit testing
        virtual void registerStationPhy(StationPhy* station);
        void registerTransmission(TransmissionPtr transmission);
        void onWorldCreated();
    
        // Lookup functions
        StationList getAllBaseStations() const;
        StationList getAllMobileStations() const;

    
        const TransmissionVector& getTransmissionsOnPRB(Direction direction, imtaphy::interface::PRB prb) const;
    
        /**
         * @brief Returns the azimuth angle (in radians -Pi..Pi) as seen from
         position me looking to position other
        */
        virtual double getAzimuth(wns::Position me, wns::Position other) const;
    
        /**
         * @brief Returns the elevation angle (in radians -Pi/2..Pi/2) as seen from
         position me looking to position other. 0 is horizontal looking down is positive.
        */
        virtual double getElevation(wns::Position me, wns::Position other) const;
    
        wns::Ratio computePathloss(imtaphy::Link* link);
        wns::Ratio computeShadowing(imtaphy::Link* link);
    
        virtual void periodically();
    
        LinkManager* getLinkManager() const;
        scm::SpatialChannelModelInterface<SCMPRECISION>* getSpatialChannelModel() const;
        Spectrum* getSpectrum() const;
        
        unsigned int getTTI() const {return tti;}
        
        // functor to call the onNewTTI in the observing stationPhys
        struct OnNewTTI
        {
            OnNewTTI(const unsigned int _tti):
                tti(_tti)
            {}
      
            void operator()(imtaphy::interface::IMTAphyObserver* observer)
            {
                // The functor calls the onNewTTI implemented by the Observer
                observer->onNewTTI(this->tti);
            }
        private:
            unsigned int tti;
        }; // end of onNewTTI functor
  
        // functor to call the onNewTTI in the observing stationPhys
        struct BeforeTTIOver
        {
            BeforeTTIOver(const unsigned int _tti):
                tti(_tti)
            {}
      
            void operator()(imtaphy::interface::IMTAphyObserver* observer)
            {
                // The functor calls the onNewTTI implemented by the Observer
                observer->beforeTTIover(this->tti);
            }
        private:
            unsigned int tti;
        }; 
  
    protected:
  
        StationList baseStations;
        StationList mobileStations;
    
        pathloss::PathlossModelInterface* pathlossModel;
        scm::SpatialChannelModelInterface<SCMPRECISION>* spatialChannelModel;
    
        lsparams::LSCorrelation* lsCorrelation;
        lsparams::LSmap* largeScaleParams;
    
        Spectrum* spectrum;
        LinkManager* linkManager;
       
    
    private:
        void evaluateAllTransmissions() const;
    
        std::vector<TransmissionsPerPRB> transmissionsPerPRB; 
        TransmissionVector allCurrentTransmissions;
    
        // we assume PRBs are counted from 0..numberOfPRBs-1
        wns::pyconfig::View config;
        wns::logger::Logger logger;
    
        unsigned int tti;
        bool initialized;
        unsigned int transmissionIdCounter;
    
    };
      
    typedef wns::SingletonHolder<Channel> TheIMTAChannel;
  
}
#endif
