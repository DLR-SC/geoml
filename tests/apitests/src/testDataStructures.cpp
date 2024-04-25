
#include "geoml/data_structures/Array2d.h"

#include <gtest/gtest.h>


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


