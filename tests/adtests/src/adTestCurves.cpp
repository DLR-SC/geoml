#include <geoml/curves/curves.h>
#include "common/ad_utility_function.h"

#include <gtest/gtest.h>

#include <gp_Pnt.hxx>
#include <Geom_BSplineCurve.hxx>

#include <iostream>


Handle(Geom_BSplineCurve) open_clamped_nurbs_curve (const std::vector<Standard_Real>& design_parameters)
{
    // define an open and clamped NURBS curve of degree 2

    // control points
    gp_Pnt p1(0.0, 0.0, 0.0);
    gp_Pnt p2(design_parameters[0], design_parameters[1], design_parameters[2]); // original values: 0.0, 1.0, 2.0
    gp_Pnt p3(0.0, 2.0, -1.0);
    gp_Pnt p4(0.0, 0.0, 3.0);
    gp_Pnt p5(0.0, 4.0, 0.0);

    std::vector<gp_Pnt> control_points {p1, p2, p3, p4, p5};

    // degree:
    Standard_Integer degree = 2;

    // weights:
    std::vector<Standard_Real> weights(5,1.0);

    // knots: note: as the number of knots (m+1), the number of control points (n+1) and the degree p
    // are related by m = n + p +1, the number of knots has to be 8 in this case. To achive this, in the following the
    // first and the last knot have to appear trice (have multiplicity of 3)

    std::vector<Standard_Real> knots{0.0, 1.0, 2.0, 3.0};

    // multiplicities: 
    std::vector<int> mults {3, 1, 1, 3};

    Handle(Geom_BSplineCurve) curve =
        geoml::nurbs_curve(
            control_points, 
            weights, 
            knots, 
            mults, 
            degree);
        
    return curve;
}


TEST(Test_nurbs_curve, open_clamped_nurbs_curve)
{
    std::cout << std::setprecision(15);

    std::vector<Standard_Real> design_parameters = {0.0, 1.0, 2.0};

    //ad sensitivities
    design_parameters[2].setADValue(0, 1.);

    Handle(Geom_BSplineCurve) ad_curve = open_clamped_nurbs_curve(design_parameters);

    gp_Pnt pnt_1_original;

    ad_curve->D0(Standard_Real(0.5), pnt_1_original);

    //std::cout << "x: " << getPrimal(pnt_1_original.X()) << " y: " << getPrimal(pnt_1_original.Y()) << " z: " << getPrimal(pnt_1_original.Z()) << std::endl;
    //std::cout << "x dot: " << pnt_1_original1.X().getADValue(0) << " y dot: " << pnt_1_original.Y().getADValue(0) << " z dot: " << pnt_1_original.Z().getADValue(0) << std::endl;

    design_parameters[2].setADValue(0, 0.);

    //finite differences
    double fd_step = 1e-6;

    double design_parameter_original_value = design_parameters[2].getValue();

    design_parameters[2] = design_parameters[2] + fd_step;

    Handle(Geom_BSplineCurve) perturbed_curve = open_clamped_nurbs_curve(design_parameters);

    gp_Pnt pnt_1_perturbed;

    perturbed_curve->D0(Standard_Real(0.5), pnt_1_perturbed);

    double fd_sensitivity_x = (pnt_1_perturbed.X().getValue() - pnt_1_original.X().getValue()) / fd_step; 
    double fd_sensitivity_y = (pnt_1_perturbed.Y().getValue() - pnt_1_original.Y().getValue()) / fd_step;
    double fd_sensitivity_z = (pnt_1_perturbed.Z().getValue() - pnt_1_original.Z().getValue()) / fd_step;

    design_parameters[2] = design_parameter_original_value;

    //std::cout << "x_fd_primal: " << getPrimal(pnt_1_perturbed.X()) << " y_fd_primal: " << getPrimal(pnt_1_perturbed.Y()) << " z_fd_primal: " << getPrimal(pnt_1_perturbed.Z()) << std::endl;
    //std::cout << "x_fd dot: " << fd_sensitivity_x  << " y_fd dot: " << fd_sensitivity_y << " z_fd dot: " << fd_sensitivity_z << std::endl;

    EXPECT_NEAR(
        pnt_1_original.Z().getADValue(0), 
        fd_sensitivity_z, 
        1e-5);
}