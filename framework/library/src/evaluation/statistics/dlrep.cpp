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

#include <cmath>

#include <WNS/evaluation/statistics/dlrep.hpp>

using namespace std;
using namespace wns::evaluation::statistics;

STATIC_FACTORY_REGISTER_WITH_CREATOR(DLREP,
                                     StatEvalInterface,
                                     "openwns.evaluation.statistics.DLREP",
                                     wns::PyConfigViewCreator);

DLREP::DLREP(std::vector<double> xValuesArr,
             int level,
             double error,
             double preFirst,
             std::string name,
             std::string description,
             bool forceRMinusAOk,
             int maxNrv,
             int skipInterval,
             formatType format)
    : DLRE(xValuesArr, level, error, preFirst, name, description, forceRMinusAOk, maxNrv, skipInterval, format)
{
    if (preFirst < xMin_)
    {
        preIndex_ = indexMin_ - 1;
    }
    else if (preFirst > xMax_)
    {
        preIndex_ = indexMax_;
    }
}


DLREP::DLREP(double xMin,
             double xMax,
             double intSize,
             double error,
             double preFirst,
             std::string name,
             std::string description,
             bool forceRMinusAOk,
             int maxNrv,
             int skipInterval,
             formatType aFormat)
    : DLRE(xMin, xMax, intSize, error, preFirst, name, description, forceRMinusAOk, maxNrv, skipInterval, aFormat)
{
    if (preFirst < xMin_)
    {
        preIndex_ = indexMin_ - 1;
    }
    else if (preFirst > xMax_)
    {
        preIndex_ = indexMax_;
    }
}


//! omnipotent pyconfig constructor
DLREP::DLREP(const wns::pyconfig::View& config) :
    DLRE(config)
{
    double preFirst = config.get<double>("initValue");

    if (preFirst < xMin_)
    {
        preIndex_ = indexMin_ - 1;
    }
    else if (preFirst > xMax_)
    {
        preIndex_ = indexMax_;
    }
}


//! Destructor
DLREP::~DLREP()
{
}


//! print probes result
void DLREP::print(ostream& aStreamRef) const
{
    printAll(aStreamRef, pf, 0.0);
}




//! put trial to probe
void DLREP::put(double value)
{
    if (numTrials_ + 1 < maxNrv_)
    {
        curIndex_ = getIndex(value);

        if (curIndex_ == noIndex)
        {
            ////Speetcl::warning("Warning: Wrong x value in DLREP::put !");
            return;
        }

        StatEval::put(value);
        h_++;

        if (curIndex_ == lower)
        {
            wastedLeft_++;
            if ((preIndex_ >= indexMin_) and (preIndex_ < indexMax_))
            {
                results_[preIndex_].c_++;
            }

            preRv_    = xMin_ - 1.0;
            preIndex_ = indexMin_ - 1;
            return;
        }
        else if (curIndex_ == greater)
        {
            wastedRight_++;
            if ((preIndex_ >= indexMin_) and (preIndex_ < indexMax_))
            {
                results_[preIndex_].c_++;
            }

            preRv_    = xMax_ + 1.0;
            preIndex_ = indexMax_;
            return;
        }
        else if ((preIndex_ != curIndex_) and
                 (preIndex_ >= indexMin_) and
                 (preIndex_ < indexMax_))
        {
            results_[preIndex_].c_++;
        }

        results_[curIndex_].h_++;
        // Increment number of sorted values counters
        for (int i = 0; i <= curIndex_; i++)
        {
            results_[i].sumh_++;
        }

        // check if ready
        if (h_ >= skipInterval_)
        {
            phase_ = rtc();
            h_     = 0;
        }

        // save current values
        preRv_    = value;
        preIndex_ = curIndex_;
    }
    else
    {
        phase_ = finish;
    }
}

