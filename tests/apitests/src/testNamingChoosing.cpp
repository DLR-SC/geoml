#include <geoml/naming_choosing/Shape.h>
#include <geoml/surfaces/surfaces.h>
#include <geoml/data_structures/Array2d.h>
#include <geoml/geom_topo_conversions/geom_topo_conversions.h>
#include <geoml/primitives/modeling.hpp>
#include <geoml/boolean_ops/modeling.hpp>
#include <geoml/predicates/predicate_functions.h>

#include <gtest/gtest.h>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include "BRepTools.hxx"
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS.hxx>

//TODO: experiment_with_naming_choosing_code is a very bad name
TEST(Test_naming_choosing_code, experiment_with_naming_choosing_code)
{   
    auto box = geoml::create_box(1., 1., 1.);
    auto cylinder_big = geoml::create_cylinder(0.5, 0.5);
    auto cylinder_small = geoml::create_cylinder(0.25, 0.75);
    auto result = box - cylinder_big - cylinder_small;

    // Get an edge from the box. Selecting first one here.
    auto box_edges = box.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_EDGE); });
    
    EXPECT_EQ(box_edges.size(), 24);

    auto const& edge_in = *box_edges.front();

    // Get edges in result that originate from the edge selected in box
    auto result_edges = result.select_subshapes([&](geoml::Shape const& s){
        return  s.is_type(TopAbs_EDGE) &&               // any edge,
               s.is_descendent_of(edge_in);            // created from edge_in thru any number of operations
    });

    EXPECT_EQ(result_edges.size(), 2);

    // Get a face from the cylinder. Selecting first one here.
    auto cyl_faces = cylinder_big.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_FACE); });

    EXPECT_EQ(cyl_faces.size(), 3);

    auto const& face_in = *cyl_faces[1];

    // Get face in result that originate from the face selected in cylinder
    auto result_faces = result.select_subshapes([&](geoml::Shape const& s){
        return  s.is_type(TopAbs_FACE) &&               // any edge,
               s.is_descendent_of(face_in);            // created from edge_in thru any number of operations
    });

    EXPECT_EQ(result_faces.size(), 1);

    // Get all faces in result that originate from any subshape contained in the two cylinders
    auto result_faces_from_cylinders = result.select_subshapes([&](geoml::Shape const& s){
        return s.is_type(TopAbs_FACE) && (s.is_descendent_of_subshape_in(cylinder_small) || s.is_descendent_of_subshape_in(cylinder_big) );
    });

    EXPECT_EQ(result_faces_from_cylinders.size(), 4);

}

