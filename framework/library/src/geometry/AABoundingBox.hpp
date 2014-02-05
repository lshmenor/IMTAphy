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

#ifndef WNS_GEOMETRY_AABOUNDINGBOX_HPP
#define WNS_GEOMETRY_AABOUNDINGBOX_HPP

#include <vector>
#include "Point.hpp"

namespace wns { namespace geometry {

    // Axis aligned bounding box of a 2D object
    class AABoundingBox
    {
        friend class Shape2D;
        friend class AxisParallelRectangle;

        public:
        AABoundingBox();
        AABoundingBox(const Point& a, const Point& b);
//      AABoundingBox(const std::vector<Point>& vertices);
        ~AABoundingBox();

        bool
        contains(const Point& other) const;

        bool
        contains(const AABoundingBox other) const;

        bool
        intersects(const AABoundingBox& other) const;

        Point
        getA() const
        {
            return a;
        }

        Point
        getB() const
        {
            return b;
        }

        double
        getMinX()
        {
            return a.getX();
        }

        double
        getMaxX()
        {
            return b.getX();
        }

        double
        getMinY()
        {
            return a.getY();
        }

        double
        getMaxY()
        {
            return b.getY();
        }

        void
        operator=(const AABoundingBox& other);

        bool
        operator==(const AABoundingBox& other) const;

        bool
        operator!=(const AABoundingBox& other) const;

        private:
        Point a;
        Point b;

    };

}//geometry
}//wns

#endif //WNS_GEOMETRY_AABOUNDINGBOX_HPP

