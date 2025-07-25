#include <geoml/surfaces/surfaces.h>
#include "geoml/utilities/utilities.h"
#include <geoml/data_structures/Array2d.h>
#include <geoml/predicates/predicate_functions.h>
#include <geoml/boolean_ops/modeling.hpp>

#include <gtest/gtest.h>

#include <gp_Pnt.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include "TopoDS.hxx"
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>

#include <vector>
#include <cmath>

TEST(Test_extract_control_points_from_surface, extract_control_points_from_surface)
{    

// define a B-spline surface    

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

geoml::Array2d<gp_Pnt> extracted_points(3,2);

extracted_points = geoml::extract_control_points_surface(test_surface);

for(int i = 0; i < control_points.rowLength(); ++i)
{
    for(int j = 0; j < control_points.colLength(); ++j)
    {
        EXPECT_EQ(extracted_points.at(i,j).X(), control_points.at(i,j).X());
        EXPECT_EQ(extracted_points.at(i,j).Y(), control_points.at(i,j).Y());
        EXPECT_EQ(extracted_points.at(i,j).Z(), control_points.at(i,j).Z());
    }
}

// test the extraction of rows and cols of control points of a surface:
EXPECT_EQ(extracted_points.getRow(2).size(), 2);

EXPECT_EQ(test_surface->NbUPoles(), 3); // this together with the previous line shows that poles in U-direction are aranged in a column of extracted_points 

EXPECT_EQ(geoml::extract_control_point_vector_in_U_direction(test_surface,1).size(), 3); 
EXPECT_EQ(geoml::extract_control_point_vector_in_V_direction(test_surface,1).size(), 2); 

EXPECT_NEAR(geoml::extract_control_point_vector_in_U_direction(test_surface,0).at(2).Y(), 2., 1e-5);
EXPECT_NEAR(geoml::extract_control_point_vector_in_U_direction(test_surface,1).at(2).Y(), 2., 1e-5);
EXPECT_NEAR(geoml::extract_control_point_vector_in_V_direction(test_surface,0).at(1).Y(), 0., 1e-5);
EXPECT_NEAR(geoml::extract_control_point_vector_in_U_direction(test_surface,1).at(0).Y(), 0., 1e-5);

}

geoml::ShapePredicate is_near_ref_point(gp_Pnt ref_point, double tolerance)
{
    return [=](geoml::Shape const& s)
    { 
        TopoDS_Vertex vert = TopoDS::Vertex(s);
        gp_Pnt vert_point = BRep_Tool::Pnt(vert);

        return vert_point.SquareDistance(ref_point) < tolerance * tolerance; 
    };
}

