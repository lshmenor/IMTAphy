/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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

#ifndef WNS_LDK_PROBE_ERRORRATE_HPP
#define WNS_LDK_PROBE_ERRORRATE_HPP

#include <WNS/ldk/fu/Plain.hpp>
#include <WNS/ldk/Forwarding.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/logger/Logger.hpp>

#include <WNS/ldk/probe/Probe.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>

#include <WNS/ldk/ErrorRateProviderInterface.hpp>


namespace wns { namespace ldk { namespace probe {

    class ErrorRate :
        public Probe,
        public fu::Plain<ErrorRate>,
        public Forwarding<ErrorRate>
    {
    public:
        ErrorRate(fun::FUN* fuNet, const wns::pyconfig::View& config);
        virtual ~ErrorRate();

        // Processor interface
        virtual void processOutgoing(const CompoundPtr& compound);
        virtual void processIncoming(const CompoundPtr& compound);

        // FunctionalUnit interface
        virtual void onFUNCreated();

    private:
        wns::probe::bus::ContextCollectorPtr probe;

        std::string errorRateProviderName;

        struct Friends
        {
            FunctionalUnit* errorRateProvider;
        }
        friends;

        logger::Logger logger;
    };

}
}
}

#endif // NOT defined WNS_LDK_PROBE_ERRORRATE_HPP


