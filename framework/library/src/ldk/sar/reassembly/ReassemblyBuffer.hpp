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
#ifndef WNS_LDK_SAR_REASSEMBLY_REASSEMBLYBUFFER_HPP
#define WNS_LDK_SAR_REASSEMBLY_REASSEMBLYBUFFER_HPP

#include <WNS/ldk/Compound.hpp>
#include <WNS/ldk/CommandReaderInterface.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

#include <set>
#include <map>

namespace wns { namespace ldk { class FunctionalUnit;}}
namespace wns { namespace ldk { namespace sar { class SegAndConcatCommand; }}}

namespace wns { namespace ldk { namespace sar { namespace reassembly {

namespace tests { class ReassemblyBufferTest; }

class ReassemblyBuffer
{
    friend class tests::ReassemblyBufferTest;

public:
    typedef std::deque<wns::ldk::CompoundPtr> SegmentContainer;

    ReassemblyBuffer();

    void
    initialize(wns::ldk::CommandReaderInterface*);

    bool
    isEmpty();

    long
    getNextExpectedSN();

    bool
    isNextExpectedSegment(const wns::ldk::CompoundPtr& c);

    bool
    accepts(const wns::ldk::CompoundPtr&);

    void
    insert(wns::ldk::CompoundPtr);

    std::string
    dump();

    void
    clear();

    size_t
    size();

    SegmentContainer
    getReassembledSegments(int &reassembledSegmentCounter);

    void
    enableDelayProbing(const wns::probe::bus::ContextCollectorPtr& minDelayCC,
        const wns::probe::bus::ContextCollectorPtr& maxDelayCC,
        wns::ldk::CommandReaderInterface* cmdReader);

private:
    bool delayProbingEnabled_;
    std::map<int, std::set<wns::simulator::Time> > delays_;
    wns::probe::bus::ContextCollectorPtr minDelayCC_;
    wns::probe::bus::ContextCollectorPtr maxDelayCC_;
    wns::ldk::CommandReaderInterface* probeCmdReader_;

    void
    prepareForProbing(int position,
        const wns::ldk::CompoundPtr& segment);

    void
    probe(const SegmentContainer& sc);

    bool
    integrityCheck();

    wns::ldk::sar::SegAndConcatCommand*
    readCommand(const wns::ldk::CompoundPtr&);

    int
    dropSegmentsOfSDU(int index = 0);

    SegmentContainer buffer_;

    wns::ldk::CommandReaderInterface* commandReader_;
};

} // reassembly
} // sar
} // ldk
} // wns

#endif // WNS_LDK_SAR_REASSEMBLY_REASSEMBLYBUFFER_HPP
