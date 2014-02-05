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
#include <WNS/distribution/Binomial.hpp>

using namespace wns::distribution;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Binomial,
    Distribution,
    "Binomial",
    wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(
    Binomial,
    Distribution,
    "Binomial",
    wns::distribution::RNGConfigCreator);

Binomial::Binomial(int numTrials , double probability, wns::rng::RNGen* rng) :
    Distribution(rng),
    numberOfTrials_(numTrials),
    probability_(probability),
    dis_(getRNG())
{
}

Binomial::Binomial(const pyconfig::View& config) :
    Distribution(),
    numberOfTrials_(config.get<int>("numberOfTrials")),
    probability_(config.get<double>("probability")),
    dis_(getRNG())
{
}

Binomial::Binomial(wns::rng::RNGen* rng, const pyconfig::View& config) :
    Distribution(rng),
    numberOfTrials_(config.get<int>("numberOfTrials")),
    probability_(config.get<double>("probability")),
    dis_(getRNG())
{
}

Binomial::~Binomial()
{
}

double
Binomial::operator()()
{
    long int num_of_events = 0;
    long int current_trial;

    for (current_trial = 0; current_trial < numberOfTrials_; current_trial++)
    {
        if (dis_() < probability_)
        {
            num_of_events++;
        }
    }
    return(double(num_of_events));
}

double
Binomial::getMean() const
{
    return probability_ * numberOfTrials_;
}

std::string
Binomial::paramString() const
{
    std::ostringstream tmp;
    tmp << "Binomial(N=" << numberOfTrials_ << ",p=" << probability_ << ")";
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

