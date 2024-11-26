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
#include <BRepBuilderAPI_MakeVertex.hxx>
#include "GProp_GProps.hxx"
#include "BRepGProp.hxx"
#include <gp_Pnt2d.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeSegment.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>

#include <filesystem>

#include <vector>
#include "BRepTools.hxx" 
#include <gp_GTrsf.hxx>
#include <gp_Mat.hxx>
#include <gp_XYZ.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <gp_Pln.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI_IntCS.hxx>
#include <ShapeAnalysis_Surface.hxx>

#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include "geometry/CCSTCurveBuilder.h"

#include "STEPControl_Writer.hxx"

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

BRepBuilderAPI_GTransform gTransform (back_profile_edge.Edge(), gTrsf, true);

TopoDS_Shape tail_profile_shape = gTransform.Shape();

std::string filename_2 = "tail_profile_edge.brep";
BRepTools::Write(tail_profile_shape, filename_2.c_str());

// create tail fuselage surface

// profile curves
std::vector<Handle(Geom_Curve)> list_of_profiles_tail_fuselage;
list_of_profiles_tail_fuselage.push_back(back_profile);

Standard_Real First;
Standard_Real Last;

TopoDS_Edge tail_profile_edge = TopoDS::Edge(tail_profile_shape); 
Handle(Geom_Curve) tail_profile_curve = BRep_Tool::Curve(tail_profile_edge, First, Last); 

list_of_profiles_tail_fuselage.push_back(tail_profile_curve);

// guide curves
std::vector<Handle(Geom_Curve)> list_of_guides_tail_fuselage;
list_of_guides_tail_fuselage.push_back(tail_upper_guide_curve);
list_of_guides_tail_fuselage.push_back(tail_lower_guide_curve);


Handle(Geom_BSplineSurface) tail_fuselage
	= geoml::interpolate_curve_network(list_of_profiles_tail_fuselage,
                          list_of_guides_tail_fuselage,
                          1.0);
filename = "tail_fuselage.brep";
BRepTools::Write(BRepBuilderAPI_MakeFace(tail_fuselage, Precision::Confusion()), filename.c_str());

// // one complete gordon surface (middle and tail part)

// // upper guide curve
// GeomConvert_CompCurveToBSplineCurve comp_curve_upper(upper_guiding_curve);
// comp_curve_upper.Add(tail_upper_guide_curve, Precision::Confusion());
// Handle(Geom_BSplineCurve) single_upper_guide_curve = comp_curve_upper.BSplineCurve();

// BRepBuilderAPI_MakeEdge single_upper_guide_make_edge (single_upper_guide_curve);

// TopoDS_Edge single_upper_guide_edge = single_upper_guide_make_edge.Edge();

// filename = "single_upper_guide_edge.brep";
// BRepTools::Write(single_upper_guide_edge, filename.c_str());

// // lower guide curve 
// GeomConvert_CompCurveToBSplineCurve comp_curve_lower(lower_guiding_curve);
// comp_curve_lower.Add(tail_lower_guide_curve, Precision::Confusion());
// Handle(Geom_BSplineCurve) single_lower_guide_curve = comp_curve_lower.BSplineCurve();

// BRepBuilderAPI_MakeEdge single_lower_guide_make_edge (single_lower_guide_curve);

// TopoDS_Edge single_lower_guide_edge = single_lower_guide_make_edge.Edge();

// filename = "single_lower_guide_edge.brep";
// BRepTools::Write(single_lower_guide_edge, filename.c_str());

// // create complete gordon surface (middle and tail)
// std::vector<Handle(Geom_Curve)> list_of_profiles_middle_and_tail_fuselage;
// list_of_profiles_middle_and_tail_fuselage.push_back(profile);
// list_of_profiles_middle_and_tail_fuselage.push_back(back_profile);
// list_of_profiles_middle_and_tail_fuselage.push_back(tail_profile_curve);

// std::vector<Handle(Geom_Curve)> list_of_guides_middle_and_tail_fuselage;
// list_of_guides_middle_and_tail_fuselage.push_back(single_upper_guide_curve);
// list_of_guides_middle_and_tail_fuselage.push_back(single_lower_guide_curve);

// Handle(Geom_BSplineSurface) middle_tail_fuselage
// 	= geoml::interpolate_curve_network(list_of_profiles_middle_and_tail_fuselage,
//                           list_of_guides_middle_and_tail_fuselage,
//                           1.0);
// filename = "middle_tail_fuselage.brep";
// BRepTools::Write(BRepBuilderAPI_MakeFace(middle_tail_fuselage, Precision::Confusion()), filename.c_str());

// create two more profile curves for tail fuselage (at positions of P_lower_guide_2 and P_lower_guide_3)

// tail prfile 2

// scaling and translation values

// create a plane through P_lower_guide_2 that is othogonal to the x-achsis
gp_Dir plane_normal(1., 0., 0.);
gp_Pln plane_2(P_lower_guide_2, plane_normal);

Handle(Geom_Plane) geom_plane_2 = new Geom_Plane(plane_2);

// intersect geom_plane_2 with tail_lower_guide_curve
GeomAPI_IntCS int_cs_2(tail_lower_guide_curve, geom_plane_2);

if(int_cs_2.IsDone() && (int_cs_2.NbPoints() == 1))
{
    std::cout << "int_cs_2 succeeded" << std::endl;
}

gp_Pnt intersection_point_2 = int_cs_2.Point(1);

difference_in_z_2 = Abs(tail_upper_guide_control_points.Value(tail_upper_guide_control_points.Upper()).Z() - intersection_point_2.Z());

difference_ratio = difference_in_z_2 / difference_in_z_1;

std::cout << "difference_in_z_1: " << difference_in_z_1 << std::endl; 
std::cout << "difference_in_z_2: " << difference_in_z_2 << std::endl; 

scale_y = difference_ratio;
scale_z = difference_ratio;

move_x = intersection_point_2.X() - back_profile_control_points.Value(1).X();

move_z = (Abs(P_back_1.Z() - P_back_7.Z()) * (1 - difference_ratio)) / 2;
  
gp_GTrsf gTrsf_2 (gp_Mat(scale_x, 0, 0, 0, scale_y, 0, 0, 0, scale_z), gp_XYZ(move_x, 0., move_z));

BRepBuilderAPI_GTransform gTransform_2 (back_profile_edge.Edge(), gTrsf_2, true);

TopoDS_Shape tail_profile_shape_2 = gTransform_2.Shape();

