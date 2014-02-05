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

#include <WNS/ldk/arq/statuscollector/TwoSizesWindowed.hpp>
#include <WNS/logger/Logger.hpp>

using namespace wns::ldk::arq::statuscollector;

STATIC_FACTORY_REGISTER_WITH_CREATOR(TwoSizesWindowed, 
                     Interface,
                     "StatusCollectorTwoSizesWindowed",
                     wns::ldk::PyConfigCreator);

TwoSizesWindowed::TwoSizesWindowed(const wns::pyconfig::View& config) :
        logger(config.get("logger")),
        windowSize(config.get<wns::simulator::Time>("windowSize")),
        minSamples(config.get<int>("minSamples")),
        insufficientSamplesReturn(config.get<double>("insufficientSamplesReturn")),
        frameSizeThreshold(config.get<Bit>("frameSizeThreshold"))
{
    smallFrames = new wns::SlidingWindow(windowSize);
    bigFrames = new wns::SlidingWindow(windowSize);
}

TwoSizesWindowed::~TwoSizesWindowed()
{
    delete smallFrames;
    delete bigFrames;
}

void TwoSizesWindowed::reset()
{
    smallFrames->reset();
    bigFrames->reset();
}

void TwoSizesWindowed::onSuccessfullTransmission(const CompoundPtr& compound)
{
    if(compound->getLengthInBits() > frameSizeThreshold)
    {
        bigFrames->put(1.0);
    }
    else
    {
        smallFrames->put(1.0);
    }
}

void TwoSizesWindowed::onFailedTransmission(const CompoundPtr& compound)
{
    if(compound->getLengthInBits() > frameSizeThreshold)
    {
        bigFrames->put(0.0);
    }
    else
    {
        smallFrames->put(0.0);
    }
}

double TwoSizesWindowed::getSuccessRate(const CompoundPtr& compound)
{
    wns::SlidingWindow* window;
    if(compound->getLengthInBits() > frameSizeThreshold)
    {
        window = bigFrames;
    }
    else
    {
        window = smallFrames;
    }

    if(window->getNumSamples() < minSamples)
    {
        return(insufficientSamplesReturn);
    }
    else
    {
        return(window->getAbsolute() / window->getNumSamples());
    }
}
