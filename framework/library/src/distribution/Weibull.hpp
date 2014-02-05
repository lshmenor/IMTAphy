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
#ifndef WNS_DISTRIBUTION_WEIBULL_HPP
#define WNS_DISTRIBUTION_WEIBULL_HPP

#include <WNS/distribution/Distribution.hpp>
#include <WNS/distribution/Uniform.hpp>

namespace wns { namespace distribution {

    /* Weibull distributed random numbers */
    class Weibull :
    public Distribution,
    public IHasMean
    {

    public:
      explicit
      Weibull(double scale, double shape, wns::rng::RNGen* rng = wns::simulator::getRNG());

      explicit
      Weibull(const pyconfig::View& config);

      explicit
      Weibull(wns::rng::RNGen* rng, const pyconfig::View& config);

      virtual
      ~Weibull();

      virtual double
      operator()();

      virtual double
      getMean() const;

      virtual std::string
      paramString() const;

      double
      gamma(double x);

    private:
      double mean_;
      double variance_;
      double scale_;
      double shape_;

      wns::distribution::Distribution* uniDis;

    };

  } // distribution
} // wns

#endif  // WNS_DISTRIBUTION_WEIBULLDIS_HPP