filename_2 = "tail_profile_edge_2.brep";
BRepTools::Write(tail_profile_shape_2, filename_2.c_str());

// tail profile 3

// scaling and translation values

// create a plane through P_lower_guide_3 that is othogonal to the x-axsis
gp_Pln plane_3(P_lower_guide_3, plane_normal);

Handle(Geom_Plane) geom_plane_3 = new Geom_Plane(plane_3);

// intersect geom_plane_3 with tail_lower_guide_curve
GeomAPI_IntCS int_cs_3(tail_lower_guide_curve, geom_plane_3);

if(int_cs_3.IsDone() && (int_cs_3.NbPoints() == 1))
{
    std::cout << "int_cs_3 succeeded" << std::endl;
}

gp_Pnt intersection_point_3 = int_cs_3.Point(1);

Standard_Real difference_in_z_3 = Abs(tail_upper_guide_control_points.Value(tail_upper_guide_control_points.Upper()).Z() - intersection_point_3.Z());

difference_ratio = difference_in_z_3 / difference_in_z_1;

std::cout << "difference_in_z_1: " << difference_in_z_1 << std::endl; 
std::cout << "difference_in_z_3: " << difference_in_z_3 << std::endl; 

scale_y = difference_ratio;
scale_z = difference_ratio;

move_x = intersection_point_3.X() - back_profile_control_points.Value(1).X();

move_z = (Abs(P_back_1.Z() - P_back_7.Z()) * (1 - difference_ratio)) / 2;
  
gp_GTrsf gTrsf_3 (gp_Mat(scale_x, 0, 0, 0, scale_y, 0, 0, 0, scale_z), gp_XYZ(move_x, 0., move_z));

BRepBuilderAPI_GTransform gTransform_3 (back_profile_edge.Edge(), gTrsf_3, true);

TopoDS_Shape tail_profile_shape_3 = gTransform_3.Shape();

filename_2 = "tail_profile_edge_3.brep";
BRepTools::Write(tail_profile_shape_3, filename_2.c_str());

// fuselage tail part (multiple profile curves)

// create gordon surface 

TopoDS_Edge tail_profile_edge_2 = TopoDS::Edge(tail_profile_shape_2); 
Handle(Geom_Curve) tail_profile_curve_2 = BRep_Tool::Curve(tail_profile_edge_2, First, Last); 

TopoDS_Edge tail_profile_edge_3 = TopoDS::Edge(tail_profile_shape_3); 
Handle(Geom_Curve) tail_profile_curve_3 = BRep_Tool::Curve(tail_profile_edge_3, First, Last); 

std::vector<Handle(Geom_Curve)> list_of_profiles_tail_fuselage_multiple_profiles;
list_of_profiles_tail_fuselage_multiple_profiles.push_back(back_profile);
list_of_profiles_tail_fuselage_multiple_profiles.push_back(tail_profile_curve_2);
list_of_profiles_tail_fuselage_multiple_profiles.push_back(tail_profile_curve_3);
list_of_profiles_tail_fuselage_multiple_profiles.push_back(tail_profile_curve);

Handle(Geom_BSplineSurface) tail_fuselage_multiple_profiles
	= geoml::interpolate_curve_network(list_of_profiles_tail_fuselage_multiple_profiles,
                          list_of_guides_tail_fuselage,
                          1.0);
filename = "tail_fuselage_multiple_profiles.brep";
BRepTools::Write(BRepBuilderAPI_MakeFace(tail_fuselage_multiple_profiles, Precision::Confusion()), filename.c_str());

// test continuity between middle and tail fuselage parts

Standard_Real u_min, u_max, v_min, v_max;

middle_fuselage->Bounds(u_min, u_max, v_min, v_max);

std::cout << "u_min: " << u_min << " u_max: " << u_max << " v_min: " << v_min << " v_max: " << v_max << std::endl;

gp_Pnt point_on_middle_fuselage;

middle_fuselage->D0(0.5, v_max, point_on_middle_fuselage);

std::cout << "point_on_middle_fuselage.X(): " << point_on_middle_fuselage.X() << std::endl;
std::cout << "point_on_middle_fuselage.Y(): " << point_on_middle_fuselage.Y() << std::endl;
std::cout << "point_on_middle_fuselage.Z(): " << point_on_middle_fuselage.Z() << std::endl;


//u=0.5, v_max:
// point_on_middle_fuselage.X(): 12500
// point_on_middle_fuselage.Y(): -1725.37
// point_on_middle_fuselage.Z(): -1.56319e-13

//u_min, v_min:
// point_on_middle_fuselage.X(): -4600
// point_on_middle_fuselage.Y(): 0
// point_on_middle_fuselage.Z(): 1950

//u_max, v_min:
// point_on_middle_fuselage.X(): -4600
// point_on_middle_fuselage.Y(): 0
// point_on_middle_fuselage.Z(): -1950

//u_min, v_max:
// point_on_middle_fuselage.X(): 12500
// point_on_middle_fuselage.Y(): 0
// point_on_middle_fuselage.Z(): 1950

//u_max, v_max:
// point_on_middle_fuselage.X(): 12500
// point_on_middle_fuselage.Y(): 0
// point_on_middle_fuselage.Z(): -1950

tail_fuselage_multiple_profiles->Bounds(u_min, u_max, v_min, v_max);

std::cout << "u_min: " << u_min << " u_max: " << u_max << " v_min: " << v_min << " v_max: " << v_max << std::endl;

gp_Pnt point_on_tail_fuselage;

tail_fuselage_multiple_profiles->D0(0.5, v_min, point_on_tail_fuselage);

std::cout << "point_on_tail_fuselage.X(): " << point_on_tail_fuselage.X() << std::endl;
std::cout << "point_on_tail_fuselage.Y(): " << point_on_tail_fuselage.Y() << std::endl;
std::cout << "point_on_tail_fuselage.Z(): " << point_on_tail_fuselage.Z() << std::endl;

//u=0.5, v_min:
// point_on_tail_fuselage.X(): 12500
// point_on_tail_fuselage.Y(): -1725.37
// point_on_tail_fuselage.Z(): -1.42109e-13

//u_min, v_min:
// point_on_tail_fuselage.X(): 12500
// point_on_tail_fuselage.Y(): 0
// point_on_tail_fuselage.Z(): 1950

//u_max, v_min:
// point_on_tail_fuselage.X(): 12500
// point_on_tail_fuselage.Y(): 0
// point_on_tail_fuselage.Z(): -1950

