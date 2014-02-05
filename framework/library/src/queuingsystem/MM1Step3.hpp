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

#ifndef WNS_QUEUINGSYSTEM_MM1STEP3_HPP
#define WNS_QUEUINGSYSTEM_MM1STEP3_HPP

#include <WNS/queuingsystem/Job.hpp>

#include <WNS/rng/RNGen.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WNS/simulator/ISimulationModel.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/IOutputStreamable.hpp>
#include <WNS/pyconfig/View.hpp>

#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>
#include <WNS/distribution/Distribution.hpp>

#include <boost/bind.hpp>

namespace wns { namespace queuingsystem {

    class SimpleMM1Step3 :
        public IOutputStreamable,
        public wns::simulator::ISimulationModel
    {
        typedef wns::rng::VariateGenerator< boost::exponential_distribution<> > Exponential;
    public:
        explicit
        SimpleMM1Step3(const wns::pyconfig::View& configuration);

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

// begin example "wns.queuingsystem.mm1step3.hpp.example"
        wns::distribution::Distribution* jobInterarrivalTime_;

        wns::distribution::Distribution* jobProcessingTime_;

        std::list<Job> queue_;

        wns::pyconfig::View config_;

        wns::logger::Logger logger_;

        // Used to obtain additional information for probed values
        wns::probe::bus::ContextProviderCollection cpc_;

        // Used to probe the sojourn time
        wns::probe::bus::ContextCollector sojournTime_;
// end example
    };
}
}

#endif // NOT defined WNS_QUEUINGSYSTEM_MM1STEP3_HPP

