/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#include <WNS/Assure.hpp>
#include <WNS/StopWatch.hpp>

using namespace wns;

StopWatch::StopWatch() :
    begin_(),
    end_(),
    isRunning_(false)
{
    // init to zero
    gettimeofday(&begin_, NULL);
    end_ = begin_;
}

void
StopWatch::start()
{
    assure(isRunning_  == false, "Must first call stop()");
    gettimeofday(&begin_, NULL);
    isRunning_ = true;
}

double
StopWatch::stop()
{
    assure(isRunning_ == true, "Must first call start()");
    gettimeofday(&end_, NULL);
    isRunning_ = false;
    return getInSeconds();
}

double
StopWatch::getInSeconds() const
{
    assure(isRunning_ == false, "Must first call stop()");
    return (static_cast<double>(end_.tv_sec) + static_cast<double>(end_.tv_usec)/1E6) -
        (static_cast<double>(begin_.tv_sec) + static_cast<double>(begin_.tv_usec)/1E6);
}

std::string
StopWatch::doToString() const
{
    std::stringstream ss;
    ss << getInSeconds() << " s";
    return ss.str();
}

