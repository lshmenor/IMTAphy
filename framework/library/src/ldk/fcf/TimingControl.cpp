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
#include "TimingControl.hpp"

#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Layer.hpp>

#include <WNS/ldk/fcf/CompoundCollector.hpp>
#include <WNS/ldk/fcf/FrameBuilder.hpp>
#include <WNS/logger/Logger.hpp>
//#include <WNS/ldk/fcf/TimingNode.hpp>


STATIC_FACTORY_REGISTER_WITH_CREATOR(
   wns::ldk::fcf::TimingControl,
   wns::ldk::fcf::TimingControlInterface,
   "wns.ldk.fcf.TimingControl",
   wns::ldk::fcf::FrameBuilderConfigCreator );

using namespace wns::ldk::fcf;

TimingControl::TimingControl( FrameBuilder* _frameBuilder, const pyconfig::View& /*config*/ )
    : frameBuilder(_frameBuilder),
      logger("WNS", "TimingControl"),
      running(false)

{
    activeCC = compoundCollectors.end();
}

void TimingControl::start()
{
    activeCC = compoundCollectors.end(); //initinal state
    running = true;

    if ( !this->hasPeriodicTimeoutSet() )
        this->startPeriodicTimeout(frameBuilder->getFrameDuration());

}

void TimingControl::pause()
{
    running = false;
    activeCC = compoundCollectors.end(); //initial state
}

void TimingControl::stop()
{
    this->cancelPeriodicTimeout();
    activeCC = compoundCollectors.end(); //initial state
    running = false;
}

int
TimingControl::getRole(PhaseDescriptorPtr /*p*/)
{
    return 0;
}

void TimingControl::configure()
{
    this->onFUNCreated();
}

void TimingControl::onFUNCreated()
{
    FrameBuilder::Descriptors descriptors (
        frameBuilder->getAllPhaseDescriptors() );
    assure( !descriptors.empty(), "no descriptors are specified" );

    for ( FrameBuilder::Descriptors::const_iterator it = descriptors.begin();
          it != descriptors.end();
          ++it )
    {
        //(*it)->getTimingNode()->setTimingControl( this );
        compoundCollectors.push_back( (*it)->getCompoundCollector() );
    }
    activeCC = compoundCollectors.end();
    MESSAGE_BEGIN(NORMAL, logger, m, "");
    m << compoundCollectors.size() << " compound collectors registered at timing control";
    MESSAGE_END();
}

void TimingControl::nextPhase()
{
    if ( activeCC == compoundCollectors.end() )
    {
        std::stringstream ss;
        ss << "timing inconsistency" << std::endl;
        ss << "nextPhase() called, but no active compound collector in  ";
        ss << getFrameBuilder()->getFUN()->getLayer()->getName() << std::endl;
        throw wns::Exception( ss.str() );
    }
    ++activeCC;

    if ( activeCC != compoundCollectors.end() )
    {
        // update the duration in case it has changed:
        MESSAGE_BEGIN(VERBOSE, logger, m, "");
        m << getFrameBuilder()->getFUN()->getName() << ": Next TimingNode: updating duration lengths";
        MESSAGE_END()
        (*activeCC)->start(0);
    }
    else
    {
        MESSAGE_BEGIN(VERBOSE, logger, m, "");
        m << getFrameBuilder()->getFUN()->getName() << ": Next TimingNode: Last TimingNode called";
        MESSAGE_END()
    }
}

void TimingControl::periodically()
{
    //Notify NewFrame-Observers about newFrame
    frameBuilder->notifyNewFrameObservers();

    // Only continue if running == true
    if ( !running )
        return;

    // Frameduration ends before last timing node is called
    if (    activeCC != compoundCollectors.end())
    {
        MESSAGE_BEGIN(NORMAL, logger, m, "");
        m << getFrameBuilder()->getFUN()->getLayer()->getName()
          << ": FrameBuilder has not received last Frame.";
        MESSAGE_END();
        //std::stringstream ss;
        //ss << getFrameBuilder()->getFUN()->getName();
        //ss << ": Timing nodes are still active at begin of frame. \n";
        //throw wns::Exception(ss.str());
    }

    MESSAGE_BEGIN(NORMAL, logger, m, "");
    m << getFrameBuilder()->getFUN()->getName()<< ": Starting Frame";
    MESSAGE_END();

    for (CompoundCollectors::iterator it = compoundCollectors.begin();
         it != compoundCollectors.end();
         ++it )
    {
        (*it)->startCollection(CompoundCollector::Sending);
    }

    for_each( compoundCollectors.begin(), compoundCollectors.end(),
          std::mem_fun(&CompoundCollectorInterface::finishCollection));

    activeCC = compoundCollectors.begin();
    (*activeCC)->start(0);
}

