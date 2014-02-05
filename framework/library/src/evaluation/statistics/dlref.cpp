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

#include <WNS/evaluation/statistics/dlref.hpp>
#include <cmath>

using namespace std;
using namespace wns::evaluation::statistics;

STATIC_FACTORY_REGISTER_WITH_CREATOR(DLREF,
                                     StatEvalInterface,
                                     "openwns.evaluation.statistics.DLREF",
                                     wns::PyConfigViewCreator);

DLREF::DLREF(std::vector<double> xValuesArrPtr,
             int level,
             double error,
             double preFirst,
             std::string name,
             std::string description,
             bool forceRMinusAOk,
             double fMin,
             int maxNrv,
             int skipInterval,
             formatType format)
    : DLRE(xValuesArrPtr, level, error, preFirst, name, description, forceRMinusAOk, maxNrv, skipInterval, format),
      fMin_(fMin)

{
    curLevelIndex_ = indexMax_ - 1;
}

DLREF::DLREF(double xMin,
             double xMax,
             double intSize,
             double error,
             double preFirst,
             std::string name,
             std::string description,
             bool forceRMinusAOk,
             double fMin,
             int maxNrv,
             int skipInterval,
             formatType format)
    : DLRE(xMin, xMax, intSize, error, preFirst, name, description, forceRMinusAOk, maxNrv, skipInterval, format),
      fMin_(fMin)
{
    curLevelIndex_ = indexMax_ - 1;
}

DLREF::DLREF(const wns::pyconfig::View& config) :
    DLRE(config),
    fMin_(config.get<double>("minLevel"))
{
    curLevelIndex_ = indexMax_ - 1;
}

DLREF::~DLREF()
{}

void DLREF::print(ostream& stream) const
{
    printAll(stream, df, fMin_);
}

void DLREF::put(double value)
{
    if (numTrials_ + 1 < maxNrv_)
    {
        curIndex_ = getIndex(value);

        if (curIndex_ == noIndex)
        {
            throw wns::Exception("Warning: Wrong x value in DLREF::put !");
            return;
        }
        StatEval::put(value);
        ++h_;

        if (curIndex_ == lower)
        {
            ++wastedLeft_;
            preRv_ = xMin_ - 1.0;
            preIndex_ = indexMin_;
            return ;
        }
        else if (curIndex_ == greater)
        {
            ++wastedRight_;
            if (preRv_ < value)
            {
                for (int i = preIndex_; i <= (indexMax_ - 1); i++)
                {
                    ++(results_[i].c_);
                }
            }

            preRv_ = xMax_ + 1.0;
            preIndex_ = indexMax_ - 1;
            return;
        }
        else if (preRv_ < value)
        {
            for (int i = preIndex_; i < curIndex_; i++)
            {
                (results_[i].c_)++;
            }
        }

        ++(results_[curIndex_].h_);
        // Increment number of sorted values counters
        for (int i = 0; i <= curIndex_; i++)
        {
            ++(results_[i].sumh_);
        }

        // check if ready
        if (h_ >= skipInterval_)
        {
            phase_ = rtc();
            h_ = 0;
        }

        // save current values
        preRv_ = value;
        preIndex_ = curIndex_;
    }
    else
    {
        phase_ = finish;
    }
}

double
DLREF::curFLev()
{
    if (not checkLargeSample(curLevelIndex_))
    {
        return 1.0;
    }
    else
    {
        // subtract all values that are 'right' from the current
        // level (including the overflows)

        int vf = numTrials_ - wastedRight_;
        for (int i = indexMax_ - 1; i >= curLevelIndex_; --i)
        {
            vf -= results_[i].h_;
        }
        return (double)vf / double(numTrials_);
    }
}

double
DLREF::f(double xt)
{
    if (numTrials_ < 1000)
    {
        return 1.0;
    }
    else
    {
        int i;
        int vf = numTrials_ - results_[indexMin_].h_ - wastedRight_;

        for (i = indexMax_ - 1;
             (i > indexMin_) and (fabs(results_[i].x_ - xt) > getMaxError<double>());
             --i)
        {
            vf -= results_[i + 1].h_;
        }

        if (fabs(results_[i].x_ - xt) < getMaxError<double>())
        {
            return (double)vf / (double)numTrials_;
        }
        else
        {
            return -1.0;
        }
    }
}

