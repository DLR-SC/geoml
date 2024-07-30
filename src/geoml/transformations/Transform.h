#pragma once

/**
 * @brief geoml/transformations/Transform.h describes a transformation
 */

#include <geoml/geoml.h>

#include <gp_GTrsf.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <memory>


namespace geoml
{

                                    
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

    GEOML_API_EXPORT Transform(Transform&& other); // Move constructor

    GEOML_API_EXPORT Transform& operator=(Transform&& other); // Move assignment operator

    GEOML_API_EXPORT void PreMultiply(const Transform& aTrans);

private: 
    
    //Transformation m_trsf;
    class Impl;
    std::unique_ptr<Impl> pImpl;

};

GEOML_API_EXPORT Transform operator*(const Transform& a, const Transform& b);

} // end namespace geoml