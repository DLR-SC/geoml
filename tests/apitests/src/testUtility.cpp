#include <geoml/surfaces/modeling.h>
#include "geoml/utility/utility.h"
#include <geoml/data_structures/Array2d.h>

#include <gtest/gtest.h>

#include <gp_Pnt.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <vector>




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

TEST(Test_make_fillet, simple_make_fillet_multiple_indices_test)
{  
// create a box  
    gp_Pnt corner_1 (0.0, 0.0, 0.0); 
    gp_Pnt corner_2 (1.0, 2.0, 3.0); 

    BRepPrimAPI_MakeBox box_maker (corner_1, corner_2);

    TopoDS_Solid my_box = box_maker.Solid();

    std::vector<int> indices {1, 3};

    TopoDS_Shape filleted_box = geoml::make_fillet(my_box, indices, 0.15);

    int edge_count = 0;

    for (TopExp_Explorer explorer(filleted_box, TopAbs_EDGE); explorer.More(); explorer.Next())
    {
        edge_count++;
    }

    EXPECT_EQ(edge_count, 36);


}

TEST(Test_make_fillet, simple_make_fillet_single_index_test)
{  
// create a box  
    gp_Pnt corner_1 (0.0, 0.0, 0.0); 
    gp_Pnt corner_2 (1.0, 2.0, 3.0); 

    BRepPrimAPI_MakeBox box_maker (corner_1, corner_2);

    TopoDS_Solid my_box = box_maker.Solid();

    int index = 1;

    TopoDS_Shape filleted_box = geoml::make_fillet(my_box, index, 0.2);

    int edge_count = 0;

    for (TopExp_Explorer explorer(filleted_box, TopAbs_EDGE); explorer.More(); explorer.Next())
    {
        edge_count++;
    }

    EXPECT_EQ(edge_count, 30);


}