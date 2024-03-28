#pragma once

/**
 * @brief geoml/surface/modeling.h includes functions to create (B-spline) surfaces
 * by interpolating or approximating points and curves
 */

#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <geoml/geoml.h>


#include <vector>

namespace geoml
{

/**
 * @brief Interpolates the curve network by a B-spline surface
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
 * @brief Interpolates the curves by a B-spline surface in v-direction
 *
 * @param curves The curves to interpolate
 * @param max_degree Maximum degree of the interpolation (actual degree <= number of curves - 1)
 * @param join_continuously If true and start and end-curves are the same,
 *        the resulting surface will be joined continuously
 */
GEOML_API_EXPORT Handle(Geom_BSplineSurface)
interpolate_curves(const std::vector<Handle(Geom_Curve)>& curves,
                   unsigned int max_degree=3,
                   bool join_continuously=false);

} // namespace geoml
