/*******************************************************************************
 * This file is part of IMTAphy
 * _____________________________________________________________________________
 *
 * Copyright (C) 2011
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

#ifndef LTEA_L2S_HARQ_DECODERINTERFACE
#define LTEA_L2S_HARQ_DECODERINTERFACE


#include <list>
#include <IMTAPHY/interface/TransmissionStatus.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/Cloneable.hpp>

namespace ltea { namespace mac { namespace harq {
    class HARQEntity;
}}}

namespace ltea { namespace l2s { namespace harq {

    typedef std::pair<wns::ldk::CompoundPtr, imtaphy::interface::TransmissionStatusPtr> RedundancyVersionStatus;
    typedef std::list<RedundancyVersionStatus> SoftCombiningBuffer;
    
    class DecoderInterface :
        public virtual wns::CloneableInterface
    {
    public:
        DecoderInterface(const wns::pyconfig::View& config) :
            entity(NULL)
        {};
        
        virtual bool canDecode(SoftCombiningBuffer &receivedRedundancyVersions) = 0;

        virtual void setEntity(ltea::mac::harq::HARQEntity* entity_) {entity = entity_;}
    protected:
        ltea::mac::harq::HARQEntity* entity;
    };
}}}


#endif 


