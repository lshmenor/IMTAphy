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

#include <WNS/ldk/harq/softcombining/UniformRandomDecoder.hpp>

#include <WNS/distribution/Uniform.hpp>
#include <WNS/ldk/FUNConfigCreator.hpp>

using namespace wns::ldk::harq::softcombining;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    UniformRandomDecoder,
    IDecoder,
    "UniformRandomDecoder",
    wns::ldk::FUNConfigCreator);

UniformRandomDecoder::UniformRandomDecoder(wns::ldk::fun::FUN*, const wns::pyconfig::View& config):
    initialPER_(config.get<double>("initialPER")),
    rolloffFactor_(config.get<double>("rolloffFactor")),
    logger_(config.get("logger")),
    dis_(new wns::distribution::StandardUniform())
{
}

UniformRandomDecoder::~UniformRandomDecoder()
{
}

void
UniformRandomDecoder::onFUNCreated()
{

}

bool
UniformRandomDecoder::canDecode(const Container<wns::ldk::CompoundPtr>& c)
{
    int numTransmissions = 0;

    for (int ii=0; ii < c.getNumRVs(); ++ii)
    {
        numTransmissions += c.getEntriesForRV(0, ii).size();
    }

    double threshold = pow(initialPER_, numTransmissions * rolloffFactor_);

    MESSAGE_BEGIN(NORMAL, logger_, m, "");
    m << "Effective PER is " << threshold;
    MESSAGE_END();

    if ((*dis_)() > threshold)
    {
        return true;
    }
    return false;
}
