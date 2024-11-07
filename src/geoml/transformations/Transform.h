#pragma once

/**
 * @brief geoml/transformations/Transform.h describes a transformation
 */

#include <geoml/geoml.h>

#include <gp_GTrsf.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <TopoDS_Shape.hxx>
#include <Geom_Surface.hxx>

#include <memory>


namespace geoml
{

class Transformation;    

class Transform
{
public:

    GEOML_API_EXPORT Transform();

    GEOML_API_EXPORT Transform(const gp_GTrsf& ocMatrix);

    GEOML_API_EXPORT Transform(const gp_Trsf& trans);

    GEOML_API_EXPORT Transform(const gp_Vec& translation);

    GEOML_API_EXPORT ~Transform();


    GEOML_API_EXPORT Transform(const Transform& other); // Copy constructor

    GEOML_API_EXPORT Transform& operator=(const Transform& other); // Copy assignment operator

    GEOML_API_EXPORT Transform(Transform&& other) = default; // Move constructor

    GEOML_API_EXPORT Transform& operator=(Transform&& other) = default; // Move assignment operator


    GEOML_API_EXPORT void pre_multiply(const Transform& aTrans);

    // Transforms a shape with the current transformation matrix and
    // returns the transformed shape
    GEOML_API_EXPORT TopoDS_Shape operator()(const TopoDS_Shape& shape) const;

    // Transforms a point with the current transformation matrix and
    // returns the transformed point
    GEOML_API_EXPORT gp_Pnt operator()(const gp_Pnt& point) const;

    // Transforms a surface with the current transformation matrix and
    // returns the transformed surface
    GEOML_API_EXPORT Handle(Geom_Surface) operator()(const Handle(Geom_Surface)& surf) const;

    // Transforms a vector with the current transformation matrix and
    // returns the transformed vector
    // Note, that the vector transformation does not include the translation part
    GEOML_API_EXPORT gp_Vec operator()(const gp_Vec& vec) const;

private: 
    
    std::unique_ptr<Transformation> pImpl;

};

GEOML_API_EXPORT Transform operator*(const Transform& a, const Transform& b);

} // end namespace geoml