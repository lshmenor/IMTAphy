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

#include <WNS/distribution/DiscreteUniform.hpp>

using namespace wns::distribution;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    DiscreteUniform,
    Distribution,
    "DiscreteUniform",
    wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(
    DiscreteUniform,
    Distribution,
    "DiscreteUniform",
    wns::distribution::RNGConfigCreator);

DiscreteUniform::DiscreteUniform(const pyconfig::View& config) :
    Distribution(),
    low_(config.get<int>("low")),
    high_(config.get<int>("high")),
    dis_(getRNG(), DiscreteUniformDist::distribution_type(low_, high_))
{
}

DiscreteUniform::DiscreteUniform(wns::rng::RNGen* rng, const pyconfig::View& config) :
    Distribution(rng),
    low_(config.get<int>("low")),
    high_(config.get<int>("high")),
    dis_(getRNG(), DiscreteUniformDist::distribution_type(low_, high_))
{
}

DiscreteUniform::DiscreteUniform(int _low, int _high, wns::rng::RNGen* rng) :
    Distribution(rng),
    low_(_low),
    high_(_high),
    dis_(getRNG(), DiscreteUniformDist::distribution_type(low_, high_))
{
}

DiscreteUniform::~DiscreteUniform()
{
}

double
DiscreteUniform::operator()()
{
    return (double)dis_();
}

double
DiscreteUniform::getMean() const
{
    return ((double)low_ + (double)high_) / 2.0;
}

std::string
DiscreteUniform::paramString() const
{
    std::ostringstream tmp;
    tmp << "DiscreteUniform(" << low_ << ".." <<high_ <<")";
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

