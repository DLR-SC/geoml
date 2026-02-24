#include <geoml/surfaces/surfaces.h>
#include "common/ad_utility_function.h"

#include <Geom_BSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Curve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepTools.hxx>

#include <gtest/gtest.h>

#include <vector>


#include <iostream>


Handle(Geom_BSplineSurface) gordon_fuselage (const std::vector<Standard_Real>& design_parameters)
{
    // middle part of fuselage

    // front fuselage profile

    gp_Pnt P_1 (-4600., design_parameters[0], design_parameters[1]);
    gp_Pnt P_2 (-4600., design_parameters[2], design_parameters[3]);
    gp_Pnt P_3 (-4600., design_parameters[4], design_parameters[5]);
    gp_Pnt P_4 (-4600., design_parameters[6], design_parameters[7]);
    gp_Pnt P_5 (-4600., design_parameters[8], design_parameters[9]);
    gp_Pnt P_6 (-4600., design_parameters[10], design_parameters[11]);
    gp_Pnt P_7 (-4600., design_parameters[12], design_parameters[13]);

    TColgp_Array1OfPnt profile_control_points(1,7);
    profile_control_points.SetValue(1,P_1);
    profile_control_points.SetValue(2,P_2);
    profile_control_points.SetValue(3,P_3);
    profile_control_points.SetValue(4,P_4);
    profile_control_points.SetValue(5,P_5);
    profile_control_points.SetValue(6,P_6);
    profile_control_points.SetValue(7,P_7);

    Handle(Geom_BezierCurve) profile = new Geom_BezierCurve(profile_control_points);

    // back fuselage profile

    gp_Pnt P_back_1 (12500., 0., 1950.);
    gp_Pnt P_back_2 (12500., -1076.95526217, 1950.);
    gp_Pnt P_back_3 (12500., -1950., 1076.95526217);
    gp_Pnt P_back_4 (12500., -1950., 0.);
    gp_Pnt P_back_5 (12500., -1950., -1076.95526217);
    gp_Pnt P_back_6 (12500., -1076.95526217, -1950.);
    gp_Pnt P_back_7 (12500., 0., -1950.);

    TColgp_Array1OfPnt back_profile_control_points(1,7);
    back_profile_control_points.SetValue(1,P_back_1);
    back_profile_control_points.SetValue(2,P_back_2);
    back_profile_control_points.SetValue(3,P_back_3);
    back_profile_control_points.SetValue(4,P_back_4);
    back_profile_control_points.SetValue(5,P_back_5);
    back_profile_control_points.SetValue(6,P_back_6);
    back_profile_control_points.SetValue(7,P_back_7);

    Handle(Geom_BezierCurve) back_profile = new Geom_BezierCurve(back_profile_control_points);

    // guiding curves

    // upper guiding curve
    TColgp_Array1OfPnt upper_guiding_curve_control_points(1,2);
    upper_guiding_curve_control_points.SetValue(1,P_1);
    upper_guiding_curve_control_points.SetValue(2,P_back_1);

    Handle(Geom_BezierCurve) upper_guiding_curve = new Geom_BezierCurve(upper_guiding_curve_control_points);

    // lower guiding curve
    TColgp_Array1OfPnt lower_guiding_curve_control_points(1,2);
    lower_guiding_curve_control_points.SetValue(1,P_7);
    lower_guiding_curve_control_points.SetValue(2,P_back_7);

    Handle(Geom_BezierCurve) lower_guiding_curve = new Geom_BezierCurve(lower_guiding_curve_control_points);

    // create middle fuselage surface
    std::vector<Handle(Geom_Curve)> list_of_profiles_middle_fuselage;
    list_of_profiles_middle_fuselage.push_back(profile);
    list_of_profiles_middle_fuselage.push_back(back_profile);

    std::vector<Handle(Geom_Curve)> list_of_guides_middle_fuselage;
    list_of_guides_middle_fuselage.push_back(upper_guiding_curve);
    list_of_guides_middle_fuselage.push_back(lower_guiding_curve);

    Handle(Geom_BSplineSurface) middle_fuselage
        = geoml::interpolate_curve_network(list_of_profiles_middle_fuselage,
                            list_of_guides_middle_fuselage,
                            1.0);

    // delete later!                        
    std::string filename = "middle_fuselage.brep";
    BRepTools::Write(BRepBuilderAPI_MakeFace(middle_fuselage, Precision::Confusion()), filename.c_str());

    return middle_fuselage;
}

