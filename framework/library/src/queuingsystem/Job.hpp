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

//begin example "wns.queuingsystem.Job.hpp.example"
#ifndef WNS_QUEUINGSYSTEM_JOB_HPP
#define WNS_QUEUINGSYSTEM_JOB_HPP

#include <WNS/simulator/Time.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/osi/PCI.hpp>
#include <WNS/SmartPtr.hpp>

namespace wns { namespace queuingsystem {

	// Jobs are the tasks that a queuingsystem has to process. In this model a
	// job can belong to two different job classes, jobs with low or high
	// priority. A job knows its priority and the moment of its birth.
    class Job :
        public wns::osi::PDU
    {
    public:

        // Each Job can have a low or high priority.
        enum Priority {
            lowPriority = 0,
            highPriority
        };

        // By default jobs are created with a low priority
        Job(Priority priority = Job::lowPriority);

	    wns::simulator::Time
        getCreationTime() const;

        Job::Priority
        getPriority() const;

    private:

        // Whenever a job is created it remembers its moment of birth
        wns::simulator::Time timeCreated_;

        // The assigned priority of the job
        Priority priority_;
    };
    typedef wns::SmartPtr<Job> JobPtr;
} // queuingsystem
} // wns

#endif // WNS_QUEUINGSYSTEM_JOB_HPP
// end example
