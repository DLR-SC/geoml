#pragma once

/**
 * @brief geoml/transformations/modeling.h includes functions to transform geometry
 */
#include <geoml/geoml.h>

#include <TopoDS_Shape.hxx>

namespace geoml
{
    
/**
 * @brief Creates a translated copy of a shape. The translation is performed along a
 * given direction and factor
 * 
 * @param origin The shape to be translated
 * @param direction The direction of translation
 * @param factor The factor applied to the direction for the translation (default: 1.0)
 * @return TopoDS_Shape 
 */
TopoDS_Shape moved(TopoDS_Shape const& origin, gp_Vec const& direction, double factor = 1.0);

}