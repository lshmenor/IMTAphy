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

#include "Point.hpp"
#include "Vector.hpp"

using namespace std;
using namespace wns::geometry;


Vector::Vector() :
    vec(valarray<double>(0.0,3))
{
}

Vector::Vector(double dx, double dy, double dz) :
    vec(valarray<double>(0.0,3))
{
    vec[0] = dx;
    vec[1] = dy;
    vec[2] = dz;
}

Vector::Vector(const valarray<double>& other) :
    vec(other)
{
}

Vector::~Vector()
{
}

void
Vector::set(double dx, double dy, double dz)
{
    vec[0] = dx;
    vec[1] = dy;
    vec[2] = dz;
}

void
Vector::setPolar(double r, double phi, double theta)
{
    vec[0] = r * sin(theta) * cos(phi);
    vec[1] = r * sin(theta) * sin(phi);
    vec[2] = r * cos(theta);
}

void
Vector::set(const valarray<double>& other)
{
    vec = other;
}

double
Vector::getDeltaX() const
{
    return vec[0];
}

double
Vector::getDeltaY() const
{
    return vec[1];
}

double
Vector::getDeltaZ() const
{
    return vec[2];
}

void
Vector::setDeltaX(double x)
{
    vec[0] = x;
}

void Vector::setDeltaY(double y)
{
    vec[1] = y;
}

void
Vector::setDeltaZ(double z)
{
    vec[2] = z;
}

double
Vector::getR() const
{
    return abs();
}

double
Vector::getAzimuth() const
{
    return atan2(vec[1], vec[0]);
}

double
Vector::getElevation() const
{
    return atan2(sqrt(vec[0]*vec[0]+vec[1]*vec[1]), vec[2]);
}

const std::valarray<double>&
Vector::get() const
{
    return vec;
}

void
Vector::setR(double r)
{
    double phi = getAzimuth();
    double theta = getElevation();

    setPolar(r, phi, theta);
}

void
Vector::setAzimuth(double phi)
{
    double r = getR();
    double theta = getElevation();

    setPolar(r, phi, theta);
}

void
Vector::setElevation(double theta)
{
    double r = getR();
    double phi = getAzimuth();

    setPolar(r, phi, theta);
}


double
Vector::getPhi() const
{
    return getAzimuth();
}

double
Vector::getTheta() const
{
    return getElevation();
}

Vector
Vector::cross(const Vector& other) const
{
    const double x = vec[1] * other.vec[2] - vec[2] * other.vec[1];
    const double y = vec[2] * other.vec[0] - vec[0] * other.vec[2];
    const double z = vec[0] * other.vec[1] - vec[1] * other.vec[0];
    return Vector(x, y, z);
}

double
Vector::dot(const Vector& other) const
{
    return (vec*other.vec).sum();
}

double
Vector::abs() const
{
    return sqrt(dot(*this));
}

Point
Vector::operator+(const Point& point) const
{
    return Point(get() + point.get());
}

Vector
Vector::operator+(const Vector& other) const
{
    return Vector(get() + other.get());
}

Vector
Vector::operator*(const int scale) const
{
    return (*this) * double(scale);
}

Vector
Vector::operator*(const double scale) const
{
    Vector retval(*this);
    retval.vec[0] *= scale;
    retval.vec[1] *= scale;
    retval.vec[2] *= scale;
    return retval;
}

void
Vector::operator=(const Vector& other)
{
    vec = other.vec;
}

bool
Vector::operator==(const Vector& other) const
{
    return
        vec[0] == other.vec[0] &&
        vec[1] == other.vec[1] &&
        vec[2] == other.vec[2];
}

bool
Vector::operator!=(const Vector& other) const
{
    return !(*this == other);
}

std::ostream&
operator<<(std::ostream &str, const Vector& other)
{
    str
        << "[ dx=" << other.getDeltaX()
        << ", dy=" << other.getDeltaY()
        << ", dz=" << other.getDeltaZ() << "]";
    return str;
}
