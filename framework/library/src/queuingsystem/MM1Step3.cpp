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

#include <WNS/queuingsystem/MM1Step3.hpp>

using namespace wns::queuingsystem;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    SimpleMM1Step3,
    wns::simulator::ISimulationModel,
    "openwns.queuingsystem.SimpleMM1Step3",
    wns::PyConfigViewCreator);

SimpleMM1Step3::SimpleMM1Step3(const wns::pyconfig::View& config) :
    config_(config),
    logger_(config.get("logger")),
    // For now we use an expty Context Collector Collection
    cpc_(),
    // The name of the measurement source. Must match the one configured
    // in the global Probe Bus Registry
    sojournTime_(cpc_, "SojournTime")
{
    wns::pyconfig::View disConfig = config.get("jobInterArrivalTimeDistribution");
    std::string disName = disConfig.get<std::string>("__plugin__");
    jobInterarrivalTime_ = 
        wns::distribution::DistributionFactory::creator(disName)->create(disConfig);

    disConfig = config.get("jobProcessingTimeDistribution");
    disName = disConfig.get<std::string>("__plugin__");
    jobProcessingTime_ = 
        wns::distribution::DistributionFactory::creator(disName)->create(disConfig);
}

// begin example "wns.queuingsystem.mm1step3.doStartup.example"
void
SimpleMM1Step3::doStartup()
{
    MESSAGE_SINGLE(NORMAL, logger_, "MM1Step3 started, generating first job\n" << *this);

    generateNewJob();
}
// end example

void
SimpleMM1Step3::doShutdown()
{
}

void
SimpleMM1Step3::generateNewJob()
{
    // Create a new job
    Job job = Job();

    queue_.push_back(job);

    wns::simulator::Time delayToNextJob = (*jobInterarrivalTime_)();

    MESSAGE_SINGLE(NORMAL, logger_, "Generated new job, next job in " << delayToNextJob << "s\n" << *this);

    // The job is the only job in the system. There is no job that is currently
    // being served -> the server is free and can process the next job
    if (queue_.size() == 1)
    {
        processNextJob();
    }

    wns::simulator::getEventScheduler()->scheduleDelay(
        boost::bind(&SimpleMM1Step3::generateNewJob, this),
        delayToNextJob);

}

void
SimpleMM1Step3::onJobProcessed()
{
    Job finished = queue_.front();

    queue_.pop_front();

    // Calculate the Jobs sojourn time
    wns::simulator::Time  now;
    wns::simulator::Time  sojournTime;
    now = wns::simulator::getEventScheduler()->getTime();
    sojournTime = now - finished.getCreationTime();

    // Give some debug output
    MESSAGE_SINGLE(NORMAL, logger_, "Finished a job\n" << *this);
    MESSAGE_BEGIN(NORMAL, logger_, m, "Sojourn Time of last job ");
    m << sojournTime;
    MESSAGE_END();

    // Forward the measurement to the Probe Bus
    sojournTime_.put(sojournTime);

    // if there are still jobs, serve them
    if (!queue_.empty())
    {
        processNextJob();
    }
}

void
SimpleMM1Step3::processNextJob()
{
    wns::simulator::Time processingTime = (*jobProcessingTime_)();

    wns::simulator::getEventScheduler()->scheduleDelay(
        boost::bind(&SimpleMM1Step3::onJobProcessed, this),
        processingTime);
    MESSAGE_SINGLE(NORMAL, logger_, "Processing next job, processing time: " << processingTime << "s\n" << *this);
}

std::string
SimpleMM1Step3::doToString() const
{
    std::stringstream ss;
    ss << "Jobs in system: " << queue_.size();
    return ss.str();
}