//! return probe's results
void
DLREF::getResultLine(int index, ResultLine& line) const
{
    if ((index < minIndex()) || (index > maxIndex()))
    {
        throw wns::Exception("DLREF::getResult(): index out of range.");
        return;
    }

    double nf = double(numTrials_);
    double vf = wastedLeft_;
    int i;
    for (i = indexMin_; i <= index; i++)
    {
        vf += double(results_[i].h_);
    }

    double F = vf / nf;
    double cf = double(results_[ index ].c_);
    // large sample conditions not fulfilled ?
    if (not checkLargeSample(index))
    {
        line.rho_ = 0.0;
        line.sigRho_ = 0.0;
        line.relErr_ = 0.0;
    }
    else
    {
        if((fabs(F - 1.0) < getMaxError<double>()) or (fabs(vf) < getMaxError<double>()))
        {
            line.rho_ = 0.0;
        }
        else
        {
            line.rho_ = 1.0 - cf/vf/( 1.0 - F );
        }

        double uf = nf - vf;

        if((fabs(vf) < getMaxError<double>()) or (fabs(uf) < getMaxError<double>()))
        {
            line.sigRho_ = 0.0;
        }
        else
        {
            line.sigRho_ = sqrt(cf * (((1.0 - cf/vf)/(vf*vf)) + ((1.0 - cf/uf)/(uf*uf))));
        }

        if((fabs(vf) < getMaxError<double>()) or (fabs(line.rho_ - 1.0) < getMaxError<double>()))
        {
            line.relErr_ = 0.0;
        }
        else
        {
            line.relErr_ = sqrt((1.0 - vf/nf)/vf * (1.0 + line.rho_)/(1.0 - line.rho_));
        }
    }
    line.vf_ = F * base_;
    line.nx_ = results_[index].h_;
    line.x_  = results_[index].x_;
}

void
DLREF::changeError(double newError)
{
    if (newError < relErrMax_)
    {
        curLevelIndex_ = indexMax_ - 1;
    }
    relErrMax_ = newError;
    phase_ = rtc();
    h_ = 0;
}

/*! Run Time Control (RTC) function, checks wether large sample conditions are
  fulfilled and measured error is lower than provided one; the number of
  values to collect before next rtc-call is determined by skipInterval.*/
DLRE::Phase
DLREF::rtc()
{
    if (numTrials_ < 1000)
    {
        phase_ = initialize;
        return phase_;
    }

    if (curFLev() <= fMin_)
    {
        phase_ = finish;
        reason_ = minimum;
        return phase_;
    }
    else
    {
        double cf;
        double dSquare;
        double rho;
        double nf = double(numTrials_);
        double vf;
        double maxErrorSquare = relErrMax_ * relErrMax_;

        // check the next level, which is the current - 1
        int i = curLevelIndex_ - 1;

        while (i > indexMin_)
        {
            cf = results_[i - 1].c_;
            vf = (double)(numTrials_ - wastedRight_ - results_[i].sumh_);

            if (not checkLargeSample(i))
            {
                // We are still waiting for the large sample conditions
                // to be fulfilled
                ////curLevelIndex_ = i;
                return iterate;
            }
            else
            {
                // The large sample conditions are fulfilled, so check
                // the estimated number of samples

                // special treatment for last level (index 0)
                if ((i == indexMin_ + 1) and
                    (fabs(vf - cf) < getMaxError<double>()) and
                    (fabs(minValue_ - xMin_) < getMaxError<double>()))
                {
                    phase_ = finish;
                    reason_ = last;
                    return phase_;
                }

                if((fabs(vf) < getMaxError<double>()) or
                   (fabs(nf) < getMaxError<double>()) or
                   (fabs(vf - nf) < getMaxError<double>()))
                {
                    rho = 0.0;
                }
                else
                {
                    rho = 1.0 - (cf / vf) / (1.0 - (vf / nf));
                }

                if((fabs(vf) < getMaxError<double>()) or
                   (fabs(nf) < getMaxError<double>()) or
                   (fabs(rho - 1.0) < getMaxError<double>()))
                {
                    dSquare = 0.0;
                }
                else
                {
                    dSquare = (1.0 - (vf / nf)) / vf * (1.0 + rho) / (1.0 - rho);
                }

                if (phase_ == initialize or dSquare > maxErrorSquare)
                {
                    phase_ = iterate;
                    ////curLevelIndex_ = i;
                    return phase_;
                }
            }
            curLevelIndex_ = i;
            --i;
        }

        phase_ = finish;
        reason_ = ok;
        return phase_;
    }
}
