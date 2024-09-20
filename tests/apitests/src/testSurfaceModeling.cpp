/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2018 German Aerospace Center (DLR)
*/


#include <geoml/surfaces/modeling.h>
#include <geoml/curves/modeling.h>
#include <geoml/geom_topo/modeling.h>
#include <geoml/data_structures/Array2d.h>

#include <gtest/gtest.h>

#include <GeomConvert.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include "GProp_GProps.hxx"
#include "BRepGProp.hxx"


#include <TopoDS_Shape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <filesystem>

namespace apitests
{

std::vector<Handle(Geom_Curve)> read_curves(const std::string& brepFile, bool& ok)
{
    // first read the brep-input file
    TopoDS_Shape shape_u;
    BRep_Builder builder_u;

    ok = BRepTools::Read(shape_u, brepFile.c_str(), builder_u);
    if (!ok) return {};

    TopExp_Explorer Explorer;

    // get the splines in u-direction from the Edges
    std::vector<Handle(Geom_Curve)> curves;
    for (Explorer.Init(shape_u, TopAbs_EDGE); Explorer.More(); Explorer.Next()) {
        const auto& edge = TopoDS::Edge(Explorer.Current());
        double beginning = 0;
        double end = 1;
        curves.push_back(BRep_Tool::Curve(edge, beginning, end));
    }

    return curves;
}

} // apitests

class interpolate_curve_network: public ::testing::TestWithParam<std::string>
{
protected:
    void SetUp() override
    {
        // get the name of the folder with the B-spline network data
        std::string path_profiles = "../unittests/TestData/CurveNetworks/" + GetParam() + "/profiles.brep";
        std::string path_guides = "../unittests/TestData/CurveNetworks/" + GetParam() + "/guides.brep";

        std::filesystem::path output_dir("TestData/CurveNetworks/" + GetParam());
        if (!std::filesystem::is_directory(output_dir)) {
            std::filesystem::create_directories(output_dir);
        }

        path_output = "TestData/CurveNetworks/" + GetParam() + "/result_gordon.brep";

        bool ok = false;
        ucurves = apitests::read_curves(path_profiles, ok);
        ASSERT_TRUE(ok);

        ok = false;
        vcurves = apitests::read_curves(path_guides, ok);
        ASSERT_TRUE(ok);

    }

    // name of the folder with the B-spline network data
    std::vector<Handle(Geom_Curve)> vcurves, ucurves;
    std::string path_output;

};

TEST_P(interpolate_curve_network, testFromBRep)
{

    Handle(Geom_BSplineSurface) gordonSurface = geoml::interpolate_curve_network(ucurves, vcurves, 3e-4);
    BRepTools::Write(BRepBuilderAPI_MakeFace(gordonSurface, Precision::Confusion()), path_output.c_str());
}

INSTANTIATE_TEST_SUITE_P(SurfaceModeling, interpolate_curve_network, ::testing::Values(
   "nacelle",
   "full_nacelle",
   "wing2",
   "spiralwing",
   "test_surface4_sorted",
   "test_surface4",
   "wing3",
   "bellyfairing",
   "helibody",
   "fuselage1",
   "fuselage2",
   "ffd"
));

class interpolate_curves: public ::testing::TestWithParam<std::string>
{
protected:
    void SetUp() override
    {
        // get the name of the folder with the B-spline network data
        std::string path_profiles = "../unittests/TestData/CurveNetworks/" + GetParam() + "/profiles.brep";

        std::filesystem::path output_dir("TestData/InterpolateCurves/" + GetParam());
        if (!std::filesystem::is_directory(output_dir)) {
            std::filesystem::create_directories(output_dir);
        }

        path_output = "TestData/InterpolateCurves/" + GetParam() + "/result.brep";

        bool ok = false;
        curves = apitests::read_curves(path_profiles, ok);
        ASSERT_TRUE(ok);
    }

    // name of the folder with the B-spline network data
    std::vector<Handle(Geom_Curve)> curves;
    std::string path_output;

};

TEST_P(interpolate_curves, testFromBRep)
{

    Handle(Geom_BSplineSurface) surface = geoml::interpolate_curves(curves);
    BRepTools::Write(BRepBuilderAPI_MakeFace(surface, Precision::Confusion()), path_output.c_str());
}

// @TODO: not all are working
INSTANTIATE_TEST_SUITE_P(SurfaceModeling, interpolate_curves, ::testing::Values(
  // "nacelle",
  "full_nacelle",
  "wing2",
  // "spiralwing",
  "test_surface4_sorted",
  "test_surface4",
  "wing3",
  // "bellyfairing",
  "helibody",
  "fuselage1",
  "fuselage2",
  "ffd"
));


