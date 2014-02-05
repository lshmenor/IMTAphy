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


#include "Shape2D.hpp"

using namespace wns::geometry;

Shape2D::Shape2D()
    :a(),
     b(),
     boundingBox()
{
}

Shape2D::Shape2D(const Point& a, const Point& b)
    :a(zeroZ(a)),
     b(zeroZ(b)),
     boundingBox()
{
}

Shape2D::Shape2D(const Point& a, const Vector& db)
    :a(zeroZ(a)),
     b(zeroZ(a + db)),
     boundingBox()
{
}

bool
Shape2D::intersectsBoundingBoxOf(const Shape2D& other) const
{
    return boundingBox.intersects(other.boundingBox);
}


Point
Shape2D::zeroZ(const Point& p)
{
    Point zZeroP(p);
    zZeroP.setZ(0);
    return zZeroP;
}


// Operators
void
Shape2D::operator=(const Shape2D& other)
{
    a = other.a;
    b = other.b;
    boundingBox = other.boundingBox;
}

bool
Shape2D::operator==(const Shape2D& other) const
{
    return
        a == other.a &&
        b == other.b;
}

bool
Shape2D::operator!=(const Shape2D& other) const
{
    return !(*this == other);
}
