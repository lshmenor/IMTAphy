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

#ifndef IMTAPHY_STATIONPHY_HPP
#define IMTAPHY_STATIONPHY_HPP

#include <WNS/node/component/Component.hpp>
#include <WNS/node/Interface.hpp>
#include <IMTAPHY/interface/DataTransmission.hpp>
#include <IMTAPHY/interface/DataReception.hpp>
#include <WNS/Position.hpp>
#include <WNS/Subject.hpp>
#include <WNS/Observer.hpp>

#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <IMTAPHY/Spectrum.hpp>
#include <IMTAPHY/antenna/AntennaInterface.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <IMTAPHY/Transmission.hpp>
#include <itpp/base/random.h>

namespace imtaphy {
        
    class Channel;
    
    class StationPhy;
    
    namespace receivers {
        class ReceiverInterface;
    }
    
    typedef std::list<StationPhy*> StationList;
    
    typedef enum {MOBILESTATION, BASESTATION} StationType;
        
    class StationPhy:
            public  wns::node::component::Component,
            public imtaphy::interface::DataTransmission
        
            // The PositionableInterface
    {
    public:
        
        StationPhy(wns::node::Interface* node, const wns::pyconfig::View& pyco);
        StationPhy(wns::node::Interface* node, const wns::pyconfig::View& pyco, int dummy); // For Unit test

        virtual const wns::Position&
        getPosition() const {return position;}
        
        virtual imtaphy::antenna::AntennaInterface* getAntenna() { return antenna;};
                
        StationType getStationType() const {return stationType;}
        
        // to be called from the Channel
        void onData(std::vector<wns::ldk::CompoundPtr> transportBlocks, wns::node::Interface* source,  imtaphy::interface::TransmissionStatusPtr status);
        
        // Component interface
        virtual void
        onNodeCreated();
        
        virtual void
        onWorldCreated();
        
        virtual void
        onShutdown();
        
        virtual void
        doStartup();
                
        virtual void
        channelInitialized();
        
        
        //
        
        unsigned int getStationID() const {return stationID;}
        
        /**
         * @brief Returns the absolute value of the mobile's speed in m/s
         */
        virtual double
        getSpeed() {return speed/3.6;}
                
        /**
         * @brief Returns the direction of travel as an angle in radians between -Pi and Pi
         */
        virtual double
        getDirectionOfTravel() const {return directionOfTravel;} 

        imtaphy::receivers::ReceiverInterface* getReceiver() const {return receiver; }
        
        // imtaphy::interface::DataTransmission interface
        
        virtual void registerTransmission(wns::node::Interface* destination,
                                 const std::vector<wns::ldk::CompoundPtr> &transportBlocks,
                                 unsigned int numberOfLayers,
                                 const imtaphy::interface::PrbPowerPrecodingMap &perPRBpowerAndPrecoding);

        virtual std::vector<wns::node::Interface*> getAssociatedNodes();    
        
    private:
        std::string rxServiceName;
        std::string txServiceName;
        imtaphy::interface::PhyNotify* rxService;
        
        imtaphy::interface::PRBVector availablePRBs;
        wns::logger::Logger logger;
        wns::Position position;
        
        imtaphy::Channel* channel;
        StationType stationType;
        
        imtaphy::antenna::AntennaInterface* antenna;
        double directionOfTravel;
        double speed; // speed in km/h
                
        imtaphy::receivers::ReceiverInterface* receiver;
        imtaphy::Direction transmitDirection;
        
        unsigned int stationID;
        
    };
    
    // helper class to compare StationPhy's not by the pointers but by their station IDs
    // using pointers is fine but can cause undeterministic behavior between runs due to
    // different memory layouts
    class StationPhyPtrCompare
    {
        public:
            bool operator()(const StationPhy* a, const StationPhy* b) const
            {
                return a->getStationID() < b->getStationID();
            }
    };
    
    template <typename T, typename KIND = T>
    struct StationModuleCreator:
            public StationModuleCreator<KIND, KIND>
    {
        virtual KIND* create(StationPhy* station,
                                const wns::pyconfig::View& config)
        {
            return new T(station, config);
        }
    };
    
    template <typename KIND>
    struct StationModuleCreator<KIND, KIND>
    {
    public:
        virtual KIND* create(StationPhy*,
                                const wns::pyconfig::View&) = 0;
            
        virtual ~StationModuleCreator()
        {}
    };
}

#endif
 
