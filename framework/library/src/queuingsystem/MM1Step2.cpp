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

#include <WNS/queuingsystem/MM1Step2.hpp>

using namespace wns::queuingsystem;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    SimpleMM1Step2,
    wns::simulator::ISimulationModel,
    "openwns.queuingsystem.SimpleMM1Step2",
    wns::PyConfigViewCreator);

SimpleMM1Step2::SimpleMM1Step2(const wns::pyconfig::View& config) :
    logger_(config.get("logger"))
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

void
SimpleMM1Step2::doStartup()
{
    MESSAGE_SINGLE(NORMAL, logger_, "MM1Step2 started, generating first job\n" << *this);

    generateNewJob();
}

void
SimpleMM1Step2::doShutdown()
{

}

// begin example "wns.queuingsystem.mm1step2.generateNewJob.example"
void
SimpleMM1Step2::generateNewJob()
{
    // Create a new job
    Job job = Job();

    // Insert the new job at the end of the queue
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
        boost::bind(&SimpleMM1Step2::generateNewJob, this),
        delayToNextJob);

}
// end example

// begin example "wns.queuingsystem.mm1step2.onJobProcessed.example"
void
SimpleMM1Step2::onJobProcessed()
{
    Job finished = queue_.front();

    queue_.pop_front();

    MESSAGE_SINGLE(NORMAL, logger_, "Finished a job\n" << *this);
    MESSAGE_BEGIN(NORMAL, logger_, m, "Sojourn Time of last job ");
    m << wns::simulator::getEventScheduler()->getTime() - finished.getCreationTime();
    MESSAGE_END();

    // if there are still jobs, serve them
    if (!queue_.empty())
    {
        processNextJob();
    }
}
// end example

void
SimpleMM1Step2::processNextJob()
{
    wns::simulator::Time processingTime = (*jobProcessingTime_)();

    wns::simulator::getEventScheduler()->scheduleDelay(
        boost::bind(&SimpleMM1Step2::onJobProcessed, this),
        processingTime);
    MESSAGE_SINGLE(NORMAL, logger_, "Processing next job, processing time: " << processingTime << "s\n" << *this);
}

std::string
SimpleMM1Step2::doToString() const
{
    std::stringstream ss;
    ss << "Jobs in system: " << queue_.size();
    return ss.str();
}
