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

#include <WNS/evaluation/statistics/moments.hpp>

#include <cmath>
#include <climits>
#include <cfloat>

using namespace wns::evaluation::statistics;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Moments,
                                     StatEvalInterface,
                                     "openwns.evaluation.statistics.Moments",
                                     wns::PyConfigViewCreator);


Moments::Moments():
    StatEval(StatEval::fixed, "", ""),
    wSum_(0.0)
{
}

Moments::Moments(std::string name,
                 std::string description,
                 formatType format)
    : StatEval(format, name, description),
      wSum_(0.0)
{
}

Moments::Moments(const wns::pyconfig::View& config) :
    StatEval(config),
    wSum_(0.0)
{
}


Moments::~Moments()
{
}


void
Moments::print(std::ostream& stream) const
{
    std::string errorString;
    errorString = "I/O Error: Can't dump Moments results";

    printBanner(stream,
                (prefix_ + " Evaluation: Moments"),
                errorString);

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }
}


void
Moments::put(double xI, double wI)
{
    // only positive weights
    assert(wI > 0.0);

    xI *= scalingFactor_;

    // this causes the square to be positive
    double square = wI * xI * xI;
    double cube = xI * square;
    sum_ += wI * xI;

    // if all weights are 1, this is equal to numTrials_
    // wSum_ is positive
    wSum_ += wI;
    squareSum_ += square;
    cubeSum_ += cube;

    if (xI > maxValue_)
    {
        maxValue_ = xI;
    }

    if (xI < minValue_)
    {
        minValue_ = xI;
    }

    numTrials_++;
}

void
Moments::put(double xI)
{
    put(xI, 1.0);
}


double
Moments::mean() const
{
    return numTrials_ ? sum_ / wSum_ : 0.0;
}

double
Moments::variance() const
{
    double theMean;

    if (numTrials_ > 1)
    {
        theMean = mean();
        assert((sqrt(DBL_MAX) > fabs(theMean)));

        double squareMean = theMean * theMean;

        // wSum_ is always positive
        assert(DBL_MAX / squareMean > wSum_);
        double sumSquare = wSum_ * squareMean;

        return std::max((squareSum_ - sumSquare) /
                        (wSum_ - 1.0) , 0.0);
    }
    else
    {
        return 0.0;
    }
}

double
Moments::getConfidenceInterval95Mean() const
{
    // ci = 0.95
    // --> p = 1-(1-cl)/2 = 0.975
    // --> norminv of p with mean 0 and variance 1 = 1.95996398454005

    return this->getConfidenceIntervalMean(1.95996398454005);
}

double
Moments::getConfidenceInterval99Mean() const
{
    // see above
    return this->getConfidenceIntervalMean(2.57582930354890);
}

double
Moments::getConfidenceIntervalMean(double x) const
{
    if (this->numTrials_ < 100)
    {
        // need at least 100 trials to approximate the student-t distribution
        // with the Gaussian distribution (--> Error < 5%)
        return (DBL_MAX);
    }

    double std = sqrt(this->variance());
    unsigned long int n = this->trials();

    double cimu = (2*x*std)/(sqrt(n));
    return cimu;
}

double
Moments::M2() const
{
    return (wSum_ > getMaxError<double>()) ?
        (squareSum_ / wSum_) : 0.0;
}

double
Moments::M3() const
{
    return (wSum_ > getMaxError<double>()) ?
        (cubeSum_ / wSum_) : 0.0 ;
}

void
Moments::reset()
{
    StatEval::reset();
    wSum_ = 0;
}
