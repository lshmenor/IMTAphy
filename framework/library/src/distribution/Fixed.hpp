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

#ifndef WNS_DISTRIBUTION_FIXED_HPP
#define WNS_DISTRIBUTION_FIXED_HPP

#include <WNS/distribution/Distribution.hpp>

namespace wns { namespace distribution {
    class Fixed :
        public Distribution,
        public IHasMean
    {
    public:
        explicit
        Fixed(const pyconfig::View& config);

        explicit
        Fixed(wns::rng::RNGen* rng, const pyconfig::View& config);

        explicit
        Fixed(double value);

        virtual double
        operator()();

        virtual double
        getMean() const;

        virtual std::string
        paramString() const;

    private:
        double value;
    }; // Fixed

} // distribution
} // wns

#endif // NOT defined WNS_DISTRIBUTION_FIXED_HPP

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

