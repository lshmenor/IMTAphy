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

#include <WNS/distribution/Cauchy.hpp>

using namespace wns::distribution;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Cauchy,
    Distribution,
    "Cauchy",
    wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Cauchy,
    Distribution,
    "Cauchy",
    wns::distribution::RNGConfigCreator);

Cauchy::Cauchy(double median, double sigma, wns::rng::RNGen* rng) :
  Distribution(rng),
  median_(median),
  sigma_(sigma),
  dis_(getRNG() , CauchyDist::distribution_type(median_, sigma_))
{
}

Cauchy::Cauchy(const pyconfig::View& config) :
  Distribution(),
  median_(config.get<double>("median")),
  sigma_(config.get<double>("sigma")),
  dis_(getRNG(), CauchyDist::distribution_type(median_, sigma_))
{
}

Cauchy::Cauchy(wns::rng::RNGen* rng, const pyconfig::View& config) :
  Distribution(rng),
  median_(config.get<double>("median")),
  sigma_(config.get<double>("sigma")),
  dis_(getRNG(), CauchyDist::distribution_type(median_, sigma_))
{
}

Cauchy::~Cauchy()
{
}

double
Cauchy::operator()()
{
      return dis_();
}


std::string
Cauchy::paramString() const
{
	std::ostringstream tmp;
	tmp << "Cauchy(median = " << median_ << ", sigma = " << sigma_ << ")";
	return tmp.str();
}
