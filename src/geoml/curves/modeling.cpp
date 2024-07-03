#include "curves/modeling.h"
#include <Geom_BSplineCurve.hxx>
#include "geometry/PointsToBSplineInterpolation.h"
#include "common/CommonFunctions.h"

#include <gp_Vec.hxx>


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

Handle(Geom_BSplineCurve) interpolate_points_to_b_spline_curve(const std::vector<gp_Pnt> &points, int degree, bool continuousIfClosed, const std::vector<Standard_Real> &parameters)
{
    Handle(TColgp_HArray1OfPnt) points_col = OccArray(points);
    
    if (parameters.size() == 0) {
        PointsToBSplineInterpolation interpolator(points_col, degree, continuousIfClosed);
        return interpolator.Curve();
    }
    else {
        PointsToBSplineInterpolation interpolator(points_col, parameters, degree, continuousIfClosed);
        return interpolator.Curve();
    }
}

Handle(Geom_BSplineCurve) blend_curve(Handle(Geom_BSplineCurve) &curve_1,
                                      Handle(Geom_BSplineCurve) &curve_2,
                                      int start_end_1, 
                                      int start_end_2,
                                      int continuity_1,
                                      int continuity_2,
                                      Standard_Real form_factor_11,
                                      Standard_Real form_factor_12,
                                      Standard_Real form_factor_21,
                                      Standard_Real form_factor_22)
{
    //extract parameter
    Standard_Real param_1;
    Standard_Real param_2;

    if(start_end_1 == 0)
    {
        param_1 = curve_1->FirstParameter();
    }
    else if(start_end_1 == 1)
    {
        param_1 = curve_1->LastParameter();
    }
    else 
    {
        //fatal error 
    }

    if(start_end_2 == 0)
    {
        param_2 = curve_2->FirstParameter();
    }
    else if(start_end_2 == 1)
    {
        param_2 = curve_2->LastParameter();
    }
    else 
    {
        //fatal error 
    }

    if (continuity_1 == 0 && continuity_2 == 0)
    {
        // define a degree 1 blending curve:

        gp_Pnt pnt_curve_1;
        gp_Pnt pnt_curve_2;

        curve_1->DO(param_1, pnt_curve_1);
        curve_2->DO(param_2, pnt_curve_2);

        std::vector<gp_Pnt> control_points {pnt_curve_1, pnt_curve_2};
        std::vector<Standard_Real> weights {1., 1.};
        std::vector<Standard_Real> knots {0., 1.};
        std::vector<int> multiplicities {2,2}; 
/*
        Handle(Geom_BSplineCurve) nurbs_curve(const std::vector<gp_Pnt> &control_points, 
                                              const std::vector<Standard_Real> &weights,
                                              const std::vector<Standard_Real> &knots, 
                                              const std::vector<int> &multiplicities,
                                              const int degree, 
                                              const bool periodic)
*/

       
    }

    //Handle(Geom_BSplineCurve) my_blend_curve = nurbs_curve(...)

    
    Handle(Geom_BSplineCurve) ret;
    return ret;
}                                  

} // namespace geoml

