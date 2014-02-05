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
#include <WNS/ldk/fcf/FrameBuilderPutter.hpp>

STATIC_FACTORY_REGISTER_WITH_CREATOR(
     wns::ldk::fcf::FrameBuilderPutter,
     wns::ldk::FunctionalUnit,
     "wns.ldk.fcf.FrameBuilderPutter",
     wns::ldk::FUNConfigCreator );

using namespace wns::ldk::fcf;

FrameBuilderPutter::FrameBuilderPutter(wns::ldk::fun::FUN* _fun, const wns::pyconfig::View& _config)
    : CommandTypeSpecifier<>(_fun)
{
    frameBuilderName = _config.get<std::string>("frameBuilderName");
}

bool FrameBuilderPutter::doIsAccepting(const wns::ldk::CompoundPtr&) const
{
    assure(0, "FrameBuilderPutter does not accept outgoing compouds");
    return false;
}

void FrameBuilderPutter::doSendData(const wns::ldk::CompoundPtr&)
{
    assure(0, "FrameBuilderPutter does not accept outgoing compounds");
}

void FrameBuilderPutter::doOnData(const wns::ldk::CompoundPtr& _compound)
{
    friends.destination->onData(_compound);
}

void FrameBuilderPutter::onFUNCreated()
{
    friends.destination = getFUN()->getFunctionalUnit(frameBuilderName);
}

