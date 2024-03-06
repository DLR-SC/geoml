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

    Handle(Geom_BSplineCurve) myCurve 
        = new Geom_BSplineCurve(*my_control_points, *my_weights, *my_knots, *my_multiplicities, degree, periodic);
    return myCurve; 
}

} // namespace geoml

