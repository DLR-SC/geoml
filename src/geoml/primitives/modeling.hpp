#pragma once

/**
 * @brief geoml/primitives/modeling.hpp includes functions to create geometric primitives
 */
#include <geoml/geoml.h>

#include "geoml/naming_choosing/Shape.hpp"

namespace geoml
{

/**
 * @brief Define a cylinder by its radius in height in z-direction
 * 
 * 
 * @param radius The radius of the cylinder
 * @param height The height of the cylinder in z-direction
 */
GEOML_API_EXPORT Shape create_cylinder(double radius, double height);

/**
 * @brief Define a box with a corner in the origin by giving its extensions in x,y,z-dimension.
 * 
 * 
 * @param dx Extension in x-dimension 
 * @param dy Extension in y-dimension
 * @param dz Extension in z-dimension
 */
GEOML_API_EXPORT Shape create_box(double dx, double dy, double dz);

} // namespace geoml