TEST(Test_gordon_surface, gordon_fuselage)
{

    std::vector<Standard_Real> design_parameters = {
        0., 1950.,
        -1076.95526217, 1950.,
        -1950., 1076.95526217, 
        -1950., 0.,
        -1950., -1076.95526217,
        -1076.95526217, -1950.,
        0., -1950.
    };

    //ad sensitivities
    design_parameters[2].setADValue(0, 1.);

    Handle(Geom_BSplineSurface) middle_fuselage = gordon_fuselage(design_parameters);

    Standard_Real u_min, u_max, v_min, v_max;

    middle_fuselage->Bounds(u_min, u_max, v_min, v_max);

    int number_of_samples_in_u = 11;
    int number_of_samples_in_v = 11;

    Standard_Real u_step = (u_max - u_min) / (number_of_samples_in_u - 1);
    Standard_Real v_step = (v_max - v_min) / (number_of_samples_in_v - 1);
  
    std::vector<std::vector<double>> surface_points_original(number_of_samples_in_u * number_of_samples_in_v, std::vector<double>(3)); 
    std::vector<std::vector<double>> ad_sensitivities(number_of_samples_in_u * number_of_samples_in_v, std::vector<double>(3)); 

    int point_id = 0;

    for (Standard_Real u = u_min; u <= u_max + 1e-6; u += u_step)
    {
        for(Standard_Real v = v_min; v <= v_max + 1e-6; v += v_step)
        {
            gp_Pnt a_pnt;

            middle_fuselage->D0(u, v, a_pnt);

            surface_points_original[point_id][0] = a_pnt.X().getValue();
            surface_points_original[point_id][1] = a_pnt.Y().getValue();
            surface_points_original[point_id][2] = a_pnt.Z().getValue();

            ad_sensitivities[point_id][0] = a_pnt.X().getADValue(0);
            ad_sensitivities[point_id][1] = a_pnt.Y().getADValue(0);
            ad_sensitivities[point_id][2] = a_pnt.Z().getADValue(0);

            point_id++;
        }
    }

    design_parameters[2].setADValue(0, 0.);

    //finite differences
    double fd_step = 1e-6;

    double design_parameter_original_value = design_parameters[2].getValue();

    design_parameters[2] = design_parameters[2] + fd_step;

    std::vector<std::vector<double>> surface_points_perturbed(number_of_samples_in_u * number_of_samples_in_v, std::vector<double>(3)); 

    Handle(Geom_BSplineSurface) middle_fuselage_perturbed = gordon_fuselage(design_parameters);
    
    point_id = 0;

    for (Standard_Real u = u_min; u <= u_max + 1e-6; u += u_step)
    {
        for(Standard_Real v = v_min; v <= v_max + 1e-6; v += v_step)
        {
            gp_Pnt a_pnt;

            middle_fuselage_perturbed->D0(u, v, a_pnt);

            surface_points_perturbed[point_id][0] = a_pnt.X().getValue();
            surface_points_perturbed[point_id][1] = a_pnt.Y().getValue();
            surface_points_perturbed[point_id][2] = a_pnt.Z().getValue();

            point_id++;
        }
    }
    
    std::vector<std::vector<double>> fd_sensitivities(number_of_samples_in_u * number_of_samples_in_v, std::vector<double>(3)); 

    for (point_id = 0; point_id < number_of_samples_in_u * number_of_samples_in_v; ++point_id)
    {
        fd_sensitivities[point_id][0] =
            (surface_points_perturbed[point_id][0] - surface_points_original[point_id][0]) / fd_step;

        fd_sensitivities[point_id][1] =
            (surface_points_perturbed[point_id][1] - surface_points_original[point_id][1]) / fd_step;

        fd_sensitivities[point_id][2] =
            (surface_points_perturbed[point_id][2] - surface_points_original[point_id][2]) / fd_step;
    } 

    design_parameters[2] = design_parameter_original_value;

    for (point_id = 0; point_id < number_of_samples_in_u * number_of_samples_in_v; ++point_id)
    {
        if(point_id % 11 == 0)
        {
            std::cout << " sensitivities x,y,z for AD: " << ad_sensitivities[point_id][0] << ", " << ad_sensitivities[point_id][1] << ", " << ad_sensitivities[point_id][2] << std::endl;
            std::cout << " sensitivities x,y,z for FD: " << fd_sensitivities[point_id][0] << ", " << fd_sensitivities[point_id][1] << ", " << fd_sensitivities[point_id][2] << std::endl;
        }

    }    

}
