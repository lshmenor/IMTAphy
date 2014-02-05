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

#ifndef WNS_QUEUINGSYSTEM_GGN_HPP
#define WNS_QUEUINGSYSTEM_GGN_HPP

#include <WNS/simulator/ISimulationModel.hpp>
#include <WNS/distribution/Distribution.hpp>

#include <WNS/logger/Logger.hpp>
#include <WNS/pyconfig/View.hpp>

#include <WNS/queuingsystem/Job.hpp>
#include <WNS/queuingsystem/Server.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>

#include <list>

namespace wns { namespace queuingsystem {

    class GGn :
        public wns::simulator::ISimulationModel
    {
        public:
            explicit
            GGn(const wns::pyconfig::View& configuration);

            virtual
            ~GGn();

            void
            onJobProcessed(Job);

            bool
            hasJob(Server*);

            Job
            getJob();

        protected:
            virtual void
            doStartup();
        
            virtual void
            doShutdown();

            virtual void
            onCreateJob();

            void
            nextJob();

            wns::distribution::Distribution* interArrivalDist_;
            wns::distribution::Distribution* processingTimeDist_;

            int serverCount_;

            std::vector<Server*> servers_;
            std::list<Server*> emptyServers_;
            std::list<Job> queue_;

            wns::probe::bus::ContextProviderCollection cpc_;
            wns::probe::bus::ContextCollector queueSize_;
            wns::probe::bus::ContextCollector processingTime_;
            wns::probe::bus::ContextCollector sojournTime_;
            wns::probe::bus::ContextCollector waitingTime_;

            wns::logger::Logger logger_;
    };
}
}

#endif // NOT defined WNS_QUEUINGSYSTEM_GGN_HPP
