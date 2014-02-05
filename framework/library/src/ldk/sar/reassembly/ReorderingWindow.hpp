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

#ifndef WNS_LDK_SAR_REASSEMBLY_REORDERINGWINDOW_HPP
#define WNS_LDK_SAR_REASSEMBLY_REORDERINGWINDOW_HPP

#include <WNS/pyconfig/View.hpp>
#include <WNS/events/scheduler/IEvent.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/ldk/Compound.hpp>

#include <boost/signals.hpp>

namespace wns { namespace ldk { namespace sar { namespace reassembly {

    class ReorderingWindow
    {
        class Segment
        {
        public:
            Segment(long sn, wns::ldk::CompoundPtr compound);

            long
            sn() const;

            wns::ldk::CompoundPtr
            compound() const;

        private:
            long sn_;

            wns::ldk::CompoundPtr compound_;
        };

        typedef std::deque<Segment> ContainerType;

        typedef boost::signal<void (long, wns::ldk::CompoundPtr)> reassemblySignalType;
        typedef reassemblySignalType::slot_type reassemblySlotType;

        typedef boost::signal<void (long, wns::ldk::CompoundPtr)> discardSignalType;
        typedef discardSignalType::slot_type discardSlotType;

    public:
        ReorderingWindow(wns::pyconfig::View config);

        ReorderingWindow(const ReorderingWindow&);

        void
        onSegment(long sn, wns::ldk::CompoundPtr compound);

        /**
         * @brief Whenever a segments are successfully reordered they are
         * reported via the ReassemblySignal.
         */
        boost::signals::connection
        connectToReassemblySignal(const reassemblySlotType& slot);

        /**
         * @brief Whenever a received segment is dicarded it is
         * reported via the DiscardedSignal.
         */
        boost::signals::connection
        connectToDiscardSignal(const reassemblySlotType& slot);

    private:

        void
        onTReorderingExpired();

        bool
        isExpired(Segment s);

        bool
        isDuplicate(Segment s);

        bool
        isConsideredForReordering(Segment s);

        bool
        isWithinReorderingWindow(Segment s);

        void
        insert(Segment s);

        void
        discard(Segment s);

        Segment
        nextMissingSegment(Segment s);

        void
        updateReassemblyBuffer(long lower);

        int snFieldLength_;

        int windowSize_;

        double tReordering_;

        long vrUH_;

        long vrUR_;

        long vrUX_;

        ContainerType reorderWindow_;

        reassemblySignalType reassemble_;

        discardSignalType discard_;

        wns::events::scheduler::IEventPtr reorderingTimer_;

        wns::logger::Logger logger_;
    };

} // reassembly
} // sar
} // ldk
} // wns
#endif // WNS_LDK_SAR_REASSEMBLY_REORDERINGWINDOW_HPP