//u_min, v_max:
// point_on_tail_fuselage.X(): 25716
// point_on_tail_fuselage.Y(): 0
// point_on_tail_fuselage.Z(): 1950

//u_max, v_max:
// point_on_tail_fuselage.X(): 25716
// point_on_tail_fuselage.Y(): 0
// point_on_tail_fuselage.Z(): 1200

// analyse the continuity
for(Standard_Real u_step = 0; u_step <= 1.001; u_step += 0.1 )
{
    LocalAnalysis_SurfaceContinuity continuity_check (middle_fuselage, u_step, 1.0, tail_fuselage_multiple_profiles, u_step, 0.0, GeomAbs_G1);

    std::cout << "G1_concontinuity_check: " << continuity_check.IsG1() << std::endl;

    std::cout << "G1Angle: " << continuity_check.G1Angle() << std::endl;
}

std::cout << "tail number of U poles: " << tail_fuselage_multiple_profiles->NbUPoles() << "tail number of V poles: " << tail_fuselage_multiple_profiles->NbVPoles() << std::endl;

// extract control points from tail fuselage part

TopoDS_Compound control_points_compound;

BRep_Builder aBuilder;

aBuilder.MakeCompound(control_points_compound);


for(int i = 1; i <= tail_fuselage_multiple_profiles->NbUPoles(); ++i)
{
    for(int j = 1; j <= tail_fuselage_multiple_profiles->NbVPoles(); ++j)
    {
        BRepBuilderAPI_MakeVertex aVertex(tail_fuselage_multiple_profiles->Pole(i,j));
        aBuilder.Add(control_points_compound, aVertex.Shape());
    }
}

filename = "tail_fuselage_control_points.brep";
BRepTools::Write(control_points_compound, filename.c_str());

TColgp_Array1OfPnt new_set_of_control_points_in_second_column(1, tail_fuselage_multiple_profiles->NbUPoles());

for(int i = 1; i <= tail_fuselage_multiple_profiles->NbUPoles(); ++i)
{
    gp_Pnt a_point = tail_fuselage_multiple_profiles->Pole(i, 2);
    std::cout << "x-coord. of second control point column: " << a_point.X() << std::endl;
    gp_Pnt tmp_point_first_column = tail_fuselage_multiple_profiles->Pole(i, 1);
    tmp_point_first_column.SetX(a_point.X());
    new_set_of_control_points_in_second_column.SetValue(i, tmp_point_first_column);
}

tail_fuselage_multiple_profiles->SetPoleCol(2, new_set_of_control_points_in_second_column);

filename = "tail_fuselage_multiple_profiles_updated_control_points.brep";
BRepTools::Write(BRepBuilderAPI_MakeFace(tail_fuselage_multiple_profiles, Precision::Confusion()), filename.c_str());

// display updated control points
TopoDS_Compound control_points_compound_updated;

BRep_Builder aBuilder_updated;

aBuilder_updated.MakeCompound(control_points_compound_updated);


for(int i = 1; i <= tail_fuselage_multiple_profiles->NbUPoles(); ++i)
{
    for(int j = 1; j <= tail_fuselage_multiple_profiles->NbVPoles(); ++j)
    {
        BRepBuilderAPI_MakeVertex aVertex(tail_fuselage_multiple_profiles->Pole(i,j));
        aBuilder_updated.Add(control_points_compound_updated, aVertex.Shape());
    }
}

filename = "tail_fuselage_control_points_updated.brep";
BRepTools::Write(control_points_compound_updated, filename.c_str());

// analyse the continuity
for(Standard_Real u_step = 0; u_step <= 1.001; u_step += 0.01 )
{
    LocalAnalysis_SurfaceContinuity continuity_check_updated (middle_fuselage, u_step, 1.0, tail_fuselage_multiple_profiles, u_step, 0.0, GeomAbs_G1);

    std::cout << "G1_concontinuity_check_updated: " << continuity_check_updated.IsG1() << std::endl;

    std::cout << "G1Angle: " << continuity_check_updated.G1Angle() << std::endl;
}

// write to step file
STEPControl_Writer writer;
writer.Transfer(BRepBuilderAPI_MakeFace(middle_fuselage, Precision::Confusion()),STEPControl_AsIs);
writer.Transfer(BRepBuilderAPI_MakeFace(tail_fuselage_multiple_profiles, Precision::Confusion()),STEPControl_AsIs);

filename = "middle_and_tail_fuselage_faces.stp";
writer.Write(filename.c_str());

// manual modelling of the nose part

// points for column 1
gp_Pnt leading_point (-11800., 0., 0.);

// points for column 2
Standard_Real z_column_2 = 783;

Standard_Real z_column_2_difference_ratio = z_column_2 / P_1.Z();

gp_Pnt P_1_column_2 (leading_point.X(), P_1.Y() * z_column_2_difference_ratio, P_1.Z() * z_column_2_difference_ratio);
gp_Pnt P_2_column_2 (leading_point.X(), P_2.Y() * z_column_2_difference_ratio, P_2.Z() * z_column_2_difference_ratio);
gp_Pnt P_3_column_2 (leading_point.X(), P_3.Y() * z_column_2_difference_ratio, P_3.Z() * z_column_2_difference_ratio);
gp_Pnt P_4_column_2 (leading_point.X(), P_4.Y() * z_column_2_difference_ratio, P_4.Z() * z_column_2_difference_ratio);
gp_Pnt P_5_column_2 (leading_point.X(), P_5.Y() * z_column_2_difference_ratio, P_5.Z() * z_column_2_difference_ratio);
gp_Pnt P_6_column_2 (leading_point.X(), P_6.Y() * z_column_2_difference_ratio, P_6.Z() * z_column_2_difference_ratio);
gp_Pnt P_7_column_2 (leading_point.X(), P_7.Y() * z_column_2_difference_ratio, P_7.Z() * z_column_2_difference_ratio);

// points for column 3
Standard_Real z_column_3 = 1800;

Standard_Real z_column_3_difference_ratio = z_column_3 / P_1.Z();

Standard_Real x_column_3 = -9050;

