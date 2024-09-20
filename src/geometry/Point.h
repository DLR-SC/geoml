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

#ifndef POINT_H
#define POINT_H

#include "geoml_internal.h"
#include <climits>
#include "gp_XYZ.hxx"
#include "gp_Pnt.hxx"
#include "gp_XYZ.hxx"
#include <iostream>

namespace geoml 
{

class Point
{

public:
    // Constructor
    GEOML_EXPORT Point(double xval = 0.0, double yval = 0.0, double zval = 0.0);

    // Copy constructor
    GEOML_EXPORT Point(const Point& aPoint);

    GEOML_EXPORT Point(const gp_XYZ& aPoint);

    // Assignment operator
    GEOML_EXPORT Point& operator=(const Point& aPoint);

    // Addition of points
    GEOML_EXPORT Point operator+(const Point& aPoint) const;

    // Addition of a point
    GEOML_EXPORT Point& operator+=(const Point& aPoint);

    // Subtraction of points
    GEOML_EXPORT Point operator-(const Point& aPoint) const;

    // Subtraction of a point
    GEOML_EXPORT Point& operator-=(const Point& aPoint);

    // Scaling of points
    GEOML_EXPORT Point operator*(double) const;

    // returns a'*a
    GEOML_EXPORT double norm2Sqr() const;

    // returns the length of the vector
    GEOML_EXPORT double norm2() const;

    // Virtual Destructor
    GEOML_EXPORT virtual ~Point();

    // Convert a Point to a OpenCascade gp_Pnt
    GEOML_EXPORT gp_Pnt Get_gp_Pnt() const;

    // Dump internal point data
    GEOML_EXPORT void Dump(std::ostream& aStream) const;

    // scalar product
    GEOML_EXPORT static double inner_prod(const Point& aPoint, const Point& bPoint);

    // cross product
    GEOML_EXPORT static Point cross_prod(const Point& a, const Point& b);

    // scalar projection of a vector a onto a nonzero vector b
    GEOML_EXPORT static double scalar_projection(const Point& a, const Point& b);

    // vector projection of a vector a onto a nonzero vector b
    GEOML_EXPORT static Point vector_projection(const Point& a, const Point& b);

    // square distance to another point
    GEOML_EXPORT double distance2(const Point& point) const;

    // returns minimum and maximum component
    GEOML_EXPORT void getMinMax(double & min, double & max) const;

    double x;
    double y;
    double z;
};

} // end namespace geoml

#endif // POINT_H
