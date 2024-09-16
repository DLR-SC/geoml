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

/**
 * @brief Check it two points are equal
 * 
 * @param point_1 First point
 * @param point_2 Second point
 */
GEOML_API_EXPORT bool check_if_two_points_equal(gp_Pnt const& point_1, gp_Pnt const& point_2);

/**
 * @brief Flips the parameter direction of a curve if argument flip_me equals true
 * 
 * @param curve A handle on a Geom_Curve
 * @param flip_me Boolean value determining if the curve is to flip
 */
GEOML_API_EXPORT Handle(Geom_Curve) flip_curve(Handle(Geom_Curve) const& curve, bool flip_me);



} // namespace geoml
