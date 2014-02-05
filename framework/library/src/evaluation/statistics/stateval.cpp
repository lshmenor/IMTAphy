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

#include <WNS/evaluation/statistics/stateval.hpp>

#include <WNS/pyconfig/View.hpp>
#include <WNS/simulator/ISimulator.hpp>
#include <WNS/Exception.hpp>

#include <iomanip>
#include <climits>
#include <cfloat>
#include <cmath>

using namespace std;

using namespace wns::evaluation::statistics;

STATIC_FACTORY_REGISTER_WITH_CREATOR(StatEval,
                                     StatEvalInterface,
                                     "wns.evaluation.statistics.StatEval",
                                     wns::PyConfigViewCreator);

StatEval::StatEval(formatType format,
                   std::string name,
                   std::string desc)
    : minValue_(DBL_MAX),
      maxValue_(-DBL_MAX),
      numTrials_(0),
      sum_(0.0),
      squareSum_(0.0),
      cubeSum_(0.0),
      format_(format),
      name_(name),
      desc_(desc),
      prefix_("#"),
      scalingFactor_(1.0)
{
}


StatEval::StatEval(const wns::pyconfig::View& config) :
    minValue_(DBL_MAX),
    maxValue_(-DBL_MAX),
    numTrials_(0),
    sum_(0.0),
    squareSum_(0.0),
    cubeSum_(0.0),
    format_((config.get<std::string>("format")=="scientific") ? StatEval::scientific : StatEval::fixed),
    name_(config.get<std::string>("name")),
    desc_(config.get<std::string>("description")),
    prefix_(config.get<std::string>("prefix")),
    scalingFactor_(config.get<double>("scalingFactor"))
{
}

StatEval::~StatEval()
{
}


void
StatEval::print(std::ostream& stream) const
{
    this->printBanner(stream,
                      std::string(prefix_ + " Evaluation: StatEval"),
                      std::string("I/O Error: Can't dump StatEval results"));
}

void
StatEval::printLog(std::ostream&)
{
}

void
StatEval::put(double xI)
{
    xI *= scalingFactor_;

    if(xI > maxValue_)
        maxValue_ = xI;
    if(xI < minValue_)
        minValue_ = xI;

    double tmp = xI;
    sum_ += tmp;
    tmp *= xI;
    squareSum_ += tmp;
    tmp *= xI;
    cubeSum_ += tmp;

    ++numTrials_;
}

double
StatEval::mean() const
{
    return numTrials_ ? sum_ / (double)numTrials_ : 0.0;
}

double
StatEval::variance() const
{
    double theMean;

    if (numTrials_ > 1)
    {
        theMean = mean();
        assert((sqrt(DBL_MAX) > fabs(theMean)));

        double square_mean = theMean * theMean;

        assert(DBL_MAX / square_mean > (double)numTrials_);
        double sum_square = (double)numTrials_ * square_mean;

        return ::max((squareSum_ - sum_square) /
                     ((double)numTrials_ - 1.0) , 0.0);

    }
    else
    {
        return 0.0;
    }
}


double
StatEval::relativeVariance() const
{
    double theMean = mean();

    if (fabs(theMean) > getMaxError<double>())
    {
        assert((sqrt(DBL_MAX) > fabs(theMean)));
        return (variance() / (theMean * theMean));
    }
    else
    {
        return 0.0;
    }
}

double
StatEval::coeffOfVariation() const
{
    double relVar = relativeVariance();

    return relVar > 0.0 ? sqrt(relVar): 0.0;
}

double
StatEval::M2() const
{
    return (numTrials_) ?
        (squareSum_ / (double) numTrials_) : 0.0 ;
}

double
StatEval::M3() const
{
    return (numTrials_) ?
        (cubeSum_ / (double) numTrials_) : 0.0 ;
}

double
StatEval::Z3() const
{
    double m1 = mean();
    double m2 = M2();
    double m3 = M3();

    assert(DBL_MAX / 2.0 / fabs(m1) / fabs(m1) > fabs(m1));

    double twoM1Cube = 2.0 * m1 * m1 * m1;

    assert(DBL_MAX / 3.0 / fabs(m1) > 3.0 * fabs(m2));

    double threeM1M2 = 3.0 * m1 * m2;

    if (m3 > 0.0)
    {
        // m3 - three_m1_2 > -DBL_MAX, as m3 > 0
        // m3 - three_m1_2 < DBL_MAX
        assert(threeM1M2 > - ( DBL_MAX - m3));
    }
    else
    {
        // m3 - threeM1M2 < DBL_MAX, as m3 < 0
        // m3 - threeM1M2 > -DBL_MAX
        // - threeM1M2 > -DBL_MAX - m3
        assert(- threeM1M2 > -DBL_MAX - m3);
    }
    if (twoM1Cube > 0.0)
    {
        assert(m3 - threeM1M2 < DBL_MAX - twoM1Cube);
    }
    else
    {
        assert(m3 - threeM1M2 > -DBL_MAX - twoM1Cube);
    }

    return m3 - threeM1M2 + twoM1Cube;
}

