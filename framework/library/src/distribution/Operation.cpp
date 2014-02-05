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

#include <WNS/distribution/Operation.hpp>

using namespace wns::distribution;
using namespace wns::distribution::operation;

STATIC_FACTORY_REGISTER_WITH_CREATOR(ADD, Distribution, "ADD", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(MUL, Distribution, "MUL", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(SUB, Distribution, "SUB", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(DIV, Distribution, "DIV", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(Above, Distribution, "ABOVE", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(Below, Distribution, "BELOW", wns::PyConfigViewCreator);

STATIC_FACTORY_REGISTER_WITH_CREATOR(ADD, Distribution, "ADD", wns::distribution::RNGConfigCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(MUL, Distribution, "MUL", wns::distribution::RNGConfigCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(SUB, Distribution, "SUB", wns::distribution::RNGConfigCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(DIV, Distribution, "DIV", wns::distribution::RNGConfigCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(Above, Distribution, "ABOVE", wns::distribution::RNGConfigCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(Below, Distribution, "BELOW", wns::distribution::RNGConfigCreator);

Binary::Binary(const pyconfig::View& config):
    Distribution(),
    config_(config)
{
    init();
} // Binary

Binary::Binary(wns::rng::RNGen* rng, const pyconfig::View& config):
    Distribution(rng),
    config_(config)
{
    init();
} // Binary

void
Binary::init()
{
    pyconfig::View firstConfig(config_, "first");
    std::string firstName = firstConfig.get<std::string>("__plugin__");
    first_ = RNGDistributionFactory::creator(firstName)->create(getRNG(), firstConfig);

    pyconfig::View secondConfig(config_, "second");
    std::string secondName = secondConfig.get<std::string>("__plugin__");
    second_ = RNGDistributionFactory::creator(secondName)->create(getRNG(), secondConfig);
}

double
ADD::operator()()
{
    return (*first_)() + (*second_)();
} // get

double
ADD::getMean() const
{
    IHasMean* disOne;
    IHasMean* disTwo;

    disOne = dynamic_cast<IHasMean*>(first_);
    assure(disOne, "First distribution does not implement getMean()");

    disTwo = dynamic_cast<IHasMean*>(second_);
    assure(disTwo, "First distribution does not implement getMean()");

    return disOne->getMean() + disTwo->getMean();
} // get

std::string
ADD::paramString() const
{
    std::ostringstream tmp;
    tmp << "(" << *first_ << "+" << *second_ << ")";
    return tmp.str();
}

double
MUL::operator()()
{
    return (*first_)() * (*second_)();
} // get

double
MUL::getMean() const
{
    IHasMean* disOne;
    IHasMean* disTwo;

    disOne = dynamic_cast<IHasMean*>(first_);
    assure(disOne, "First distribution does not implement getMean()");

    disTwo = dynamic_cast<IHasMean*>(second_);
    assure(disTwo, "First distribution does not implement getMean()");

    return disOne->getMean() * disTwo->getMean();
} // get

std::string
MUL::paramString() const
{
    std::ostringstream tmp;
    tmp << "(" << *first_ << "*" << *second_ << ")";
    return tmp.str();
}


double
SUB::operator()()
{
    return (*first_)() - (*second_)();
} // get

double
SUB::getMean() const
{
    IHasMean* disOne;
    IHasMean* disTwo;

    disOne = dynamic_cast<IHasMean*>(first_);
    assure(disOne, "First distribution does not implement getMean()");

    disTwo = dynamic_cast<IHasMean*>(second_);
    assure(disTwo, "First distribution does not implement getMean()");

    return disOne->getMean() - disTwo->getMean();
} // get

std::string
SUB::paramString() const
{
    std::ostringstream tmp;
    tmp << "(" << *first_ << "-" << *second_ << ")";
    return tmp.str();
}

double
DIV::operator()()
{
    return (*first_)() / (*second_)();
} // get

double
DIV::getMean() const
{
    IHasMean* disOne;
    IHasMean* disTwo;

    disOne = dynamic_cast<IHasMean*>(first_);
    assure(disOne, "First distribution does not implement getMean()");

    disTwo = dynamic_cast<IHasMean*>(second_);
    assure(disTwo, "First distribution does not implement getMean()");

    assure(disTwo->getMean() != 0.0, "Distributions::DIV: divide by zero");
    return disOne->getMean() / disTwo->getMean();
} // get

std::string
DIV::paramString() const
{
    std::ostringstream tmp;
    tmp << "(" << *first_ << "/" << *second_ << ")";
    return tmp.str();
}


DistributionAndFloat::DistributionAndFloat(const pyconfig::View& config) :
    Distribution(),
    config_(config)
{
    init();
}

DistributionAndFloat::DistributionAndFloat(wns::rng::RNGen* rng, const pyconfig::View& config) :
    Distribution(rng),
    config_(config)
{
    init();
}

void
DistributionAndFloat::init()
{
    pyconfig::View subjectConfig(config_, "subject");
    std::string subjectName = subjectConfig.get<std::string>("__plugin__");
    subject_ = RNGDistributionFactory::creator(subjectName)->create(getRNG(), subjectConfig);

    arg_ = config_.get<double>("arg");
}

double
Above::operator()()
{
    double result;

    do
    {
        result = (*subject_)();
    }

    while(result <= arg_);

    return result;
} // get

std::string
Above::paramString() const
{
    std::ostringstream tmp;
    tmp << "Above(" << *subject_ << "," << arg_ << ")";
    return tmp.str();
}

double
Below::operator()()
{
    double result;

    do
    {
        result = (*subject_)();
    }

    while(result >= arg_);

    return result;
} // get

std::string
Below::paramString() const
{
    std::ostringstream tmp;
    tmp << "Below(" << *subject_ << "," << arg_ << ")";
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