gp_Pnt P_1_column_3 (x_column_3, P_1.Y() * z_column_3_difference_ratio, P_1.Z() * z_column_3_difference_ratio);
gp_Pnt P_2_column_3 (x_column_3, P_2.Y() * z_column_3_difference_ratio, P_2.Z() * z_column_3_difference_ratio);
gp_Pnt P_3_column_3 (x_column_3, P_3.Y() * z_column_3_difference_ratio, P_3.Z() * z_column_3_difference_ratio);
gp_Pnt P_4_column_3 (x_column_3, P_4.Y() * z_column_3_difference_ratio, P_4.Z() * z_column_3_difference_ratio);
gp_Pnt P_5_column_3 (x_column_3, P_5.Y() * z_column_3_difference_ratio, P_5.Z() * z_column_3_difference_ratio);
gp_Pnt P_6_column_3 (x_column_3, P_6.Y() * z_column_3_difference_ratio, P_6.Z() * z_column_3_difference_ratio);
gp_Pnt P_7_column_3 (x_column_3, P_7.Y() * z_column_3_difference_ratio, P_7.Z() * z_column_3_difference_ratio);

// points for column 4
Standard_Real x_column_4 = -7480;

gp_Pnt P_1_column_4 (x_column_4, P_1.Y(), P_1.Z());
gp_Pnt P_2_column_4 (x_column_4, P_2.Y(), P_2.Z());
gp_Pnt P_3_column_4 (x_column_4, P_3.Y(), P_3.Z());
gp_Pnt P_4_column_4 (x_column_4, P_4.Y(), P_4.Z());
gp_Pnt P_5_column_4 (x_column_4, P_5.Y(), P_5.Z());
gp_Pnt P_6_column_4 (x_column_4, P_6.Y(), P_6.Z());
gp_Pnt P_7_column_4 (x_column_4, P_7.Y(), P_7.Z());

// create nose part as Bezier surface
TColgp_Array2OfPnt nose_surface_control_points (1,7,1,5);

for(int i = 1; i <= nose_surface_control_points.NbRows(); ++i)
{
    nose_surface_control_points.SetValue(i, 1, leading_point);
}

nose_surface_control_points.SetValue(1, 2, P_1_column_2);
nose_surface_control_points.SetValue(2, 2, P_2_column_2);
nose_surface_control_points.SetValue(3, 2, P_3_column_2);
nose_surface_control_points.SetValue(4, 2, P_4_column_2);
nose_surface_control_points.SetValue(5, 2, P_5_column_2);
nose_surface_control_points.SetValue(6, 2, P_6_column_2);
nose_surface_control_points.SetValue(7, 2, P_7_column_2);

nose_surface_control_points.SetValue(1, 3, P_1_column_3);
nose_surface_control_points.SetValue(2, 3, P_2_column_3);
nose_surface_control_points.SetValue(3, 3, P_3_column_3);
nose_surface_control_points.SetValue(4, 3, P_4_column_3);
nose_surface_control_points.SetValue(5, 3, P_5_column_3);
nose_surface_control_points.SetValue(6, 3, P_6_column_3);
nose_surface_control_points.SetValue(7, 3, P_7_column_3);

nose_surface_control_points.SetValue(1, 4, P_1_column_4);
nose_surface_control_points.SetValue(2, 4, P_2_column_4);
nose_surface_control_points.SetValue(3, 4, P_3_column_4);
nose_surface_control_points.SetValue(4, 4, P_4_column_4);
nose_surface_control_points.SetValue(5, 4, P_5_column_4);
nose_surface_control_points.SetValue(6, 4, P_6_column_4);
nose_surface_control_points.SetValue(7, 4, P_7_column_4);

nose_surface_control_points.SetValue(1, 5, P_1);
nose_surface_control_points.SetValue(2, 5, P_2);
nose_surface_control_points.SetValue(3, 5, P_3);
nose_surface_control_points.SetValue(4, 5, P_4);
nose_surface_control_points.SetValue(5, 5, P_5);
nose_surface_control_points.SetValue(6, 5, P_6);
nose_surface_control_points.SetValue(7, 5, P_7);

Handle(Geom_BezierSurface) nose_surface = new Geom_BezierSurface(nose_surface_control_points); 

filename = "nose_surface.brep";
BRepTools::Write(BRepBuilderAPI_MakeFace(nose_surface, Precision::Confusion()), filename.c_str());

// testing coninutiy between nose and middle fuselage
Standard_Integer number_of_testing_points = 101;

TColgp_Array1OfPnt testing_points_on_nose_surface(1, number_of_testing_points);

nose_surface->Bounds(u_min, u_max, v_min, v_max);

Standard_Real v_max_nose_surface = v_max;

TColStd_Array1OfReal u_steps_testing_points_nose_fuselage (1, number_of_testing_points);

for(Standard_Integer i = 1; i <= number_of_testing_points - 1; ++i)
{
    gp_Pnt testing_point;
    Standard_Real u_step = u_min + (i - 1) * (u_max - u_min) / Standard_Real(number_of_testing_points - 1);

    nose_surface->D0(u_step, v_max_nose_surface, testing_point);
    testing_points_on_nose_surface.SetValue(i, testing_point);

    u_steps_testing_points_nose_fuselage.SetValue(i, u_step);

    std::cout << "u steps: " << u_step << "point coordinates: " 
    << testing_point.X() << ", " << testing_point.Y() << ", " << testing_point.Z() << std::endl;
}

gp_Pnt testing_point;
nose_surface->D0(u_max, v_max_nose_surface, testing_point);
testing_points_on_nose_surface.SetValue(number_of_testing_points, testing_point);

u_steps_testing_points_nose_fuselage.SetValue(number_of_testing_points, u_max);

std::cout << "last u step: " << u_max << "point coordinates: " 
    << testing_point.X() << ", " << testing_point.Y() << ", " << testing_point.Z() << std::endl;

TColStd_Array1OfReal u_steps_testing_points_middle_fuselage (1, number_of_testing_points);

ShapeAnalysis_Surface shape_analysis_middle_fuselage (middle_fuselage);

for(Standard_Integer i = 1; i <= number_of_testing_points; ++i)
{
    gp_Pnt2d projected_point = shape_analysis_middle_fuselage.ValueOfUV(testing_points_on_nose_surface.Value(i), Precision::Confusion());
    u_steps_testing_points_middle_fuselage.SetValue(i, projected_point.X());
    std::cout << "projected point u value: " << u_steps_testing_points_middle_fuselage.Value(i) << std::endl;
}