TEST(Test_revolving_shape, simple_revolving_surface)
{    

// define an open and clamped NURBS curve of degree 2

// control points
gp_Pnt p1(1.0, 0.0, 0.0);
gp_Pnt p2(1.0, 0.0, 1.0);
gp_Pnt p3(1.0, 0.0, 2.0);
gp_Pnt p4(1.0, 0.0, 3.0);
gp_Pnt p5(1.0, 0.0, 4.0);

std::vector<gp_Pnt> control_points {p1, p2, p3, p4, p5};

// degree:
Standard_Integer degree = 2;

// weights:
std::vector<Standard_Real> weights(5,1.0);

// knots: 
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

TopoDS_Edge edge = geoml::CurveToEdge(curve);

gp_Pnt pnt (0., 0., 0.);
gp_Vec vec (0., 0., 1.);

TopoDS_Shape face = geoml::revolving_shape(edge, pnt, vec); 

// calculate area of a cylinder with radius 1 and hight 4:
double expected_surface_area = 2 * M_PI * 1 * 4; 

// calculate intersection area
GProp_GProps props = GProp_GProps();
BRepGProp::SurfaceProperties(face, props);
double area_surf = props.Mass();

EXPECT_NEAR(area_surf, expected_surface_area, 1e-1);

}

TEST(Test_nurbs_surface, simple_nurbs_surface)
{    
// define a non-rational B-spline surface

// control points
gp_Pnt p_00(0.0, 0.0, 0.0);
gp_Pnt p_10(0.0, 1.0, 0.0);
gp_Pnt p_20(0.0, 2.0, 0.0);
gp_Pnt p_01(1.0, 0.0, 0.0);
gp_Pnt p_11(1.0, 1.0, 0.0);
gp_Pnt p_21(1.0, 2.0, 1.0);

geoml::Array2d<gp_Pnt> control_points (3,2);

control_points.setValue(0, 0, p_00);
control_points.setValue(1, 0, p_10);
control_points.setValue(2, 0, p_20);
control_points.setValue(0, 1, p_01);
control_points.setValue(1, 1, p_11);
control_points.setValue(2, 1, p_21);

geoml::Array2d<Standard_Real> weights (3,2);

weights.setValue(0, 0, 1.);
weights.setValue(1, 0, 1.);
weights.setValue(2, 0, 1.);
weights.setValue(0, 1, 1.);
weights.setValue(1, 1, 1.);
weights.setValue(2, 1, 1.);

std::vector<Standard_Real> U_knots;

U_knots.push_back(0.0);
U_knots.push_back(1.0);

std::vector<Standard_Real> V_knots;

V_knots.push_back(0.0);
V_knots.push_back(1.0);

std::vector<int> U_mults;

U_mults.push_back(3);
U_mults.push_back(3);

std::vector<int> V_mults;

V_mults.push_back(2);
V_mults.push_back(2);

int U_degree = 2;
int V_degree = 1;

Handle(Geom_BSplineSurface) test_surface = geoml::nurbs_surface(control_points, weights, U_knots, V_knots, U_mults, V_mults, U_degree, V_degree);

gp_Pnt test_pt_00;

test_surface->D0(0., 0., test_pt_00);

EXPECT_NEAR(test_pt_00.X(),0., 1e-5);
EXPECT_NEAR(test_pt_00.Y(),0., 1e-5);
EXPECT_NEAR(test_pt_00.Z(),0., 1e-5);

gp_Pnt test_pt_20;

test_surface->D0(1., 0., test_pt_20);

EXPECT_NEAR(test_pt_20.X(),0., 1e-5);
EXPECT_NEAR(test_pt_20.Y(),2., 1e-5);
EXPECT_NEAR(test_pt_20.Z(),0., 1e-5);

gp_Pnt test_pt_01;

test_surface->D0(0., 1., test_pt_01);

EXPECT_NEAR(test_pt_01.X(),1., 1e-5);
EXPECT_NEAR(test_pt_01.Y(),0., 1e-5);
EXPECT_NEAR(test_pt_01.Z(),0., 1e-5);

gp_Pnt test_pt_21;

test_surface->D0(1., 1., test_pt_21);

EXPECT_NEAR(test_pt_21.X(),1., 1e-5);
EXPECT_NEAR(test_pt_21.Y(),2., 1e-5);
EXPECT_NEAR(test_pt_21.Z(),1., 1e-5);

}
