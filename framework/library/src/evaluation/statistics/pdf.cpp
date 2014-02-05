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

#include <WNS/evaluation/statistics/pdf.hpp>

#include <cmath>
#include <iomanip>
#include <climits>
#include <cfloat>

using namespace wns::evaluation::statistics;

STATIC_FACTORY_REGISTER_WITH_CREATOR(PDF,
                                     StatEvalInterface,
                                     "openwns.evaluation.statistics.PDF",
                                     wns::PyConfigViewCreator);

PDF::PDF(double minXValue,
         double maxXValue,
         unsigned long int resolution,
         scaleType scaleType,
         formatType format,
         std::string name,
         std::string description)
    : StatEval(format, name, description),
      minXValue_(minXValue),
      maxXValue_(maxXValue),
      resolution_(resolution),
      scaleType_(scaleType),
      values_(resolution_+1, 0),
      underFlows_(0),
      overFlows_(0)
{
    assure(minXValue_ < maxXValue_,
           "Wrong min/max values. min=" << minXValue_ << ", max="<<maxXValue_);
    assure(resolution_ > 0, "Resolution must be >0, but is " << resolution_);
}

PDF::PDF(const wns::pyconfig::View& config)
    : StatEval(config),
      minXValue_(config.get<double>("minXValue")),
      maxXValue_(config.get<double>("maxXValue")),
      resolution_(config.get<int>("resolution")),
      scaleType_(config.get<std::string>("xScaleType") == "logarithmical" ? logarithmical : linear),
      values_(resolution_+1, 0),
      underFlows_(0),
      overFlows_(0)
{
    assure(minXValue_ < maxXValue_,
           "Wrong min/max values. min=" << minXValue_ << ", max="<<maxXValue_);
    assure(resolution_ > 0, "Resolution must be >0, but is " << resolution_);
}


//! Default copy constructor is correct

PDF::PDF(const PDF& other)
    : StatEval(other),
      minXValue_(0.0),
      maxXValue_(1.0),
      resolution_(100),
      scaleType_(linear),
      values_(),
      underFlows_(0),
      overFlows_(0)
{
    // Copy all internal data
    minXValue_ = other.minXValue_;
    maxXValue_ = other.maxXValue_;
    resolution_ = other.resolution_;
    scaleType_ = other.scaleType_;
    values_ = other.values_;
    name_ = other.name_;
    desc_ = other.desc_;
}

PDF::~PDF()
{
}

void
PDF::print(std::ostream& stream) const
{
    std::string errorString;
    errorString = "I/O Error: Can't dump PDF results";

    printBanner(stream,
                (std::string) (prefix_ + " Evaluation: PDF"),
                errorString);

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }

    std::string prefix = (prefix_ + " ");

    std::string separator;

    unsigned long int i = 0;

    unsigned long int numTrials = 0;

    double x = 0.0;
    double f = 0.0;
    double g = 0.0;
    double p = 0.0;

    separator = prefix + "-------------------------------------";
    separator += "----------------------";

    stream << std::resetiosflags(std::ios::fixed)
           << std::resetiosflags(std::ios::scientific)
           << std::resetiosflags(std::ios::right)
           << std::setiosflags(std::ios::left)
           << std::setiosflags(std::ios::dec)
           << std::setprecision(7)
           << std::setw(6)

           << separator << std::endl << prefix
           << "PDF statistics " << std::endl << prefix
           << " Left border of x-axis: "
           << minXValue_
           << std::endl << prefix
           << " Right border of x-axis: "
           << maxXValue_
           << std::endl << prefix
           << " Resolution of x-axis: "
           << resolution_
           << std::endl
           << separator
           << std::endl;

    stream << prefix
           << "PDF data " << std::endl << prefix
           << " Underflows: "
           << underFlows_ << std::endl;

    stream << prefix
           << " Underflows in percent: "
           << ((numTrials_ > 0) ? (double)(underFlows_) /
               (double)numTrials_ : 0.0)
           << std::endl;

    stream << prefix
           << " Overflows: "
           << (overFlows_) << std::endl;

    stream << prefix
           << " Overflows in percent: "
           << ((numTrials_ > 0) ?
               (double)(overFlows_) /
               (double)numTrials_ : 0.0) << std::endl;

    stream << separator
           << std::endl << prefix
           << "Percentiles" << std::endl;

    for (int ii=1; ii <= 100; ++ii)
    {
        stream << prefix; this->printPercentile( ii, stream);
    }

    stream << separator
           << std::endl << prefix << std::endl << prefix
           << "x_n                  F(x_n)           G(x_n)     P(x_n-1 < X    n"
           << std::endl << prefix
           << "                   =P(X<=x_n)       =P(X>x_n)   AND X <= x_n)"
           << std::endl << prefix << std::endl
           << (format_ == fixed ? setiosflags(std::ios::fixed) :
               setiosflags(std::ios::scientific));


    if (!stream)
    {
        throw(wns::Exception(errorString));
    }

    // Handle the normal intervals
    for (i = 0, numTrials = underFlows_;
         i <= resolution_; ++i)
    {
        // Normal interval
        x = getAbscissa(i);
        numTrials += values_.at(i);

        if (numTrials_ != 0)
        {
            f = double(numTrials) / double(numTrials_);
            assert(0.0 <= f && f <= 1.0);
            g = 1.0 - f;
            p = double(values_.at(i)) / double(numTrials_);
        }
        else
        {
            f = g = p = 0.0;
        }

        stream << resetiosflags(std::ios::right)
               << setiosflags(std::ios::left)
               << std::setw(15)
               << x
               << resetiosflags(std::ios::left)
               << setiosflags(std::ios::right)
               << std::setw(14)
               << f
               << "  "
               << std::setw(14)
               << g
               << "  "
               << std::setw(14)
               << p
               << "  "
               << std::setw(4)
               << i
               << std::endl;
        if (!stream)
        {
            throw(wns::Exception(errorString));
        }
    }

    stream << separator;

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }
}

