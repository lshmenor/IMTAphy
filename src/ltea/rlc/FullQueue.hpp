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

// based on code from openWNS with the following license:

/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2009
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef LTEA_RLC_SEGMENTINGQUEUE_HPP
#define LTEA_RLC_SEGMENTINGQUEUE_HPP

#include <IMTAPHY/ltea/rlc/IQueue.hpp>
#include <IMTAPHY/ltea/Layer2.hpp>
#include <IMTAPHY/ltea/helper/Window.hpp>

#include <WNS/ldk/CommandReaderInterface.hpp>
#include <WNS/ldk/probe/Packet.hpp>


namespace ltea { namespace rlc { 
    
    class FullQueue :
        public IQueue
    {
    public:
        FullQueue(const wns::pyconfig::View& config);
        virtual ~FullQueue();

        virtual bool 
        isAccepting(const wns::ldk::CompoundPtr& compound) const;

        /** @brief compound in */
        virtual void 
        put(const wns::ldk::CompoundPtr& compound);

        virtual unsigned long int 
        numCompoundsForUser(UserID user) const;

        virtual Bit
        numBitsForUser(UserID user) const;

        virtual bool 
        queueHasPDUs(UserID user) const;

        virtual void 
        setFUN(wns::ldk::fun::FUN* fun);

        /** @brief get compound out and do segmentation into #bits (gross) */
        virtual wns::ldk::CompoundPtr 
        getHeadOfLinePDUSegment(UserID user, int bits);

    private:
        wns::logger::Logger logger;
        wns::pyconfig::View config;
        wns::ldk::fun::FUN* myFUN;

        wns::ldk::CommandReaderInterface* pdcpCommandReader;
        wns::ldk::CommandReaderInterface* windowProbeCommandReader;
        wns::ldk::CommandReaderInterface* packetProbeCommandReader;
        ltea::helper::Window* windowProbe;
        wns::ldk::probe::Packet* packetProbe;

        Bit headerSize;
        unsigned long long int sequenceNumber;

        ltea::Layer2* layer;
    };


}}
#endif 

