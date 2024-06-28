#pragma once

/**
 * @brief geoml/curves/modeling.h includes functions to create and modify curves
 */
#include <geoml/geoml.h>

#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>

#include <vector>


namespace geoml
{

/**
 * @brief Define a NURBS curve by setting the control points, the weights, the degree,
 * the knots, the multiplicities
 * 
 * 
 * @param control_points The control points of the NURBS curve
 * @param weights The weights of the NURBS curve
 * @param knots The knots of the NURBS curve, knots with multiplicity larger than 1 only noted once 
 * @param multiplicities The multiplicities of the knots
 * @param degree The degree of the NURBS curve
 * @param periodic The periodicity, as it is defined for Open Cascade's Geom_BSplineCurve
 */
GEOML_API_EXPORT Handle(Geom_BSplineCurve)
nurbs_curve(
    const std::vector<gp_Pnt> &control_points, 
    const std::vector<Standard_Real> &weights,
    const std::vector<Standard_Real> &knots, 
    const std::vector<int> &multiplicities,
    const int degree, 
    const bool periodic = false);

/**
 * @brief Interpolates points to a B-spline curve
 * 
 * @param points Multiple points that will be interpolated
 */
GEOML_API_EXPORT Handle(Geom_BSplineCurve)
interpolate_points_to_b_spline_curve(const std::vector<gp_Pnt> &points, int degree = 3, bool continuousIfClosed = false, const std::vector<Standard_Real> &parameters = std::vector<Standard_Real>());

/**
 * @brief Create a blending curve between two given curves.
 * 
 * 
 * @param curve_1 First curve to blend
 * @param curve_2 Second curve to blend
 * @param start_end_1 Sets if the start of the end point of the first curve should be blended
 * @param start_end_2 Sets if the start of the end point of the second curve should be blended
 * @param start_end_1 Continuity of blend to first curve (G_0=0, G_1=1, G_2=2)
 * @param start_end_2 Continuity of blend to second curve (G_0=0, G_1=1, G_2=2) 
 */
GEOML_API_EXPORT Handle(Geom_BSplineCurve)
blend_curve(Handle(Geom_BSplineCurve) &curve_1,
            Handle(Geom_BSplineCurve) &curve_2,
            bool start_end_1, 
            bool start_end_2,
            int continuity_1,
            int continuity_2);

} // namespace geoml