//TODO: experiment_with_naming_choosing_code is a very bad name
TEST(Test_naming_choosing_code, example_rectangle_triangles)
{   
    // create a rectangular surface

    // control points
    gp_Pnt p_00(0.0, 2.0, 0.0);
    gp_Pnt p_10(0.0, 0.0, 0.0);
    gp_Pnt p_01(4.0, 2.0, 0.0);
    gp_Pnt p_11(4.0, 0.0, 0.0);

    geoml::Array2d<gp_Pnt> control_points (2,2);

    control_points.setValue(0, 0, p_00);
    control_points.setValue(1, 0, p_10);
    control_points.setValue(0, 1, p_01);
    control_points.setValue(1, 1, p_11);

    geoml::Array2d<Standard_Real> weights (2,2);

    weights.setValue(0, 0, 1.);
    weights.setValue(1, 0, 1.);
    weights.setValue(0, 1, 1.);
    weights.setValue(1, 1, 1.);

    std::vector<Standard_Real> U_knots;

    U_knots.push_back(0.0);
    U_knots.push_back(1.0);

    std::vector<Standard_Real> V_knots;

    V_knots.push_back(0.0);
    V_knots.push_back(1.0);

    std::vector<int> U_mults;

    U_mults.push_back(2);
    U_mults.push_back(2);

    std::vector<int> V_mults;

    V_mults.push_back(2);
    V_mults.push_back(2);

    int U_degree = 1;
    int V_degree = 1;

    Handle(Geom_BSplineSurface) srf = geoml::nurbs_surface(control_points, weights, U_knots, V_knots, U_mults, V_mults, U_degree, V_degree);

    geoml::Shape rectangular_srf (geoml::SurfaceToFace(srf,1e-5));

    // define a design parameter for translating a triangle
    Standard_Real d (3.0);

    // create a triangular surface
    gp_Pnt p1(5.0 - d, -1.0, 0.0); 
    gp_Pnt p2(7.0 - d, -1.0, 0.0); 
    gp_Pnt p3(6.0 - d, 1.0, 0.0); 

    // create a polygon from these points
    BRepBuilderAPI_MakePolygon polygon;
    polygon.Add(p1);
    polygon.Add(p2);
    polygon.Add(p3);
    polygon.Close(); 

    // create a face from the polygon
    TopoDS_Face triangle_face = BRepBuilderAPI_MakeFace(polygon.Wire());

    geoml::Shape triangular_srf (triangle_face);

    // define a second design parameter for translating a triangle
    Standard_Real d_2 (5.0);

    // create a second triangular surface
    gp_Pnt p1_2(5.0 - d_2, -1.0, 0.0); 
    gp_Pnt p2_2(7.0 - d_2, -1.0, 0.0); 
    gp_Pnt p3_2(6.0 - d_2, .8, 0.0); 

    // create a polygon from these points
    BRepBuilderAPI_MakePolygon polygon_2;
    polygon_2.Add(p1_2);
    polygon_2.Add(p2_2);
    polygon_2.Add(p3_2);
    polygon_2.Close(); 

    // create a face from the polygon
    TopoDS_Face triangle_face_2 = BRepBuilderAPI_MakeFace(polygon_2.Wire());

    geoml::Shape triangular_srf_2(triangle_face_2);

    // perform the modelling steps
    geoml::Shape cut_result = rectangular_srf - triangular_srf;
    geoml::Shape cut_result_2 = cut_result - triangular_srf_2; 

    // Select edges from the rectangular_srf.
    auto rectangular_srf_edges = rectangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_EDGE); });

    EXPECT_EQ(rectangular_srf_edges.size(), 4);

    auto const&  X = *rectangular_srf_edges.at(2); // "pick" edge
    auto const&  Y = *rectangular_srf_edges.at(0); // "pick" edge
    auto const&  Z = *rectangular_srf_edges.at(1); // "pick" edge
    auto const&  W = *rectangular_srf_edges.at(3); // "pick" edge

    // Select vertices from the rectangular_srf.
    auto rectangular_srf_vertices = rectangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_VERTEX); });
   
    EXPECT_EQ(rectangular_srf_vertices.size(), 8);

    auto const&  V1 = *rectangular_srf_vertices.at(3); // "pick" vertex
    auto const&  V2 = *rectangular_srf_vertices.at(0); // "pick" vertex
    auto const&  V3 = *rectangular_srf_vertices.at(1); // "pick" vertex
    auto const&  V4 = *rectangular_srf_vertices.at(2); // "pick" vertex
    auto const&  V5 = *rectangular_srf_vertices.at(4); // "pick" vertex
    auto const&  V6 = *rectangular_srf_vertices.at(5); // "pick" vertex
    auto const&  V7 = *rectangular_srf_vertices.at(6); // "pick" vertex
    auto const&  V8 = *rectangular_srf_vertices.at(7); // "pick" vertex

    // Select edges from the triangular_srf. 
    auto triangular_srf_edges = triangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_EDGE); });

    EXPECT_EQ(triangular_srf_edges.size(), 3);

    auto const&  C = *triangular_srf_edges.at(0); // "pick" edge
    auto const&  B = *triangular_srf_edges.at(1); // "pick" edge
    auto const&  A = *triangular_srf_edges.at(2); // "pick" edge

    // Get edges in result that originate from edge X and have a vertex that originates from V1
    auto cut_result_edges_X_V1 = cut_result.select_subshapes([&](geoml::Shape const& s){
        return  s.is_type(TopAbs_EDGE) &&               
                s.is_descendent_of(X) &&
                s.has_subshape_that(is_child_of(V1));       
    });

    EXPECT_EQ(cut_result_edges_X_V1.size(), 1);

    if(cut_result_edges_X_V1.size() == 1) {
    auto const& result_X_V1 = *cut_result_edges_X_V1.at(0); // there is only one entry in the std::vector
    }

    // Get edges in result that originate from edge X or from A or B
    auto cut_result_edges_X_A_B = cut_result.select_subshapes([&](geoml::Shape const& s){
        return  s.is_type(TopAbs_EDGE) &&               
                ( s.is_descendent_of(X) || 
                  s.is_descendent_of(A) ||
                  s.is_descendent_of(B)    );       
    });

    EXPECT_EQ(cut_result_edges_X_A_B.size(), 4);

    // test choosing after two modelling steps:

    // Get edges in result that originate from edge X and have a vertex originating from V1 or originates from A:
    auto cut_result_2_edges_X_V1_A = cut_result_2.select_subshapes([&](geoml::Shape const& s){
        return  s.is_type(TopAbs_EDGE) &&               
                ( (s.is_descendent_of(X) && s.has_subshape_that(is_child_of(V1))) ||
                  s.is_descendent_of(A) );       
    });

    EXPECT_EQ(cut_result_2_edges_X_V1_A.size(), 2);

    // test horizontal tags:

    // define criterion_2
    std::function<bool(geoml::Shape const&)> criterion_2 = [&](geoml::Shape const& s){
        return s.is_type(TopAbs_EDGE) &&  
               s.is_descendent_of(X) &&      
               !(s.has_subshape_that(is_child_of(V1))); 
    }; 

    // define tag 
    std::string tag_2 ("tag_2");

    // add tag to subshapes that fulfill criterion_2
    cut_result.add_meta_tag_to_subshapes(criterion_2, tag_2);

    // Select edges in cut_result carrying tag_2
    auto cut_result_edges_tag_2 = cut_result.select_subshapes([&](geoml::Shape& s){
        return  s.has_tag(tag_2);       
    });

    EXPECT_EQ(cut_result_edges_tag_2.size(), 1);

    // test tag tracks:
    
    // define criterion_1
    std::function<bool(geoml::Shape const&)> criterion_1 = [&](geoml::Shape const& s){
        return s.is_type(TopAbs_EDGE) &&               
             ( s.is_descendent_of(A) ||
               s.is_descendent_of(B) ); 
    };

    std::string tag_1 = "persistent name 1";

    int remainingSteps_1 = 2;

    // define tag_track_1
    geoml::TagTrack tag_track_1(tag_1, criterion_1, remainingSteps_1);

    // get edges in cut_result carrying tag_1
    auto cut_result_edges_tag_1 = cut_result.select_subshapes([&](geoml::Shape& s){
        return  s.has_tag(tag_track_1.m_tag);       
    });

    EXPECT_EQ(cut_result_edges_tag_1.size(), 0);
}

