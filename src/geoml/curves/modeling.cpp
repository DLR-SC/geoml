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
                                      Standard_Real form_factor_1_curve_1,
                                      Standard_Real form_factor_1_curve_2,
                                      Standard_Real form_factor_2_curve_1,
                                      Standard_Real form_factor_2_curve_2)
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

        gp_Pnt pnt_curve_1;
        gp_Pnt pnt_curve_2;

        gp_Vec derivative_1_curve_1;
        gp_Vec derivative_1_curve_2;

        gp_Vec derivative_2_curve_1;
        gp_Vec derivative_2_curve_2;

        gp_Pnt cp_1_blend_curve_start;
        gp_Pnt cp_1_blend_curve_end;

        gp_Pnt cp_2_blend_curve_start;
        gp_Pnt cp_2_blend_curve_end;

        // define functions to compute the second and third control points of the blending curves (via the formulas):
        auto compute_cp_1_blend_curve_start = [pnt_curve_1, derivative_1_curve_1, form_factor_1_curve_1](){return pnt_curve_1.Translated(form_factor_1_curve_1 / 3 * derivative_1_curve_1);};
        auto compute_cp_2_blend_curve_start = [pnt_curve_1, derivative_1_curve_1, derivative_2_curve_1, form_factor_1_curve_1, form_factor_2_curve_1]()
                                              {
                                                  gp_Vec trans_vec = form_factor_1_curve_1 / 6 * derivative_2_curve_1 + (form_factor_2_curve_1 / 6 + 2 / 3 * form_factor_1_curve_1) * derivative_1_curve_1;
                                                  return pnt_curve_1.Translated(trans_vec);  
                                              };

        auto compute_cp_1_blend_curve_end = [pnt_curve_2, derivative_1_curve_2, form_factor_1_curve_2](){return pnt_curve_2.Translated(form_factor_1_curve_2 / 3 * (-derivative_1_curve_2));};                                              
        auto compute_cp_2_blend_curve_end = [pnt_curve_2, derivative_1_curve_2, derivative_2_curve_2, form_factor_1_curve_2, form_factor_2_curve_2]()
                                              {
                                                  gp_Vec trans_vec = form_factor_1_curve_2 / 6 * derivative_2_curve_2 + (form_factor_2_curve_2 / 6 + 2 / 3 * form_factor_1_curve_2) * (-derivative_1_curve_2);
                                                  return pnt_curve_2.Translated(trans_vec);  
                                              };

    if (continuity_1 == 0 && continuity_2 == 0) 
    {
        // define a degree 1 blending curve:
        curve_1->D0(param_1, pnt_curve_1);
        curve_2->D0(param_2, pnt_curve_2);

        std::vector<gp_Pnt> control_points {pnt_curve_1, pnt_curve_2};
        std::vector<Standard_Real> weights {1., 1.};
        std::vector<Standard_Real> knots {0., 1.};
        std::vector<int> multiplicities {2,2}; 
        int degree = 1;

        return nurbs_curve(control_points, weights, knots, multiplicities, degree);       
    }
    else if (continuity_1 == 1 && continuity_2 == 0)
    {
        // define a degree 2 blending curve
        curve_1->D1(param_1, pnt_curve_1, derivative_1_curve_1);
        curve_2->D0(param_2, pnt_curve_2);

        cp_1_blend_curve_start = compute_cp_1_blend_curve_start();

        std::vector<gp_Pnt> control_points {pnt_curve_1, cp_1_blend_curve_start, pnt_curve_2};
        std::vector<Standard_Real> weights {1., 1., 1.};
        std::vector<Standard_Real> knots {0., 1.};
        std::vector<int> multiplicities {3,3}; 
        int degree = 2;

        return nurbs_curve(control_points, weights, knots, multiplicities, degree); 
    }
    else if (continuity_1 == 2 && continuity_2 == 0)
    {
        // define a degree 3 blending curve
        curve_1->D2(param_1, pnt_curve_1, derivative_1_curve_1, derivative_2_curve_1);
        curve_2->D0(param_2, pnt_curve_2);

        cp_1_blend_curve_start = compute_cp_1_blend_curve_start();
        cp_2_blend_curve_start = compute_cp_2_blend_curve_start();

        std::vector<gp_Pnt> control_points {pnt_curve_1, cp_1_blend_curve_start, cp_2_blend_curve_start, pnt_curve_2};
        std::vector<Standard_Real> weights {1., 1., 1., 1.};
        std::vector<Standard_Real> knots {0., 1.};
        std::vector<int> multiplicities {4,4}; 
        int degree = 3;

        return nurbs_curve(control_points, weights, knots, multiplicities, degree); 
    }
    else if (continuity_1 == 0 && continuity_2 == 1) 
    {
        // define a degree 2 blending curve:
        curve_1->D0(param_1, pnt_curve_1);
        curve_2->D1(param_2, pnt_curve_2, derivative_1_curve_2);

        cp_1_blend_curve_end = compute_cp_1_blend_curve_end();

        std::vector<gp_Pnt> control_points {pnt_curve_1, cp_1_blend_curve_end, pnt_curve_2};
        std::vector<Standard_Real> weights {1., 1., 1.};
        std::vector<Standard_Real> knots {0., 1.};
        std::vector<int> multiplicities {3,3}; 
        int degree = 2;

        return nurbs_curve(control_points, weights, knots, multiplicities, degree);       
    }
    else if (continuity_1 == 1 && continuity_2 == 1) 
    {
        // define a degree 3 blending curve:
        curve_1->D1(param_1, pnt_curve_1, derivative_1_curve_1);
        curve_2->D1(param_2, pnt_curve_2, derivative_1_curve_2);

        cp_1_blend_curve_start = compute_cp_1_blend_curve_start();
        cp_1_blend_curve_end = compute_cp_1_blend_curve_end();

        std::vector<gp_Pnt> control_points {pnt_curve_1, cp_1_blend_curve_start, cp_1_blend_curve_end, pnt_curve_2};
        std::vector<Standard_Real> weights {1., 1., 1., 1.};
        std::vector<Standard_Real> knots {0., 1.};
        std::vector<int> multiplicities {4,4}; 
        int degree = 3;

        return nurbs_curve(control_points, weights, knots, multiplicities, degree);       
    }
    else if (continuity_1 == 2 && continuity_2 == 1) 
    {
        // define a degree 4 blending curve:
        curve_1->D2(param_1, pnt_curve_1, derivative_1_curve_1, derivative_2_curve_1);
        curve_2->D1(param_2, pnt_curve_2, derivative_1_curve_2);

        cp_1_blend_curve_start = compute_cp_1_blend_curve_start();
        cp_2_blend_curve_start = compute_cp_2_blend_curve_start();
        cp_1_blend_curve_end = compute_cp_1_blend_curve_end();

        std::vector<gp_Pnt> control_points {pnt_curve_1, cp_1_blend_curve_start, cp_2_blend_curve_start, cp_1_blend_curve_end, pnt_curve_2};
        std::vector<Standard_Real> weights {1., 1., 1., 1., 1.};
        std::vector<Standard_Real> knots {0., 1.};
        std::vector<int> multiplicities {5,5}; 
        int degree = 4;

        return nurbs_curve(control_points, weights, knots, multiplicities, degree);       
    }
    else if (continuity_1 == 0 && continuity_2 == 2) 
    {
        // define a degree 3 blending curve:
        curve_1->D0(param_1, pnt_curve_1);
        curve_2->D2(param_2, pnt_curve_2, derivative_1_curve_2, derivative_2_curve_2);

        cp_1_blend_curve_end = compute_cp_1_blend_curve_end();
        cp_2_blend_curve_end = compute_cp_2_blend_curve_end();

        std::vector<gp_Pnt> control_points {pnt_curve_1, cp_2_blend_curve_end, cp_1_blend_curve_end, pnt_curve_2};
        std::vector<Standard_Real> weights {1., 1., 1., 1.};
        std::vector<Standard_Real> knots {0., 1.};
        std::vector<int> multiplicities {4,4}; 
        int degree = 3;

        return nurbs_curve(control_points, weights, knots, multiplicities, degree);       
    }    
    else if (continuity_1 == 1 && continuity_2 == 2)
    {
        // define a degree 4 blending curve:
        curve_1->D1(param_1, pnt_curve_1, derivative_1_curve_1);
        curve_2->D2(param_2, pnt_curve_2, derivative_1_curve_2, derivative_2_curve_2);

        cp_1_blend_curve_start = compute_cp_1_blend_curve_start();
        cp_1_blend_curve_end = compute_cp_1_blend_curve_end();
        cp_2_blend_curve_end = compute_cp_2_blend_curve_end();

        std::vector<gp_Pnt> control_points {pnt_curve_1, cp_1_blend_curve_start, cp_2_blend_curve_end, cp_1_blend_curve_end, pnt_curve_2};
        std::vector<Standard_Real> weights {1., 1., 1., 1., 1.};
        std::vector<Standard_Real> knots {0., 1.};
        std::vector<int> multiplicities {5,5}; 
        int degree = 4;

        return nurbs_curve(control_points, weights, knots, multiplicities, degree);       
    } 
    else if (continuity_1 == 2 && continuity_2 == 2)
    {
        // define a degree 5 blending curve:
        curve_1->D2(param_1, pnt_curve_1, derivative_1_curve_1, derivative_2_curve_1);
        curve_2->D2(param_2, pnt_curve_2, derivative_1_curve_2, derivative_2_curve_2);

        cp_1_blend_curve_start = compute_cp_1_blend_curve_start();
        cp_2_blend_curve_start = compute_cp_2_blend_curve_start();
        cp_1_blend_curve_end = compute_cp_1_blend_curve_end();
        cp_2_blend_curve_end = compute_cp_2_blend_curve_end();

        std::vector<gp_Pnt> control_points {pnt_curve_1, cp_1_blend_curve_start, cp_2_blend_curve_start, cp_2_blend_curve_end, cp_1_blend_curve_end, pnt_curve_2};
        std::vector<Standard_Real> weights {1., 1., 1., 1., 1., 1.};
        std::vector<Standard_Real> knots {0., 1.};
        std::vector<int> multiplicities {6,6}; 
        int degree = 5;

        return nurbs_curve(control_points, weights, knots, multiplicities, degree);       
    } 

}                                  

} // namespace geoml

