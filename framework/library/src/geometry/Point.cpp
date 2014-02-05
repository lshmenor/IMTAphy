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

#include <WNS/Position.hpp>
#include <stdexcept>

using namespace std;
using namespace wns::geometry;

Point::Point() :
point(valarray<double>(0.0,3))
{
}

Point::Point(double x, double y, double z) :
point(valarray<double>(0.0,3))
{
 	if (x < 0.0 || y < 0.0 || z < 0.0)
    {
 		throw out_of_range("x, y, z must be > 0.0!");
 	}
	point[0] = x;
	point[1] = y;
	point[2] = z;
}

Point::Point(const valarray<double>& other) :
point(other)
{
}

Point::Point(const wns::pyconfig::View& view) :
point(valarray<double>(0.0,3))
{
	point[0] = view.get<double>("x");
	point[1] = view.get<double>("y");
	point[2] = view.get<double>("z");
 	if (point[0] < 0.0 || point[1] < 0.0 || point[2] < 0.0)
    {
 		throw out_of_range("x, y, z must be > 0.0!");
 	}
}

Point::~Point()
{
}

double
Point::getX() const
{
	return point[0];
}

double
Point::getY() const
{
	return point[1];
}

double
Point::getZ() const
{
	return point[2];
}

const std::valarray<double>&
Point::get() const
{
	return point;
}

void
Point::set(double x, double y, double z)
{
    point[0] = x;
    point[1] = y;
    point[2] = z;
}

void
Point::setPolar(double r, double phi, double theta)
{
    point[0] = r * sin(theta) * cos(phi);
    point[1] = r * sin(theta) * sin(phi);
    point[2] = r * cos(theta);
}

void
Point::set(const valarray<double>& other)
{
    point = other;
}


void
Point::setX(double x)
{
	// in Antenna::drawAntennaPattern() it is ok to have negative coordinates
	point[0] = x;
 	if (x < 0.0)
    {
 		throw out_of_range("x must be > 0.0!");
 	}
}

void
Point::setY(double y)
{
	// in Antenna::drawAntennaPattern() it is ok to have negative coordinates
	point[1] = y;
 	if (y < 0.0)
    {
 		throw out_of_range("y must be > 0.0!");
 	}
}

void
Point::setZ(double z)
{
	// in Antenna::drawAntennaPattern() it is ok to have negative coordinates
	point[2] = z;
 	if (z < 0.0)
    {
 		throw out_of_range("z must be > 0.0!");
 	}
}

Point
Point::operator+(const Vector& v) const
{
	return Point(get()+v.get());
}

void
Point::operator+=(const Vector& v)
{
	point += v.get();
}

Vector
Point::operator-(const Point& other) const
{
	return Vector(*this, other);
}

void
Point::operator=(const Point& other)
{
	point = other.point;
}

bool
Point::operator==(const Point &other) const
{
	return
		get()[0] == other.get()[0] &&
		get()[1] == other.get()[1] &&
		get()[2] == other.get()[2];
}

bool
Point::operator!=(const Point &other) const
{
	return !(*this == other);
}

std::ostream&
operator<<(std::ostream &str, const Point& p)
{
	str
	<< "[ x=" << p.getX()
	<< ", y=" << p.getY()
	<< ", z=" << p.getZ() << "]";
	return str;
}

bool
Point::operator<(const Point& other) const
{
	// This is a valid definition of a comparision criterion that achieves
	// "strict weak ordering" as needed by the STL:

    return (get()[0] < other.get()[0]) ||
    	   ((get()[0] == other.get()[0]) && (get()[1] < other.get()[1]) ||
		    ((get()[1] == other.get()[1]) && get()[2] < other.get()[2]));


// Alternative using boost tuples:

//#include <boost/tuple/tuple.hpp>
//#include <boost/tuple/tuple_comparison.hpp>

//	return boost::tie(get()[0], get()[1], get()[2]) < boost::tie(other.get()[0], other.get()[1], other.get()[2]);
}

