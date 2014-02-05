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

#include "AABoundingBox.hpp"

using namespace wns::geometry;

AABoundingBox::AABoundingBox() :
    a(),
    b()
{
}

AABoundingBox::AABoundingBox(const Point& a, const Point& b) :
    a(std::min(a.getX(), b.getX()), std::min(a.getY(), b.getY()), 0),
    b(std::max(a.getX(), b.getX()), std::max(a.getY(), b.getY()), 0)
{
}


// AABoundingBox::AABoundingBox(const std::vector<Point>& vertices) :
//  a(),
//  b()
// {
//  /** @todo: minX, minY, minY, maxY aus allen Points */
// }

AABoundingBox::~AABoundingBox()
{
}


bool
AABoundingBox::intersects(const AABoundingBox& other) const
{
    const double& ourX1 = a.getX();
    const double& ourX2 = b.getX();
    const double& ourY1 = a.getY();
    const double& ourY2 = b.getY();
    const double& theirX1 = other.a.getX();
    const double& theirX2 = other.b.getX();
    const double& theirY1 = other.a.getY();
    const double& theirY2 = other.b.getY();
    return (ourX2 >= theirX1)
    && (ourX1 <= theirX2)
    && (ourY2 >= theirY1)
    && (ourY1 <= theirY2);
}

bool
AABoundingBox::contains(const Point& other) const
{
    const double& ourX1 = a.getX();
    const double& ourX2 = b.getX();
    const double& ourY1 = a.getY();
    const double& ourY2 = b.getY();
    const double& theirX = other.getX();
    const double& theirY = other.getY();
    return (ourX1 <= theirX)
    && (ourX2 >= theirX)
    && (ourY1 <= theirY)
    && (ourY2 >= theirY);
}

bool
AABoundingBox::contains(const AABoundingBox other) const
{
    const double& ourX1 = a.getX();
    const double& ourX2 = b.getX();
    const double& ourY1 = a.getY();
    const double& ourY2 = b.getY();
    const double& theirX1 = other.a.getX();
    const double& theirX2 = other.b.getX();
    const double& theirY1 = other.a.getY();
    const double& theirY2 = other.b.getY();
    return (ourX1 <= theirX1)
    && (ourX2 >= theirX2)
    && (ourY1 <= theirY1)
    && (ourY2 >= theirY2);
}




// Operators
void 
AABoundingBox::operator=(const AABoundingBox& other)
{
    a = other.a;
    b = other.b;
}

bool 
AABoundingBox::operator==(const AABoundingBox& other) const
{
    return
            a == other.a &&
            b == other.b;

}

bool 
AABoundingBox::operator!=(const AABoundingBox& other) const
{
    return !(*this == other);
}

