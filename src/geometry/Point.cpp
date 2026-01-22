/* 
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2010-08-13 Markus Litz <Markus.Litz@dlr.de>
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
/**
* @file
* @brief  Implementation of a geoml point.
*/

#include "geometry/Point.h"

namespace geoml 
{

// Constructor
Point::Point(Standard_Real xval, Standard_Real yval, Standard_Real zval)
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

Point Point::operator*(Standard_Real s) const
{
    Point p;
    p.x = x*s;
    p.y = y*s;
    p.z = z*s;
    return p;
}

Standard_Real Point::norm2Sqr() const 
{
    return x*x+y*y+z*z;
}

Standard_Real Point::norm2() const 
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

Standard_Real Point::inner_prod(const Point& a, const Point& b) 
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
Standard_Real Point::scalar_projection(const Point& a, const Point& b)
{
    return Point::inner_prod(a,b)/b.norm2();
}

// vector projection of a vector a onto a nonzero vector b
Point Point::vector_projection(const Point& a, const Point& b)
{
    return b * (Point::inner_prod(a,b)/b.norm2Sqr());
}

Standard_Real Point::distance2(const Point &p) const 
{
    return (x-p.x)*(x-p.x) + (y-p.y)*(y-p.y) + (z-p.z)*(z-p.z);
}

void Point::getMinMax(Standard_Real & min, Standard_Real & max) const 
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
