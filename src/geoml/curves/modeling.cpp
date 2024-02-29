#include "curves/modeling.h"
#include <Geom_BSplineCurve.hxx>
#include "geometry/PointsToBSplineInterpolation.h"

Handle(Geom_BSplineCurve)
geoml::nurbs_curve(
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

