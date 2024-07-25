#include <geoml/transformations/transformations.h>

#include <gtest/gtest.h>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>


TEST(Test_translate, simple_translate_test)
{    

    gp_Pnt test_point (3., -5., 1.);

    gp_Vec translation_vector (1., 0.5, 0.);

   // gp_Pnt translated_point = geoml::translate(test_point, translation_vector);

}