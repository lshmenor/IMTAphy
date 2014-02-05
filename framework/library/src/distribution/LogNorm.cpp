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

#include <WNS/distribution/LogNorm.hpp>

using namespace wns::distribution;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    LogNorm,
    Distribution,
    "LogNorm",
    wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(
    LogNorm,
    Distribution,
    "LogNorm",
    wns::distribution::RNGConfigCreator);

/* mean(mu) and standard deviation(sigma) of the underlying normal distribution
   are calculated as follows:
   mu = log(pow(mean_, 2)/sqrt(pow(std_, 2) + pow(mean_, 2)))
   sigma = sqrt(log(1 + pow(std_, 2)/pow(mean_, 2)))
   However, mu and sigma are not needed here. The boost lognormal distribution
   takes mean and std of the lognormal distribution as parameters
   and NOT mu and sigma of the underlying normal distribution as it is done usually.
   -Sources:
   1. http://www.boost.org/doc/libs/1_39_0/libs/random/random-distributions.html
   2. http://www.boost.org/doc/libs/1_39_0/boost/random/lognormal_distribution.hpp
*/
LogNorm::LogNorm(double mean, double std, wns::rng::RNGen* rng) :
  Distribution(rng),
  mean_(mean),
  std_(std),
  variance_(pow(std_, 2)),
  dis_(getRNG(), LogNormalDist::distribution_type(mean_, std_))
{
  if (mean_ == 0.0)
    {
      assure(false, "LogNorm: Mean must not be zero!");
    }

  if (variance_ < 0.0)
    {
      assure(false, "LogNorm: Variance must not be negative!");
    }
}

LogNorm::LogNorm(const pyconfig::View& config) :
  Distribution(),
  mean_(config.get<double>("mean")),
  std_(config.get<double>("std")),
  variance_(pow(std_, 2)),
  dis_(getRNG(), LogNormalDist::distribution_type(mean_, std_))
{
}

LogNorm::LogNorm(wns::rng::RNGen* rng, const pyconfig::View& config) :
  Distribution(rng),
  mean_(config.get<double>("mean")),
  std_(config.get<double>("std")),
  variance_(pow(std_, 2)),
  dis_(getRNG(), LogNormalDist::distribution_type(mean_, std_))
{
}

LogNorm::~LogNorm()
{
}


double
LogNorm::operator()()
{
  return dis_();
}

double
LogNorm::getMean() const
{
  return mean_;
}


std::string
LogNorm::paramString() const
{
	std::ostringstream tmp;
	tmp << "LogNorm(mean = " << mean_ << ", variance = " << variance_ << ")";
	return tmp.str();
}
