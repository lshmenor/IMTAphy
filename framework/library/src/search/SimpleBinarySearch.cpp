/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
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

#include <WNS/search/SimpleBinarySearch.hpp>
#include <WNS/search/SearchCreator.hpp>

#include <iostream>

using namespace wns::search;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
    SimpleBinarySearch,
    ISearch,
    "wns.search.SimpleBinarySearch",
    SearchCreator);

SimpleBinarySearch::SimpleBinarySearch(int lowerBound, int startValue, Comparator comp)
    : lowerBound_(lowerBound),
      startValue_(startValue),
      comp_(comp)
{
} // SimpleBinarySearch


int SimpleBinarySearch::search(int upperBound)
{
    int currentValue = startValue_;
    int currentLowerBound = lowerBound_;
    int currentUpperBound = upperBound;

    while (true)
    {
        if (comp_(currentValue))
        {
            currentLowerBound = currentValue;
            currentValue += (currentUpperBound - currentValue) / 2;
        }
        else
        {
            currentUpperBound = currentValue;
            currentValue -= (currentValue - currentLowerBound) / 2;
        }

        if (currentLowerBound + 1 == currentUpperBound)
        {
            return currentLowerBound;
        }
        else if (currentLowerBound == currentUpperBound)
        {
            return currentLowerBound - 1;
        }
    }
} // search
