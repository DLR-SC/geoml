#pragma once

/**
 * @brief geoml/surface/modeling.h includes functions to create (B-spline) surfaces
 * by interpolating or approximating points and curves
 */


#include <geoml/geoml.h>

#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <gp_Vec.hxx>
#include <TopoDS_Shape.hxx>

#include <vector>
#include <cmath>

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

/**
 * @brief Create a revolving shape
 *
 * //@param profile_shape Profile shape (can be a point, a curve or a surface)
 * //@param start_point_rotation_axis Start point of the rotation axis
 * //@param rotation_axis_direction The rotation axis direction defined by a vector
 * //@param angle The rotation angle (2*PI by default)
 */
GEOML_API_EXPORT TopoDS_Shape
revolving_shape(const TopoDS_Shape& profile_shape,
                  const gp_Pnt& start_point_rotation_axis,
                  const gp_Vec& rotation_axis_direction,
                  const Standard_Real angle=2*M_PI); 

/**
 * @brief Create a NURBS surface
 *
 * //@param control_points A 2d control point net
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
nurbs_surface(const Array2d<gp_Pnt> &control_points, // now, we have to decide what to do here
                 const Array2d<Standard_Real> &weights, 
                 const std::vector<Standard_Real> &U_knots, 
                 const std::vector<Standard_Real> &V_knots, 
                 const std::vector<int> &U_mults, 
                 const std::vector<int> &V_mults, 
                 const int U_degree, 
                 const int V_degree, 
                 const bool U_periodic=false, 
                 const bool V_periodic=false);
/*
GEOML_API_EXPORT Handle(Geom_BSplineSurface)
nurbs_surface(const TColgp_Array2OfPnt &control_points, // now, we have to decide what to do here
                 const TColStd_Array2OfReal &weights, 
                 const TColStd_Array1OfReal &U_knots, 
                 const TColStd_Array1OfReal &V_knots, 
                 const TColStd_Array1OfInteger &U_mults, 
                 const TColStd_Array1OfInteger &V_mults, 
                 const Standard_Integer U_degree, 
                 const Standard_Integer V_degree, 
                 const Standard_Boolean U_periodic=Standard_False, 
                 const Standard_Boolean V_periodic=Standard_False);

*/



} // namespace geoml
