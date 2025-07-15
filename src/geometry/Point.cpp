/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2010-08-13 Markus Litz <Markus.Litz@dlr.de>
*/
/**
* @file
* @brief  Implementation of a geoml point.
*/

#include "geometry/Point.h"

namespace geoml 
{

// Constructor
Point::Point(double xval, double yval, double zval)
    : x(xval)
    , y(yval)
    , z(zval)
{
}

// Copy constructor
Point::Point(const Point& aPoint)
{
    x = aPoint.x;
    y = aPoint.y;
    z = aPoint.z;
}

Point::Point(const gp_XYZ& aPoint)
{
    x = aPoint.X();
    y = aPoint.Y();
    z = aPoint.Z();
}

// Assignment operator
Point& Point::operator=(const Point& aPoint)
{
    if (&aPoint != this) {
        x = aPoint.x;
        y = aPoint.y;
        z = aPoint.z;
    }
    return *this;
}

Point Point::operator+(const Point& aPoint) const
{
    Point p;
    p.x = x + aPoint.x;
    p.y = y + aPoint.y;
    p.z = z + aPoint.z;
    return p;
}

Point& Point::operator+=(const Point& aPoint) 
{
    x += aPoint.x;
    y += aPoint.y;
    z += aPoint.z;
    return *this;
}

Point Point::operator-(const Point& aPoint) const
{
    Point p;
    p.x = x - aPoint.x;
    p.y = y - aPoint.y;
    p.z = z - aPoint.z;
    return p;
}

Point& Point::operator-=(const Point& aPoint) 
{
    x -= aPoint.x;
    y -= aPoint.y;
    z -= aPoint.z;
    return *this;
}

Point Point::operator*(double s) const
{
    Point p;
    p.x = x*s;
    p.y = y*s;
    p.z = z*s;
    return p;
}

double Point::norm2Sqr() const 
{
    return x*x+y*y+z*z;
}

double Point::norm2() const 
{
    return sqrt(x*x + y*y + z*z);
}

// Destructor
Point::~Point()
{
}

// Convert a Point to a OpenCascade gp_Pnt
gp_Pnt Point::Get_gp_Pnt() const
{
    return gp_Pnt(x, y, z);
}

// Dump internal point data
void Point::Dump(std::ostream& aStream) const 
{
    aStream << "Point: (" << x << ", " << y << ", " << z << ")";
}

double Point::inner_prod(const Point& a, const Point& b) 
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Point Point::cross_prod(const Point& a, const Point& b) 
{
    Point c;
    c.x = a.y*b.z - a.z*b.y;
    c.y = a.z*b.x - a.x*b.z;
    c.z = a.x*b.y - a.y*b.x;
    return c;
}

// scalar projection of a vector a onto a nonzero vector b
double Point::scalar_projection(const Point& a, const Point& b)
{
    return Point::inner_prod(a,b)/b.norm2();
}

// vector projection of a vector a onto a nonzero vector b
Point Point::vector_projection(const Point& a, const Point& b)
{
    return b * (Point::inner_prod(a,b)/b.norm2Sqr());
}

double Point::distance2(const Point &p) const 
{
    return (x-p.x)*(x-p.x) + (y-p.y)*(y-p.y) + (z-p.z)*(z-p.z);
}

void Point::getMinMax(double & min, double & max) const 
{
    min = x;
    if (y < min) {
        min = y;
    }
    if (z < min) {
        min = z;
    }
    max = x;
    if (y > max) {
        max = y;
    }
    if (z > max) {
        max = z;
    }
}

} // end namespace geoml
