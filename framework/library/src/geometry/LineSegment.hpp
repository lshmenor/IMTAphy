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

#ifndef WNS_GEOMETRY_LINESEGMENT_HPP
#define WNS_GEOMETRY_LINESEGMENT_HPP

#include "Shape2D.hpp"
#include "AABoundingBox.hpp"

#include <WNS/pyconfig/View.hpp>

namespace wns {  namespace geometry {

    class LineSegment : public Shape2D
    {
    public:
        LineSegment();
        LineSegment(const Point& a, const Point& b);
        LineSegment(const Point& a, const Vector& db);
        LineSegment(const wns::pyconfig::View& config);

    virtual
    ~LineSegment();

    const Point& 
    getA() const
    {
        return a;
    }

    const Point& 
    getB() const
    {
        return b;
    }

    bool
    contains(const Point& point) const;

    bool
    leftOf(const Point& point) const;

    bool
    rightOf(const Point& point) const;

    bool
    touches(const LineSegment& other) const;

    bool
    intersects(const LineSegment& that) const;

    virtual unsigned int
    countBorderIntersections(const LineSegment& line) const;

    double
    crossProduct(const Point& x) const;

    bool
    straddles(const LineSegment& other) const;

    };
}//geometry
}//wns


#endif // WNS_GEOMETRY_LINESEGMENT_HPP
