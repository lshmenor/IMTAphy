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
#include <WNS/ldk/fcf/PhaseDescriptor.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/ldk/fun/FUN.hpp>
#include <WNS/ldk/Layer.hpp>

#include <WNS/ldk/fcf/FrameBuilder.hpp>
#include <WNS/ldk/fcf/FrameBuilderConfigCreator.hpp>
#include <WNS/ldk/fcf/CompoundCollector.hpp>


STATIC_FACTORY_REGISTER_WITH_CREATOR(
   wns::ldk::fcf::BasicPhaseDescriptor,
   wns::ldk::fcf::PhaseDescriptor,
   "wns.ldk.fcf.BasicPhaseDescriptor",
   wns::ldk::fcf::FrameBuilderConfigCreator );

using namespace wns::ldk::fcf;

PhaseDescriptor::PhaseDescriptor( FrameBuilder* frameBuilder,
                  const pyconfig::View& config ) :
    frameBuilder_( frameBuilder ), config_(config)
{
}

void PhaseDescriptor::onFUNCreated()
{
    // find the friends
    std::string compoundCollectorName =
        config_.get<std::string>("compoundCollector");

    wns::ldk::FunctionalUnit* compoundCollector =
        getFrameBuilder()->getFUN()->getFunctionalUnit( compoundCollectorName );

    compoundCollector_ =
        dynamic_cast<CompoundCollectorInterface*>( compoundCollector );
    assure( compoundCollector_, "cast to CompoundCollector failed" );

    compoundCollector_->setFrameBuilder( getFrameBuilder() );
}


