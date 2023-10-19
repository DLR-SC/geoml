#pragma once

/**
 * @brief geoml/curves/modeling.h includes functions to create (B-spline) curves
 * by interpolating or approximating points or functions
 */

#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
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
interpolate_points_to_b_spline_curve(const Handle(TColgp_HArray1OfPnt) & points);


/**
 * @brief Define a B-spline curve by setting the control points, the weights, the degree,
 * the knots, the multiplicities
 * 
 * 
 * @param control_points The control points of the B-spline curve
 * @param weights The weights of the B-spline curve
 * @param knots The knots of the B-spline curve, knots with multiplicity larger than 1 only noted once 
 * @param multiplicities The multiplicities of the knots
 * @param degree The degree of the B-spline curve
 * @param periodic ### to do: understand this better! Test it ###
 * @param check ### to do: test this ####
 */
GEOML_API_EXPORT Handle(Geom_BSplineCurve)
b_spline_curve(
    const TColgp_Array1OfPnt & control_points, 
    const TColStd_Array1OfReal & weights,
    const TColStd_Array1OfReal & knots, 
    const TColStd_Array1OfInteger & multiplicities,
    const int degree, 
    const bool periodic = false, 
    const bool checkRational = false);


} // namespace geoml
