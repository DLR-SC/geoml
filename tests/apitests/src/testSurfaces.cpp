#include <geoml/surfaces/surfaces.h>
#include <geoml/curves/curves.h>
#include <geoml/curves/BlendCurve.h>
#include <geoml/geom_topo_conversions/geom_topo_conversions.h>
#include <geoml/data_structures/Array2d.h>

#include <gtest/gtest.h>

#include <GeomConvert.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include "GProp_GProps.hxx"
#include "BRepGProp.hxx"
#include "BRepTools.hxx" 

#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeSegment.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI_IntCS.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

#include "STEPControl_Writer.hxx"

#include <filesystem>

#include <string>

// for debugging 
#include <iostream>

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

TEST(Test_surface_from_4_points, simple_surface_from_4_points_test)
{    
    // corner points
    gp_Pnt p_0(0.0, 0.0, 0.0);
    gp_Pnt p_1(1.0, 0.0, 0.0);
    gp_Pnt p_2(1.0, 2.0, 0.0);
    gp_Pnt p_3(0.0, 2.0, 1.0);

    Handle(Geom_Surface) my_surface = geoml::create_surface(p_0, p_1, p_2, p_3);

    Standard_Real u_min, u_max, v_min, v_max;
    my_surface->Bounds(u_min, u_max, v_min, v_max);

    gp_Pnt test_pt_0;

    my_surface->D0(u_min, v_min, test_pt_0);

    EXPECT_NEAR(test_pt_0.X(),p_0.X(), 1e-5);
    EXPECT_NEAR(test_pt_0.Y(),p_0.Y(), 1e-5);
    EXPECT_NEAR(test_pt_0.Z(),p_0.Z(), 1e-5);

    gp_Pnt test_pt_1;

    my_surface->D0(u_max, v_min, test_pt_1);

    EXPECT_NEAR(test_pt_1.X(),p_1.X(), 1e-5);
    EXPECT_NEAR(test_pt_1.Y(),p_1.Y(), 1e-5);
    EXPECT_NEAR(test_pt_1.Z(),p_1.Z(), 1e-5);

    gp_Pnt test_pt_2;

    my_surface->D0(u_max, v_max, test_pt_2);

    EXPECT_NEAR(test_pt_2.X(),p_2.X(), 1e-5);
    EXPECT_NEAR(test_pt_2.Y(),p_2.Y(), 1e-5);
    EXPECT_NEAR(test_pt_2.Z(),p_2.Z(), 1e-5);

    gp_Pnt test_pt_3;

    my_surface->D0(u_min, v_max, test_pt_3);

    EXPECT_NEAR(test_pt_3.X(),p_3.X(), 1e-5);
    EXPECT_NEAR(test_pt_3.Y(),p_3.Y(), 1e-5);
    EXPECT_NEAR(test_pt_3.Z(),p_3.Z(), 1e-5);

    // test the order of the four corner points - expect an anti-clockwise setup
    // in U,V-coordinates, this would mean: 
    //  p_0 is the corner point of the surface for U = u_min, V = v_min,
    //  p_1 is the corner point of the surface for U = u_max, V = v_min,
    //  p_2 is the corner point of the surface for U = u_max, V = v_max,
    //  p_3 is the corner point of the surface for U = u_min, V = v_max.

    gp_Pnt u_min_v_min_corner_point_of_my_surface;
    gp_Pnt u_max_v_min_corner_point_of_my_surface;
    gp_Pnt u_max_v_max_corner_point_of_my_surface;
    gp_Pnt u_min_v_max_corner_point_of_my_surface;

    my_surface->D0(u_min, v_min, u_min_v_min_corner_point_of_my_surface);
    my_surface->D0(u_max, v_min, u_max_v_min_corner_point_of_my_surface);
    my_surface->D0(u_max, v_max, u_max_v_max_corner_point_of_my_surface);
    my_surface->D0(u_min, v_max, u_min_v_max_corner_point_of_my_surface);

    EXPECT_NEAR(p_0.X(), u_min_v_min_corner_point_of_my_surface.X(), 1e-5);
    EXPECT_NEAR(p_0.Y(), u_min_v_min_corner_point_of_my_surface.Y(), 1e-5);
    EXPECT_NEAR(p_0.Z(), u_min_v_min_corner_point_of_my_surface.Z(), 1e-5);

    EXPECT_NEAR(p_1.X(), u_max_v_min_corner_point_of_my_surface.X(), 1e-5);
    EXPECT_NEAR(p_1.Y(), u_max_v_min_corner_point_of_my_surface.Y(), 1e-5);
    EXPECT_NEAR(p_1.Z(), u_max_v_min_corner_point_of_my_surface.Z(), 1e-5);

    EXPECT_NEAR(p_2.X(), u_max_v_max_corner_point_of_my_surface.X(), 1e-5);
    EXPECT_NEAR(p_2.Y(), u_max_v_max_corner_point_of_my_surface.Y(), 1e-5);
    EXPECT_NEAR(p_2.Z(), u_max_v_max_corner_point_of_my_surface.Z(), 1e-5);

    EXPECT_NEAR(p_3.X(), u_min_v_max_corner_point_of_my_surface.X(), 1e-5);
    EXPECT_NEAR(p_3.Y(), u_min_v_max_corner_point_of_my_surface.Y(), 1e-5);
    EXPECT_NEAR(p_3.Z(), u_min_v_max_corner_point_of_my_surface.Z(), 1e-5);


}