// analyse the continuity
for(Standard_Integer i = 1; i <= number_of_testing_points; ++i)
{
    LocalAnalysis_SurfaceContinuity continuity_check (nose_surface, u_steps_testing_points_nose_fuselage.Value(i), v_max_nose_surface, 
        middle_fuselage, u_steps_testing_points_middle_fuselage.Value(i), 0.0, GeomAbs_C0);

    gp_Pnt testing_point_nose_surface;
    gp_Pnt testing_point_middle_fuselage;

    nose_surface->D0(u_steps_testing_points_nose_fuselage.Value(i), v_max_nose_surface, testing_point_nose_surface);
    middle_fuselage->D0(u_steps_testing_points_middle_fuselage.Value(i), 0.0, testing_point_middle_fuselage);

    std::cout << "G1_concontinuity_check: " << continuity_check.IsC0() << " difference in X: " << Abs(testing_point_nose_surface.X() - testing_point_middle_fuselage.X())
        << " difference in Y: " << Abs(testing_point_nose_surface.Y() - testing_point_middle_fuselage.Y())
        << " difference in Z: " << Abs(testing_point_nose_surface.Z() - testing_point_middle_fuselage.Z()) << std::endl;

    std::cout << "G1Angle: " << continuity_check.G1Angle() << std::endl;
}

// test sewing nose and miggle fuselage with different parametrizations

BRepBuilderAPI_Sewing sew;

TopoDS_Face nose_face = BRepBuilderAPI_MakeFace(nose_surface, Precision::Confusion());
TopoDS_Face middle_fuselage_face = BRepBuilderAPI_MakeFace(middle_fuselage, Precision::Confusion());

sew.Add(nose_face);
sew.Add(middle_fuselage_face);

sew.Perform();

TopoDS_Shape resulting_shape = sew.SewedShape();

std::cout << "Result of Sewing (number of degenerated shapes: " << sew.NbDegeneratedShapes() << std::endl;

filename = "sewed_nose_and_middle_fuselage.brep";
BRepTools::Write(resulting_shape, filename.c_str());

TopExp_Explorer face_explorer(resulting_shape, TopAbs_FACE);

Standard_Integer face_counter = 0;

while(face_explorer.More())
{
    TopoDS_Face aFace = TopoDS::Face(face_explorer.Current());
    Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace); 

    if(aSurface->DynamicType() == STANDARD_TYPE(Geom_BezierSurface))
    {
        std::cout << "Bezier Surface found, face_counter = " << face_counter << std::endl; 
        Handle(Geom_BezierSurface) aBezierSurface = Handle(Geom_BezierSurface)::DownCast(aSurface);

        std::cout << "Number of U poles: " << aBezierSurface->NbUPoles() << " U degree: " << aBezierSurface->UDegree() << std::endl;
        std::cout << "Number of V poles: " << aBezierSurface->NbVPoles() << " V degree: " << aBezierSurface->VDegree() << std::endl;
    }
    else if(aSurface->DynamicType() == STANDARD_TYPE(Geom_BSplineSurface))
    {
        std::cout << "BSpline Surface found, face_counter = " << face_counter << std::endl; 
        Handle(Geom_BSplineSurface) aBSplineSurface = Handle(Geom_BSplineSurface)::DownCast(aSurface);

        std::cout << "Number of U poles: " << aBSplineSurface->NbUPoles() << " U degree: " << aBSplineSurface->UDegree() << std::endl;
        std::cout << "Number of V poles: " << aBSplineSurface->NbVPoles() << " V degree: " << aBSplineSurface->VDegree() << std::endl;
    }

    face_counter++;
    face_explorer.Next();
}

} 

