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
 * @param periodic ### to do: understand this better! Test it ###
 * @param check ### to do: test this ####
 */
 /*
GEOML_API_EXPORT Handle(Geom_BSplineCurve)
nurbs_curve(
    const std::vector<gp_Pnt> &control_points, 
    const std::vector<double> &weights,
    const std::vector<double> &knots, 
    const std::vector<int> &multiplicities,
    const int degree, 
    const bool periodic = false, 
    const bool checkRational = false);

} // namespace geoml
