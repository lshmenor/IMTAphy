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

#ifndef WNS_GEOMETRY_AXISPARALLELRECTANGLE_HPP
#define WNS_GEOMETRY_AXISPARALLELRECTANGLE_HPP

#include "Shape2D.hpp"
#include "Point.hpp"
#include "Vector.hpp"
#include "LineSegment.hpp"
#include "AABoundingBox.hpp"

#include <WNS/pyconfig/View.hpp>

namespace wns {  namespace geometry {

    class AxisParallelRectangle : public Shape2D
    {
    public:
        AxisParallelRectangle();
        AxisParallelRectangle(const Point& a, const Point& b);
        AxisParallelRectangle(const Point& a, const Vector& db);
        AxisParallelRectangle(const wns::pyconfig::View& config);

    Point
    getA() const
    {
        return Point(std::min(a.getX(), b.getX()), 
                     std::min(a.getY(), b.getY()), 0);
    }

    Point
    getB() const
    {
        return Point(std::max(a.getX(), b.getX()),
                     std::max(a.getY(), b.getY()), 0);
    }

    bool
    contains(const wns::geometry::Point& point) const;

    bool
    contains(const AxisParallelRectangle& other) const;

    bool
    intersects(const AxisParallelRectangle& other) const;

    bool
    intersects(const wns::geometry::LineSegment& line) const;

    virtual unsigned int
    countBorderIntersections(const LineSegment& line) const;

    bool
    bordersIntersect(const LineSegment& line) const;
    };
}//geometry
}//wns


#endif // WNS_GEOMETRY_AXISPARALLELRECTANGLE
