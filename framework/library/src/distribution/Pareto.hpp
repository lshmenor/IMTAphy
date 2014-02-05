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

#ifndef WNS_DISTRIBUTION_PARETO_HPP
#define WNS_DISTRIBUTION_PARETO_HPP

#include <WNS/distribution/Distribution.hpp>

#include <WNS/distribution/Uniform.hpp>

namespace wns { namespace distribution {
    /**
     * @brief class for random numbers according to
     * Pareto distribution.
     *
     * @author Rainer Schoenen <rs@comnets.rwth-aachen.de>
     *
     * The distriubtion is classified by the
     * shape parameter A (often called beta) and
     * the scale parameter B (proportional to mean outcome).
     *
     * @see http://en.wikipedia.org/wiki/Pareto_distribution
    **/
    class Pareto :
        public Distribution,
        public IHasMean
    {
    public:
        explicit
        Pareto(double shapeA, double scaleB, 
            wns::rng::RNGen* rng = wns::simulator::getRNG());

        explicit
        Pareto(const pyconfig::View& config);

        explicit
        Pareto(wns::rng::RNGen* rng, const pyconfig::View& config);

        virtual
        ~Pareto();

        virtual double
        operator()();

        virtual double
        getMean() const;

        virtual std::string
        paramString() const;

    private:
        /**
         * @brief Shape parameter 'a'
         */
        double shapeParamA_;

        /**
         * @brief Scale parameter 'b'
         *
         * @note b >= 0
         */
        double scaleParamB_;

        /**
         * @brief The distribution itself
         */
        StandardUniform dis_;
    }; // Pareto

} // distribution
} // wns

#endif // NOT defined WNS_DISTRIBUTION_PARETO_HPP

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

