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

#include <WNS/ldk/sar/reassembly/ReorderingWindow.hpp>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/bind.hpp>

#include <math.h>

boost::lambda::placeholder1_type Arg1;
boost::lambda::placeholder2_type Arg2;

using namespace wns::ldk::sar::reassembly;

ReorderingWindow::Segment::Segment(long sn, wns::ldk::CompoundPtr compound):
    sn_(sn),
    compound_(compound)
{
}

long
ReorderingWindow::Segment::sn() const
{
    return sn_;
}

wns::ldk::CompoundPtr
ReorderingWindow::Segment::compound() const
{
    return compound_;
}

ReorderingWindow::ReorderingWindow(wns::pyconfig::View config):
    snFieldLength_(config.get<int>("snFieldLength")),
    windowSize_(pow(2, config.get<int>("snFieldLength") - 1)),
    tReordering_(config.get<double>("tReordering")),
    vrUH_(0),
    vrUR_(0),
    vrUX_(0),
    reorderingTimer_(),
    logger_(config.get("logger"))
{
    MESSAGE_BEGIN(NORMAL, logger_, m, "ReorderingWindow (snFieldLength=" << snFieldLength_);
    m << ", windowSize_ = " << windowSize_ << ")";
    MESSAGE_END();
    assure(windowSize_ > 0, "Missconfigured windowSize (possible overflow). Try decreasing snFieldLength");
    assure(tReordering_ > 0, "tReordering must be positive");
}

ReorderingWindow::ReorderingWindow(const ReorderingWindow& other):
    snFieldLength_(other.snFieldLength_),
    windowSize_(other.windowSize_),
    tReordering_(other.tReordering_),
    vrUH_(other.vrUH_),
    vrUR_(other.vrUR_),
    vrUX_(other.vrUX_),
    reorderingTimer_(other.reorderingTimer_),
    logger_(other.logger_)
{
}

boost::signals::connection
ReorderingWindow::connectToReassemblySignal(const reassemblySlotType& slot)
{
    return reassemble_.connect(slot);
}

boost::signals::connection
ReorderingWindow::connectToDiscardSignal(const discardSlotType& slot)
{
    return discard_.connect(slot);
}

void
ReorderingWindow::onSegment(long sn, wns::ldk::CompoundPtr compound)
{
    Segment s(sn, compound);

    MESSAGE_BEGIN(NORMAL, logger_, m, "onSegment: ");
    m << "New segment " << s.sn() << " (vrUH_ = " << vrUH_ << " vrUR_=" << vrUR_ << " vrUX_ = " << vrUX_ << ")";
    MESSAGE_END();

    // 3GPP TS 36.322 Section 5.1.2.2.2 (V8.5.0 2009-03)
    // Actions when an UMD PDU is received from lower layer
    if (isExpired(s) || (isConsideredForReordering(s) && isDuplicate(s)))
    {
        discard(s);
        return;
    }

    // place the received UMD PDU in the reception buffer
    insert(s);

    // 3GPP TS 36.322 Section 5.1.2.2.3 (V8.5.0 2009-03)
    // Actions when an UMD PDU is placed in the reception buffer
    if (!isWithinReorderingWindow(s))
    {
        vrUH_ = s.sn() + 1;
        MESSAGE_SINGLE(VERBOSE, logger_, "onSegment: Updated vrUH_=" << vrUH_);
        updateReassemblyBuffer(vrUH_ - windowSize_);

        // If VR(UR) falls outside of the reordering window
        if (!isWithinReorderingWindow(Segment(vrUR_, wns::ldk::CompoundPtr())))
        {
            vrUR_ = vrUH_ - windowSize_;
            MESSAGE_SINGLE(VERBOSE, logger_, "onSegment: Updated vrUR_=" << vrUR_);
        }
    }

    ContainerType::iterator it;
    {
        using namespace boost::lambda;
        it = find_if(reorderWindow_.begin(), reorderWindow_.end(), (bind(&Segment::sn, Arg1) == vrUR_));
    }

    if(it!=reorderWindow_.end())
    {
        vrUR_ = nextMissingSegment(Segment(vrUR_, wns::ldk::CompoundPtr())).sn();
        MESSAGE_SINGLE(VERBOSE, logger_, "onSegment: Updated vrUR_=" << vrUR_);

        updateReassemblyBuffer(vrUR_);
    }

    // t-Reordering Processing
    if (reorderingTimer_ != wns::events::scheduler::IEventPtr() && reorderingTimer_->isQueued())
    {
        // t-Reordering is running
        if ( (vrUX_ <= vrUR_) ||
             (!isWithinReorderingWindow(Segment(vrUX_, wns::ldk::CompoundPtr())) &&
              vrUX_ != vrUH_
             ))
        {
            MESSAGE_SINGLE(NORMAL, logger_, "onSegment: tReordering stopped");
            reorderingTimer_->cancel();
            reorderingTimer_ = wns::events::scheduler::IEventPtr();
        }
    }

    if (reorderingTimer_ == wns::events::scheduler::IEventPtr())
    {
        // t-Reordering is not running
        if (vrUH_ > vrUR_)
        {
            reorderingTimer_ = wns::simulator::getEventScheduler()->scheduleDelay(boost::lambda::bind(&ReorderingWindow::onTReorderingExpired, this), tReordering_);
            vrUX_ = vrUH_;
            MESSAGE_SINGLE(NORMAL, logger_, "onSegment: tReordering started (vrUX = " << vrUX_ << ")");
        }
    }
    MESSAGE_SINGLE(NORMAL, logger_, "onSegment: finished (vrUH_ = " << vrUH_ << " vrUR_=" << vrUR_ << " vrUX_ = " << vrUX_ << ")");
} // onSegment

