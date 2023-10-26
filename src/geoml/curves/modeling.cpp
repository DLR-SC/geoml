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
    const std::vector<double> & weights,
    const std::vector<double> & knots,
    const std::vector<int> & multiplicities,
    const int degree,
    const bool periodic, 
    const bool checkRational)
{
    // control points
    TColgp_Array1OfPnt control_points_col(1, control_points.size());
    for(int i = 1; i <= control_points.size(); ++i)
    {
        control_points_col.SetValue(i, control_points.at(i - 1));
    }

    // weights:
    TColStd_Array1OfReal weights_col(1, weights.size());
    for(int i = 1; i <= weights.size(); ++i)
    {
        weights_col.SetValue(i, weights.at(i - 1));
    }

    // knots:
    TColStd_Array1OfReal knots_col(1,knots.size());
    for(int i = 1; i <= knots.size(); ++i)
    {
        knots_col.SetValue(i, knots.at(i - 1));
    }

    //multiplicities:
    TColStd_Array1OfInteger multiplicities_col(1, multiplicities.size());
    for(int i = 1; i <= multiplicities.size(); ++i)
    {
        multiplicities_col.SetValue(i, multiplicities.at(i - 1));
    }

    Handle(Geom_BSplineCurve) myCurve 
        = new Geom_BSplineCurve(control_points_col, weights_col, knots_col, multiplicities_col, degree);
    return myCurve; 
}

