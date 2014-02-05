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

#include <WNS/distribution/Rice.hpp>

using namespace wns::distribution;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Rice,
    Distribution,
    "Rice",
    wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Rice,
    Distribution,
    "Rice",
    wns::distribution::RNGConfigCreator);

Rice::Rice(double losFactor, double variance, wns::rng::RNGen* rng) :
    Distribution(rng),
    losFactor_(losFactor),
    variance_(variance), 
    disA_(losFactor_, variance_ * variance_, getRNG()),
    disB_(0.0, variance_ * variance_, getRNG())
{
}

Rice::Rice(const pyconfig::View& config) :
    Distribution(),
    losFactor_(config.get<double>("losFactor")), 
    variance_(config.get<double>("variance")), 
    disA_(losFactor_, variance_*variance_, getRNG()),
    disB_(0.0, variance_ * variance_, getRNG())
{
}

Rice::Rice(wns::rng::RNGen* rng, const pyconfig::View& config) :
    Distribution(rng),
    losFactor_(config.get<double>("losFactor")), 
    variance_(config.get<double>("variance")), 
    disA_(losFactor_, variance_*variance_, getRNG()),
    disB_(0.0, variance_ * variance_, getRNG())
{
}


Rice::~Rice()
{
}

double
Rice::operator()()
{
    double rnd_a    = disA_();
    double rnd_b    = disB_();

    return sqrt(rnd_a * rnd_a + rnd_b * rnd_b);
}

std::string
Rice::paramString() const
{
    std::ostringstream tmp;
    tmp << "Rice(LOS Factor=" << losFactor_ << " variance=" << variance_ << ")";
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