void
PDF::put(double value)
{
    StatEval::put(value);

    value *= scalingFactor_;

    // Underflow?
    if (value < minXValue_)
    {
        ++underFlows_;
    }
    // Overflow?
    else if (value > maxXValue_)
    {
        ++overFlows_;
    }
    else
    {
        values_.at(this->getIndex(value))++;
    }
}


unsigned long int
PDF::getIndex(double value) const
{
    assert((value >= minXValue_ && value <= maxXValue_) && "Huge, Fat Error!");

    if (this->scaleType_ == PDF::linear)
    {
        return (unsigned long int)(
            ceil((value - minXValue_) * double(resolution_) /
                 (maxXValue_ - minXValue_)));
    }
    else if (this->scaleType_ == PDF::logarithmical)
    {
        double logXMin = log10(minXValue_);
        double logXMax = log10(maxXValue_);
        double logValue = log10(value);
        double logXStep = (logXMax - logXMin)/double(resolution_);

        return  (unsigned long int)( ceil ((logValue - logXMin)/logXStep) );
    }

    throw wns::Exception("Unknown scaleType in PDF!");
    return 0;
}

void
PDF::reset()
{
    StatEval::reset();
    values_ = std::vector<int>(resolution_ + 1);
}

double
PDF::getAbscissa(unsigned long int index) const
{
    if (this->scaleType_ == PDF::linear)
    {
        return minXValue_
            + (maxXValue_ - minXValue_) * double(index) / double(resolution_);
    }
    else if (this->scaleType_ == PDF::logarithmical)
    {
        double logXMin = log10(minXValue_);
        double logXMax = log10(maxXValue_);
        double logXStep = (logXMax - logXMin)/double(resolution_);
        return double(pow(10, logXMin + double(index) * logXStep));
    }
    else
    {
        throw wns::Exception("Unknown scaleType in PDF!");
    }
}

double
PDF::getPercentile(int p) const
{
    assert(p>0 && "Percentile has to be greater than 0!");

    if (numTrials_ == 0)
        return 0.0;

    double P = double(p)/100.0;

    if ( P<= double(underFlows_)/double(numTrials_) )
        throw PercentileUnderFlow();

    if ( P > 1.0 - double(overFlows_)/double(numTrials_) )
        throw PercentileOverFlow();

    double x = 0.0;
    double F = 0.0;
    double dummy1 = 0.0;
    double dummy2 = 0.0;
    double tmpF = 0.0;
    double tmpX = 0.0;

    this->getResult(0, x, F, dummy1, dummy2);

    if (F >= P)
        throw PercentileUnderFlow();

    for (unsigned int ii = 1; ii <= resolution_; ++ii)
    {
        this->getResult(ii, tmpX, tmpF, dummy1, dummy2);

        if (F<P && tmpF >= P)
        {
            if (tmpF == 1.0)
                tmpX = maxValue_;

            return x + (tmpX-x) / (tmpF-F) * (P-F);
        }

        F = tmpF;
        x = tmpX;
    }

    throw wns::Exception("This point should never be reached");
}

void
PDF::printPercentile(int p, std::ostream& stream) const
{
    std::stringstream ss;

    ss << " P"
       << std::setiosflags(std::ios::right) << std::setfill('0') << std::setw(2)
       << p << ": ";

    try
    {
        ss << std::setiosflags(std::ios::dec) << std::setprecision(7) << std::setw(6)
           << std::setiosflags(std::ios::fixed) << this->getPercentile(p);
    }
    catch (PercentileUnderFlow)
    {
        ss << "NaN";
    }
    catch (PercentileOverFlow)
    {
        ss << "NaN";
    }
    stream << ss.str() << std::endl;
}


void
PDF::getResult(unsigned long int index,
               double& abscissa,
               double& F,
               double& G,
               double& P) const
{
    unsigned long int i;
    unsigned long int numTrials = underFlows_;

    assert(resolution_);

    if (numTrials_ == 0)
    {
        abscissa = 0.0;
        F = 0.0;
        G = 0.0;
        P = 0.0;
        return;
    }

    for (i = 0; i <= index; i++)
    {
        numTrials += values_.at(i);
    }
    abscissa = getAbscissa(index);
    F = double(numTrials) / double(numTrials_);
    assert(0.0 <= F && F <= 1.0);
    G = 1.0 - F;
    P = double(values_.at(index)) / double(numTrials_);
}


