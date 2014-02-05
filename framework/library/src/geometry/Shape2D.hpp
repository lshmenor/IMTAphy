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

#ifndef WNS_GEOMETRY_SHAPE2D_HPP
#define WNS_GEOMETRY_SHAPE2D_HPP

#include "Point.hpp"
#include "AABoundingBox.hpp"

namespace wns { namespace geometry {

    class Shape2D
    {

    public:
    virtual ~Shape2D()
    {
    };

    AABoundingBox getBoundingBox() const
    {
        return boundingBox;
    }

    virtual bool
    contains(const Point& point)const = 0;

    bool intersectsBoundingBoxOf(const Shape2D& other) const;

    void
    operator = (const Shape2D& other);

    bool
    operator==(const Shape2D& other) const;

    bool
    operator!=(const Shape2D& other) const;


//  virtual unsigned int countBorderIntersections(const LineSegments& line)

    protected:
    Shape2D();

    Shape2D(const Point& a, const Point& b);

    Shape2D(const Point& a, const Vector& v);

    static Point zeroZ(const Point& p);

    Point a;
    Point b;
    AABoundingBox boundingBox;

    };

} // geometry
} // wns

#endif // WNS_GEOMETRY_SHAPE2D_HPP
