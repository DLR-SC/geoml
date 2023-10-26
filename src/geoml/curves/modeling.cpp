#include "curves/modeling.h"
#include <Geom_BSplineCurve.hxx>
#include "geometry/PointsToBSplineInterpolation.h"

Handle(Geom_BSplineCurve)
geoml::interpolate_points_to_b_spline_curve(const Handle(TColgp_HArray1OfPnt) & points)
{
    geoml::PointsToBSplineInterpolation interpolator(points);

    return interpolator.Curve();
}

// this is a factory function to create a Geom_BSplineCurve
Handle(Geom_BSplineCurve)
geoml::b_spline_curve(
    const std::vector<gp_Pnt> & control_points,
    const TColStd_Array1OfReal & weights,
    const TColStd_Array1OfReal & knots,
    const TColStd_Array1OfInteger & multiplicities,
    const int degree,
    const bool periodic, 
    const bool checkRational)
{
    TColgp_Array1OfPnt control_points_col(1,control_points.size());
    
    for(int i = 1; i <= control_points.size(); ++i)
    {
        control_points_col.SetValue(i, control_points.at(i - 1));
    }

    Handle(Geom_BSplineCurve) myCurve 
        = new Geom_BSplineCurve(control_points_col, weights, knots, multiplicities, degree);
    return myCurve; 
}

