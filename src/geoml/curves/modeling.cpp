#include "curves/modeling.h"
#include <Geom_BSplineCurve.hxx>
#include "geometry/PointsToBSplineInterpolation.h"

Handle(Geom_BSplineCurve)
geoml::interpolate_points_to_b_spline_curve(const Handle(TColgp_HArray1OfPnt) & points)
{
    geoml::PointsToBSplineInterpolation interpolator(points);

    return interpolator.Curve();
}