TEST(Test_naming_choosing_code, test_tag_tracks_with_operations)
{   
    // create a rectangular surface

    // control points
    gp_Pnt p_00(0.0, 2.0, 0.0);
    gp_Pnt p_10(0.0, 0.0, 0.0);
    gp_Pnt p_01(4.0, 2.0, 0.0);
    gp_Pnt p_11(4.0, 0.0, 0.0);

    geoml::Array2d<gp_Pnt> control_points (2,2);

    control_points.setValue(0, 0, p_00);
    control_points.setValue(1, 0, p_10);
    control_points.setValue(0, 1, p_01);
    control_points.setValue(1, 1, p_11);

    geoml::Array2d<Standard_Real> weights (2,2);

    weights.setValue(0, 0, 1.);
    weights.setValue(1, 0, 1.);
    weights.setValue(0, 1, 1.);
    weights.setValue(1, 1, 1.);

    std::vector<Standard_Real> U_knots;

    U_knots.push_back(0.0);
    U_knots.push_back(1.0);

    std::vector<Standard_Real> V_knots;

    V_knots.push_back(0.0);
    V_knots.push_back(1.0);

    std::vector<int> U_mults;

    U_mults.push_back(2);
    U_mults.push_back(2);

    std::vector<int> V_mults;

    V_mults.push_back(2);
    V_mults.push_back(2);

    int U_degree = 1;
    int V_degree = 1;

    Handle(Geom_BSplineSurface) srf = geoml::nurbs_surface(control_points, weights, U_knots, V_knots, U_mults, V_mults, U_degree, V_degree);

    geoml::Shape rectangular_srf (geoml::SurfaceToFace(srf,1e-5));

    // define a design parameter for translating a triangle
    Standard_Real d (3.0);

    // create a triangular surface
    gp_Pnt p1(5.0 - d, -1.0, 0.0); 
    gp_Pnt p2(7.0 - d, -1.0, 0.0); 
    gp_Pnt p3(6.0 - d, 1.0, 0.0); 

    // create a polygon from these points
    BRepBuilderAPI_MakePolygon polygon;
    polygon.Add(p1);
    polygon.Add(p2);
    polygon.Add(p3);
    polygon.Close(); 

    // create a face from the polygon
    TopoDS_Face triangle_face = BRepBuilderAPI_MakeFace(polygon.Wire());

    geoml::Shape triangular_srf (triangle_face);

    // define a second design parameter for translating a triangle
    Standard_Real d_2 (5.0);

    // create a second triangular surface
    gp_Pnt p1_2(5.0 - d_2, -1.0, 0.0); 
    gp_Pnt p2_2(7.0 - d_2, -1.0, 0.0); 
    gp_Pnt p3_2(6.0 - d_2, .8, 0.0); 

    // create a polygon from these points
    BRepBuilderAPI_MakePolygon polygon_2;
    polygon_2.Add(p1_2);
    polygon_2.Add(p2_2);
    polygon_2.Add(p3_2);
    polygon_2.Close(); 

    // create a face from the polygon
    TopoDS_Face triangle_face_2 = BRepBuilderAPI_MakeFace(polygon_2.Wire());

    geoml::Shape triangular_srf_2(triangle_face_2);

    // Select edges from the rectangular_srf.
    auto rectangular_srf_edges = rectangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_EDGE); });

    EXPECT_EQ(rectangular_srf_edges.size(), 4);

    auto const&  X = *rectangular_srf_edges.at(2); // "pick" edge
    auto const&  Y = *rectangular_srf_edges.at(0); // "pick" edge
    auto const&  Z = *rectangular_srf_edges.at(1); // "pick" edge
    auto const&  W = *rectangular_srf_edges.at(3); // "pick" edge

    // Select vertices from the rectangular_srf.
    auto rectangular_srf_vertices = rectangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_VERTEX); });

    EXPECT_EQ(rectangular_srf_vertices.size(), 8);

    auto const&  V1 = *rectangular_srf_vertices.at(3); // "pick" vertex
    auto const&  V2 = *rectangular_srf_vertices.at(0); // "pick" vertex
    auto const&  V3 = *rectangular_srf_vertices.at(1); // "pick" vertex
    auto const&  V4 = *rectangular_srf_vertices.at(2); // "pick" vertex
    auto const&  V5 = *rectangular_srf_vertices.at(4); // "pick" vertex
    auto const&  V6 = *rectangular_srf_vertices.at(5); // "pick" vertex
    auto const&  V7 = *rectangular_srf_vertices.at(6); // "pick" vertex
    auto const&  V8 = *rectangular_srf_vertices.at(7); // "pick" vertex

    // Select edges from the triangular_srf. 
    auto triangular_srf_edges = triangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_EDGE); });

    EXPECT_EQ(triangular_srf_edges .size(), 3);

    auto const&  C = *triangular_srf_edges.at(0); // "pick" edge
    auto const&  B = *triangular_srf_edges.at(1); // "pick" edge
    auto const&  A = *triangular_srf_edges.at(2); // "pick" edge

    // add a tag track to rectangular_srf
    std::function<bool(geoml::Shape const&)> criterion = [&](geoml::Shape const& s){
        return s.is_type(TopAbs_EDGE) &&  
               s.is_descendent_of(X) &&      
               s.has_subshape_that(is_child_of(V1)); 
    }; 

    std::string tag = "vertically persistent name";
    int remainingSteps = 2;

    geoml::TagTrack tag_track(tag, criterion, remainingSteps);

    rectangular_srf.add_tag_track(tag_track);

    // apply the tag tracks of rectangular_srf to the Shape itself
    rectangular_srf.apply_tag_tracks();

    // get edges in rectuangular_srf carrying tag "vertically persisten name"
    auto rectangular_srf_tagged_edges = rectangular_srf.select_subshapes([&](geoml::Shape& s){
        return  s.has_tag(tag_track.m_tag);       
    });

    EXPECT_EQ(rectangular_srf_tagged_edges.size(), 1);

    // perform first modelling operation
    geoml::Shape cut_result = rectangular_srf - triangular_srf;

    // get edges in cut_result carrying tag "vertically persisten name"
    auto cut_result_tagged_edges = cut_result.select_subshapes([&](geoml::Shape& s){
        return  s.has_tag(tag_track.m_tag);       
    });

    EXPECT_EQ(cut_result_tagged_edges .size(), 1);

    // perform second modelling operation
    geoml::Shape cut_result_2 = cut_result - triangular_srf_2;

    // get edges in cut_result_2 carrying tag "vertically persisten name"
    auto cut_result_2_tagged_edges = cut_result_2.select_subshapes([&](geoml::Shape& s){
        return  s.has_tag(tag_track.m_tag);       
    });

    EXPECT_EQ(cut_result_2_tagged_edges.size(), 1);

}

