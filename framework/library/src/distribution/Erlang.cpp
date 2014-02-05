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

#include <WNS/distribution/Erlang.hpp>

using namespace wns::distribution;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Erlang,
    Distribution,
    "Erlang",
    wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Erlang,
    Distribution,
    "Erlang",
    wns::distribution::RNGConfigCreator);

Erlang::Erlang(const double rate, const int shape, wns::rng::RNGen* rng) :
    Distribution(rng),
    rate_(rate),
    shape_(shape),
    dis_(getRNG())
{
}

Erlang::Erlang(const pyconfig::View& config) :
    Distribution(),
    rate_(config.get<double>("rate")),
    shape_(config.get<int>("shape")),
    dis_(getRNG())
{
}

Erlang::Erlang(wns::rng::RNGen* rng, const pyconfig::View& config) :
    Distribution(rng),
    rate_(config.get<double>("rate")),
    shape_(config.get<int>("shape")),
    dis_(getRNG())
{
}

Erlang::~Erlang()
{
}


double
Erlang::operator()()
{
    double product = 1.0;

    for (unsigned long int i = 0; i < shape_; i++)
    {
        product *= dis_();
    }
    return -log(product) / rate_ ;
}

double
Erlang::getMean() const
{
    return (double)shape_ / rate_;
}

std::string
Erlang::paramString() const
{
    std::ostringstream tmp;
    tmp << "Erlang(rate=" << this->getMean() << ",shape=" << shape_ << ")";
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