double
StatEval::skewness() const
{
    double var = variance();
    assert(sqrt(var) < pow(DBL_MAX, 1.0/3.0));

    return (var > getMaxError<double>()) ? Z3() / (var * sqrt(var)) : 0.0;
}

double
StatEval::deviation() const
{
    double var = variance();
    return (var > getMaxError<double>()) ? sqrt(var) : 0.0;
}

double
StatEval::relativeDeviation() const
{
    double m1 = mean();
    double var = variance();

    return ((fabs(m1) > getMaxError<double>()) &&
            (var > getMaxError<double>())) ? sqrt(var) / m1 : 0.0;
}

unsigned long int
StatEval::trials() const
{
    return numTrials_;
}

double
StatEval::min() const
{
    return minValue_;
}

double
StatEval::max() const
{
    return maxValue_;
}

void
StatEval::reset()
{
    numTrials_ = 0;
    sum_ = 0;
    squareSum_ = 0;
    cubeSum_ = 0;
    minValue_ = DBL_MAX;
    maxValue_ = -DBL_MAX;
}

const std::string&
StatEval::getName() const
{
    return name_;
}

const std::string&
StatEval::getDesc() const
{
    return desc_;
}

void
StatEval::setFormat(formatType format)
{
    format_ = format;
}

StatEval::formatType
StatEval::getFormat() const
{
    return format_;
}

StatEval::statEvalType
StatEval::mapToStatEvalType(std::string statTypeName)

{
    StatEval::statEvalType statType;


    if (statTypeName == std::string("LREF"))
    {
        statType = StatEval::lref;
    }
    else if (statTypeName == std::string("LREG"))
    {
        statType = StatEval::lreg;
    }
    else if (statTypeName == std::string("PLREF"))
    {
        statType = StatEval::plref;
    }
    else if (statTypeName == std::string("PLREG"))
    {
        statType = StatEval::plreg;
    }
    else if (statTypeName == std::string("Moments"))
    {
        statType = StatEval::moments;
    }
    else if (statTypeName == std::string("PMoments"))
    {
        statType = StatEval::pmoments;
    }
    else if (statTypeName == std::string("BatchMns"))
    {
        statType = StatEval::batchMns;
    }
    else if (statTypeName == std::string("PBatchMns"))
    {
        statType = StatEval::pbatchMns;
    }
    else if (statTypeName == std::string("Histogrm"))
    {
        statType = StatEval::histogrm;
    }
    else if (statTypeName == std::string("PHistogrm"))
    {
        statType = StatEval::phistogrm;
    }
    else if (statTypeName == std::string("DLREF"))
    {
        statType = StatEval::dlref;
    }
    else if (statTypeName == std::string("DLREG"))
    {
        statType = StatEval::dlreg;
    }
    else if (statTypeName == std::string("DLREP"))
    {
        statType = StatEval::dlrep;
    }
    else if (statTypeName == std::string("PDLREF"))
    {
        statType = StatEval::pdlref;
    }
    else if (statTypeName == std::string("PDLREG"))
    {
        statType = StatEval::pdlreg;
    }
    else if (statTypeName == std::string("PDLREP"))
    {
        statType = StatEval::pdlrep;
    }
    else if (statTypeName == std::string("DLREF_NONEQUI"))
    {
        statType = StatEval::dlref_nonequi;
    }
    else if (statTypeName == std::string("DLREG_NONEQUI"))
    {
        statType = StatEval::dlreg_nonequi;
    }
    else if (statTypeName == std::string("DLREP_NONEQUI"))
    {
        statType = StatEval::dlrep_nonequi;
    }
    else if (statTypeName == std::string("PDLREF_NONEQUI"))
    {
        statType = StatEval::pdlref_nonequi;
    }
    else if (statTypeName == std::string("PDLREG_NONEQUI"))
    {
        statType = StatEval::pdlreg_nonequi;
    }
    else if (statTypeName == std::string("PDLREP_NONEQUI"))
    {
        statType = StatEval::pdlrep_nonequi;
    }
    else if (statTypeName == std::string("PDF"))
    {
        statType = StatEval::pdf;
    }
    else if (statTypeName == std::string("PPDF"))
    {
        statType = StatEval::ppdf;
    }
    else if (statTypeName == std::string("Log"))
    {
        statType = StatEval::logeval;
    }
    else if (statTypeName == std::string("PLog"))
    {
        statType = StatEval::plogeval;
    }
    else if (statTypeName == std::string("ProbeText"))
    {
        statType = StatEval::probetext;
    }
    else if (statTypeName == std::string("PProbeText"))
    {
        statType = StatEval::pprobetext;
    }
    else
    {
        throw wns::Exception("StatEval: Unknown type '" + statTypeName + "'!");
    }

    return statType;
}

