
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
#include <WNS/ldk/fcf/FrameBuilder.hpp>

#include <WNS/Exception.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/ldk/Layer.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/ldk/fcf/CompoundCollector.hpp>
#include <WNS/ldk/fcf/PhaseDescriptor.hpp>
#include <WNS/ldk/fcf/TimingControl.hpp>


STATIC_FACTORY_REGISTER_WITH_CREATOR(
    wns::ldk::fcf::FrameBuilder,
    wns::ldk::FunctionalUnit,
    "wns.ldk.fcf.FrameBuilder",
    wns::ldk::FUNConfigCreator );

using namespace wns::ldk::fcf;
using namespace std;


FrameBuilder::FrameBuilder( wns::ldk::fun::FUN* fun, const wns::pyconfig::View& config ) :
    wns::ldk::multiplexer::Dispatcher( fun, config ),
    wns::ldk::fcf::NewFrameProvider(fun->getName()),
    timingControl_(NULL),
    logger("WNS", "FrameBuilder")
{
    frameDuration_ = config.get<simTimeType>("frameDuration");
    symbolDuration_ = config.get<simTimeType>("symbolDuration");

    // Create Timing Control from static Factory
    wns::pyconfig::View timingControlConfig( config, "timingControl");
    std::string name = timingControlConfig.get<std::string>("name");
    timingControl_ =  TimingControlFactory::creator(name)->create( this , timingControlConfig);

    // Create the phase descriptors 
    for(int i = 0; i < config.len("phaseDescriptor"); i++)
    {
        wns::pyconfig::View pDesc(config.get("phaseDescriptor", i));
        std::string plugin = pDesc.get<std::string>("__plugin__");
        PhaseDescriptorCreator* pCreator = PhaseDescriptorFactory::creator(plugin);
        PhaseDescriptorPtr pDescPtr(pCreator->create(this, pDesc));
        descriptors_.push_back(pDescPtr);
    }
}

FrameBuilder::~FrameBuilder()
{
    delete timingControl_;
}

void FrameBuilder::onFUNCreated()
{
    for(FrameBuilder::Descriptors::const_iterator it = descriptors_.begin(); 
        it != descriptors_.end(); 
        ++it)
    {
        (*it)->onFUNCreated();
    }

    // timing node must be configured after phase descriptors
    timingControl_->configure();
}

void FrameBuilder::start()
{
    MESSAGE_BEGIN(NORMAL, logger,  m, "" );
    m << getFUN()->getName() << ": starting FrameBuilder";
    MESSAGE_END();

    timingControl_->start();
}

void FrameBuilder::pause()
{
    MESSAGE_BEGIN(NORMAL, logger,  m, "" );
    m << getFUN()->getName() <<": pausing FrameBuilder ";
    MESSAGE_END();

    timingControl_->pause();
}

void FrameBuilder::stop()
{
    MESSAGE_BEGIN(NORMAL, logger,  m, "" );
    m << getFUN()->getName() << ": stopping FrameBuilder";
    MESSAGE_END();

    timingControl_->stop();
}


void FrameBuilder::finishedPhase( CompoundCollectorInterface* collector )
{
    getTimingControl()->finishedPhase( collector );
}


