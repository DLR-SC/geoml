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
* @brief  Implementation of geoml transformations.
*/

#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "geoml_internal.h"
#include "logging/Logging.h"
#include "gp_GTrsf.hxx"
#include "gp_Pnt.hxx"
#include "TopoDS.hxx"
#include "Geom_Surface.hxx"
#include "PNamedShape.h"

namespace geoml
{

class Transformation
{

public:
    // Constructor
    GEOML_EXPORT Transformation();
    GEOML_EXPORT explicit Transformation(const gp_GTrsf& ocMatrix);

    // Constructor for transformation based on gp_Trsf
    GEOML_EXPORT explicit Transformation(const gp_Trsf& trans);

    GEOML_EXPORT explicit Transformation(const gp_Vec& translation);
    
    GEOML_EXPORT Transformation& operator=(const Transformation&);

    // Converts degree to radian, utility function
    GEOML_EXPORT static double DegreeToRadian(double degree);

    // Converts radian to degree, utility function
    GEOML_EXPORT static double RadianToDegree(double radian);

    // Sets matrix to identity matrix
    GEOML_EXPORT void SetIdentity();

    // Sets a value of the transformation matrix by row/col
    GEOML_EXPORT void SetValue(int row, int col, double value);

    // Returns the current transformation as gp_GTrsf object
    GEOML_EXPORT gp_GTrsf Get_gp_GTrsf() const;

    // Post multiply this matrix with another matrix and stores 
    // the result in this matrix
    GEOML_EXPORT void PostMultiply(const Transformation& aTrans);

    // Pre multiply this matrix with another matrix and stores 
    // The result in this matrix
    GEOML_EXPORT void PreMultiply(const Transformation& aTrans);

    // Adds a translation to the matrix
    GEOML_EXPORT void AddTranslation(double tx, double ty, double tz);

    // Adds a scaling transformation to the matrix
    GEOML_EXPORT void AddScaling(double sx, double sy, double sz);

    // Adds a rotation around the x,y,z axis to the matrix
    GEOML_EXPORT void AddRotationX(double degreeX);
    GEOML_EXPORT void AddRotationY(double degreeY);
    GEOML_EXPORT void AddRotationZ(double degreeZ);

    // Adds a rotation in intrinsic x-y'-z'' Euler convention to the matrix
    GEOML_EXPORT void AddRotationIntrinsicXYZ(double phi, double theta, double psi);

    // Adds projection on xy plane by setting the z coordinate to 0
    GEOML_EXPORT void AddProjectionOnXYPlane();

    // Adds projection on xz plane by setting the y coordinate to 0
    GEOML_EXPORT void AddProjectionOnXZPlane();

    // Adds projection on yz plane by setting the x coordinate to 0
    GEOML_EXPORT void AddProjectionOnYZPlane();

    // Adds mirroring at xy plane
    GEOML_EXPORT void AddMirroringAtXYPlane();

    // Adds mirroring at xz plane
    GEOML_EXPORT void AddMirroringAtXZPlane();

    // Adds mirroring at yz plane
    GEOML_EXPORT void AddMirroringAtYZPlane();

    // Transforms a shape with the current transformation matrix and
    // returns the transformed shape
    GEOML_EXPORT TopoDS_Shape Transform(const TopoDS_Shape& shape) const;

    // Transforms the CNamedShape. It also makes sure to update
    // the local face transformation meta data.
    GEOML_EXPORT PNamedShape Transform(PNamedShape shape) const;

    // Transforms a point with the current transformation matrix and
    // returns the transformed point
    GEOML_EXPORT gp_Pnt Transform(const gp_Pnt& point) const;

    // Transforms a surface with the current transformation matrix and
    // returns the transformed surface
    GEOML_EXPORT Handle(Geom_Surface) Transform(const Handle(Geom_Surface)& surf) const;

    // Transforms a vector with the current transformation matrix and
    // returns the transformed vector
    // Note, that the vector transformation does not include the translation part
    GEOML_EXPORT gp_Vec Transform(const gp_Vec& vec) const;
    
    // Returns the inverted Transformation
    GEOML_EXPORT Transformation Inverted() const;

    // Decompose the Transformation into the three operations
    // scale first, rotate second (extr. Euler as defined in CPACS),
    // translate third
    GEOML_EXPORT void Decompose(double scale[3], double rotation[3], double translation[3]) const;

    // Default copy constructor and assignment operator are correct
    // since memberwise copy is enough for this class.

    // for debug purposes
    GEOML_EXPORT friend std::ostream& operator<<(std::ostream& os, const Transformation& t);

    // Getter for matrix values
    GEOML_EXPORT double GetValue(int row, int col) const;

private:
    bool IsUniform() const;

    double m_matrix[4][4];
};

GEOML_EXPORT Transformation operator*(const Transformation& a, const Transformation& b);

} // end namespace geoml

#endif // TRANSFORMATION_H
