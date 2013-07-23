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

#ifndef LTEA_L2S_HARQ_CHASECOMBININGDECODER
#define LTEA_L2S_HARQ_CHASECOMBININGDECODER

#include <IMTAPHY/ltea/l2s/harq/DecoderInterface.hpp>
#include <IMTAPHY/link2System/BlockErrorModel.hpp>
#include <IMTAPHY/link2System/MMIBeffectiveSINR.hpp>
#include <IMTAPHY/link2System/MMSE-FDE.hpp>
#include <WNS/distribution/Uniform.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

namespace ltea { namespace l2s { namespace harq {

    class ChaseCombiningDecoder :
        public DecoderInterface,
        public wns::Cloneable<ChaseCombiningDecoder>
    {
    public:
        ChaseCombiningDecoder(const wns::pyconfig::View& config);
        
        virtual bool canDecode(SoftCombiningBuffer &receivedRedundancyVersions);

    private:
        imtaphy::l2s::BlockErrorModel* blerModel;
        imtaphy::l2s::MMIBeffectiveSINR* effSinrModel;
        imtaphy::l2s::MMSEFrequencyDomainEqualization uplinkMMSEFDE;
        

        wns::distribution::Uniform uniformRandom;
        wns::ldk::CommandReaderInterface* dciReader;
        wns::logger::Logger logger;
        wns::probe::bus::ContextCollector* effSINRContextCollector;
        wns::probe::bus::ContextCollectorPtr linkAdaptationMismatchContextCollector;

    };
}}}


#endif 


