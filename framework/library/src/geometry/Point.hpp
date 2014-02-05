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

#ifndef WNS_GEOMETRY_POINT_HPP
#define WNS_GEOMETRY_POINT_HPP

#include <WNS/pyconfig/View.hpp>

#include <ostream>
#include <valarray>

namespace wns { namespace geometry {
	class Vector;

	/**
	 * @brief A 3D position (x,y,z)
	 */
	class Point
    {
    	public:
		// con-/destructors
		Point();

		Point(double x, double y, double z);

		explicit
		Point(const std::valarray<double>& other);

		explicit
		Point(const wns::pyconfig::View& view);

		virtual
		~Point();

		// getter
		double
		getX() const;

		double
		getY() const;

		double
		getZ() const;

		const std::valarray<double>&
		get() const;

		// setter
		void
		setX(double x);

		void
		setY(double y);

		void
		setZ(double z);

		void
		set(double x, double y, double z);

		void
		set(const std::valarray<double>& other);

		void
		setPolar(double r, double phi, double theta);

		// operator
		Point
		operator+(const Vector& v) const;

		void
		operator+=(const Vector& v);

		Vector
		operator-(const Point& other) const;

		void
		operator=(const Point& other);

		bool
		operator==(const Point& other) const;

		bool
		operator!=(const Point& other) const;
        
        bool operator<(const Point& other) const;

		friend std::ostream&
		operator<<(std::ostream& str, const Point& point)
		{
			str << "x: " << point.getX()
			    << ", y: " << point.getY()
			    << ", z: " << point.getZ();
			return str;
		}

	private:
		std::valarray<double> point;
	};
} // geometry
} // wns
#endif // WNS_GEOMETRY_POINT_HPP


