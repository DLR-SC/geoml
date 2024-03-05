#include "curves/modeling.h"
#include <Geom_BSplineCurve.hxx>
#include "geometry/PointsToBSplineInterpolation.h"

#include <CommonFunctions.h>

Handle(Geom_BSplineCurve)
geoml::nurbs_curve(
    const std::vector<gp_Pnt> &control_points, 
    const std::vector<Standard_Real> &weights,
    const std::vector<Standard_Real> &knots, 
    const std::vector<int> &multiplicities,
    const int degree, 
    const bool periodic)
{
    Handle(Geom_BSplineCurve) myCurve 
        = new Geom_BSplineCurve(*OccArray(control_points), *OccFArray(weights), *OccFArray(knots), *OccIArray(multiplicities), degree, periodic);
    return myCurve; 
}

