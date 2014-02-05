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

#include <WNS/probe/bus/detail/Sorter.hpp>
#include <sstream>

using namespace wns::probe::bus::detail;

Sorter::Sorter(const wns::pyconfig::View& pyco) :
    idName_(pyco.get<std::string>("idName")),
    min_(pyco.get<IDType>("minimum")),
    max_(pyco.get<IDType>("maximum")),
    resolution_(pyco.get<int>("resolution")),
    stepsize_((max_-min_)/resolution_)
{
    assure(stepsize_!=0, "Stepsize=0!!, Wrong parameters: min="<<min_<<",max="<<max_<<",resolution="<<resolution_);
}

Sorter::Sorter(std::string _idName, IDType _min, IDType _max, int _resolution) :
    idName_(_idName),
    min_(_min),
    max_(_max),
    resolution_(_resolution),
    stepsize_((max_-min_)/resolution_)
{}

int
Sorter::calcIndex(IDType id) const
{
    if (id == max_)
        return resolution_-1;

    return (id-min_) / stepsize_;
}

int
Sorter::getIndex(IDType id) const
{
    assure(checkIndex(id), "Sorter: wrong value"<<id<<", try checkIndex first");
    return calcIndex(id);
}

bool
Sorter::checkIndex(IDType id) const
{
    int index = calcIndex(id);

    if (index < 0 || index>=resolution_)
        return false;

    return true;
}

std::string
Sorter::getInterval(int index) const
{
    IDType imin =  getMin(index);
    IDType imax = imin + stepsize_;
    std::stringstream ss;
    ss<<"["<< imin<<"-"<<imax<< ( index == resolution_-1 ? "]" : "[" );
    return ss.str();
}

int
Sorter::getResolution() const
{
    return resolution_;
}

IDType
Sorter::getMin(int index) const
{
    assure(0 <= index, "Invalid index: " << index << ", must be larger or equal to 0");
    assure(index < resolution_, "Invalid index: "<< index << ", must be smaller than resolution = " << resolution_);

    return min_ + stepsize_ * index;
}

std::string
Sorter::getIdName() const
{
    return idName_;
}

