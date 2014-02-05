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

#include <WNS/distribution/Pareto.hpp>

#include <limits>


using namespace wns::distribution;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Pareto,
    Distribution,
    "Pareto",
    wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Pareto,
    Distribution,
    "Pareto",
    wns::distribution::RNGConfigCreator);

Pareto::Pareto(double shapeA, double scaleB, wns::rng::RNGen* rng) :
    Distribution(rng),
    shapeParamA_(shapeA),
    scaleParamB_(scaleB),
    dis_(getRNG())
{
    assert(shapeParamA_ > 0.0);
    assert(scaleParamB_ >= 0.0);
}

Pareto::Pareto(const pyconfig::View& config) :
    Distribution(),
    shapeParamA_(config.get<double>("shapeA")),
    scaleParamB_(config.get<double>("scaleB")),
    dis_(getRNG())
{
    assert(shapeParamA_ > 0.0);
    assert(scaleParamB_ >= 0.0);
}

Pareto::Pareto(wns::rng::RNGen* rng, const pyconfig::View& config) :
    Distribution(rng),
    shapeParamA_(config.get<double>("shapeA")),
    scaleParamB_(config.get<double>("scaleB")),
    dis_(getRNG())
{
    assert(shapeParamA_ > 0.0);
    assert(scaleParamB_ >= 0.0);
}


Pareto::~Pareto()
{
}


double
Pareto::operator()()
{
    double f = 0.0;
    double x = scaleParamB_;
    // Avoid endless loops
    unsigned long int max_num_loops = 1000; 

    while (max_num_loops--)
    {
        f = dis_();
        assert(0.0 <= f && f <= 1.0);
        if (f == 1.0)
        {
            continue;
        }
        x = scaleParamB_ / pow(1.0 - f, 1.0 / shapeParamA_);
        return x;
    }
    assure(false, "Something very unlikely happened.");
    return x;
}

double
Pareto::getMean() const
{
    if (shapeParamA_ <= 1.0)
    {
        return std::numeric_limits<double>::max();
    }

    return shapeParamA_ * scaleParamB_ / (shapeParamA_ - 1.0);
}

std::string
Pareto::paramString() const
{
    std::ostringstream tmp;
    tmp << "Pareto(A=" << shapeParamA_ << ",B=" << scaleParamB_ << ")";
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