TEST(Test_naming_choosing_code, test_shape_predicates)
{   
    // create a rectangular surface

    // control points
    gp_Pnt p_00(0.0, 2.0, 0.0);
    gp_Pnt p_10(0.0, 0.0, 0.0);
    gp_Pnt p_01(4.0, 2.0, 0.0);
    gp_Pnt p_11(4.0, 0.0, 0.0);

    geoml::Array2d<gp_Pnt> control_points (2,2);

    control_points.setValue(0, 0, p_00);
    control_points.setValue(1, 0, p_10);
    control_points.setValue(0, 1, p_01);
    control_points.setValue(1, 1, p_11);

    geoml::Array2d<Standard_Real> weights (2,2);

    weights.setValue(0, 0, 1.);
    weights.setValue(1, 0, 1.);
    weights.setValue(0, 1, 1.);
    weights.setValue(1, 1, 1.);

    std::vector<Standard_Real> U_knots;

    U_knots.push_back(0.0);
    U_knots.push_back(1.0);

    std::vector<Standard_Real> V_knots;

    V_knots.push_back(0.0);
    V_knots.push_back(1.0);

    std::vector<int> U_mults;

    U_mults.push_back(2);
    U_mults.push_back(2);

    std::vector<int> V_mults;

    V_mults.push_back(2);
    V_mults.push_back(2);

    int U_degree = 1;
    int V_degree = 1;

    Handle(Geom_BSplineSurface) srf = geoml::nurbs_surface(control_points, weights, U_knots, V_knots, U_mults, V_mults, U_degree, V_degree);

    geoml::Shape rectangular_srf (geoml::SurfaceToFace(srf,1e-5));

    // define a predicate
    geoml::ShapePredicate pred = [&](geoml::Shape const& s){
        return s.is_type(TopAbs_EDGE);   
    }; 

    // get edges in rectuangular_srf 
    auto rectangular_srf_edges = rectangular_srf.select_subshapes(pred);

    EXPECT_EQ(rectangular_srf_edges.size(), 4);

    rectangular_srf.add_meta_tag_to_subshapes(pred, "edge_tag");

    auto rectangular_srf_edges_with_tag = rectangular_srf.select_subshapes(geoml::has_tag("edge_tag"));    

    EXPECT_EQ(rectangular_srf_edges_with_tag.size(), 4);
}


