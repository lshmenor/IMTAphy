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

#ifndef WNS_DISTRIBUTION_POISSON_HPP
#define WNS_DISTRIBUTION_POISSON_HPP

#include <WNS/distribution/Distribution.hpp>
#include <WNS/distribution/Uniform.hpp>

namespace wns { namespace distribution {
    /**
     * @brief Poisson distributed random numbers.
     *
     * @author Rainer Schoenen <rs@comnets.rwth-aachen.de>
     *
     * Provided number of arrivals within a given time interval t with
     * (NegExp) arrival rate lambda. Here: lambda * t = mean
     */
    class Poisson :
        public Distribution,
        public IHasMean
    {
    public:
        explicit
        Poisson(double mean, 
            wns::rng::RNGen* rng = wns::simulator::getRNG());

        explicit
        Poisson(const pyconfig::View& config);

        explicit
        Poisson(wns::rng::RNGen* rng, const pyconfig::View& config);

        virtual
        ~Poisson();

        virtual double
        operator()();

        virtual double
        getMean() const;

        virtual std::string
        paramString() const;

    private:
        double mean_;
        StandardUniform dis_;
    }; // Poission

} // distribution
} // wns

#endif // NOT defined WNS_DISTRIBUTION_POISSON_HPP

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

