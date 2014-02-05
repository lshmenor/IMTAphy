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
// begin example "wns.queuingsystem.mm1step1.cpp.example"
#include <WNS/queuingsystem/MM1Step1.hpp>

using namespace wns::queuingsystem;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    SimpleMM1Step1,
    wns::simulator::ISimulationModel,
    "openwns.queuingsystem.SimpleMM1Step1",
    wns::PyConfigViewCreator);

SimpleMM1Step1::SimpleMM1Step1(const wns::pyconfig::View& config) :
    jobsInSystem_(0),
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
SimpleMM1Step1::doStartup()
{
    MESSAGE_SINGLE(NORMAL, logger_, "MM1Step1 started, generating first job\n" << *this);

    generateNewJob();
}

void
SimpleMM1Step1::doShutdown()
{

}

void
SimpleMM1Step1::generateNewJob()
{
    ++jobsInSystem_;

    wns::simulator::Time delayToNextJob = (*jobInterarrivalTime_)();

    MESSAGE_SINGLE(NORMAL, logger_, "Generated new job, next job in " << delayToNextJob << "s\n" << *this);

    // The job is the only job in the system. There is no job that is currently
    // being served -> the server is free and can process the next job
    if (jobsInSystem_ == 1)
    {
        processNextJob();
    }

    wns::simulator::getEventScheduler()->scheduleDelay(
        boost::bind(&SimpleMM1Step1::generateNewJob, this),
        delayToNextJob);

}

void
SimpleMM1Step1::onJobProcessed()
{
    --jobsInSystem_;
    MESSAGE_SINGLE(NORMAL, logger_, "Finished a job\n" << *this);
    // if there are still jobs, serve them
    if (jobsInSystem_ > 0)
    {
        processNextJob();
    }
}

void
SimpleMM1Step1::processNextJob()
{
    wns::simulator::Time processingTime = (*jobProcessingTime_)();

    wns::simulator::getEventScheduler()->scheduleDelay(
        boost::bind(&SimpleMM1Step1::onJobProcessed, this),
        processingTime);
    MESSAGE_SINGLE(NORMAL, logger_, "Processing next job, processing time: " << processingTime << "s\n" << *this);
}

std::string
SimpleMM1Step1::doToString() const
{
    std::stringstream ss;
    ss << "Jobs in system: " << jobsInSystem_;
    return ss.str();
}
// end example

