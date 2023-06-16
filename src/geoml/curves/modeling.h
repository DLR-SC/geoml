#pragma once

/**
 * @brief geoml/curves/modeling.h includes functions to create (B-spline) curves
 * by interpolating or approximating points or functions
 */

#include <TColgp_HArray1OfPnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <geoml/geoml.h>


#include <vector>

namespace geoml
{

/**
 * @brief Interpolates points to a B-spline curve
 * 
 * @param points Multiple points that will be interpolated
 */
GEOML_API_EXPORT Handle(Geom_BSplineCurve)
interpolate_points_to_b_spline_curve(const Handle(TColgp_HArray1OfPnt) & points); //start with a simple version to try it out first

} // namespace geoml
