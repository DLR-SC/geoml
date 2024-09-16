#pragma once

/**
 * @brief geoml/utility/utility.h includes a collection of functions that are helpful for geometry modeling
 */

#include <geoml/geoml.h>

#include "geoml/data_structures/Array2d.h"

#include <Geom_BSplineSurface.hxx>
#include <gp_Pnt.hxx>


namespace geoml
{

/**
 * @brief Extracts the control point net of a Geom_BSplineSurface
 * 
 * @param b_spline_surface A Geom_BSplineSurface
 */
GEOML_API_EXPORT Array2d<gp_Pnt> 
extract_control_points_surface(const Handle(Geom_BSplineSurface) &b_spline_surface);



} // namespace geoml