void
DLREP::getResultLine(const int index, ResultLine& line) const
{
    if ((index < minIndex()) or (index > maxIndex()))
    {
        //Speetcl::warning("DLREP::getResult():", "index out of range.");
        return;
    }

    double nf = double(numTrials_);
    double vf = 0.0;
    int i;
    for (i = indexMin_; i <= index; i++)
    {
        vf = double(results_[i].h_);
    }

    double p = vf / nf;
    double cf = double(results_[index].c_);

    // large sample conditions not fulfilled ?
    if (not checkLargeSample(index))
    {
        line.rho_ = 0.0;
        line.sigRho_ = 0.0;
        line.relErr_ = 0.0;
    }
    else
    {
        if((fabs(p) < getMaxError<double>()) or (fabs(vf) < getMaxError<double>()))
        {
            line.rho_ = 0.0;
        }
        else
        {
            line.rho_ = 1.0 - cf/vf/(1.0 - p);
        }

        double uf = nf - vf;
        if((fabs(vf) < getMaxError<double>()) or (fabs(uf) < getMaxError<double>()))
        {
            line.sigRho_ = 0.0;
        }
        else
        {
            line.sigRho_ = sqrt(cf * (((1 - cf/vf)/(vf*vf)) + ((1 - cf/uf)/(uf*uf))));
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
    line.vf_ = p * base_;
    line.nx_ = results_[index].h_;
    line.x_  = results_[index].x_;
}


//! change maximum relative error
void DLREP::changeError(double newError)
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
  values to collect before next rtc-call is determined by skipInterval_.*/
DLRE::Phase
DLREP::rtc()
{
    if (numTrials_ < 1000)
    {
        phase_ = initialize;
        return phase_;
    }
    else
    {
        double cf;
        double d_square;
        double rho;
        double nf = double(numTrials_);
        double vf;
        double max_error_square = relErrMax_ * relErrMax_;
        int    i  = indexMin_;

        bool warnVfMinusCfLower10 = false;
        bool vfMinusCfAlwaysZero  = true;

        while (i < indexMax_ - 1)
        {
            vf = results_[i].h_;
            cf = results_[i].c_;

            if (not checkLargeSample(i))
            {
                // We are still waiting for the large sample conditions
                // to be fulfilled
                curLevelIndex_ = i;
                return iterate;
            }
            else
            {
                if (((vf - cf) < 10.0) || (cf < 10.0))
                {
                    if (forceRminusAOK_)
                    {
                        curLevelIndex_ = i;
                        return iterate;
                    }
                    else
                    {
                        warnVfMinusCfLower10 = true;
                    }
                }

                if (((vf - cf) > 0.0) && (cf > 0.0))
                {
                    vfMinusCfAlwaysZero   = false;
                }

                if((fabs(vf) < getMaxError<double>()) or
                   (fabs(nf) < getMaxError<double>()) or
                   (fabs(vf - nf) < getMaxError<double>()))
                {
                    rho =  0.0;
                }
                else
                {
                    rho = 1.0 - cf / vf / (1.0 - vf / nf);
                }

                if((fabs(vf) < getMaxError<double>()) or
                   (fabs(nf) < getMaxError<double>()) or
                   (fabs(rho - 1.0) < getMaxError<double>()))
                {
                    d_square = 0.0;
                }
                else
                {
                    d_square = (1.0 - vf / nf) / vf * (1.0 + rho) / (1.0 - rho);
                }

                if (phase_ == initialize or d_square > max_error_square)
                {
                    phase_ = iterate;
                    curLevelIndex_ = i;
                    return phase_;
                }
            }
            ++i;
        }
        if (warnVfMinusCfLower10)
        {
            //Speetcl::warning("::DLREP::rtc: ",
            //                 "large sample condition (r-a>=10) not fulfilled.");
        }

        if (vfMinusCfAlwaysZero)
        {
            //Speetcl::warning("::DLREP::rtc: ",
            //                 "large sample condition (r-a>=10) not fulfilled;\n",
            //                 "\tr-a yields always zero.");
        }
        phase_ = finish;
        reason_ = ok;
        return phase_;
    }
}