TEST(Test_make_fillet, simple_make_fillet_test)
{  
    // first we create a box
    using namespace geoml;

    gp_Pnt corner_1 (0.0, 0.0, 0.0); 
    gp_Pnt corner_2 (1.0, 2.0, 3.0); 

    BRepPrimAPI_MakeBox box_maker (corner_1, corner_2);

    Shape my_box = box_maker.Solid();

    // define the following point in order to select the edges that have a vertex near it
    gp_Pnt ref_point (1., 2. , 3.);

    // select the edges accordingly
    Shape edges_1 = my_box.select_subshapes(is_edge).filter(has_subshape_that(is_vertex && is_near_ref_point(ref_point, 1e-5))); 

    // test if the number of edges due to the selection is as expected
    EXPECT_EQ(edges_1.size(), 3);

    // we don't expect an exception to be thrown by running the following code
    EXPECT_NO_THROW({ Shape filleted_box = make_fillet(my_box, edges_1, 0.15); });

    // create a filleted box
    Shape filleted_box = make_fillet(my_box, edges_1, 0.15);

    // now, we create three sample points in order to check (for one edge of the fillet surface, if the shape lie as expected on a circular arc. 
    // the coordinates of the sample points are extracted from another CAD tool
    gp_Pnt sample_point_1 (0.937, 1.937, 2.937);
    gp_Pnt sample_point_2 (0.956, 0.000, 2.956);
    gp_Pnt sample_point_3 (0.000, 1.956, 2.956);

    // make vertices out of the sample points
    BRepBuilderAPI_MakeVertex vertex_builder_1 (sample_point_1);
    BRepBuilderAPI_MakeVertex vertex_builder_2 (sample_point_2);
    BRepBuilderAPI_MakeVertex vertex_builder_3 (sample_point_3);

    TopoDS_Vertex sample_vertex_1 = vertex_builder_1.Vertex();
    TopoDS_Vertex sample_vertex_2 = vertex_builder_2.Vertex();
    TopoDS_Vertex sample_vertex_3 = vertex_builder_3.Vertex();

    // measure the distance of the sample vertices to the filleted box
    BRepExtrema_DistShapeShape dist_tool_1 (filleted_box, sample_vertex_1);
    dist_tool_1.Perform();
    Standard_Real sample_distance_1 = dist_tool_1.Value();

    BRepExtrema_DistShapeShape dist_tool_2 (filleted_box, sample_vertex_2);
    dist_tool_2.Perform();
    Standard_Real sample_distance_2 = dist_tool_2.Value();

    BRepExtrema_DistShapeShape dist_tool_3 (filleted_box, sample_vertex_3);
    dist_tool_3.Perform();
    Standard_Real sample_distance_3 = dist_tool_3.Value();
 
    // check if the distance is small enough (ideally zero), to confirm, that the tests shape is as expected
    EXPECT_TRUE(sample_distance_1 < 1e-3);
    EXPECT_TRUE(sample_distance_2 < 1e-3);
    EXPECT_TRUE(sample_distance_3 < 1e-3);

    // here, contrary to the previous selection, we select such subshapes that are faces
    Shape edges_2 = my_box.select_subshapes(is_vertex).filter(has_subshape_that(is_near_ref_point(ref_point, 1e-5))); 

    // we expect the program to throw an exception in the case that edges_2 contain a shape that is not an edge (as in this case)
    EXPECT_THROW({ Shape filleted_box = make_fillet(my_box, edges_2, 0.15); }, geoml::Error);

    // now, we want to test the history tracking functionality of make_fillet via the previously created filleted_box

    // select some shapes of my_box first
    Shape selected_face = my_box.select_subshapes(is_face).filter(
        has_subshape_that(is_vertex && is_near_ref_point(gp_Pnt(0. ,0. , 3.), 1e-5)) &&
        !has_subshape_that(is_vertex && is_near_ref_point(gp_Pnt(0., 0., 0.), 1e-5))
    ).unique_element();

    EXPECT_EQ(TopoDS_Shape(selected_face).ShapeType(), TopAbs_FACE);

    TopoDS_Face face_sel = TopoDS::Face(selected_face);

    // now, get a decendent of selected_face
    Shape des_of_selected_face = filleted_box.select_subshapes(is_modified_descendent_of(selected_face)).unique_element();

    // consider the boolean cut of selected_face with des_of_seledted_face, to create a stronger test
    Shape cut_face = boolean_subtract(selected_face, des_of_selected_face);

    // compute the areas of the different faces
    GProp_GProps surfaceProperties_selected_face;
    BRepGProp::SurfaceProperties(selected_face, surfaceProperties_selected_face);
    double area_selected_face = surfaceProperties_selected_face.Mass();

    GProp_GProps surfaceProperties_des_of_selected_face;
    BRepGProp::SurfaceProperties(des_of_selected_face, surfaceProperties_des_of_selected_face);
    double area_des_of_selected_face = surfaceProperties_des_of_selected_face.Mass();

    GProp_GProps surfaceProperties_cut_face;
    BRepGProp::SurfaceProperties(cut_face, surfaceProperties_cut_face);
    double area_cut_face = surfaceProperties_cut_face.Mass();

    EXPECT_NEAR(area_cut_face, area_selected_face - area_des_of_selected_face, 1e-5);

    // make sure that area_cut_face is not zero
    EXPECT_NEAR(area_selected_face - area_des_of_selected_face, 0.4275, 1e-3);
}