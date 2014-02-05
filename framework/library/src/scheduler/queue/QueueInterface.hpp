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

#ifndef WNS_SCHEDULER_QUEUE_QUEUEINTERFACE_HPP
#define WNS_SCHEDULER_QUEUE_QUEUEINTERFACE_HPP

#include <WNS/scheduler/SchedulerTypes.hpp>
#include <WNS/scheduler/RegistryProxyInterface.hpp>
#include <WNS/HasReceptorConfigCreator.hpp>
#include <queue>

namespace wns { namespace scheduler { namespace queue {

            /**
             * @brief Interface class for the queue module of the scheduler framework
             *
             * Like with all scheduler modules internal data types are used @see
             * SchedulerTypes.hpp
             * <ul>
             * <li> wns::scheduler::UserID to distinguish users (SSs, UTs, etc)
             * <li> wns::scheduler::ConnectionID to distinguish logical connections of
             * one or multiple users
             * </ul>
             * queues are distinguished by index cid=ConnectionID (=FlowID in WinProSt)
             */

            class QueueInterface {
            public:
                struct ProbeOutput{
                    ProbeOutput()
                        : bits(0),
                          compounds(0)
                    {}
                    Bits bits;
                    int compounds;
                };

                virtual ~QueueInterface() {};

                /**
                 * @brief
                 *
                 * Returns true if the queue belonging to cid has backlogged PDUs.
                 */
                virtual bool queueHasPDUs(ConnectionID cid) const = 0;

                /**
                 * @brief
                 *
                 * Returns the set of connections for which the queue has backlogged PDUs.
                 */
                virtual ConnectionSet filterQueuedCids(ConnectionSet connections) = 0;

                /**
                 * @brief
                 *
                 * Delivers the PDU that is the head-of-line element, e.g. the oldest
                 * entry of the queue, in the queue specified by cid. The exact sorting
                 * criterion for the queues (FIFO/LIFO etc.) depends on the specific
                 * implementation of the queue.
                 */
                virtual wns::ldk::CompoundPtr getHeadOfLinePDU(ConnectionID cid) = 0;

                /**
                 * @brief
                 *
                 * Delivers the size in bits of the PDU that will be returned by the
                 * next call to getHeadOfLinePDU(ConnectionID cid)
                 */
                virtual int getHeadOfLinePDUbits(ConnectionID cid) = 0;

                /**
                 * @brief
                 *
                 * Return True if there is no compound in the queue
                 */
                virtual bool isEmpty() const = 0;

                /**
                 * @brief
                 *
                 * Return True if a queue for this cid exists.
                 */
                virtual bool hasQueue(ConnectionID cid) = 0;

                /**
                 * @brief
                 *
                 * Resets all internal queues and deletes all backlogged PDUs.
                 */
                virtual ProbeOutput resetAllQueues() = 0;

                /**
                 * @brief
                 *
                 * Resets all queues belonging to the given user and deletes all
                 * backlogged PDUs in these queues.
                 */
                virtual ProbeOutput resetQueues(UserID user) = 0;

                /**
                 * @brief May be called to signal that a frame starts
                 */
                virtual void frameStarts() = 0;

                /**
                 * @brief
                 *
                 * Resets only the queue belonging to the given ConnectionID and all
                 * backlogged PDUs from this queue.
                 */
                virtual ProbeOutput resetQueue(ConnectionID cid) = 0;

                /** @brief true if getHeadOfLinePDUSegment() is supported */
                virtual bool supportsDynamicSegmentation() const = 0;
                /** @brief get compound out and do segmentation into #bits (gross) */
                virtual wns::ldk::CompoundPtr getHeadOfLinePDUSegment(ConnectionID cid, int bits) = 0;
                /** @brief if supportsDynamicSegmentation, this is the minimum size of a segment in bits */
                virtual int getMinimumSegmentSize() const { return 0;};

                /** @brief Retrieves a copy of the queue for a CID. Queued compounds are not copied but
                * point to the same ones as in the original queue!
                **/
                virtual std::queue<wns::ldk::CompoundPtr> 
                getQueueCopy(ConnectionID cid) = 0;

                /**
                 * @brief
                 *
                 * Returns a UserSet of all users who have backlogged PDUs in at least
                 * one of their queues.
                 */
                virtual UserSet getQueuedUsers() const = 0;

                /**
                 * @brief
                 *
                 * Returns a ConnectionSet of all connections who have backlogged PDUs in his queue.
                 */
                virtual ConnectionSet getActiveConnections() const = 0;

                /**
                 * @brief
                 *
                 * Returns the number of Compounds the Queue has stored for a
                 * certain user
                 */
                virtual unsigned long int numCompoundsForCid(ConnectionID cid) const = 0;

                /**
                 * @brief
                 *
                 * Returns the number of Bits schedules for one user
                 */
                virtual unsigned long int numBitsForCid(ConnectionID cid) const = 0;

                /**
                 * @brief
                 *
                 * Returns the a container of QueueStatus for each cid
                 */
                virtual QueueStatusContainer getQueueStatus(bool forFuture) const = 0;

                /**
                 * @brief
                 *
                 * Has to be called before calling put(CompoundPtr) to see whether the
                 * queue module can still accept the PDU. According to the specific
                 * implementation the acceptance may be determined by the fill level of
                 * the correspondig queue.
                 * called in doIsAccepting(..) of the ResourceScheduler-FU.
                 */
                virtual bool isAccepting(const wns::ldk::CompoundPtr& compound) const = 0;

                /**
                 * @brief
                 *
                 * Is used to store a PDU in the queue.  Before calling put(compound), a
                 * call to isAccepting(compound) has to be performed. The queue module
                 * has to retrieve the corresponding UserID and ConnctionID on its own.
                 */
                virtual void put(const wns::ldk::CompoundPtr& compound) = 0;

                /**
                 * @brief
                 *
                 * Gives the queue module access to the RegistryProxy
                 */
                virtual void setColleagues(RegistryProxyInterface* registry) = 0;

                /**
                 * @brief
                 *
                 * Gives the queue module access to the FUN
                 */
                virtual void setFUN(wns::ldk::fun::FUN* fun) = 0;

                /**
                 * @brief print number of bits and pdus in each queue
                 */
                virtual std::string printAllQueues() = 0;

            };
            //typedef wns::PyConfigViewCreator<QueueInterface, QueueInterface> QueueCreator;
            typedef wns::HasReceptorConfigCreator<QueueInterface, QueueInterface> QueueCreator;
            typedef wns::StaticFactory<QueueCreator> QueueFactory;



        }}} // namespace wns::scheduler::queue
#endif // WNS_SCHEDULER_QUEUE_QUEUEINTERFACE_HPP


