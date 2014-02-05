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

#ifndef WNS_SCHEDULER_QUEUE_IQUEUEMANAGER_HPP
#define WNS_SCHEDULER_QUEUE_IQUEUEMANAGER_HPP

#include <WNS/scheduler/queue/QueueInterface.hpp>
#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/ldk/ManagementServiceInterface.hpp>
#include <set>

namespace wns { namespace scheduler { namespace queue {

            typedef std::map<wns::scheduler::ConnectionID, QueueInterface*> QueueContainer;

            /**
             * @brief Interface class for the queue manager used by the QueueProxy.
             * It requires a system specific implementation to map CIDs to queues.
             */            
            class IQueueManager:
                public wns::ldk::ManagementService 
            {
            public:
                IQueueManager(wns::ldk::ManagementServiceRegistry* msr, const wns::pyconfig::View& config) :
                    wns::ldk::ManagementService(msr)
                {};

                virtual ~IQueueManager() {};

                /**
                 * @brief Return all managed queues
                 */
                virtual QueueContainer
                getAllQueues() = 0;

                /**
                 * @brief Get queue for CID
                 */
                virtual QueueInterface*
                getQueue(wns::scheduler::ConnectionID cid) = 0;

                /**
                * @brief Calls wakeUp to fill the queue
                */
                virtual void
                startCollection(wns::scheduler::ConnectionID cid) = 0;
            };
        }}} // namespace wns::scheduler::queue
#endif // WNS_SCHEDULER_QUEUE_IQUEUEMANAGER_HPP


