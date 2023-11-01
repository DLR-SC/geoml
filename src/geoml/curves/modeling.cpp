#include "curves/modeling.h"
#include <Geom_BSplineCurve.hxx>
#include "geometry/PointsToBSplineInterpolation.h"

Handle(Geom_BSplineCurve)
geoml::interpolate_points_to_b_spline_curve(const Handle(TColgp_HArray1OfPnt) & points)
{
    geoml::PointsToBSplineInterpolation interpolator(points);

    return interpolator.Curve();
}

Handle(Geom_BSplineCurve)
geoml::interpolate_points_to_b_spline_curve(const std::vector<gp_Pnt> & points)
{
    Handle(TColgp_HArray1OfPnt) points_col = new TColgp_HArray1OfPnt(1, points.size());
    for(int i = 0; i < points.size(); ++i)
    {
        points_col->SetValue(i + 1, points.at(i));
    }

    geoml::PointsToBSplineInterpolation interpolator(points_col);

    return interpolator.Curve();
}

// this is a factory function to create a Geom_BSplineCurve
Handle(Geom_BSplineCurve)
geoml::b_spline_curve(
    const TColgp_Array1OfPnt & control_points, 
    const TColStd_Array1OfReal & weights,
    const TColStd_Array1OfReal & knots, 
    const TColStd_Array1OfInteger & multiplicities,
    const int degree, 
    const bool periodic, 
    const bool checkRational)
{
    Handle(Geom_BSplineCurve) myCurve 
        = new Geom_BSplineCurve(control_points, weights, knots, multiplicities, degree, periodic, checkRational);
    return myCurve; 
}

