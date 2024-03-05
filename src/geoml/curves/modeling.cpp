#include "curves/modeling.h"
#include <Geom_BSplineCurve.hxx>
#include "geometry/PointsToBSplineInterpolation.h"

#include <CommonFunctions.h>

Handle(Geom_BSplineCurve)
geoml::nurbs_curve(
    const std::vector<gp_Pnt> &control_points, 
    const std::vector<double> &weights,
    const std::vector<double> &knots, 
    const std::vector<int> &multiplicities,
//    const TColgp_HArray1OfPnt & control_points, 
//    const TColStd_HArray1OfReal & weights,
//    const TColStd_HArray1OfReal & knots, 
//    const TColStd_HArray1OfInteger & multiplicities,
    const int degree, 
    const bool periodic, 
    const bool checkRational)
{
    Handle(Geom_BSplineCurve) myCurve 
        = new Geom_BSplineCurve(*OccArray(control_points), *OccFArray(weights), *OccFArray(knots), *OccIArray(multiplicities), degree, periodic, checkRational);
    return myCurve; 
}

