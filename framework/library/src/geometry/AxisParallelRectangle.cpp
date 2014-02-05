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

#include "AxisParallelRectangle.hpp"
#include "LineSegment.hpp"
#include "AABoundingBox.hpp"

using namespace wns::geometry;

AxisParallelRectangle::AxisParallelRectangle()
    :Shape2D(Point(0,0,0),Point(0,0,0))
{
}

AxisParallelRectangle::AxisParallelRectangle(const Point& a, const Point& b)
    : Shape2D(a,b)
{
    boundingBox = AABoundingBox(a, b);
}

AxisParallelRectangle::AxisParallelRectangle(const Point& a, const Vector& db)
    : Shape2D(a, a + db) 
{
    boundingBox = AABoundingBox(a, a + db);
}

AxisParallelRectangle::AxisParallelRectangle(const wns::pyconfig::View& config)
    : Shape2D(wns::geometry::Point(config.getView("pointA")),
              wns::geometry::Point(config.getView("pointB")))
{
    boundingBox = AABoundingBox(wns::geometry::Point(config.getView("pointA")),
                                wns::geometry::Point(config.getView("pointB")));
}

bool 
AxisParallelRectangle::contains(const Point& point) const
{
    return boundingBox.contains(point);
}

bool 
AxisParallelRectangle::contains(const AxisParallelRectangle& other) const
{
    return boundingBox.contains(other.boundingBox);
}

bool 
AxisParallelRectangle::intersects(const AxisParallelRectangle& other) const
{
    return intersectsBoundingBoxOf(other);
}

bool 
AxisParallelRectangle::intersects(const LineSegment& line) const
{
    if (intersectsBoundingBoxOf(line))
    {
        if (contains(line.getA()) || contains(line.getB())) return true;
        else return bordersIntersect(line);
    }
    else return false;
}

unsigned int 
AxisParallelRectangle::countBorderIntersections(const LineSegment& line) const
{
    if (intersectsBoundingBoxOf(line))
    {
        if (contains(line.getA()) xor contains(line.getB())) return 1;
        else if (bordersIntersect(line)) return 2;
        else return 0;
    }
    else return 0;
}

bool
AxisParallelRectangle::bordersIntersect(const LineSegment& line) const
{
    const double& x1 = boundingBox.a.getX();
    const double& x2 = boundingBox.b.getX();
    const double& y1 = boundingBox.a.getY();
    const double& y2 = boundingBox.b.getY();

    const Point A(x1, y1, 0);
    const Point B(x2, y1, 0);
    const Point C(x2, y2, 0);
    const Point D(x1, y2, 0);

    const LineSegment a(A, B);
    const LineSegment b(B, C);
    const LineSegment c(C, D);
    const LineSegment d(D, A);

    return a.intersects(line) || b.intersects(line)
    || c.intersects(line) || d.intersects(line);
}


