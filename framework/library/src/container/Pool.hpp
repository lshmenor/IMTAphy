/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#ifndef WNS_CONTAINER_POOL_HPP
#define WNS_CONTAINER_POOL_HPP

#include <WNS/Assure.hpp>

#include <WNS/simulator/ISimulator.hpp>
#include <WNS/events/scheduler/Interface.hpp>

#include <set>

#include <iostream>

namespace wns { namespace container {

    /**
     * @brief Bookkeeping for TCP ports
     *
     * Available Ports range from 1 to 65535. O is not available!
     */
    template<typename ID>
    class Pool
    {
        typedef std::set<ID> PoolContainer;
    public:
        /**
         * @brief Full Pool (all 65535 Ports are available == in the
         * pool)
         * @param[in] unbindDelay When ports are freed they are returned to the port pool after unbindDelay time has passed.
         */
        explicit
        Pool(simTimeType _unbindDelay, ID _lowestId, ID _highestId)
            {
                unbindDelay = _unbindDelay;
                highestId = _highestId;
                lowestId = _lowestId;

                maximumCapacity = 0;
                ID tmp = lowestId;
                while(tmp < highestId)
                {
                    ++tmp;
                    ++maximumCapacity;
                }
            }

        /**
         * @brief Destructor
         */
        virtual
        ~Pool()
        {
        }

        /**
         * @brief Tell the pool this Port is used
         */
        void
        bind(ID id)
            {
                assure(id <= highestId, "Please choose a lower ID (<" << highestId);
                assure(id >= lowestId, "Please choose a higher ID (>" << lowestId);
                assure(isAvailable(id), "ID is already bound");
                boundIds.insert(id);

            }

        /**
         * @brief Tell the pool this Port is not used any longer
         */
        void
        unbind(ID id)
            {
                assure(id <= highestId, "Please choose a lower ID (<" << highestId);
                assure(id >= lowestId, "Please choose a higher ID (>" << lowestId);
                assure(!isAvailable(id), "ID is not bound");

                wns::simulator::getEventScheduler()->scheduleDelay(Unbind(this, id), unbindDelay);
            }

        /**
         * @brief Suggest a free port (will not bind the port!)
         *
         * @param[in] lowerBorder start search from here (default 1024)
         *
         * Use suggestPort() with bind() in combination:
         * @code
         * Port p = portPool.suggestPort();
         * portPool.bind(p);
         * @endcode
         */
        ID
        suggestPort() const
        {
            ID id = lowestId;

            // This searches for a free port beginning at lowerBorder.
            while(
                // go on as long as no free port found
                boundIds.find(id) != boundIds.end() &&
                // go on as long as not highest port number reached
                id <= highestId)
            {
                ++id;
            }

            assure(id <= highestId, "No free ID available");
            return id;
        }

        /**
         * @brief Check if a Port is still available
         *
         * @returns true if Port is available, false else
         */
        bool
        isAvailable(ID id) const
        {
            assure(id <= highestId, "Please choose a lower ID (<" << highestId);
            assure(id >= lowestId, "Please choose a higher ID (>" << lowestId);
            return boundIds.find(id) == boundIds.end();
        }

        int
        getCapacity() const
        {
            int capacity = maximumCapacity - boundIds.size();
            assure(capacity >= 0, "Pool allocated more elements than were allowed! " << maximumCapacity << "/" << boundIds.size());
            return capacity;
        }
        /**
         * @brief Get the unbindDelay for this PortPool
         *
         * @returns unbindDelay
         */
        simTimeType
        getUnbindDelay() const
            {
                return unbindDelay;
            }

    private:
        /**
         * @brief Unbind event which is used to delay the port reuse.
         */
        class Unbind
        {
        public:
            /**
             * @brief Constructor
             * @param[in] pp The instance of the port pool we are working on
             * @param[in] port The port that should be freed when event is executed.
             */
            Unbind(Pool<ID>* pp,  ID unbindId)
                {
                    assure(pp, "No valid instance of Pool given!");
                    portPoolInstance = pp;
                    id = unbindId;
                }
            /**
             * @brief This is called by the event handler. The port that was given upon construction is returned to the port pool.
             */
            void
            operator()()
                {
                    portPoolInstance->boundIds.erase(id);
                }

        private:
            /**
             * @brief Instance of the PortPool this event is excuted on.
             */
            Pool<ID>* portPoolInstance;

            /**
             * @brief Which port should be freed when event is executed.
             */
            ID id;
        };

        /**
         * @brief The delay between call to unbind and the time instance when the port is returned to the port pool.
         */
        simTimeType unbindDelay;
        /**
         * @brief Keeps track of bound ports
         */
        PoolContainer boundIds;

        /**
         * @brief The highest possible port number (2^16 - 1)
         */
        ID highestId;

        /**
         * @brief The highest possible port number (2^16 - 1)
         */
        ID lowestId;

        /**
         * @brief The number of IDs that may be bound.
         */
        int maximumCapacity;
    };
} // container
} // wns

#endif // NOT defined WNS_CONTAINER_POOL_HPP


/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/
