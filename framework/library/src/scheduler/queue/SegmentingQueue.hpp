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

#ifndef WNS_SCHEDULER_QUEUE_SEGMENTINGQUEUE_HPP
#define WNS_SCHEDULER_QUEUE_SEGMENTINGQUEUE_HPP

#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/queue/QueueInterface.hpp>
#include <WNS/scheduler/queue/ISegmentationCommand.hpp>
#include <WNS/scheduler/queue/detail/InnerQueue.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/ldk/CommandTypeSpecifier.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>

#include <map>
#include <list>

namespace wns { namespace scheduler { namespace queue {

            /** @brief the queues handled by this class all use a FIFO strategy
                the queues are organized by ConnectionID cid (several per user) */
            class SegmentingQueue :
            public QueueInterface
            {
            public:
                SegmentingQueue(wns::ldk::HasReceptorInterface*, const wns::pyconfig::View& config);
                virtual ~SegmentingQueue();

                bool isAccepting(const wns::ldk::CompoundPtr& compound) const;
                /** @brief compound in */
                void put(const wns::ldk::CompoundPtr& compound);

                UserSet getQueuedUsers() const;
                ConnectionSet getActiveConnections() const;

                unsigned long int numCompoundsForCid(ConnectionID cid) const;
                unsigned long int numBitsForCid(ConnectionID cid) const;

                QueueStatusContainer getQueueStatus(bool forFuture) const;

                /** @brief compound out */
                wns::ldk::CompoundPtr getHeadOfLinePDU(ConnectionID cid);
                int getHeadOfLinePDUbits(ConnectionID cid);

                bool isEmpty() const;
                bool hasQueue(ConnectionID cid);
                bool queueHasPDUs(ConnectionID cid) const;
                ConnectionSet filterQueuedCids(ConnectionSet connections);

                void setColleagues(RegistryProxyInterface* _registry);
                /** @brief needed for probes */
                void setFUN(wns::ldk::fun::FUN* fun);

                std::string printAllQueues();

                ProbeOutput resetAllQueues();
                ProbeOutput resetQueues(UserID user);
                ProbeOutput resetQueue(ConnectionID cid);

                void frameStarts();

                /** @brief true if getHeadOfLinePDUSegment() is supported */
                bool supportsDynamicSegmentation() const { return true; }
                /** @brief get compound out and do segmentation into #bits (gross) */
                wns::ldk::CompoundPtr getHeadOfLinePDUSegment(ConnectionID cid, int bits);
                /** @brief if supportsDynamicSegmentation, this is the minimum size of a segment in bits */
                int getMinimumSegmentSize() const { return minimumSegmentSize; };

                /** @brief Retrieves a copy of the queue for a CID. Queued compounds are not copied but
                * point to the same ones as in the original queue!
                **/
                std::queue<wns::ldk::CompoundPtr> 
                getQueueCopy(ConnectionID cid);

                unsigned long int 
                getMinimumSegmentSize(){ return minimumSegmentSize;};

                Bit
                getFixedHeaderSize(){ return fixedHeaderSize; };

                Bit
                getExtensionHeaderSize(){ return fixedHeaderSize; };

                bool
                getByteAlignHeader(){ return byteAlignHeader; };

            protected:
                void
                probe();

            private:
                wns::probe::bus::ContextCollectorPtr sizeProbeBus;
                wns::probe::bus::ContextCollectorPtr overheadProbeBus;

                wns::probe::bus::ContextCollectorPtr delayProbeBus;
                wns::ldk::CommandReaderInterface* probeHeaderReader;

                wns::ldk::CommandReaderInterface* segmentHeaderReader;
                wns::logger::Logger logger;
                wns::pyconfig::View config;
                wns::ldk::fun::FUN* myFUN;

                long int maxSize;
                unsigned long int minimumSegmentSize;

                typedef std::map<ConnectionID, detail::InnerQueue> QueueContainer;
                QueueContainer queues;

                typedef std::map<ConnectionID, int> FixedOverheadContainer;

                FixedOverheadContainer fixedOverhead;

                struct Colleagues {
                    RegistryProxyInterface* registry;
                } colleagues;

                Bit fixedHeaderSize;

                Bit extensionHeaderSize;

                bool usePadding;

                bool byteAlignHeader;

                bool isDropping;
            };


        }}} // namespace wns::scheduler::queue
#endif // WNS_SCHEDULER_QUEUE_SEGMENTINGQUEUE_HPP


