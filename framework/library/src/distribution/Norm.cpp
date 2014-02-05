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

#include <WNS/distribution/Norm.hpp>

#include <WNS/module/Base.hpp>

using namespace wns::distribution;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Norm,
    Distribution,
    "Norm",
    wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Norm,
    Distribution,
    "Norm",
    wns::distribution::RNGConfigCreator);

Norm::Norm(double mean, double variance, wns::rng::RNGen* rng) :
    Distribution(rng),
    mean_(mean),
    variance_(variance),
    dis_(getRNG(), NormalDist::distribution_type(mean_, sqrt(variance_)))
{
}

Norm::Norm(const pyconfig::View& config) :
    Distribution(),
    mean_(config.get<double>("mean")),
    variance_(config.get<double>("variance")),
    dis_(getRNG(), NormalDist::distribution_type(mean_, sqrt(variance_)))
{
}

Norm::Norm(wns::rng::RNGen* rng, const pyconfig::View& config) :
    Distribution(rng),
    mean_(config.get<double>("mean")),
    variance_(config.get<double>("variance")),
    dis_(getRNG(), NormalDist::distribution_type(mean_, sqrt(variance_)))
{
}

Norm::~Norm()
{
}


double
Norm::operator()()
{
    return dis_();
}

double
Norm::getMean() const
{
    return mean_;
}

std::string
Norm::paramString() const
{
    std::ostringstream tmp;
    tmp << "Norm(mean=" << this->getMean() << ",v=" << variance_ << ")";
    return tmp.str();
}

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

