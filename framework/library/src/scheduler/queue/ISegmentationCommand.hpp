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

#ifndef WNS_SCHEDULER_QUEUE_ISEGMENTATIONCOMMAND_HPP
#define WNS_SCHEDULER_QUEUE_ISEGMENTATIONCOMMAND_HPP

#include <WNS/ldk/Command.hpp>

namespace wns { namespace scheduler { namespace queue {
class ISegmentationCommand:
    public wns::ldk::Command
{
public:
    virtual void
    setBeginFlag() = 0;

    virtual bool
    getBeginFlag() = 0;

    virtual void
    clearBeginFlag() = 0;

    virtual void
    setEndFlag() = 0;

    virtual bool
    getEndFlag() = 0;

    virtual void
    clearEndFlag() = 0;

    virtual void
    setSequenceNumber(long) = 0;

    virtual long
    getSequenceNumber() = 0;

    virtual void
    increaseHeaderSize(Bit size) = 0;

    virtual void
    increaseDataSize(Bit size) = 0;

    virtual void
    increasePaddingSize(Bit size) = 0;

    virtual Bit
    headerSize() = 0;

    virtual Bit
    dataSize() = 0;

    virtual Bit
    paddingSize() = 0;

    virtual Bit
    totalSize() = 0;

    virtual void
    addSDU(wns::ldk::CompoundPtr) = 0;

    virtual unsigned int
    getNumSDUs() = 0;

};

} // queue
} // scheduler
} // wns

#endif // WNS_SCHEDULER_QUEUE_ISEGMENTATIONCOMMAND_HPP
