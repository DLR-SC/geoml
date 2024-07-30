#pragma once

/**
 * @brief geoml/transformations/Transform.h describes a transformation
 */

#include <geoml/geoml.h>
#include <geometry/Transformation.h>

#include <gp_GTrsf.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>


namespace geoml
{
                                    
class Transform
{

public:

    GEOML_API_EXPORT Transform();

    GEOML_API_EXPORT Transform(const gp_GTrsf& ocMatrix);

    GEOML_API_EXPORT Transform(const gp_Trsf& trans);

    GEOML_API_EXPORT Transform(const gp_Vec& translation);

    GEOML_API_EXPORT void PreMultiply(const Transform& aTrans);

private: 
    
    Transformation m_trsf;

};

GEOML_API_EXPORT Transform operator*(const Transform& a, const Transform& b);

} // end namespace geoml