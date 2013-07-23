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

#ifndef IMTAPHY_TRANSMISSION_HPP
#define IMTAPHY_TRANSMISSION_HPP

#include <WNS/PowerRatio.hpp>
#include <WNS/ldk/Compound.hpp>
#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <IMTAPHY/interface/DataTransmission.hpp>
#include <IMTAPHY/detail/LinearAlgebra.hpp>
#include <boost/shared_ptr.hpp>
#include <IMTAPHY/Spectrum.hpp>

namespace imtaphy {

    class Link;
    class StationPhy;
    

    
    class Transmission
    {
    public:
        
        // This is a dirty trick to allow deriving a stub without all these members
        Transmission() {};
        
        Transmission(imtaphy::Direction direction,
                     const std::vector<wns::ldk::CompoundPtr> &transportBlocks,
                     imtaphy::Link* link,
                     imtaphy::StationPhy* source, 
                     imtaphy::StationPhy* destination,
                     const imtaphy::interface::PrbPowerPrecodingMap &perPRBpowerAndPrecoding,
                     unsigned int numTxAntennas,
                     unsigned int numLayers
            );
            
        std::vector<wns::ldk::CompoundPtr> getTransportBlocks() const {return transportBlocks;}
        imtaphy::Link* getLink() const {return link;}
        imtaphy::StationPhy* getDestination() const {return destination;}
        imtaphy::StationPhy* getSource() const {return source;}
        imtaphy::Direction getDirection() const {return direction;}
        
        imtaphy::detail::ComplexFloatMatrixPtr getPrecodingMatrix(imtaphy::interface::PRB prb) const 
        {
            assure(perPRBpowerAndPrecoding.find(prb) != perPRBpowerAndPrecoding.end(), "Invalid PRB requested");
            return perPRBpowerAndPrecoding.find(prb)->second.precoding;
        }
        
        wns::Power getTxPower(imtaphy::interface::PRB prb) const 
        {
            assure(perPRBpowerAndPrecoding.find(prb) != perPRBpowerAndPrecoding.end(), "Invalid PRB requested");
        
            return perPRBpowerAndPrecoding.find(prb)->second.power;
        }

        const imtaphy::interface::PrbPowerPrecodingMap& getPrbPowerPrecodingMap() const {return perPRBpowerAndPrecoding;} 
        
        unsigned int getNumTxAntennas() const {return numTxAntennas;}
        unsigned int getNumLayers() const {return numLayers;}
        
        unsigned int getId() const {return id;}
        void setId(unsigned int id_) {id = id_;}
        
    protected:
        imtaphy::Direction direction; // UL or DL
        std::vector<wns::ldk::CompoundPtr> transportBlocks;
        imtaphy::StationPhy* source;
        imtaphy::StationPhy* destination;
        imtaphy::Link* link;  
        imtaphy::interface::PrbPowerPrecodingMap perPRBpowerAndPrecoding;
        unsigned int numTxAntennas;
        unsigned int numLayers;
        unsigned int id;
    };    

    typedef boost::shared_ptr<Transmission> TransmissionPtr;
    // list might be more intuitive but is slower
    typedef std::vector<TransmissionPtr> TransmissionVector; 
    typedef std::vector<TransmissionVector> TransmissionsPerPRB;

}
#endif

