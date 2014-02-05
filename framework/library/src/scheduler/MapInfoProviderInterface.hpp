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

#ifndef WNS_SCHEDULER_MAPINFOPROVIDERINTERFACE_H
#define WNS_SCHEDULER_MAPINFOPROVIDERINTERFACE_H

#include <WNS/ldk/ldk.hpp>
#include <WNS/scheduler/MapInfoEntry.hpp>
#include <WNS/scheduler/SchedulingMap.hpp>

#include <vector>

namespace wns { namespace scheduler {

        /** @brief base class for system-specific ResourceSchedulers.
            This old interface promises MapInfoCollectionPtr's. */
        class MapInfoProviderInterface {
        public:
            virtual ~MapInfoProviderInterface() {}

            /**
             * @brief Returns a vector with burst start/end times and subBand identifier
             * for every user scheduled by the scheduler. To be used by the map
             * writer to write UL (and DL) maps
             */
            virtual MapInfoCollectionPtr getMapInfo() const = 0;

            virtual int getNumBursts() const = 0;
        };

        /** @brief base class for system-specific ResourceSchedulers.
            This new interface promises SchedulingMaps. */
        class SchedulingMapProviderInterface {
        public:
            virtual ~SchedulingMapProviderInterface() {}

            /**
             * @brief Returns the scheduling result (SchedulingMap).
             * To be used by the map writer to write UL (and DL) maps
             */
            virtual wns::scheduler::SchedulingMapPtr
            getSchedulingMap(int frameNr) const = 0;
        };

}}

#endif