//TODO: I also don't like "Test_naming_choosing_code"
TEST(Test_naming_choosing_code, split_edge)
{
    using namespace geoml;

    /****************************
     * create a face and a tool *
     ****************************/

    auto v = BRepBuilderAPI_MakeVertex(gp_Pnt(0., 0., 0.));
    auto e = BRepPrimAPI_MakePrism(v, gp_Vec(2., 0., 0.));
    auto f = BRepPrimAPI_MakePrism(e, gp_Vec(0., 2., 0.));
    auto face = Shape(f);

    auto t = BRepPrimAPI_MakeBox(gp_Pnt(-0.5, -0.5, -0.5), 1., 1., 1.);
    auto tool = Shape(t);

    /***********************************************
     * extract some edges and vertices of the face *
     ***********************************************/

    /*
       Vertices: alpha, beta, gamma, delta. (only interested in alpha, beta)
       edges: a,b,c,d (only interested in a,b,d)

             +----+
             |    |
          d  |    |  b
             +----+
       alpha    a   beta 
    */

    // let a be any edge of the face
    auto edges_candidates = face.select_subshapes(is_edge);
    ASSERT_TRUE(edges_candidates.size() > 0);
    Shape const& a = *edges_candidates[0];

    // let alpha and beta be the two vertices of edge a
    auto vertices = a.select_subshapes(is_vertex);
    ASSERT_EQ(vertices.size(), 2);
    Shape const& alpha = *vertices[0];
    Shape const& beta = *vertices[1];

    // let b be the edge that is adjacent to a at beta
    auto b_candidates = face.select_subshapes(
        is_edge &&
        !is_same(a) &&
        has_subshape(beta)
    );
    ASSERT_EQ(b_candidates.size(), 1);
    Shape const& b = *b_candidates[0];

    // let d be the edge that is adjacent to a at alpha
    auto d_candidates = face.select_subshapes(
        is_edge &&
        !is_same(a) &&
        has_subshape(beta)
    );
    ASSERT_EQ(d_candidates.size(), 1);
    Shape const& d = *d_candidates[0];

    {
        auto test_alpha = BRep_Tool::Pnt(TopoDS::Vertex(alpha));
        EXPECT_NEAR(test_alpha.X(), 0., 1e-14);
        EXPECT_NEAR(test_alpha.Y(), 0., 1e-14);
        EXPECT_NEAR(test_alpha.Z(), 0., 1e-14);

        auto test_beta = BRep_Tool::Pnt(TopoDS::Vertex(beta));
        EXPECT_NEAR(test_beta.X(), 0., 1e-14);
        EXPECT_NEAR(test_beta.Y(), 2., 1e-14);
        EXPECT_NEAR(test_beta.Z(), 0., 1e-14);
    }

    /******************************
     * cut the face with the tool *
     ******************************/

    auto face_cut = face - tool;

    /*********************************
     * select edge of cutting result *
     *********************************/

    /* modeling intent:

       Always choose the edge that is a descendent of a. 
       If this is not unique, choose the edge of that is a descendent of a and
       closest to alpha.

       Challenge formulate predicate only with history- and topological predicates
       for our specific parametric model
    */ 

    auto edge1 = 
        face_cut.filter(is_edge && is_descendent_of(a) && has_subshape(alpha)) ||
        face_cut.filter(is_edge && is_descendent_of(a));

    auto has_common_vertex_with = [](Shape const& other) {
        return has_subshape_that(is_vertex && is_subshape_of(other)) &&
               !is_same(other);
    };

    auto edge2 = face_cut.filter(
        is_edge && 
        has_common_vertex_with(edge1) &&
        !is_same(d)
    );

    // we could just sweep edge1 and edge2 seperately or make a compound out of them, 
    // but just for fun lets use a filter again
    auto edges = face_cut.filter(is_same(edge1) || is_same(edge2));

    auto result = BRepPrimAPI_MakePrism(edges, gp_Vec(0., 0., -2.));

    BRepTools::Write(tool, "tool.brep");
    BRepTools::Write(edge1, "edge1.brep");
    BRepTools::Write(edge2, "edge2.brep");
    BRepTools::Write(edges, "edges.brep");
    BRepTools::Write(face_cut, "face_cut.brep");
    BRepTools::Write(result, "result.brep");
}