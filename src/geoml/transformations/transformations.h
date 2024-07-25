#pragma once

/**
 * @brief geoml/transformations/transformations.h includes a collection of transformations
 */

#include <geoml/geoml.h>

#include <TopoDS_Shape.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>


namespace geoml
{

/**
 * @brief Creates a translated copy of a TopoDS_Shape. The translation is performed along a
 * given direction and factor
 * 
 * @param origin The shape to be translated
 * @param direction The direction of translation
 * @param factor The factor applied to the direction for the translation (default: 1.0)
 * @return TopoDS_Shape 
 */
TopoDS_Shape translate(TopoDS_Shape const& origin, gp_Vec const& direction, double factor = 1.0);

/**
 * @brief Creates a translated copy of a gp_Pnt. The translation is performed along a
 * given direction and factor
 * 
 * @param origin The point to be translated
 * @param direction The direction of translation
 * @param factor The factor applied to the direction for the translation (default: 1.0)
 * @return gp_Pnt 
 */
gp_Pnt translate(gp_Pnt const& origin, gp_Vec const& direction, double factor = 1.0);


} // namespace geoml