std::string
StatEval::mapEvalTypeToString(statEvalType statType)
{

    switch(statType)
    {
    case StatEval::lref:
    case StatEval::plref:

        return std::string("LREF");
        break;
    case StatEval::lreg:
    case StatEval::plreg:

        return std::string("LREG");
        break;

    case StatEval::moments:
    case StatEval::pmoments:

        return std::string("Mom");
        break;

    case StatEval::batchMns:
    case StatEval::pbatchMns:

        return std::string("BaM");
        break;

    case StatEval::histogrm:
    case StatEval::phistogrm:

        return std::string("His");
        break;

    case StatEval::dlref:
    case StatEval::pdlref:
    case StatEval::dlref_nonequi:
    case StatEval::pdlref_nonequi:

        return std::string("DLREF");
        break;

    case StatEval::dlreg:
    case StatEval::pdlreg:
    case StatEval::dlreg_nonequi:
    case StatEval::pdlreg_nonequi:

        return std::string("DLREG");
        break;

    case StatEval::dlrep:
    case StatEval::pdlrep:
    case StatEval::dlrep_nonequi:
    case StatEval::pdlrep_nonequi:

        return std::string("DLREP");
        break;

    case StatEval::pdf:
    case StatEval::ppdf:

        return std::string("PDF");
        break;

    case StatEval::logeval:
    case StatEval::plogeval:

        return std::string("Log");
        break;

    case StatEval::probetext:
    case StatEval::pprobetext:

        return std::string("Text");
        break;

    default:

        throw wns::Exception("StatEval: Unknown type");
        break;
    }

}

void
StatEval::printBanner(ostream& stream,
                      std::string probeTypeDesc,
                      std::string errorString) const
{
    std::string prefix(prefix_ + " ");

    std::string separator = prefix + "-------------------------------------";
    separator += "--------------------------------------\n";

    stream << prefix + " PROBE RESULTS (THIS IS A MAGIC LINE)"
           << endl
           << separator
           << probeTypeDesc
           << endl
           << separator
           << prefix+ " Name: "
           << getName()
           << endl
           << prefix + " Description: "
           << getDesc()
           << endl
           << separator;
    if (!stream)
    {
        throw(wns::Exception(errorString));
    }

    stream << prefix + " Evaluation terminated successfully!"
           << endl
           << separator;
    if (!stream)
    {
        throw(wns::Exception(errorString));
    }

    stream << resetiosflags(ios::fixed)
           << resetiosflags(ios::scientific)
           << resetiosflags(ios::right)
           << setiosflags(ios::left)
           << setiosflags(ios::dec)
           << setprecision(7)
           << setw(6)
           << (format_ == fixed ? setiosflags(ios::fixed) :
               setiosflags(ios::scientific))
           << prefix + " Common Statistics " << endl



           << prefix + " Minimum: ";
    if (!stream)
    {
        throw(wns::Exception(errorString));
    }
    if (minValue_ == DBL_MAX)
    {
        stream << "+infinity";
    }
    else
    {
        stream << minValue_;
    }


    stream << endl
           << prefix + " Maximum: ";
    if (!stream)
    {
        throw(wns::Exception(errorString));
    }
    if (maxValue_ == -DBL_MAX)
    {
        stream << "-infinity";
    }
    else
    {
        stream << maxValue_;
    }
    if (!stream)
    {
        throw(wns::Exception(errorString));
    }


    stream << endl
           << prefix + " Trials: "
           << numTrials_
           << endl


           << prefix + " Mean: ";
    if (!stream)
    {
        throw(wns::Exception(errorString));
    }
    if (mean() == DBL_MAX)
    {
        stream << "+infinity";
    }
    else
    {
        stream << mean();
    }
    if (!stream)
    {
        throw(wns::Exception(errorString));
    }


    stream << endl << prefix << endl
           << prefix + " Variance: "
           << variance()
           << endl;

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }


    stream << prefix + " Relative variance: "
           << relativeVariance()
           << endl;

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }


    stream << prefix + " Coefficient of variation: "
           << coeffOfVariation()
           << endl;

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }



    stream << prefix + " Standard deviation: "
           << deviation()
           << endl;

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }


    stream << prefix + " Relative standard deviation: "
           << relativeDeviation()
           << endl;

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }


    stream << prefix << endl
           << prefix + " Skewness: "
           << skewness()
           << endl;

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }


    stream << prefix << endl
           << prefix + " 2nd moment: "
           << M2()
           << endl;

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }


    stream << prefix + " 3rd moment: "
           << M3()
           << endl;

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }


    stream << prefix << endl
           << prefix + " Sum of all values: "
           << sum_
           << endl;

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }


    stream << prefix + " (Sum of all values)^2: "
           << squareSum_
           << endl;

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }


    stream << prefix + " (Sum of all values)^3: "
           << cubeSum_
           << endl;

    if (!stream)
    {
        throw(wns::Exception(errorString));
    }


}

