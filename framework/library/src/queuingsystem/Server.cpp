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

#include <WNS/queuingsystem/Server.hpp>
#include <WNS/queuingsystem/GGn.hpp>
#include <boost/bind.hpp>

using namespace wns::queuingsystem;

Server::Server(wns::distribution::Distribution* procDist, GGn* parent) :
    busy_(false),
    parent_(parent),
    dist_(procDist)
{
}

Server::~Server()
{
}

void 
Server::processJob(Job job)
{
    assure(!busy_, "processedJob called while server busy");
    busy_ = true;
    job_ = job;
    wns::simulator::Time pt = (*dist_)();

    wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();

    wns::simulator::getEventScheduler()->scheduleDelay(
        boost::bind(&Server::onJobFinished, this),
        pt);
}

void
Server::onJobFinished()
{
    parent_->onJobProcessed(job_);
    busy_ = false;
    if(parent_->hasJob(this))
        processJob(parent_->getJob());
}


