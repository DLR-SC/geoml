#pragma once

/**
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * @brief geoml/utility/utility.h includes a collection of functions that are helpful for geometry modeling
 */

#include <geoml/geoml.h>

#include "geoml/data_structures/Array2d.h"

#include <Geom_BSplineSurface.hxx>
#include <gp_Pnt.hxx>


namespace geoml
{

/**
 * @brief Extracts the control point net of a Geom_BSplineSurface. The first index corresponds to the U-direction and the second index corresponds to the V-direction. 
 * 
 * @param b_spline_surface A Geom_BSplineSurface 
 */
GEOML_API_EXPORT Array2d<gp_Pnt> 
extract_control_points_surface(const Handle(Geom_BSplineSurface) &b_spline_surface);

/**
 * @brief Extracts the j-th control point vector of a Geom_BSplineSurface in U direction
 * 
 * @param b_spline_surface A handle on a Geom_BSplineSurface
 * @param j The V-direction-index of the control point vector (starting with 0) 

 */
GEOML_API_EXPORT std::vector<gp_Pnt>  
extract_control_point_vector_in_U_direction (const Handle(Geom_BSplineSurface) &b_spline_surface, int j);

/**
 * @brief Extracts the i-th control point vector of a Geom_BSplineSurface in V direction
 * 
 * @param b_spline_surface A handle on a Geom_BSplineSurface
 * @param i The U-direction-index of the control point vector (starting with 0) 

 */
GEOML_API_EXPORT std::vector<gp_Pnt>  
extract_control_point_vector_in_V_direction (const Handle(Geom_BSplineSurface) &b_spline_surface, int i);


} // namespace geoml