TEST(Test_face_from_4_points, simple_face_from_4_points_test)
{
    // corner points
    gp_Pnt p_0(0.0, 0.0, 0.0);
    gp_Pnt p_1(1.0, 0.0, 0.0);
    gp_Pnt p_2(1.0, 2.0, 0.0);
    gp_Pnt p_3(0.0, 2.0, 1.0);

    TopoDS_Face my_face = geoml::create_face(p_0, p_1, p_2, p_3);

    std::vector<gp_Pnt> corner_points;
    
    // retrieve the corner points from my_face
    TopExp_Explorer explorer(my_face, TopAbs_VERTEX);

    while (explorer.More()) {
        TopoDS_Vertex vertex = TopoDS::Vertex(explorer.Current());
        gp_Pnt point = BRep_Tool::Pnt(vertex);
        corner_points.push_back(point);
        explorer.Next();
    }

    EXPECT_EQ(corner_points.size(), 8);

    EXPECT_NEAR(corner_points.at(0).X(), 0., 1e-5);
    EXPECT_NEAR(corner_points.at(0).Y(), 0., 1e-5);
    EXPECT_NEAR(corner_points.at(0).Z(), 0., 1e-5);
    
    EXPECT_NEAR(corner_points.at(1).X(), 1., 1e-5);
    EXPECT_NEAR(corner_points.at(1).Y(), 0., 1e-5);
    EXPECT_NEAR(corner_points.at(1).Z(), 0., 1e-5);

    EXPECT_NEAR(corner_points.at(2).X(), 1., 1e-5);
    EXPECT_NEAR(corner_points.at(2).Y(), 0., 1e-5);
    EXPECT_NEAR(corner_points.at(2).Z(), 0., 1e-5);

    EXPECT_NEAR(corner_points.at(3).X(), 1., 1e-5);
    EXPECT_NEAR(corner_points.at(3).Y(), 2., 1e-5);
    EXPECT_NEAR(corner_points.at(3).Z(), 0., 1e-5);

    EXPECT_NEAR(corner_points.at(4).X(), 0., 1e-5);
    EXPECT_NEAR(corner_points.at(4).Y(), 2., 1e-5);
    EXPECT_NEAR(corner_points.at(4).Z(), 1., 1e-5);

    EXPECT_NEAR(corner_points.at(5).X(), 1., 1e-5);
    EXPECT_NEAR(corner_points.at(5).Y(), 2., 1e-5);
    EXPECT_NEAR(corner_points.at(5).Z(), 0., 1e-5);

    EXPECT_NEAR(corner_points.at(6).X(), 0., 1e-5);
    EXPECT_NEAR(corner_points.at(6).Y(), 0., 1e-5);
    EXPECT_NEAR(corner_points.at(6).Z(), 0., 1e-5);

    EXPECT_NEAR(corner_points.at(7).X(), 0., 1e-5);
    EXPECT_NEAR(corner_points.at(7).Y(), 2., 1e-5);
    EXPECT_NEAR(corner_points.at(7).Z(), 1., 1e-5);
}

