#include <geoml/transformations/transformations.h>

#include <gtest/gtest.h>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>



TEST(Test_translate, simple_translate_gp_Pnt_test)
{    

    gp_Pnt test_point (3., -5., 1.);

    gp_Vec translation_vector (1., 0.5, 0.);

    gp_Pnt translated_point = geoml::translate(test_point, translation_vector);

    EXPECT_NEAR(translated_point.X(), test_point.X() + 1., 1e-5);
    EXPECT_NEAR(translated_point.Y(), test_point.Y() + 0.5, 1e-5);
    EXPECT_NEAR(translated_point.Z(), test_point.Z() + 0., 1e-5);

}

TEST(Test_translate, simple_translate_TopoDS_Shape_test)
{    

    gp_Pnt test_point (3., -5., 1.);

    BRepBuilderAPI_MakeVertex vertexMaker(test_point);
    TopoDS_Vertex vertex_test_point = vertexMaker.Vertex();

    gp_Vec translation_vector (1., 0.5, 0.);

    TopoDS_Shape translated_vertex_shape = geoml::translate(vertex_test_point, translation_vector);

    TopExp_Explorer explorer(translated_vertex_shape, TopAbs_VERTEX);

    TopoDS_Vertex vertex = TopoDS::Vertex(explorer.Current());
    gp_Pnt vertexPoint = BRep_Tool::Pnt(vertex);
    
    EXPECT_NEAR(vertexPoint.X(), test_point.X() + 1., 1e-5);
    EXPECT_NEAR(vertexPoint.Y(), test_point.Y() + 0.5, 1e-5);
    EXPECT_NEAR(vertexPoint.Z(), test_point.Z() + 0., 1e-5);

}

TEST(Test_translate, simple_translate_vector_gp_Pnt_test)
{    

    gp_Pnt test_point_1 (3., -5., 1.);
    gp_Pnt test_point_2 (6., 2., -1.);

    std::vector<gp_Pnt> vec {test_point_1, test_point_2};

    gp_Vec translation_vector (1., 0.5, 0.);

    std::vector<gp_Pnt> translated_points = geoml::translate(vec, translation_vector);

    EXPECT_NEAR(translated_points.at(0).X(), test_point_1.X() + 1., 1e-5);
    EXPECT_NEAR(translated_points.at(0).Y(), test_point_1.Y() + 0.5, 1e-5);
    EXPECT_NEAR(translated_points.at(0).Z(), test_point_1.Z() + 0., 1e-5);

    EXPECT_NEAR(translated_points.at(1).X(), test_point_2.X() + 1., 1e-5);
    EXPECT_NEAR(translated_points.at(1).Y(), test_point_2.Y() + 0.5, 1e-5);
    EXPECT_NEAR(translated_points.at(1).Z(), test_point_2.Z() + 0., 1e-5);

}

TEST(Test_Transform, simple_Transform_test)
{    
    gp_Pnt test_pnt (1., 2., 3.);

    gp_Vec trsf_vec_1 (1., 2., 4.2);
    gp_Vec trsf_vec_2 (-1., 2.1, 14.7);

    geoml::Transform my_trsf_1 (trsf_vec_1);
    geoml::Transform my_trsf_2 (trsf_vec_2);

    geoml::Transform my_trsf_3 = my_trsf_1 * my_trsf_2;

    gp_Pnt moved_pnt = my_trsf_3.Apply_Transform(test_pnt);

    EXPECT_NEAR(moved_pnt.X(), 1., 1e-5);
    EXPECT_NEAR(moved_pnt.Y(), 6.1, 1e-5);
    EXPECT_NEAR(moved_pnt.Z(), 21.9, 1e-5);

}

TEST(Test_repeat_shapes, repeat_a_point)
{    
    gp_Pnt test_pnt (1., 2., 3.);

    TopoDS_Vertex vert = BRepBuilderAPI_MakeVertex(test_pnt);

    TopoDS_Shape shape = vert;

    gp_Vec trsf_vec (1., 2., 4.2);

    geoml::Transform my_trsf (trsf_vec);

    TopoDS_Compound result = geoml::repeat_shape(shape, my_trsf, 3);

    int vertexCount = 0;

    for (TopExp_Explorer explorer(result, TopAbs_VERTEX); explorer.More(); explorer.Next()) {
        vertexCount++;
    }

    EXPECT_EQ(vertexCount, 3);
}