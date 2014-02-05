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

#include <WNS/queuingsystem/GGn.hpp>
#include <boost/bind.hpp>

using namespace wns::queuingsystem;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    GGn,
    wns::simulator::ISimulationModel,
    "openwns.queuingsystem.ggn",
    wns::PyConfigViewCreator);

GGn::GGn(const wns::pyconfig::View& config) :
    interArrivalDist_(NULL),
    processingTimeDist_(NULL),
    serverCount_(config.get<int>("serverCount")),
    servers_(serverCount_),
    cpc_(),
    queueSize_(cpc_, "QueueSize"),
    processingTime_(cpc_, "ProcessingTime"),
    sojournTime_(cpc_, "SojournTime"),
    waitingTime_(cpc_, "WaitingTime"),
    logger_(config.get("logger"))
{
    wns::pyconfig::View disConfig(config, "iatDist");
    std::string disName = disConfig.get<std::string>("__plugin__");
    interArrivalDist_ = 
        wns::distribution::DistributionFactory::creator(disName)->create(disConfig);

    disConfig = config.get("procDist");
    disName = disConfig.get<std::string>("__plugin__");
    processingTimeDist_ = 
        wns::distribution::DistributionFactory::creator(disName)->create(disConfig);

    for(int i = 0; i < servers_.size(); i++)
    {
        servers_[i] = new Server(processingTimeDist_, this);
        emptyServers_.push_back(servers_[i]);    
    }
}

GGn::~GGn()
{
    for(int i = 0; i < servers_.size(); i++)
        delete servers_[i];
}

void
GGn::doStartup()
{
    double iatMean;
    double ptMean;

    iatMean = (dynamic_cast<wns::distribution::IHasMean*>(interArrivalDist_))->getMean();
    ptMean = (dynamic_cast<wns::distribution::IHasMean*>(processingTimeDist_))->getMean();

    double load = ptMean / (serverCount_ * iatMean);

    MESSAGE_BEGIN(NORMAL, logger_, m, "G/G/n Started ");
    m << "\nMean IAT: " << iatMean;
    m << "\nMean PT: " << ptMean;
    m << "\nServers (n): " << serverCount_;
    m << "\nLoad: " << load;
    MESSAGE_END();

    assure(load < 1.0, "Load must be < 1.0!");

    onCreateJob();
}

void
GGn::onCreateJob()
{
    Job job = Job();

    // PASTA 
    queueSize_.put(queue_.size());

    queue_.push_back(job);

    double iat = (*interArrivalDist_)();

    wns::simulator::getEventScheduler()->scheduleDelay(
        boost::bind(&GGn::onCreateJob, this),
        iat);

    wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();

    MESSAGE_SINGLE(NORMAL, logger_, "Created a job. Total: " << queue_.size()
        << " Next Job will be created at: " << now + iat);

    // There are empty servers that can process the job directly
    if(!emptyServers_.empty())
    {
        assure(queue_.size() == 1, "Queue not empty but there are empty servers");
        Server* srv = emptyServers_.front();
        emptyServers_.pop_front();
        srv->processJob(getJob());
    }
}

bool
GGn::hasJob(Server* server)
{
    if(queue_.empty())
    {
        emptyServers_.push_back(server);
        return false;
    }
    else
    {
        return true;
    }
}

Job
GGn::getJob()
{
    assure(!queue_.empty(), "getJob called while queue empty");
    Job job = queue_.front();
    queue_.pop_front();

    wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();
    waitingTime_.put(now - job.getCreationTime());

    return job;
}

void
GGn::onJobProcessed(Job job)
{
    wns::simulator::Time now = wns::simulator::getEventScheduler()->getTime();
    sojournTime_.put(now - job.getCreationTime());

    MESSAGE_SINGLE(NORMAL, logger_, "Finished a job. Left: " << queue_.size());
}

void
GGn::doShutdown()
{
    MESSAGE_SINGLE(NORMAL, logger_, "Jobs left: " << queue_.size());
}

