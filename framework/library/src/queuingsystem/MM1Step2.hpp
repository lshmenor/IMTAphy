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

// begin example "wns.queuingsystem.mm1step2.hpp.example"
#ifndef WNS_QUEUINGSYSTEM_MM1STEP2_HPP
#define WNS_QUEUINGSYSTEM_MM1STEP2_HPP

#include <WNS/queuingsystem/Job.hpp>

#include <WNS/rng/RNGen.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WNS/simulator/ISimulationModel.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/IOutputStreamable.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/distribution/Distribution.hpp>

#include <boost/bind.hpp>


namespace wns { namespace queuingsystem {

    class SimpleMM1Step2 :
        public IOutputStreamable,
        public wns::simulator::ISimulationModel
    {
    public:
        explicit
        SimpleMM1Step2(const wns::pyconfig::View& configuration);

    private:
        virtual void
        doStartup();

        virtual void
        doShutdown();

        void
        generateNewJob();

        void
        onJobProcessed();

        void
        tryToProcessNextJob();

        void
        processNextJob();

        virtual std::string
        doToString() const;

        wns::distribution::Distribution* jobInterarrivalTime_;

        wns::distribution::Distribution* jobProcessingTime_;

        std::list<Job> queue_;

        wns::logger::Logger logger_;
    };
}
}

#endif // NOT defined WNS_QUEUINGSYSTEM_MM1STEP2_HPP
// end example

/**
 * @page wns.queuingsystem.mm1step2 Calculating sojurn time
 *
 * @section wns.queuingsystem.mm1step2.contents Contents
 *   -# @ref queue
 *   -# @ref statefuljobs
 *
 * @section wns.queuingsystem.mm1step2.queue Implementing the Queue
 * A Queue can be filled with jobs. For the time beeing it is only important
 * that a job can be created, stored in a queue, and processed, which means that
 * it leaves the queuing system. The Job class is used in order to create new
 * jobs, which are inserted at the end of the queue with help of the push_back()
 * method of the queue. The jobs within the queue are processed following the
 * first-in-first-out strategy. Jobs arrive with an arrival rate. This is why we
 * store the time when the next job arrives in the delayToNextJob variable. The
 * EventScheduler is used to schedule a call to the generateNewJob function when
 * delayToNextJob has passed. If the new generated job is the only job within
 * the queue it is processed direclty. Notice, that the job still remains within
 * the queue as long as the processing of the job has not finished. In our model
 * the server of the queuing system is represented by the head of the queue. The
 * queue is implemented as a std::list which acts as a container and is able to
 * store objects of our Job class. See how easy this can be done:
 * std::list<Job> queue_;
 * To append jobs at the end of the list/queue, we use the push_back() method. To
 * get the object at the head of the queue we call the front() function. The
 * pop_front() method is used to remove the object at the front of the queue.
 * @include "wns.queuingsystem.Job.hpp.example"
 * @include "wns.queuingsystem.mm1step2.generateNewJob.example"
 *
 * @section wns.queuingsystem.mm1step2.statefuljobs Stateful Jobs
 *
 * Now we want to create some statistic for our queuing system, e.g. we want
 * to compute the sojourn time of a job. Therefore the job has to remember its
 * time when it has been created. In the constructor of the Job class we get
 * the current simulation time, by requesting the current simulation time from
 * the EventScheduler of the simulator.
 *
 * When the job has been processed we can calculate the sojourn time, which is
 * the time a job stayed in the queue. Therefore we take the actual time -
 * again with the assistance of the EventScheduler - and subtract the
 * creation time of the job. The first code snippet shows the Job class and
 * the getter method (getCreationTime()) that is needed in order to retrieve
 * the creation time of the finished job. The second code snippet gives an
 * example for the implmentation of the debug output of the sojourn
 * time. Whenever the queue processes a job, it takes the oldest job of the
 * queue (first-in-first-out strategy) by calling the front method. Afterwards
 * the job is removed from the queue by calling the pop method. Now we use the
 * logger to output the sojourn time. Afterwards the queue continues to
 * process the remaining jobs, if any.
 * @include "wns.queuingsystem.Job.cpp.example"
 * @include "wns.queuingsystem.mm1step2.onJobProcessed.example"
 */

