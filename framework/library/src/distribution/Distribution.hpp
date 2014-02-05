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

#ifndef WNS_DISTRIBUTION_DISTRIBUTION_HPP
#define WNS_DISTRIBUTION_DISTRIBUTION_HPP

#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/distribution/RNGConfigCreator.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/simulator/Simulator.hpp>
#include <WNS/rng/RNGen.hpp>
#include <WNS/logger/Logger.hpp>


#include <sstream>

namespace wns { namespace distribution {

    /**
     * @brief Interface for random number distributions (generic)
     *
     * @author Fabian Debus <fds@comnets.rwth-aachen.de>
     */
    class Distribution
    {
    public:
        Distribution() :
            rng_(wns::simulator::getRNG())
        {
        };

        Distribution(wns::rng::RNGen* rng) :
            rng_(rng)
        {
        };

        virtual
        ~Distribution()
        {
        };

        /**
         * @brief returns the random value according to the
         * distribution.
         */
        virtual double
        operator()() = 0;

        friend std::ostream&
        operator <<(std::ostream& os, const Distribution& d)
        {
            os << d.paramString();
            return os;
        };

    protected:

        wns::rng::RNGen*
        getRNG()
        {
            return rng_;
        };

    private:
        /**
         * @brief returns a string describing distribution and
         * parameters
         */
        virtual std::string
        paramString() const = 0;

        wns::rng::RNGen* rng_;

    }; // Distribution

    typedef PyConfigViewCreator<Distribution> DistributionCreator;
    typedef StaticFactory<DistributionCreator> DistributionFactory;

    typedef RNGConfigCreator<Distribution> RNGDistributionCreator;
    typedef StaticFactory<RNGDistributionCreator> RNGDistributionFactory;

    /**
     * @brief Interface for random number distributions which have nice
     * statistical properties like known mean (variance etc) values
     *
     * @author Rainer Schoenen <rs@comnets.rwth-aachen.de>
     */
    class IHasMean
    {
    public:

        /**
         * @brief Mean value of the distribution
         */
        virtual double
        getMean() const = 0;
    };
} // distribution
} // wns

#endif // NOT defined WNS_DISTRIBUTION_DISTRIBUTION_HPP

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 8
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 8
  End:
*/

