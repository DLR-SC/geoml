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