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

#ifndef WNS_QUEUINGSYSTEM_JOBCONTEXT_PROVIDER_HPP
#define WNS_QUEUINGSYSTEM_JOBCONTEXT_PROVIDER_HPP

#include <WNS/queuingsystem/Job.hpp>
#include <WNS/probe/bus/ContextProvider.hpp>

namespace wns { namespace queuingsystem {

    // This Context Provider is called when a Context Collector
    // calls put() with a value AND a Job as arguments.
    // The Job Context Provider will extract the priority
    // from the Job and save the Context under the key 'priority'.
	class JobContextProvider :
        public wns::probe::bus::PDUContextProvider<Job>
    {
    public:

        JobContextProvider(){};
        
    private:
        void
        doVisit(wns::probe::bus::IContext& c, const JobPtr& job) const;
    };
} // queuingsystem
} // wns

#endif // WNS_QUEUINGSYSTEM_JOBCONTEXT_PROVIDER_HPP
