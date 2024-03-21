#include "curves/modeling.h"
#include <Geom_BSplineCurve.hxx>
#include "geometry/PointsToBSplineInterpolation.h"

#include <CommonFunctions.h>

namespace geoml{

Handle(Geom_BSplineCurve) nurbs_curve(
    const std::vector<gp_Pnt> &control_points, 
    const std::vector<Standard_Real> &weights,
    const std::vector<Standard_Real> &knots, 
    const std::vector<int> &multiplicities,
    const int degree, 
    const bool periodic)
{
    Handle(TColgp_HArray1OfPnt) my_control_points = OccArray(control_points);
    Handle(TColStd_HArray1OfReal) my_weights = OccFArray(weights);
    Handle(TColStd_HArray1OfReal) my_knots = OccFArray(knots);
    Handle(TColStd_HArray1OfInteger) my_multiplicities = OccIArray(multiplicities);

    return new Geom_BSplineCurve(*my_control_points, *my_weights, *my_knots, *my_multiplicities, degree, periodic); 
}

Handle(Geom_BSplineCurve) interpolate_points_to_b_spline_curve(const std::vector<gp_Pnt> & points)
{
    Handle(TColgp_HArray1OfPnt) points_col = OccArray(points);

    PointsToBSplineInterpolation interpolator(points_col);

    return interpolator.Curve();
}


} // namespace geoml