TEST(Test_nurbs_surface, bezier_fuselage)
{  

// design parameters

Standard_Real x_position_of_middle_fuselage_front_profile = -4600.;
Standard_Real x_position_of_nose_tip = -11800.;
Standard_Real x_position_of_middle_fuselage_back_profile = 12500.;
Standard_Real x_position_of_tail_back_profile = 25716.;

Standard_Real x_difference_tail_column_1_2 = 1500.;
Standard_Real x_difference_tail_column_2_3 = 1500.;

// middle fuselage front profile points

gp_Pnt P_1 (x_position_of_middle_fuselage_front_profile, 0., 1950.);
gp_Pnt P_2 (x_position_of_middle_fuselage_front_profile, -1076.95526217, 1950.);
gp_Pnt P_3 (x_position_of_middle_fuselage_front_profile, -1950., 1076.95526217);
gp_Pnt P_4 (x_position_of_middle_fuselage_front_profile, -1950., 0.);
gp_Pnt P_5 (x_position_of_middle_fuselage_front_profile, -1950., -1076.95526217);
gp_Pnt P_6 (x_position_of_middle_fuselage_front_profile, -1076.95526217, -1950.);
gp_Pnt P_7 (x_position_of_middle_fuselage_front_profile, 0., -1950.);

// points for column 1
gp_Pnt leading_point (x_position_of_nose_tip, 0., 0.);

// points for column 2
Standard_Real z_column_2 = 783; // taken from power point slide

Standard_Real z_column_2_difference_ratio = z_column_2 / P_1.Z();

gp_Pnt P_1_column_2 (leading_point.X(), P_1.Y() * z_column_2_difference_ratio, P_1.Z() * z_column_2_difference_ratio);
gp_Pnt P_2_column_2 (leading_point.X(), P_2.Y() * z_column_2_difference_ratio, P_2.Z() * z_column_2_difference_ratio);
gp_Pnt P_3_column_2 (leading_point.X(), P_3.Y() * z_column_2_difference_ratio, P_3.Z() * z_column_2_difference_ratio);
gp_Pnt P_4_column_2 (leading_point.X(), P_4.Y() * z_column_2_difference_ratio, P_4.Z() * z_column_2_difference_ratio);
gp_Pnt P_5_column_2 (leading_point.X(), P_5.Y() * z_column_2_difference_ratio, P_5.Z() * z_column_2_difference_ratio);
gp_Pnt P_6_column_2 (leading_point.X(), P_6.Y() * z_column_2_difference_ratio, P_6.Z() * z_column_2_difference_ratio);
gp_Pnt P_7_column_2 (leading_point.X(), P_7.Y() * z_column_2_difference_ratio, P_7.Z() * z_column_2_difference_ratio);

// points for column 3
Standard_Real z_column_3 = 1800; // taken from power point slide

Standard_Real z_column_3_difference_ratio = z_column_3 / P_1.Z();

Standard_Real x_column_3 = -9050; // taken from power point slide

gp_Pnt P_1_column_3 (x_column_3, P_1.Y() * z_column_3_difference_ratio, P_1.Z() * z_column_3_difference_ratio);
gp_Pnt P_2_column_3 (x_column_3, P_2.Y() * z_column_3_difference_ratio, P_2.Z() * z_column_3_difference_ratio);
gp_Pnt P_3_column_3 (x_column_3, P_3.Y() * z_column_3_difference_ratio, P_3.Z() * z_column_3_difference_ratio);
gp_Pnt P_4_column_3 (x_column_3, P_4.Y() * z_column_3_difference_ratio, P_4.Z() * z_column_3_difference_ratio);
gp_Pnt P_5_column_3 (x_column_3, P_5.Y() * z_column_3_difference_ratio, P_5.Z() * z_column_3_difference_ratio);
gp_Pnt P_6_column_3 (x_column_3, P_6.Y() * z_column_3_difference_ratio, P_6.Z() * z_column_3_difference_ratio);
gp_Pnt P_7_column_3 (x_column_3, P_7.Y() * z_column_3_difference_ratio, P_7.Z() * z_column_3_difference_ratio);

// points for column 4
Standard_Real x_column_4 = -7480; // taken from power point slide

gp_Pnt P_1_column_4 (x_column_4, P_1.Y(), P_1.Z());
gp_Pnt P_2_column_4 (x_column_4, P_2.Y(), P_2.Z());
gp_Pnt P_3_column_4 (x_column_4, P_3.Y(), P_3.Z());
gp_Pnt P_4_column_4 (x_column_4, P_4.Y(), P_4.Z());
gp_Pnt P_5_column_4 (x_column_4, P_5.Y(), P_5.Z());
gp_Pnt P_6_column_4 (x_column_4, P_6.Y(), P_6.Z());
gp_Pnt P_7_column_4 (x_column_4, P_7.Y(), P_7.Z());

// create nose part as Bezier surface
TColgp_Array2OfPnt nose_surface_control_points (1,7,1,5);

for(int i = 1; i <= nose_surface_control_points.NbRows(); ++i)
{
    nose_surface_control_points.SetValue(i, 1, leading_point);
}

nose_surface_control_points.SetValue(1, 2, P_1_column_2);
nose_surface_control_points.SetValue(2, 2, P_2_column_2);
nose_surface_control_points.SetValue(3, 2, P_3_column_2);
nose_surface_control_points.SetValue(4, 2, P_4_column_2);
nose_surface_control_points.SetValue(5, 2, P_5_column_2);
nose_surface_control_points.SetValue(6, 2, P_6_column_2);
nose_surface_control_points.SetValue(7, 2, P_7_column_2);

nose_surface_control_points.SetValue(1, 3, P_1_column_3);
nose_surface_control_points.SetValue(2, 3, P_2_column_3);
nose_surface_control_points.SetValue(3, 3, P_3_column_3);
nose_surface_control_points.SetValue(4, 3, P_4_column_3);
nose_surface_control_points.SetValue(5, 3, P_5_column_3);
nose_surface_control_points.SetValue(6, 3, P_6_column_3);
nose_surface_control_points.SetValue(7, 3, P_7_column_3);

nose_surface_control_points.SetValue(1, 4, P_1_column_4);
nose_surface_control_points.SetValue(2, 4, P_2_column_4);
nose_surface_control_points.SetValue(3, 4, P_3_column_4);
nose_surface_control_points.SetValue(4, 4, P_4_column_4);
nose_surface_control_points.SetValue(5, 4, P_5_column_4);
nose_surface_control_points.SetValue(6, 4, P_6_column_4);
nose_surface_control_points.SetValue(7, 4, P_7_column_4);

nose_surface_control_points.SetValue(1, 5, P_1);
nose_surface_control_points.SetValue(2, 5, P_2);
nose_surface_control_points.SetValue(3, 5, P_3);
nose_surface_control_points.SetValue(4, 5, P_4);
nose_surface_control_points.SetValue(5, 5, P_5);
nose_surface_control_points.SetValue(6, 5, P_6);
nose_surface_control_points.SetValue(7, 5, P_7);

Handle(Geom_BezierSurface) nose_surface = new Geom_BezierSurface(nose_surface_control_points); 

std::string filename = "Bezier_nose_surface.brep";
BRepTools::Write(BRepBuilderAPI_MakeFace(nose_surface, Precision::Confusion()), filename.c_str());

// middle fuselage

// middle fuselage back profile points

gp_Pnt P_back_1 (x_position_of_middle_fuselage_back_profile, P_1.Y(), P_1.Z());
gp_Pnt P_back_2 (x_position_of_middle_fuselage_back_profile, P_2.Y(), P_2.Z());
gp_Pnt P_back_3 (x_position_of_middle_fuselage_back_profile, P_3.Y(), P_3.Z());
gp_Pnt P_back_4 (x_position_of_middle_fuselage_back_profile, P_4.Y(), P_4.Z());
gp_Pnt P_back_5 (x_position_of_middle_fuselage_back_profile, P_5.Y(), P_5.Z());
gp_Pnt P_back_6 (x_position_of_middle_fuselage_back_profile, P_6.Y(), P_6.Z());
gp_Pnt P_back_7 (x_position_of_middle_fuselage_back_profile, P_7.Y(), P_7.Z());

// create middle part as Bezier surface
TColgp_Array2OfPnt middle_fuselage_surface_control_points (1,7,1,2);

middle_fuselage_surface_control_points.SetValue(1, 1, P_1);
middle_fuselage_surface_control_points.SetValue(2, 1, P_2);
middle_fuselage_surface_control_points.SetValue(3, 1, P_3);
middle_fuselage_surface_control_points.SetValue(4, 1, P_4);
middle_fuselage_surface_control_points.SetValue(5, 1, P_5);
middle_fuselage_surface_control_points.SetValue(6, 1, P_6);
middle_fuselage_surface_control_points.SetValue(7, 1, P_7);

middle_fuselage_surface_control_points.SetValue(1, 2, P_back_1);
middle_fuselage_surface_control_points.SetValue(2, 2, P_back_2);
middle_fuselage_surface_control_points.SetValue(3, 2, P_back_3);
middle_fuselage_surface_control_points.SetValue(4, 2, P_back_4);
middle_fuselage_surface_control_points.SetValue(5, 2, P_back_5);
middle_fuselage_surface_control_points.SetValue(6, 2, P_back_6);
middle_fuselage_surface_control_points.SetValue(7, 2, P_back_7);


Handle(Geom_BezierSurface) middle_fuselage_surface = new Geom_BezierSurface(middle_fuselage_surface_control_points); 

filename = "Bezier_middle_fuselage_surface.brep";
BRepTools::Write(BRepBuilderAPI_MakeFace(middle_fuselage_surface, Precision::Confusion()), filename.c_str());

// tail part

// tail part profile points

gp_Pnt P_1_tail_column_2 (P_back_1.X() + x_difference_tail_column_1_2, P_1.Y(), P_1.Z());
gp_Pnt P_2_tail_column_2 (P_back_2.X() + x_difference_tail_column_1_2, P_2.Y(), P_2.Z());
gp_Pnt P_3_tail_column_2 (P_back_3.X() + x_difference_tail_column_1_2, P_3.Y(), P_3.Z());
gp_Pnt P_4_tail_column_2 (P_back_4.X() + x_difference_tail_column_1_2, P_4.Y(), P_4.Z());
gp_Pnt P_5_tail_column_2 (P_back_5.X() + x_difference_tail_column_1_2, P_5.Y(), P_5.Z());
gp_Pnt P_6_tail_column_2 (P_back_6.X() + x_difference_tail_column_1_2, P_6.Y(), P_6.Z());
gp_Pnt P_7_tail_column_2 (P_back_7.X() + x_difference_tail_column_1_2, P_7.Y(), P_7.Z());

gp_Pnt P_1_tail_column_3 (P_1_tail_column_2.X() + x_difference_tail_column_2_3, P_1.Y(), P_1.Z());
gp_Pnt P_2_tail_column_3 (P_2_tail_column_2.X() + x_difference_tail_column_2_3, P_2.Y(), P_2.Z());
gp_Pnt P_3_tail_column_3 (P_3_tail_column_2.X() + x_difference_tail_column_2_3, P_3.Y(), P_3.Z());
gp_Pnt P_4_tail_column_3 (P_4_tail_column_2.X() + x_difference_tail_column_2_3, P_4.Y(), P_4.Z());
gp_Pnt P_5_tail_column_3 (P_5_tail_column_2.X() + x_difference_tail_column_2_3, P_5.Y(), P_5.Z());
gp_Pnt P_6_tail_column_3 (P_6_tail_column_2.X() + x_difference_tail_column_2_3, P_6.Y(), P_6.Z());
gp_Pnt P_7_tail_column_3 (P_7_tail_column_2.X() + x_difference_tail_column_2_3, P_7.Y(), P_7.Z());

Standard_Real tail_z_column_4_difference_ratio = (P_1.Z() - 1200.)  / (P_1.Z() - P_7.Z()); // as in power point slides
Standard_Real tail_z_column_4_translation = P_1.Z() - P_1.Z() * tail_z_column_4_difference_ratio;

gp_Pnt P_1_tail_column_4 (x_position_of_tail_back_profile, P_1.Y() * tail_z_column_4_difference_ratio, P_1.Z() * tail_z_column_4_difference_ratio + tail_z_column_4_translation);
gp_Pnt P_2_tail_column_4 (x_position_of_tail_back_profile, P_2.Y() * tail_z_column_4_difference_ratio, P_2.Z() * tail_z_column_4_difference_ratio + tail_z_column_4_translation);
gp_Pnt P_3_tail_column_4 (x_position_of_tail_back_profile, P_3.Y() * tail_z_column_4_difference_ratio, P_3.Z() * tail_z_column_4_difference_ratio + tail_z_column_4_translation);
gp_Pnt P_4_tail_column_4 (x_position_of_tail_back_profile, P_4.Y() * tail_z_column_4_difference_ratio, P_4.Z() * tail_z_column_4_difference_ratio + tail_z_column_4_translation);
gp_Pnt P_5_tail_column_4 (x_position_of_tail_back_profile, P_5.Y() * tail_z_column_4_difference_ratio, P_5.Z() * tail_z_column_4_difference_ratio + tail_z_column_4_translation);
gp_Pnt P_6_tail_column_4 (x_position_of_tail_back_profile, P_6.Y() * tail_z_column_4_difference_ratio, P_6.Z() * tail_z_column_4_difference_ratio + tail_z_column_4_translation);
gp_Pnt P_7_tail_column_4 (x_position_of_tail_back_profile, P_7.Y() * tail_z_column_4_difference_ratio, P_7.Z() * tail_z_column_4_difference_ratio + tail_z_column_4_translation);

// create tail part as Bezier surface
TColgp_Array2OfPnt tail_part_surface_control_points (1,7,1,4);

tail_part_surface_control_points.SetValue(1, 1, P_back_1);
tail_part_surface_control_points.SetValue(2, 1, P_back_2);
tail_part_surface_control_points.SetValue(3, 1, P_back_3);
tail_part_surface_control_points.SetValue(4, 1, P_back_4);
tail_part_surface_control_points.SetValue(5, 1, P_back_5);
tail_part_surface_control_points.SetValue(6, 1, P_back_6);
tail_part_surface_control_points.SetValue(7, 1, P_back_7);

tail_part_surface_control_points.SetValue(1, 2, P_1_tail_column_2);
tail_part_surface_control_points.SetValue(2, 2, P_2_tail_column_2);
tail_part_surface_control_points.SetValue(3, 2, P_3_tail_column_2);
tail_part_surface_control_points.SetValue(4, 2, P_4_tail_column_2);
tail_part_surface_control_points.SetValue(5, 2, P_5_tail_column_2);
tail_part_surface_control_points.SetValue(6, 2, P_6_tail_column_2);
tail_part_surface_control_points.SetValue(7, 2, P_7_tail_column_2);

tail_part_surface_control_points.SetValue(1, 3, P_1_tail_column_3);
tail_part_surface_control_points.SetValue(2, 3, P_2_tail_column_3);
tail_part_surface_control_points.SetValue(3, 3, P_3_tail_column_3);
tail_part_surface_control_points.SetValue(4, 3, P_4_tail_column_3);
tail_part_surface_control_points.SetValue(5, 3, P_5_tail_column_3);
tail_part_surface_control_points.SetValue(6, 3, P_6_tail_column_3);
tail_part_surface_control_points.SetValue(7, 3, P_7_tail_column_3);

tail_part_surface_control_points.SetValue(1, 4, P_1_tail_column_4);
tail_part_surface_control_points.SetValue(2, 4, P_2_tail_column_4);
tail_part_surface_control_points.SetValue(3, 4, P_3_tail_column_4);
tail_part_surface_control_points.SetValue(4, 4, P_4_tail_column_4);
tail_part_surface_control_points.SetValue(5, 4, P_5_tail_column_4);
tail_part_surface_control_points.SetValue(6, 4, P_6_tail_column_4);
tail_part_surface_control_points.SetValue(7, 4, P_7_tail_column_4);


Handle(Geom_BezierSurface) tail_part_surface = new Geom_BezierSurface(tail_part_surface_control_points); 

filename = "Bezier_tail_part_surface.brep";
BRepTools::Write(BRepBuilderAPI_MakeFace(tail_part_surface, Precision::Confusion()), filename.c_str());

// close tail part

TColgp_Array1OfPnt closed_tail_curve_1_control_points (1, 7);

closed_tail_curve_1_control_points.SetValue(1, P_1_tail_column_4);
closed_tail_curve_1_control_points.SetValue(2, P_2_tail_column_4);
closed_tail_curve_1_control_points.SetValue(3, P_3_tail_column_4);
closed_tail_curve_1_control_points.SetValue(4, P_4_tail_column_4);
closed_tail_curve_1_control_points.SetValue(5, P_5_tail_column_4);
closed_tail_curve_1_control_points.SetValue(6, P_6_tail_column_4);
closed_tail_curve_1_control_points.SetValue(7, P_7_tail_column_4);

Handle(Geom_BezierCurve) closed_tail_curve_1 = new Geom_BezierCurve(closed_tail_curve_1_control_points);
Handle(Geom_TrimmedCurve) closed_tail_curve_2 = GC_MakeSegment(P_7_tail_column_4, P_1_tail_column_4);

TopoDS_Edge closed_tail_edge_1 = BRepBuilderAPI_MakeEdge(closed_tail_curve_1); 
TopoDS_Edge closed_tail_edge_2 = BRepBuilderAPI_MakeEdge(closed_tail_curve_2); 

TopoDS_Wire closed_tail_wire = BRepBuilderAPI_MakeWire(closed_tail_edge_1, closed_tail_edge_2); 

TopoDS_Face closed_tail_face = BRepBuilderAPI_MakeFace(closed_tail_wire);

Handle(Geom_Surface) closed_tail_surface = BRep_Tool::Surface(closed_tail_face); 

if(closed_tail_surface->DynamicType() == STANDARD_TYPE(Geom_BezierSurface))
{
    std::cout << "Bezier Surface found" << std::endl; 
    Handle(Geom_BezierSurface) aBezierSurface = Handle(Geom_BezierSurface)::DownCast(closed_tail_surface);

    std::cout << "Number of U poles: " << aBezierSurface->NbUPoles() << " U degree: " << aBezierSurface->UDegree() << std::endl;
    std::cout << "Number of V poles: " << aBezierSurface->NbVPoles() << " V degree: " << aBezierSurface->VDegree() << std::endl;
}
else if(closed_tail_surface->DynamicType() == STANDARD_TYPE(Geom_BSplineSurface))
{
    std::cout << "BSpline Surface found" << std::endl; 
    Handle(Geom_BSplineSurface) aBSplineSurface = Handle(Geom_BSplineSurface)::DownCast(closed_tail_surface);

    std::cout << "Number of U poles: " << aBSplineSurface->NbUPoles() << " U degree: " << aBSplineSurface->UDegree() << std::endl;
    std::cout << "Number of V poles: " << aBSplineSurface->NbVPoles() << " V degree: " << aBSplineSurface->VDegree() << std::endl;
}
else if(closed_tail_surface->DynamicType() == STANDARD_TYPE(Geom_Plane))
{
    std::cout << "Plane found" << std::endl; 
    Handle(Geom_Plane) aPlaneSurface = Handle(Geom_Plane)::DownCast(closed_tail_surface);
}

// sewing of three faces

BRepBuilderAPI_Sewing sew;

TopoDS_Face nose_face = BRepBuilderAPI_MakeFace(nose_surface, Precision::Confusion());
TopoDS_Face middle_fuselage_face = BRepBuilderAPI_MakeFace(middle_fuselage_surface, Precision::Confusion());
TopoDS_Face tail_part_face = BRepBuilderAPI_MakeFace(tail_part_surface, Precision::Confusion());

sew.Add(nose_face);
sew.Add(middle_fuselage_face);
sew.Add(tail_part_face);
sew.Add(closed_tail_face);

sew.Perform();

TopoDS_Shape resulting_shape = sew.SewedShape();

std::cout << "Result of Sewing (number of degenerated shapes: " << sew.NbDegeneratedShapes() << std::endl;

filename = "Bezier_fuselage_geometry_complete.brep";
BRepTools::Write(resulting_shape, filename.c_str());

// write to step file
STEPControl_Writer writer;
writer.Transfer(resulting_shape, STEPControl_AsIs);

filename = "Bezier_fuselage_geometry_complete.stp";
writer.Write(filename.c_str());

// now define a CST parametrization

Standard_Real N_1 = 0.5;
Standard_Real N_2 = 1.0;

std::vector<Standard_Real> upper_cst_b {0.11809019, 0.18951797, 0.20255648};
Standard_Real relative_te_thickness = 0.005;

geoml::CCSTCurveBuilder upper_cst_curve_builder (N_1, N_2, upper_cst_b, relative_te_thickness / 2);

Handle(Geom_BSplineCurve) upper_cst = upper_cst_curve_builder.Curve();

BRepBuilderAPI_MakeEdge upper_cst_edge_builder (upper_cst);

TopoDS_Edge upper_cst_edge = upper_cst_edge_builder.Edge();

filename = "upper_cst_edge.brep";
BRepTools::Write(upper_cst_edge, filename.c_str());

std::vector<Standard_Real> lower_cst_b {-0.11809019, -0.23641964, 0.03463958};

geoml::CCSTCurveBuilder lower_cst_curve_builder (N_1, N_2, lower_cst_b, -1 * (relative_te_thickness / 2));

Handle(Geom_BSplineCurve) lower_cst = lower_cst_curve_builder.Curve();

BRepBuilderAPI_MakeEdge lower_cst_edge_builder (lower_cst);

TopoDS_Edge lower_cst_edge = lower_cst_edge_builder.Edge();

filename = "lower_cst_edge.brep";
BRepTools::Write(lower_cst_edge, filename.c_str());




}