/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
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

#ifndef WNS_SCHEDULER_QUEUE_DETAIL_INNERQUEUE_HPP
#define WNS_SCHEDULER_QUEUE_DETAIL_INNERQUEUE_HPP

#include <WNS/simulator/Bit.hpp>
#include <WNS/ldk/Compound.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>
#include <queue>

namespace wns { namespace scheduler { namespace queue { namespace detail {

class InnerQueue
{
public:
    /**
     * @brief Thrown if a too small segment is requested
     */
    class RequestBelowMinimumSize :
        public Exception
    {
    public:
        explicit
        RequestBelowMinimumSize(const Bit size, const Bit headerSize) :
            Exception("Request is below minimum size.\n")
        {
            (*this) << "The minimum size to request is "
                    << headerSize << " but " << size << " bits were requested.";
        }

        virtual
        ~RequestBelowMinimumSize() throw()
        {
        }
    };

    /**
     * @brief Thrown if an error occured when retrieving segments from the queue
     */
    class RetrieveException :
        public Exception
    {
    public:
        explicit
            RetrieveException(std::string reason) :
            Exception("An exception occured when trying to retrieve data from InnerQueue:\n")
        {
            (*this) << "Reason : " << reason;
        }

        virtual
            ~RetrieveException() throw()
        {
        }
    };

    InnerQueue();

    /**
     * @brief The current queue length in Bits
     * @returns The current length of the queue in bits
     */
    Bit
    queuedNettoBits() const;

    /**
     * @brief How many bits will be delivered if all queued compounds are
     * delivered in one PDU
     *
     * Brutto bits are calculated as follows:
     *
     * bruttoBits = fixedHeaderSize + (N-1) * extensionHeaderSize + getNettoBits()
     *
     * where N is the number of queued compounds
     *
     * Optionally the header size is increased in the end to be byte aligned.
     */
    Bit
    queuedBruttoBits(Bit fixedHeaderSize, Bit extensionHeaderSize, bool byteAlignHeader) const;

    /**
     * @brief Get number of compounds in queue
     */
    int
    queuedCompounds() const;

    /**
     * @brief Is the queue empty ?
     */
    bool
    empty() const;

    /**
     * @brief Append a compound to the end of the queue
     */
    void
    put(const wns::ldk::CompoundPtr& compound);

    /**
     * @brief Retrieve a new compound from the front of the queue of required length (brutto) and segment and concatenate as necessary.
     */
    wns::ldk::CompoundPtr
    retrieve(Bit requestedBits, 
        Bit fixedHeaderSize, 
        Bit extensionHeaderSize, 
        bool usePadding, 
        bool byteAlignHeader, 
        wns::ldk::CommandReaderInterface* reader,
        const wns::probe::bus::ContextCollectorPtr& = wns::probe::bus::ContextCollectorPtr(),
        wns::ldk::CommandReaderInterface* = NULL);

    /** @brief Retrieves a copy of the raw queue. Queued compounds are not copied but
    * point to the same ones as in the original queue!
    **/
    std::queue<wns::ldk::CompoundPtr> 
    getQueueCopy();

private:
    void 
    probe(const wns::ldk::CompoundPtr& compound,
        const wns::probe::bus::ContextCollectorPtr& probeCC,
        wns::ldk::CommandReaderInterface* cmdReader);

    typedef std::queue<wns::ldk::CompoundPtr> CompoundContainer;

    CompoundContainer pduQueue_;

    Bit nettoBits_;

    long sequenceNumber_;

    Bit frontSegmentSentBits_;
};

} // detail
} // queue
} // scheduler
} // wns

#endif // WNS_SCHEDULER_QUEUE_DETAIL_INNERQUEUE_HPP
