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

#include <WNS/distribution/CDFTable.hpp>
#include <WNS/module/Base.hpp>
#include <WNS/Interval.hpp>
#include <WNS/container/RangeMap.hpp>

using namespace wns::distribution;

STATIC_FACTORY_REGISTER_WITH_CREATOR(CDFTable, Distribution, "CDFTable", wns::PyConfigViewCreator);
STATIC_FACTORY_REGISTER_WITH_CREATOR(CDFTable, Distribution, "CDFTable", wns::distribution::RNGConfigCreator);

typedef wns::Interval<double> CDFRange;

CDFTable::CDFTable(const pyconfig::View& config) :
    Distribution(),
    dis_(getRNG()),
    mean_(0.0)
{
    int tableLen = config.len("cdfTable");
    assure(tableLen>0, "empty cdfTable");
    double lastCdfValue=0.0;

    for (int ii=0; ii<tableLen; ++ii)
    {
        std::stringstream index;
        index << ii;
        std::string subviewName = "cdfTable" + std::string("[") + index.str() + std::string("]");
        double rnValue = config.get<double>(subviewName, 0);
        double cdfValue = config.get<double>(subviewName,1);
        mean_ += rnValue * (cdfValue - lastCdfValue);
        CDFRange cdfInterval = CDFRange::FromExcluding(lastCdfValue).ToIncluding(cdfValue);
        rangeMap_.insert(cdfInterval, rnValue);
        lastCdfValue = cdfValue;
    }
}

CDFTable::CDFTable(wns::rng::RNGen* rng, const pyconfig::View& config) :
    Distribution(rng),
    dis_(getRNG()),
    mean_(0.0)
{
    int tableLen = config.len("cdfTable");
    assure(tableLen>0, "empty cdfTable");
    double lastCdfValue=0.0;
    for (int ii=0; ii<tableLen; ++ii)

    {
        std::stringstream index;
        index << ii;
        std::string subviewName = "cdfTable" + std::string("[") + index.str() + std::string("]");
        double rnValue = config.get<double>(subviewName, 0);
        double cdfValue = config.get<double>(subviewName, 1);
        mean_ += rnValue * (cdfValue - lastCdfValue);
        CDFRange cdfInterval = CDFRange::FromExcluding(lastCdfValue).ToIncluding(cdfValue);
        rangeMap_.insert(cdfInterval, rnValue);
        lastCdfValue = cdfValue;
    }
}

CDFTable::~CDFTable()
{
}

double
CDFTable::operator()()
{
    return rangeMap_.get(dis_());
}

double
CDFTable::getMean() const
{
    return mean_;
}

std::string
CDFTable::paramString() const
{
    std::ostringstream tmp;
    tmp << "CDFTable()";
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