TEST(Wing_tip, wing_tip_modelling)
{
// now define approximations of CST curves

// upper curve

gp_Pnt P_1_upper (0., 0., 0.);
gp_Pnt P_2_upper (0., 0.01143, 0.);
gp_Pnt P_3_upper (0.05032, 0.03167, 0.);
gp_Pnt P_4_upper (0.19040, 0.05659, 0.);
gp_Pnt P_5_upper (0.42957, 0.07027, 0.);
gp_Pnt P_6_upper (0.65534, 0.05882, 0.);
gp_Pnt P_7_upper (0.86221, 0.02922, 0.);
gp_Pnt P_8_upper (1., 0.0025, 0.);

std::vector<gp_Pnt> control_points_upper {P_1_upper, P_2_upper, P_3_upper, P_4_upper, P_5_upper, P_6_upper, P_7_upper, P_8_upper};

Standard_Integer degree_upper = 4;
std::vector<Standard_Real> weights_upper (8,1.0);

// knots: note: as the number of knots (m+1), the number of control points (n+1) and the degree p
// are related by m = n + p +1, the number of knots has to be 13 in this case.
std::vector<Standard_Real> knots_upper{0.0, 1.0, 2.0, 3.0, 4.0};
std::vector<int> mults_upper{5, 1, 1, 1, 5};

Handle(Geom_BSplineCurve) upper_cst = geoml::nurbs_curve(control_points_upper, weights_upper, knots_upper, mults_upper, degree_upper);

TopoDS_Edge upper_cst_edge = geoml::CurveToEdge(upper_cst);

// write to brep file
std::string filename = "upper_cst_edge_approx.brep";
BRepTools::Write(upper_cst_edge, filename.c_str());

// write to step file
STEPControl_Writer writer_for_cst_curve_upper;
writer_for_cst_curve_upper.Transfer(upper_cst_edge, STEPControl_AsIs);

filename = "upper_cst_edge_approx.stp";
writer_for_cst_curve_upper.Write(filename.c_str());

// lower curve

gp_Pnt P_1_lower (0., 0., 0.);
gp_Pnt P_2_lower (0., -0.01149, 0.);
gp_Pnt P_3_lower (0.05032, -0.03195, 0.);
gp_Pnt P_4_lower (0.19040, -0.062226, 0.);
gp_Pnt P_5_lower (0.42957, -0.06356, 0.);
gp_Pnt P_6_lower (0.65534, -0.03086, 0.);
gp_Pnt P_7_lower (0.86221, -0.002578, 0.);
gp_Pnt P_8_lower (0.94566, -0.001034, 0.);
gp_Pnt P_9_lower (1., -0.0025, 0.);

std::vector<gp_Pnt> control_points_lower {P_1_lower, P_2_lower, P_3_lower, P_4_lower, P_5_lower, P_6_lower, P_7_lower, P_8_lower, P_9_lower};

Standard_Integer degree_lower = 4;
std::vector<Standard_Real> weights_lower (9,1.0);

// knots: note: as the number of knots (m+1), the number of control points (n+1) and the degree p
// are related by m = n + p +1, the number of knots has to be 14 in this case.
std::vector<Standard_Real> knots_lower{0.0, 1.0, 2.0, 3.0, 4.0, 5.0};
std::vector<int> mults_lower{5, 1, 1, 1, 1, 5};

Handle(Geom_BSplineCurve) lower_cst = geoml::nurbs_curve(control_points_lower, weights_lower, knots_lower, mults_lower, degree_lower);

TopoDS_Edge lower_cst_edge = geoml::CurveToEdge(lower_cst);

// write to brep file
filename = "lower_cst_edge_approx.brep";
BRepTools::Write(lower_cst_edge, filename.c_str());

// write to step file
STEPControl_Writer writer_for_cst_curve_lower;
writer_for_cst_curve_lower.Transfer(lower_cst_edge, STEPControl_AsIs);

filename = "lower_cst_edge_approx.stp";
writer_for_cst_curve_lower.Write(filename.c_str()); 

// closing trailing edge

Standard_Real last_parameter_upper_cst = upper_cst->LastParameter();
Standard_Real last_parameter_lower_cst = lower_cst->LastParameter();

gp_Pnt trailing_edge_upper_point;
gp_Pnt trailing_edge_lower_point;

upper_cst->D0(last_parameter_upper_cst, trailing_edge_upper_point);
lower_cst->D0(last_parameter_lower_cst, trailing_edge_lower_point);

Handle(Geom_TrimmedCurve) trailing_edge_curve = GC_MakeSegment(trailing_edge_upper_point, trailing_edge_lower_point); 

TopoDS_Edge trailing_edge = BRepBuilderAPI_MakeEdge(trailing_edge_curve);

TopoDS_Wire wing_airfoil_base_wire = BRepBuilderAPI_MakeWire(upper_cst_edge, trailing_edge, lower_cst_edge);

filename = "wing_airfoil_base_wire_approx.brep";
BRepTools::Write(wing_airfoil_base_wire, filename.c_str());

// write to step file
STEPControl_Writer writer_for_wing_airfoil_base_wire_approx;
writer_for_wing_airfoil_base_wire_approx.Transfer(wing_airfoil_base_wire, STEPControl_AsIs);

filename = "wing_airfoil_base_wire_approx.stp";
writer_for_wing_airfoil_base_wire_approx.Write(filename.c_str()); 

// rotate the wing airfoli base wire around the x-axis

gp_Pnt leading_edge_wing_base_profile (0., 0., 0.);

gp_Trsf gp_transform_ox; 

gp_transform_ox.SetRotation(gp::OX(), M_PI/2);

BRepBuilderAPI_Transform transform_ox (wing_airfoil_base_wire, gp_transform_ox, Standard_True);

TopoDS_Shape wing_airfoil_base_wire_xz = transform_ox.Shape();

gp_Trsf gp_transform_scale_base;

gp_transform_scale_base.SetScale(leading_edge_wing_base_profile, 6567.896);

BRepBuilderAPI_Transform transform_scale (wing_airfoil_base_wire_xz, gp_transform_scale_base, Standard_True);

TopoDS_Shape wing_airfoil_base_wire_xz_scaled = transform_scale.Shape();

filename = "wing_airfoil_base_wire_xz_scaled.brep";
BRepTools::Write(wing_airfoil_base_wire_xz_scaled, filename.c_str());

// construct tip wire

gp_Trsf gp_transform_scale_tip;

gp_transform_scale_tip.SetScale(leading_edge_wing_base_profile, 1313.579);

BRepBuilderAPI_Transform transform_scale_tip (wing_airfoil_base_wire_xz, gp_transform_scale_tip, Standard_True);

TopoDS_Shape wing_airfoil_tip_wire_xz_scaled = transform_scale_tip.Shape();

filename = "wing_airfoil_tip_wire_xz_scaled.brep";
BRepTools::Write(wing_airfoil_tip_wire_xz_scaled, filename.c_str());

// translation vector

Standard_Real translation_angle = 27.5;

Standard_Real translation_z = 0;
Standard_Real translation_y = 16748.134;
Standard_Real translation_x = translation_y * std::tan(translation_angle * M_PI / 180);

gp_Vec translation_vec (translation_x, translation_y, translation_z);

gp_Trsf gp_transform_translate_tip;

gp_transform_translate_tip.SetTranslation(translation_vec);

BRepBuilderAPI_Transform transform_translate_tip (wing_airfoil_tip_wire_xz_scaled, gp_transform_translate_tip, Standard_True);

TopoDS_Shape wing_airfoil_tip_wire_xz_scaled_and_translated = transform_translate_tip.Shape();

filename = "wing_airfoil_tip_wire_xz_scaled_and_translated.brep";
BRepTools::Write(wing_airfoil_tip_wire_xz_scaled_and_translated, filename.c_str());

// create wing surface

BRepOffsetAPI_ThruSections loft_generator (Standard_False, Standard_True);

TopoDS_Wire tip_wire = TopoDS::Wire(wing_airfoil_tip_wire_xz_scaled_and_translated);

loft_generator.AddWire(TopoDS::Wire(wing_airfoil_base_wire_xz_scaled));
loft_generator.AddWire(tip_wire);

loft_generator.Build();

TopoDS_Shape wing_shape = loft_generator.Shape();

filename = "wing_shape.brep";
BRepTools::Write(wing_shape, filename.c_str());

// write to step file
STEPControl_Writer writer_for_wing_wing_shape_approx;
writer_for_wing_wing_shape_approx.Transfer(wing_shape, STEPControl_AsIs);

filename = "wing_shape_approx.stp";
writer_for_wing_wing_shape_approx.Write(filename.c_str()); 

/*
    ////////////////////
    Wing tip modelling
    ////////////////////
*/

TopExp_Explorer Explorer_wing_faces;

std::vector<TopoDS_Face> wing_faces;
for (Explorer_wing_faces.Init(wing_shape, TopAbs_FACE); Explorer_wing_faces.More(); Explorer_wing_faces.Next()) {
    TopoDS_Face face = TopoDS::Face(Explorer_wing_faces.Current());
    wing_faces.push_back(face);
}

TopoDS_Face upper_face_wing = wing_faces[0];
TopoDS_Face trailing_face_wing = wing_faces[1];
TopoDS_Face lower_face_wing = wing_faces[2];


TopExp_Explorer Explorer_upper_wing_face;

std::vector<TopoDS_Edge> edges_upper_wing;
for (Explorer_upper_wing_face.Init(upper_face_wing, TopAbs_EDGE); Explorer_upper_wing_face.More(); Explorer_upper_wing_face.Next()) {
    TopoDS_Edge edge = TopoDS::Edge(Explorer_upper_wing_face.Current());
    edges_upper_wing.push_back(edge);
}

TopoDS_Edge leading_edge_upper = edges_upper_wing[3];
TopoDS_Edge trailing_edge_upper = edges_upper_wing[1];
TopoDS_Edge outer_edge_upper = edges_upper_wing[2];
TopoDS_Edge inner_edge_upper = edges_upper_wing[0];

Handle(Geom_Curve) leading_curve_upper = geoml::EdgeToCurve(leading_edge_upper);
Handle(Geom_Curve) trailing_curve_upper = geoml::EdgeToCurve(trailing_edge_upper);
Handle(Geom_Curve) outer_curve_upper = geoml::EdgeToCurve(outer_edge_upper);
Handle(Geom_Curve) inner_curve_upper = geoml::EdgeToCurve(inner_edge_upper);

TopExp_Explorer Explorer_lower_wing_face;

std::vector<TopoDS_Edge> edges_lower_wing;
for (Explorer_lower_wing_face.Init(lower_face_wing, TopAbs_EDGE); Explorer_lower_wing_face.More(); Explorer_lower_wing_face.Next()) {
    TopoDS_Edge edge = TopoDS::Edge(Explorer_lower_wing_face.Current());
    edges_lower_wing.push_back(edge);
}

TopoDS_Edge leading_edge_lower = edges_lower_wing[1]; 
TopoDS_Edge trailing_edge_lower = edges_lower_wing[3];  
TopoDS_Edge outer_edge_lower = edges_lower_wing[2]; 
TopoDS_Edge inner_edge_lower = edges_lower_wing[0]; 

Handle(Geom_Curve) leading_curve_lower = geoml::EdgeToCurve(leading_edge_lower);
Handle(Geom_Curve) trailing_curve_lower = geoml::EdgeToCurve(trailing_edge_lower);
Handle(Geom_Curve) outer_curve_lower = geoml::EdgeToCurve(outer_edge_lower);
Handle(Geom_Curve) inner_curve_lower = geoml::EdgeToCurve(inner_edge_lower);


Standard_Real first_parameter_leading_curve_upper = leading_curve_upper->FirstParameter();
Standard_Real last_parameter_leading_curve_upper = leading_curve_upper->LastParameter();

Standard_Real LE_relative_start_parameter = 0.97;
Standard_Real LE_reference_point_parameter_upper = first_parameter_leading_curve_upper + (last_parameter_leading_curve_upper - first_parameter_leading_curve_upper) * LE_relative_start_parameter;

gp_Pnt LE_reference_point;
gp_Vec LE_derivative_in_reference_point;

leading_curve_upper->D1(LE_reference_point_parameter_upper, LE_reference_point, LE_derivative_in_reference_point);

Standard_Real last_parameter_trailing_curve_upper = trailing_curve_upper->LastParameter();

gp_Pnt TE_reference_point_upper = trailing_curve_upper->Value(last_parameter_trailing_curve_upper);


Standard_Real last_parameter_trailing_curve_lower = trailing_curve_lower->LastParameter();
gp_Pnt TE_reference_point_lower = trailing_curve_lower->Value(last_parameter_trailing_curve_lower);

gp_Pnt TE_reference_point ((TE_reference_point_upper.X() + TE_reference_point_lower.X()) / 2., (TE_reference_point_upper.Y() + TE_reference_point_lower.Y()) / 2., (TE_reference_point_upper.Z() + TE_reference_point_lower.Z()) / 2.);

gp_Pnt LE_outer_point = leading_curve_upper->Value(last_parameter_leading_curve_upper);

TColgp_Array1OfPnt TE_reference_line_control_points(1,2);
TE_reference_line_control_points.SetValue(1, TE_reference_point);
TE_reference_line_control_points.SetValue(2, LE_outer_point);

Handle(Geom_BezierCurve) TE_reference_curve = new Geom_BezierCurve(TE_reference_line_control_points);
TopoDS_Edge TE_reference_edge = geoml::CurveToEdge(TE_reference_curve);


// split leading_curve_upper at LE_reference_point
Handle(Geom_TrimmedCurve) leading_curve_upper_first_trimmed_part = new Geom_TrimmedCurve(leading_curve_upper, first_parameter_leading_curve_upper, LE_reference_point_parameter_upper);

TopoDS_Edge leading_edge_upper_first_trimmed_part = geoml::CurveToEdge(leading_curve_upper_first_trimmed_part);


// write to step file
STEPControl_Writer writer_leading_edge_upper_first_trimmed_part;
writer_leading_edge_upper_first_trimmed_part.Transfer(leading_edge_upper_first_trimmed_part, STEPControl_AsIs);

filename = "leading_edge_upper_first_trimmed_part.stp";
writer_leading_edge_upper_first_trimmed_part.Write(filename.c_str()); 




geoml::BlendCurveConnection bend_start_connection_upper_outer (leading_edge_upper_first_trimmed_part, LE_reference_point, geoml::GContinuity::G2, true, 0.015);
geoml::BlendCurveConnection bend_end_connection_upper_outer (TE_reference_edge, TE_reference_point, geoml::GContinuity::G1, false, 0.5);

TopoDS_Edge blend_edge_upper_outer = geoml::blend_curve(bend_start_connection_upper_outer, bend_end_connection_upper_outer);


// write to step file
STEPControl_Writer writer_blend_edge_upper_outer;
writer_blend_edge_upper_outer.Transfer(blend_edge_upper_outer, STEPControl_AsIs);

filename = "blend_edge_upper_outer.stp";
writer_blend_edge_upper_outer.Write(filename.c_str()); 




gp_Pnt LE_inner_point = leading_curve_upper->Value(first_parameter_leading_curve_upper);


Standard_Real first_parameter_trailing_curve_upper = trailing_curve_upper->FirstParameter();

gp_Pnt TE_reference_point_upper_inner = trailing_curve_upper->Value(first_parameter_trailing_curve_upper);


Standard_Real first_parameter_trailing_curve_lower = trailing_curve_lower->FirstParameter();

gp_Pnt TE_reference_point_lower_inner = trailing_curve_lower->Value(first_parameter_trailing_curve_lower);

gp_Pnt TE_reference_point_inner ((TE_reference_point_upper_inner.X() + TE_reference_point_lower_inner.X()) / 2., (TE_reference_point_upper_inner.Y() + TE_reference_point_lower_inner.Y()) / 2., (TE_reference_point_upper_inner.Z() + TE_reference_point_lower_inner.Z()) / 2.);

TColgp_Array1OfPnt TE_reference_line_inner_control_points(1,2);
TE_reference_line_inner_control_points.SetValue(1, TE_reference_point_inner);
TE_reference_line_inner_control_points.SetValue(2, LE_inner_point);

Handle(Geom_BezierCurve) TE_reference_curve_inner = new Geom_BezierCurve(TE_reference_line_inner_control_points);
TopoDS_Edge TE_reference_edge_inner = geoml::CurveToEdge(TE_reference_curve_inner);


Standard_Real isoparameter = 0.85;

gp_Pnt isocurve_inner_point = TE_reference_curve_inner->Value(isoparameter);
gp_Pnt isocurve_outer_point = TE_reference_curve ->Value(isoparameter);

TColgp_Array1OfPnt isocurve_control_points(1,2);
isocurve_control_points.SetValue(1, isocurve_inner_point);
isocurve_control_points.SetValue(2, isocurve_outer_point);

Handle(Geom_BezierCurve) isocurve_curve = new Geom_BezierCurve(isocurve_control_points);
TopoDS_Edge isocurve_edge = geoml::CurveToEdge(isocurve_curve);

// orthogonal LE plane

gp_Dir orth_direction_LE_plane (LE_derivative_in_reference_point);

Handle(Geom_Plane) orthogonal_LE_plane = new Geom_Plane(LE_reference_point, orth_direction_LE_plane);



// intersect orthogonal LE plane with isocurve

GeomAPI_IntCS intersection_isocurve_orthogonal_LE_plane (isocurve_curve, orthogonal_LE_plane);

gp_Pnt intersection_point_isocurve_orthogonal_LE_plane = intersection_isocurve_orthogonal_LE_plane.Point(1);


// split isocurve_curve_upper at intersection point

// get parameter for split
GeomAPI_ProjectPointOnCurve projector_intersection_point_on_isocurve(intersection_point_isocurve_orthogonal_LE_plane, isocurve_curve);

Standard_Real parameter_isocurve_split = projector_intersection_point_on_isocurve.Parameter(1);

Standard_Real first_parameter_isocurve = isocurve_curve->FirstParameter();

Handle(Geom_TrimmedCurve) isocurve_inner_trimmed_part = new Geom_TrimmedCurve(isocurve_curve, first_parameter_isocurve, parameter_isocurve_split);

TopoDS_Edge isocurve_edge_inner_trimmed_part = geoml::CurveToEdge(isocurve_inner_trimmed_part);


TColgp_Array1OfPnt TE_reference_curve_middle_control_points(1,2);
TE_reference_curve_middle_control_points.SetValue(1, TE_reference_point_inner);
TE_reference_curve_middle_control_points.SetValue(2, TE_reference_point);

Handle(Geom_BezierCurve) TE_reference_curve_middle = new Geom_BezierCurve(TE_reference_curve_middle_control_points);
TopoDS_Edge TE_reference_edge_middle = geoml::CurveToEdge(TE_reference_curve_middle);

Standard_Real first_parameter_TE_reference_curve_middle = TE_reference_curve_middle->FirstParameter();
Standard_Real last_parameter_TE_reference_curve_middle = TE_reference_curve_middle->LastParameter();

Standard_Real relative_parameter_TE_reference_curve_middle = 0.997;
Standard_Real TE_reference_parameter = first_parameter_TE_reference_curve_middle + (last_parameter_TE_reference_curve_middle - first_parameter_TE_reference_curve_middle) * relative_parameter_TE_reference_curve_middle;


gp_Pnt TE_reference_point_inside = TE_reference_curve_middle->Value(TE_reference_parameter);

std::cout << "X: " << TE_reference_point_inside.X() << "Y: " << TE_reference_point_inside.Y() << "Z: " << TE_reference_point_inside.Z() << std::endl;

gp_Pnt end_point_for_reference_line_for_inner_bend (10445., 16700., 0.);

TColgp_Array1OfPnt reference_line_for_inner_bend_control_points(1,2);
reference_line_for_inner_bend_control_points.SetValue(1, TE_reference_point_inside);
reference_line_for_inner_bend_control_points.SetValue(2, end_point_for_reference_line_for_inner_bend);

Handle(Geom_BezierCurve) reference_line_for_inner_bend = new Geom_BezierCurve(reference_line_for_inner_bend_control_points);
TopoDS_Edge reference_edge_for_inner_bend = geoml::CurveToEdge(reference_line_for_inner_bend);





geoml::BlendCurveConnection bend_inner_start_connection_ (isocurve_edge_inner_trimmed_part, intersection_point_isocurve_orthogonal_LE_plane, geoml::GContinuity::G2, true, 0.011);
geoml::BlendCurveConnection bend_inner_end_connection (reference_edge_for_inner_bend, TE_reference_point_inside, geoml::GContinuity::G1, true, 1.5);

TopoDS_Edge blend_edge_inner_bend = geoml::blend_curve(bend_inner_start_connection_, bend_inner_end_connection);




// write to step file
STEPControl_Writer writer_blend_edge_inner_bend;
writer_blend_edge_inner_bend.Transfer(blend_edge_inner_bend, STEPControl_AsIs);

filename = "blend_edge_inner_bend.stp";
writer_blend_edge_inner_bend.Write(filename.c_str()); 

}