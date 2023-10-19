#pragma once

/**
 * @brief geoml/surfaces/modeling.h includes functions to create (B-spline) surfaces
 * by interpolating or approximating points and curves
 */

#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <geoml/geoml.h>

#include <gp_Ax1.hxx>
#include <Geom_BSplineCurve.hxx>

#include <vector>
#include <cmath>

namespace geoml
{

/**
 * @brief Interpolates the curve network by a bspline curve
 * 
 * This uses the Gordon interpolation method.
 * The u curves and v curves must intersect each other within the tolerance.
 * 
 * __Note:__ The input curves are reparametrized to fullfill the compatibity criteria of the gordon method,
 *       which might introduce a small error.
 * 
 * @param ucurves Multiple curves that will be interpolated in u direction by the final shape
 * @param vcurves Multiple curves that will be interpolated in v direction by the final shape,
 *                must intersect the ucurves
 * @param tolerance Tolerance, in which the u- and v-curves need to intersect each other
 */
GEOML_API_EXPORT Handle(Geom_BSplineSurface)
interpolate_curve_network(const std::vector<Handle(Geom_Curve)>& ucurves,
                          const std::vector<Handle(Geom_Curve)>& vcurves,
                          double tolerance);

/**
 * @brief Interpolates the curve network by a bspline curve in v-direction
 *
 * @param curves The curves to interpolate
 * @param max_degree Maximum degree of the interpolation (actual degree <= number of curves - 1)
 * @param join_continously If true and start and end-curves are the same,
 *        the resulting surface will be joined continously
 */
GEOML_API_EXPORT Handle(Geom_BSplineSurface)
interpolate_curves(const std::vector<Handle(Geom_Curve)>& curves,
                   unsigned int max_degree=3,
                   bool join_continously=false);

/**
 * @brief Create a revolving surface
 *
 * //@param profile_curve Profile curve (contained in a plane)
 * //@param rotation_axis The rotation axis
 * //@param angle The rotation angle (2*PI by default)
 */

GEOML_API_EXPORT Handle(Geom_BSplineSurface)
revolving_surface(const Handle(Geom_BSplineCurve)& profile_curve,
                  const gp_Ax1& rotation_axis,
                  const Standard_Real angle=2*M_PI); // check if it works to choose another
                                                     // angle

/**
 * @brief Create a B-spline surface
 *
 * //@param control_points A 2-dimensional control point net
 * //@param weights A 2-dimensional weights net 
 * //@param U_knots The knot vector in U-direction
 * //@param V_knots The knot vector in V-direction
 * //@param U_mults The multiplicities of the U-direction knots
 * //@param V_mults The multiplicities of the V-direction knots
 * //@param U_degree The degree in U-direction
 * //@param V_degree The degree in V-direction
 * //@param U_periodic Boolean flag for periodicity in U-direction
 * //@param V_periodic Boolean flag for periodicity in V-direction
 */

GEOML_API_EXPORT Handle(Geom_BSplineSurface)
b_spline_surface(const TColgp_Array2OfPnt &control_points, 
                 const TColStd_Array2OfReal &weights, 
                 const TColStd_Array1OfReal &U_knots, 
                 const TColStd_Array1OfReal &V_knots, 
                 const TColStd_Array1OfInteger &U_mults, 
                 const TColStd_Array1OfInteger &V_mults, 
                 const Standard_Integer U_degree, 
                 const Standard_Integer V_degree, 
                 const Standard_Boolean U_periodic=Standard_False, 
                 const Standard_Boolean V_periodic=Standard_False); 
                                     

} // namespace geoml
