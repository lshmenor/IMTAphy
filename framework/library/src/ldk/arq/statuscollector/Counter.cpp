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

#include <WNS/ldk/arq/statuscollector/Counter.hpp>
#include <WNS/logger/Logger.hpp>

using namespace wns::ldk::arq::statuscollector;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Counter, Interface, "StatusCollectorCounter", wns::ldk::PyConfigCreator);

Counter::Counter(const wns::pyconfig::View& config) :
        logger(config.get("logger"))
{
    this->reset();
}
void Counter::reset()
{
    numSucc = 0;
    numFailed = 0;
}

double Counter::getSuccessRate(const CompoundPtr& /*compound*/)
{
    if(numSucc + numFailed > 0)
    {
        return(numSucc/(numSucc + numFailed));
    }
    else
    {
        return(0.0);
    }
}

void Counter::onSuccessfullTransmission(const CompoundPtr& /*compound*/)
{
    ++numSucc;
    MESSAGE_SINGLE(NORMAL, this->logger, "successfull tx, " << numSucc << " of " << (numSucc+numFailed));
}

void Counter::onFailedTransmission(const CompoundPtr& /*compound*/)
{
    ++numFailed;
    MESSAGE_SINGLE(NORMAL, this->logger, "failed tx, " << numFailed << " of " << (numSucc+numFailed));
}
