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

#include <WNS/distribution/tests/VarEstimator.hpp>

using namespace wns::distribution::test;

VarEstimator::VarEstimator()
{
    reset();
}

VarEstimator::~VarEstimator()
{
}

void
VarEstimator::reset()
{
    mean_.reset();
    sampleCount_ = 0;
    squareSum_ = 0.0;
}

void
VarEstimator::put(double value)
{
    mean_.put(value);
    sampleCount_++;
    squareSum_ += (value * value);
}

double
VarEstimator::get()
{
    double temp1 = (1.0 / (sampleCount_ - 1.0)) * squareSum_;
    double temp2 = (sampleCount_ / (sampleCount_ - 1.0)) * (mean_.get() * mean_.get());  
    return temp1 - temp2;
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