void
ReorderingWindow::onTReorderingExpired()
{
    // 3GPP TS 36.322 Section 5.1.2.2.4 (V8.5.0 2009-03)

    MESSAGE_SINGLE(NORMAL, logger_, "onTReorderingExpired: (vrUR = " << vrUR_ << " vrUX = " << vrUX_ << ")");
    vrUR_ = nextMissingSegment(Segment(vrUX_ - 1, wns::ldk::CompoundPtr())).sn();

    updateReassemblyBuffer(vrUR_);

    if (vrUH_ > vrUR_)
    {
        reorderingTimer_ = wns::simulator::getEventScheduler()->scheduleDelay(boost::lambda::bind(&ReorderingWindow::onTReorderingExpired, this), tReordering_);
        vrUX_ = vrUH_;
        MESSAGE_SINGLE(NORMAL, logger_, "onTReorderingExpired: tReordering started (vrUX = " << vrUX_ << ")");
    }
    else
    {
        MESSAGE_SINGLE(VERBOSE, logger_, "onTReorderingExpired: Clearing reorderingTimer handle");
        reorderingTimer_ = wns::events::scheduler::IEventPtr();
    }

}

void
ReorderingWindow::insert(Segment s)
{
    MESSAGE_SINGLE(VERBOSE, logger_, "insert: New segment " << s.sn());

    using namespace boost::lambda;

    ContainerType::iterator it;

    // Find position to insert new segment. reorderWindow is sorted ascending by SN
    // See bosst:lambda documentation for bind and Arg1
    it = find_if(this->reorderWindow_.begin(),
                 this->reorderWindow_.end(),
                 (bind(&Segment::sn, Arg1) > s.sn()));

    MESSAGE_SINGLE(VERBOSE, logger_, "insert: Inserting New segment " << s.sn());
    this->reorderWindow_.insert(it, s);
}

bool
ReorderingWindow::isDuplicate(Segment s)
{
    using namespace boost::lambda;

    ContainerType::iterator it;

    // Find segment. reorderWindow is sorted ascending by SN
    // See bosst:lambda documentation for bind and Arg1
    it = find_if(this->reorderWindow_.begin(),
                 this->reorderWindow_.end(),
                 (bind(&Segment::sn, Arg1) == s.sn()));

    if (it!=this->reorderWindow_.end())
    {
        MESSAGE_SINGLE(VERBOSE, logger_, "isDuplicate: Segment " << s.sn() << " is duplicate");
        return true;
    }

    MESSAGE_SINGLE(VERBOSE, logger_, "isDuplicate: Segment " << s.sn() << " is not duplicate");
    return false;
}

bool
ReorderingWindow::isExpired(Segment s)
{

    bool expired = (s.sn() >= (this->vrUH_ - this->windowSize_)) &&
                   (s.sn() < this->vrUR_);

    if (expired)
    {
        MESSAGE_SINGLE(VERBOSE, logger_, "isExpired: Segment " << s.sn() << " is expired (vrUH_ = " << vrUH_ << " vrUR_ = " << vrUR_ << ")");
        return true;
    }

    MESSAGE_SINGLE(VERBOSE, logger_, "isExpired: Segment " << s.sn() << " is not expired");
    return false;
}

bool
ReorderingWindow::isConsideredForReordering(Segment s)
{
    bool aboveEarliestEdge = s.sn() > this->vrUR_;
    bool belowUpperEdge = s.sn() < this->vrUH_;

    bool considered = aboveEarliestEdge && belowUpperEdge;

    if (considered)
    {
        MESSAGE_SINGLE(VERBOSE, logger_, "isConsideredForReordering: Segment " << s.sn() << " is considered");
        return true;
    }

    MESSAGE_SINGLE(VERBOSE, logger_, "isConsideredForReordering: Segment " << s.sn() << " is not considered");
    return false;
}

bool
ReorderingWindow::isWithinReorderingWindow(Segment s)
{
    bool aboveLowerEdge = s.sn() >= (vrUH_ - windowSize_);
    bool belowUpperEdge = s.sn() < vrUH_;

    bool inWindow = aboveLowerEdge && belowUpperEdge;

    if (inWindow)
    {
        MESSAGE_SINGLE(VERBOSE, logger_, "isWithinReorderingWindow: Segment " << s.sn() << " is in window");
        return true;
    }

    MESSAGE_SINGLE(VERBOSE, logger_, "isWithinReorderingWindow: Segment " << s.sn() << " is not in window");
    return false;
}

void
ReorderingWindow::discard(Segment s)
{
    MESSAGE_SINGLE(NORMAL, logger_, "Discarding segment " << s.sn());

    // Send discard signal
    discard_(s.sn(), s.compound());
}

ReorderingWindow::Segment
ReorderingWindow::nextMissingSegment(Segment s)
{
    long ii=(s.sn() + 1);
    while(true)
    {
        if (!isDuplicate(Segment(ii, wns::ldk::CompoundPtr())))
        {
            return Segment(ii, wns::ldk::CompoundPtr());
        }
        ii += 1;
    }
}

void
ReorderingWindow::updateReassemblyBuffer(long lower)
{
    {
        using namespace boost::lambda;
        sort(reorderWindow_.begin(), reorderWindow_.end(),
             (bind(&Segment::sn, Arg1) < bind(&Segment::sn, Arg2)));
    }

    ContainerType::iterator it;
    for(it = reorderWindow_.begin(); it != reorderWindow_.end(); /* noop */)
    {
        if (it->sn() < lower)
        {
            MESSAGE_SINGLE(VERBOSE, logger_, "Moving segment " << it->sn() << " to reassembly Buffer");

            // Fire reassemble signal
            reassemble_(it->sn(), it->compound());

            // Now remove from list and adjust iterator
            it = reorderWindow_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
