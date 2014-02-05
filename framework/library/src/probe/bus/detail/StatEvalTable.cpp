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

#include <WNS/probe/bus/detail/StatEvalTable.hpp>

using namespace wns::probe::bus::detail;

Storage::Storage() :
	se(wns::evaluation::statistics::StatEval::scientific,
	   "dummy", "dummy")
{}

Storage::~Storage()
{}

void
Storage::put(double value)
{
    se.put(value);
}

double
Storage::get(const std::string& valueType) const
{
    if (valueType == "mean")
        return se.mean();
    else if (valueType == "variance")
        return se.variance();
    else if (valueType == "relativeVariance")
        return se.relativeVariance();
    else if (valueType == "coeffOfVariation")
        return se.coeffOfVariation();
    else if (valueType == "M2")
        return se.M2();
    else if (valueType == "M3")
        return se.M3();
    else if (valueType == "Z3")
        return se.Z3();
    else if (valueType == "skewness")
        return se.skewness();
    else if (valueType == "deviation")
        return se.deviation();
    else if (valueType == "relativeDeviation")
        return se.relativeDeviation();
    else if (valueType == "trials")
        return se.trials();
    else if (valueType == "min")
        return se.min();
    else if (valueType == "max")
        return se.max();
    else
        assure(false, "Table requested unknown statistics: " << valueType);

    return 0;
}
