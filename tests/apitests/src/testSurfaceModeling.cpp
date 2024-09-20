#include <geoml/surfaces/modeling.h>
#include <geoml/curves/modeling.h>
#include <geoml/geom_topo/modeling.h>
#include <geoml/data_structures/Array2d.h>

#include <gtest/gtest.h>

#include <GeomConvert.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include "GProp_GProps.hxx"
#include "BRepGProp.hxx"


#include <TopoDS_Shape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Geom_BezierCurve.hxx>

#include <filesystem>

#include <vector>
#include "BRepTools.hxx" 
#include <gp_GTrsf.hxx>
#include <gp_Mat.hxx>
#include <gp_XYZ.hxx>
#include <BRepBuilderAPI_GTransform.hxx>

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


TEST(Test_nurbs_surface, gordon_fuselage)
{  

// middle part of fuselage

// front fuselage profile

gp_Pnt P_1 (-4600., 0., 1950.);
gp_Pnt P_2 (-4600., -1076.95526217, 1950.);
gp_Pnt P_3 (-4600., -1950., 1076.95526217);
gp_Pnt P_4 (-4600., -1950., 0.);
gp_Pnt P_5 (-4600., -1950., -1076.95526217);
gp_Pnt P_6 (-4600., -1076.95526217, -1950.);
gp_Pnt P_7 (-4600., 0., -1950.);

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
std::string filename = "middle_fuselage.brep";
BRepTools::Write(BRepBuilderAPI_MakeFace(middle_fuselage, Precision::Confusion()), filename.c_str());

// tail

// tail guides

// upper guide curve
gp_Pnt P_upper_guide_end_point (25716., 0., 1950.);


TColgp_Array1OfPnt tail_upper_guide_control_points(1,2);
tail_upper_guide_control_points.SetValue(1,P_back_1);
tail_upper_guide_control_points.SetValue(2,P_upper_guide_end_point);

Handle(Geom_BezierCurve) tail_upper_guide_curve = new Geom_BezierCurve(tail_upper_guide_control_points);

// lower guide curve
gp_Pnt P_lower_guide_2 (14000., 0., -1950.);
gp_Pnt P_lower_guide_3 (15500., 0., -1950.);
gp_Pnt P_lower_guide_end (25716., 0., 1200.);

TColgp_Array1OfPnt tail_lower_guide_curve_control_points(1,4);
tail_lower_guide_curve_control_points.SetValue(1,P_back_7);
tail_lower_guide_curve_control_points.SetValue(2,P_lower_guide_2);
tail_lower_guide_curve_control_points.SetValue(3,P_lower_guide_3);
tail_lower_guide_curve_control_points.SetValue(4,P_lower_guide_end);

Handle(Geom_BezierCurve) tail_lower_guide_curve = new Geom_BezierCurve(tail_lower_guide_curve_control_points);


// tail profile

// scaling and translation values
Standard_Real difference_in_z_1 = Abs(tail_upper_guide_control_points.Value(1).Z() - tail_lower_guide_curve_control_points.Value(1).Z());
Standard_Real difference_in_z_2 = Abs(tail_upper_guide_control_points.Value(tail_upper_guide_control_points.Upper()).Z() - tail_lower_guide_curve_control_points.Value(tail_lower_guide_curve_control_points.Upper()).Z());
 
Standard_Real difference_ratio = difference_in_z_2 / difference_in_z_1;

std::cout << "difference_in_z_1: " << difference_in_z_1 << std::endl; 
std::cout << "difference_in_z_2: " << difference_in_z_2 << std::endl; 

Standard_Real scale_x = 1.;
Standard_Real scale_y = difference_ratio;
Standard_Real scale_z = difference_ratio;

Standard_Real move_x = tail_upper_guide_control_points.Value(tail_upper_guide_control_points.Upper()).X() - back_profile_control_points.Value(1).X();

Standard_Real move_z = (Abs(P_back_1.Z() - P_back_7.Z()) * (1 - difference_ratio)) / 2;
  
gp_GTrsf gTrsf (gp_Mat(scale_x, 0, 0, 0, scale_y, 0, 0, 0, scale_z), gp_XYZ(move_x, 0., move_z));


// create an edge of back_profile
BRepBuilderAPI_MakeEdge back_profile_edge (back_profile);

BRepBuilderAPI_GTransform gTransform (back_profile_edge, gTrsf, true);

TopoDS_Shape tail_profile_edge = gTransform.Shape();

std::string filename_2 = "tail_profile_edge.brep";
BRepTools::Write(tail_profile_edge, filename_2.c_str());






 



// gp_Pnt 
// // now, define the upper wing's surface:
// std::vector<Handle(Geom_Curve)> list_of_profiles_upper_wing;
// list_of_profiles_upper_wing.push_back(i_u_profile_curve);
// list_of_profiles_upper_wing.push_back(m_u_profile_curve);
// list_of_profiles_upper_wing.push_back(o_u_profile_curve);

// std::vector<Handle(Geom_Curve)> list_of_guides_upper_wing;
// list_of_guides_upper_wing.push_back(leading_curve);
// list_of_guides_upper_wing.push_back(trailing_curve);

// Handle(Geom_BSplineSurface) upper_wing_surface
// 	= geoml::interpolate_curve_network(list_of_profiles_upper_wing,
//                           list_of_guides_upper_wing,
//                           1.0);

// cap for nose surface:

// define a profile curve tangential to the profile curve of 
// nose_ogive_srf (arc_b_spline_curve):

// last control point of the cap-profile curve:
// gp_Pnt end_point_arc_cap(1000.0, 0.0, 0.0); // original: x = 1000.0

// // get the first control point of the cap-profile curve and the starting tangent vector:
// double start_param_cap = arc_b_spline_curve->LastParameter();
// gp_Pnt start_point_arc_cap;
// gp_Vec tangent_vector_cap;
// arc_b_spline_curve->D1(start_param_cap, start_point_arc_cap, tangent_vector_cap);

// // second control point of cap-profile curve:
// gp_Pnt cp_cap_2 = geoml::move(start_point_arc_cap, tangent_vector_cap, 0.04);

// //third control point of cap-profile curve:
// gp_Pnt cp_cap_3 = geoml::move(end_point_arc_cap, gp_Vec(0., 0., 1.), 550.0);

// // define the cap-profile curve:
// TColgp_Array1OfPnt profile_points_cap(1,4);
// profile_points_cap.SetValue(1,start_point_arc_cap);
// profile_points_cap.SetValue(2,cp_cap_2);
// profile_points_cap.SetValue(3,cp_cap_3);
// profile_points_cap.SetValue(4,end_point_arc_cap);

// // degree:
// Standard_Integer degree_profile_cap = 3;

// // weights:
// TColStd_Array1OfReal weights_profile_cap(1, 4);
// weights_profile_cap.SetValue(1, 1.0);  
// weights_profile_cap.SetValue(2, 1.0);
// weights_profile_cap.SetValue(3, 1.0);  
// weights_profile_cap.SetValue(4, 1.0);  

// // knots:
// TColStd_Array1OfReal knots_profile_cap(1,2); 
// knots_profile_cap.SetValue(1,0.0);           
// knots_profile_cap.SetValue(2,1.0);

// // multiplicities: 
// TColStd_Array1OfInteger mults_profile_cap(1,2); 
// mults_profile_cap.SetValue(1,4);                
// mults_profile_cap.SetValue(2,4);

// // create the profile curve:
// Handle(Geom_BSplineCurve) profile_curve_cap 
// 	= geoml::b_spline_curve(profile_points_cap, weights_profile_cap, 
// 							knots_profile_cap, mults_profile_cap, degree_profile_cap);

// writeToStp(profile_curve_cap, "profile_curve_cap.stp"); 

// // now, create a surface of revolution around the profile curve profile_curve_cap:
// Handle(Geom_BSplineSurface) nose_ogive_cap_srf
// 	= geoml::revolving_surface(profile_curve_cap,
//                   rot_axis);


}                          