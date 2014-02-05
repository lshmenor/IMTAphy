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

#include <WNS/queuingsystem/MM1Step5.hpp>
#include <WNS/probe/bus/ContextProvider.hpp>

using namespace wns::queuingsystem;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    SimpleMM1Step5,
    wns::simulator::ISimulationModel,
    "openwns.queuingsystem.SimpleMM1Step5",
    wns::PyConfigViewCreator);

SimpleMM1Step5::SimpleMM1Step5(const wns::pyconfig::View& config) :
    priorityDistribution_(Job::lowPriority, Job::highPriority),
    config_(config),
    logger_(config.get("logger")),
    idle(true),
    // Below we will put one Context Provider in the collection
    cpc_(new wns::probe::bus::ContextProviderCollection()),
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

    // Callback Context Provider will call the given function to fill
    // in Context information
    cpc_->addProvider(wns::probe::bus::contextprovider::Callback(
            "priority",
            boost::bind(&SimpleMM1Step5::getCurrentJobPriority, this)));
}

void
SimpleMM1Step5::doStartup()
{
    MESSAGE_SINGLE(NORMAL, logger_, "MM1Step5 started, generating first job\n" << *this);

    generateNewJob();
}

void
SimpleMM1Step5::doShutdown()
{
    assure(cpc_, "No Context Provider Collection");
    delete cpc_;
}

void
SimpleMM1Step5::generateNewJob()
{
    // Create a new job
    Job job = Job(drawJobPriority());

    if (job.getPriority() == Job::lowPriority)
    {
        lowPriorityQueue_.push_back(job);
    }
    else
    {
        highPriorityQueue_.push_back(job);
    }


    wns::simulator::Time delayToNextJob = (*jobInterarrivalTime_)();

    MESSAGE_SINGLE(NORMAL, logger_, "Generated new job, next job in " << delayToNextJob << "s\n" << *this);

    if (idle)
    {
        processNextJob();
    }

    wns::simulator::getEventScheduler()->scheduleDelay(
        boost::bind(&SimpleMM1Step5::generateNewJob, this),
        delayToNextJob);
}

void
SimpleMM1Step5::onJobProcessed()
{
    // Calculate the Jobs sojourn time
    wns::simulator::Time  now;
    wns::simulator::Time  sojournTime;
    now = wns::simulator::getEventScheduler()->getTime();
    sojournTime = now - currentJob_.getCreationTime();

    // Give some debug output
    MESSAGE_SINGLE(NORMAL, logger_, "Finished a job\n" << *this);
    MESSAGE_BEGIN(NORMAL, logger_, m, "Sojourn Time of last job ");
    m << sojournTime;
    MESSAGE_END();

    // Probe the value. The Context Provider will automatically
    // attach information about the priority of the job.
    sojournTime_.put(sojournTime);

    // if there are still jobs, serve them
    if ( getNumberOfJobs() > 0 )
    {
        processNextJob();
    }
    else
    {
        idle = true;
    }
}

void
SimpleMM1Step5::processNextJob()
{
    currentJob_ = getNextJob();

    wns::simulator::Time processingTime = (*jobProcessingTime_)();

    wns::simulator::getEventScheduler()->scheduleDelay(
        boost::bind(&SimpleMM1Step5::onJobProcessed, this),
        processingTime);

    idle = false;

    MESSAGE_SINGLE(NORMAL, logger_, "Processing next job, processing time: " << processingTime << "s\n" << *this);
}

int
SimpleMM1Step5::getNumberOfJobs() const
{
    return lowPriorityQueue_.size() + highPriorityQueue_.size();
}

Job
SimpleMM1Step5::getNextJob()
{
    Job nextJob;

    if (highPriorityQueue_.size() > 0)
    {
        nextJob = highPriorityQueue_.front();

        highPriorityQueue_.pop_front();
    }
    else if(lowPriorityQueue_.size() > 0)
    {
        nextJob = lowPriorityQueue_.front();

        lowPriorityQueue_.pop_front();
    }
    else
    {
        throw wns::Exception("getNextJob called but now Job queued. You must check for available jobs before calling getNextJob!");
    }

    return nextJob;
}

std::string
SimpleMM1Step5::doToString() const
{
    std::stringstream ss;
    ss << "Jobs in system: " << getNumberOfJobs();
    return ss.str();
}

Job::Priority
SimpleMM1Step5::drawJobPriority()
{
    if (priorityDistribution_() == static_cast<int>(Job::lowPriority))
    {
        return Job::lowPriority;
    }
    else
    {
        return Job::highPriority;
    }
}

int
SimpleMM1Step5::getCurrentJobPriority() const
{
    return currentJob_.getPriority();
}
