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

#include "LineSegment.hpp"
#include "Vector.hpp"

using namespace wns::geometry;

//Constructors
LineSegment::LineSegment()
    : Shape2D()
{
}

LineSegment::LineSegment(const Point& a, const Point& b)
    : Shape2D(a,b)
{
    boundingBox = AABoundingBox(a, b);
}

LineSegment::LineSegment(const Point& a, const Vector& db)
    : Shape2D(a, a + db)
{
    boundingBox = AABoundingBox(a, a + db);
}

LineSegment::LineSegment(const wns::pyconfig::View& config)
    : Shape2D(wns::geometry::Point(config.getView("pointA")),
              wns::geometry::Point(config.getView("pointB")))
{
        boundingBox = AABoundingBox(wns::geometry::Point(config.getView("pointA")),
                                    wns::geometry::Point(config.getView("pointB")));
}

LineSegment::~LineSegment()
{
}


bool
LineSegment::contains(const Point& point) const
{
    return crossProduct(point) == 0.0
        && boundingBox.contains(point);
}


bool
LineSegment::leftOf(const Point& point) const
{
    return crossProduct(point) > 0.0;
}

bool
LineSegment::rightOf(const Point& point) const
{
    return crossProduct(point) < 0.0;
}

//LineSegment contains one endpoint of other
bool
LineSegment::touches(const LineSegment& other) const
{
    return contains(other.a) != contains(other.b) 
        && (crossProduct(other.a) != 0.0 
            || crossProduct(other.b) != 0.0);
}


bool
LineSegment::intersects(const LineSegment& that) const
{
    return intersectsBoundingBoxOf(that)
        && ( (this->straddles(that)
          && that.straddles(*this))
            ||(this->touches(that)
            || that.touches(*this)));
}

unsigned int 
LineSegment::countBorderIntersections(const LineSegment& line) const
{
    return intersects(line) ? 1 : 0;
}

double
LineSegment::crossProduct(const Point& x) const
{
    return (b - a).cross(x - a).getDeltaZ();
}

bool
LineSegment::straddles(const LineSegment& other) const
{
    return (other.leftOf(a) == other.rightOf(b));
}
