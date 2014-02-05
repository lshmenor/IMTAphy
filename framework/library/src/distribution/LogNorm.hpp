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

#ifndef WNS_DISTRIBUTION_LOGNORM_HPP
#define WNS_DISTRIBUTION_LOGNORM_HPP


#include <WNS/distribution/Distribution.hpp>
#include <WNS/rng/RNGen.hpp>

namespace wns { namespace distribution {

    typedef wns::rng::VariateGenerator< boost::lognormal_distribution<> > LogNormalDist;

    /* Log-normal distributed random numbers */
    class LogNorm :
    public Distribution,
    public IHasMean
    {
    public:
      explicit
      LogNorm(double mean, double std,
	      wns::rng::RNGen* rng = wns::simulator::getRNG());

      explicit
      LogNorm(const pyconfig::View& config);

      explicit
      LogNorm(wns::rng::RNGen* rng, const pyconfig::View& config);

      virtual
      ~LogNorm();

      virtual double
      operator()();

      virtual double
      getMean() const;

      virtual std::string
      paramString() const;

    private:
      double mean_;
      double std_;
      double variance_;

      LogNormalDist dis_;

    };

  } // distribution
} // wns

#endif  // WNS_DISTRIBUTION_LOGNORM_HPP

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
