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

#ifndef WNS_GEOMETRY_VECTOR_HPP
#define WNS_GEOMETRY_VECTOR_HPP

#include <WNS/pyconfig/View.hpp>
#include <valarray>

namespace wns { namespace geometry {
    /**
     * @brief A 3D position offset (deltaX,deltaY,deltaZ)
     */
    class Vector
    {
    public:
        // con-/destructor
        Vector();

        Vector(double dx, double dy, double dz);

        explicit
        Vector(const std::valarray<double>& other);

        /**
         * @brief Vector between p1 and p2.
         *
         * The resulting Vector will point from p1 to p2.
         */
        Vector(const Point& p1, const Point& p2) :
            vec(p1.get()-p2.get())
        {
        }

        virtual
        ~Vector();

        // setter
        void
        set(double dx, double dy = 0.0, double dz = 0.0);

        void
        setPolar(double r, double phi, double theta);

        void
        set(const std::valarray<double>& posOffset);

        void
        setDeltaX(double dx);

        void
        setDeltaY(double dy);

        void
        setDeltaZ(double dz);

        void
        setR(double r);

        void
        setAzimuth(double phi);

        void
        setElevation(double theta);

        // getter
        /**
         * @brief forwards to getAzimuth
         * @deprecated pls use getAzimuth
         */
        double
        getPhi() const;
        /**
         * @brief forwards to getElevation
         * @deprecated pls use getElevation
         */
        double
        getTheta() const;

        const std::valarray<double>&
        get() const;

        double
        getDeltaX() const;

        double
        getDeltaY() const;

        double
        getDeltaZ() const;

        double
        getR() const;

        double
        getAzimuth() const;

        double
        getElevation() const;

        // operator
        Point
        operator + (const Point& point) const;

        Vector
        operator + (const Vector& other) const;

        Vector
        operator * (const int scale) const;

        Vector
        operator * (const double scale) const;

        void
        operator = (const Vector& other);

        bool
        operator==(const Vector& other) const;

        bool
        operator!=(const Vector& other) const;

        // misc
        Vector
        cross(const Vector& other) const;

        double
        dot(const Vector& other) const;

        double
        abs() const;

    private:
        std::valarray<double> vec;
    };
} // geometry
} // wns

#endif // NOT defined WNS_GEOMETRY_VECTOR_HPP
