
#include "geoml/data_structures/Array2d.h"
#include "geoml/data_structures/conversions.h"

#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_Array1OfPnt.hxx>

#include <gtest/gtest.h>

#include <iostream>


TEST(Test_Array2d, simple_Array2d_test)
{    

geoml::Array2d<int> vec2d_int (5,4);

geoml::Array2d<double> my_vec2d_double (2,5);

EXPECT_EQ(vec2d_int.at(3,2), 0);

vec2d_int.at(3,2) = 55;

EXPECT_EQ(vec2d_int.at(3,2), 55);
EXPECT_EQ(vec2d_int(3,2), 55);

const geoml::Array2d<int> vec2d_int_const (4,3);

EXPECT_EQ(vec2d_int_const.at(1,1), 0);
EXPECT_EQ(vec2d_int_const(1,1), 0);

vec2d_int.setValue(3,2,888);

EXPECT_EQ(vec2d_int.at(3,2), 888);

EXPECT_EQ(vec2d_int.rowLength(), 5);
EXPECT_FALSE(vec2d_int.rowLength() == 3);
EXPECT_EQ(vec2d_int.colLength(), 4);
EXPECT_FALSE(vec2d_int.colLength() == 3);

}

TEST(Test_ConvertArray2d_gp_Pnt_ToTCol, simple_conversion_from_Array2d_to_TCol_gp_Pnt)
{
    gp_Pnt pt_00 (0., 0., 0.);
    gp_Pnt pt_10 (1., 0., 0.);
    gp_Pnt pt_20 (2., 0., 0.);
    gp_Pnt pt_01 (3., 0., 0.);
    gp_Pnt pt_11 (4., 0., 0.);
    gp_Pnt pt_21 (5., 0., 0.);

    geoml::Array2d<gp_Pnt> vec2d_gp_Pnt (3,2);

    vec2d_gp_Pnt.setValue(0, 0, pt_00);
    vec2d_gp_Pnt.setValue(1, 0, pt_10);
    vec2d_gp_Pnt.setValue(2, 0, pt_20);
    vec2d_gp_Pnt.setValue(0, 1, pt_01);
    vec2d_gp_Pnt.setValue(1, 1, pt_11);
    vec2d_gp_Pnt.setValue(2, 1, pt_21);

    TColgp_Array2OfPnt points_2d = geoml::Array2d_to_TCol(vec2d_gp_Pnt);

    EXPECT_EQ(points_2d.Value(1,2).X(),pt_01.X());

}

TEST(Test_ConvertStdVector_ToTCol, simple_conversion_from_std_vector_to_TCol_gp_Pnt)
{
    gp_Pnt pt_0 (0., 0., 0.);
    gp_Pnt pt_1 (1., 0., 0.);
    gp_Pnt pt_2 (2., 0., 0.);

    std::vector<gp_Pnt> vec_gp_Pnt;

    vec_gp_Pnt.push_back(pt_0);
    vec_gp_Pnt.push_back(pt_1);
    vec_gp_Pnt.push_back(pt_2);

    TColgp_Array1OfPnt points = geoml::StdVector_to_TCol(vec_gp_Pnt);

    EXPECT_EQ(points.Value(2).X(),pt_1.X());

}


