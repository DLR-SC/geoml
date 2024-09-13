#include <geoml/naming_choosing/BRepBuilderAPI_MakeShape_Operation.hpp>
#include <geoml/naming_choosing/Shape.hpp>
#include <geoml/naming_choosing/Operation.hpp>
#include <geoml/surfaces/modeling.h>
#include <geoml/data_structures/Array2d.h>
#include <geoml/geom_topo/modeling.h>

#include <gtest/gtest.h>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include "BRepTools.hxx"
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>


TEST(Test_naming_choosing_code, experiment_with_naming_choosing_code)
{   
    // usage by user

    auto box = geoml::create_box(1., 1., 1.);
    auto cylinder_big = geoml::create_cylinder(0.5, 0.5);
    auto cylinder_small = geoml::create_cylinder(0.25, 0.75);
    auto result = box - cylinder_big - cylinder_small;

    // Get an edge from the box. Selecting first one here.
    auto box_edges = box.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_EDGE); });
    std::cout << box_edges.size() << " edges in box\n";
    auto const& edge_in = *box_edges.front();

    // Get edges in result that originate from the edge selected in box
    auto result_edges = result.select_subshapes([&](geoml::Shape const& s){
        return  s.is_type(TopAbs_EDGE) &&               // any edge,
               s.is_descendent_of(edge_in);            // created from edge_in thru any number of operations
    });
    std::cout << result_edges.size() << " edges in result that originate from first edge in box\n";


    // Get a face from the cylinder. Selecting first one here.
    auto cyl_faces = cylinder_big.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_FACE); });
    std::cout << cyl_faces.size() << " faces in cylinder_big\n";
    auto const& face_in = *cyl_faces[1];

    // Get face in result that originate from the face selected in cylinder
    auto result_faces = result.select_subshapes([&](geoml::Shape const& s){
        return  s.is_type(TopAbs_FACE) &&               // any edge,
               s.is_descendent_of(face_in);            // created from edge_in thru any number of operations
    });
    std::cout << result_faces.size() << " faces in result that originate from first face in cylinder_big\n";


    // Get all faces in result that originate from any subshape contained in the two cylinders
    auto result_faces_from_cylinders = result.select_subshapes([&](geoml::Shape const& s){
        return s.is_type(TopAbs_FACE) && (s.is_descendent_of_subshape_in(cylinder_small) || s.is_descendent_of_subshape_in(cylinder_big) );
    });
    std::cout << result_faces_from_cylinders.size() << " faces in result that originate from any subshape in cylinder_big or cylinder_small\n";

    // Write some files for debuggin
    BRepTools::Write(edge_in, "edge_in.brep");
    BRepTools::Write(face_in, "face_in.brep");

    if (result_edges.size() > 0) {
        int idx = 0;
        for (auto const& edge_out : result_edges)
        {
            std::string filename = "edge_out" + std::to_string(idx++) + ".brep";
            BRepTools::Write(*edge_out, filename.c_str());
        }
    }

    if (result_faces.size() > 0) {
        int idx = 0;
        for (auto const& face_out : result_faces)
        {
            std::string filename = "face_out" + std::to_string(idx++) + ".brep";
            BRepTools::Write(*face_out, filename.c_str());
        }
    }

    if (result_faces_from_cylinders.size() > 0) {
        int idx = 0;
        for (auto const& face_out : result_faces_from_cylinders)
        {
            std::string filename = "cylface_out" + std::to_string(idx++) + ".brep";
            BRepTools::Write(*face_out, filename.c_str());
        }
    }

    BRepTools::Write(result, "result.brep");

}

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

    // write to file for debugging
    BRepTools::Write(rectangular_srf, "rectangular_srf.brep");
    BRepTools::Write(triangular_srf, "triangular_srf.brep");
    BRepTools::Write(cut_result, "cut_result.brep");
    BRepTools::Write(cut_result_2, "cut_result_2.brep");

    // extrusion direction
    gp_Vec direction(1., -1., 0.);  

    // Select edges from the rectangular_srf.
    auto rectangular_srf_edges = rectangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_EDGE); });

    auto const&  X = *rectangular_srf_edges.at(2); // "pick" edge
    auto const&  Y = *rectangular_srf_edges.at(0); // "pick" edge
    auto const&  Z = *rectangular_srf_edges.at(1); // "pick" edge
    auto const&  W = *rectangular_srf_edges.at(3); // "pick" edge

    // write to file for debugging
    BRepTools::Write(X, "X.brep");
    BRepTools::Write(Y, "Y.brep");
    BRepTools::Write(Z, "Z.brep");
    BRepTools::Write(W, "W.brep");

    // Select vertices from the rectangular_srf.
    auto rectangular_srf_vertices = rectangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_VERTEX); });
   
    auto const&  V1 = *rectangular_srf_vertices.at(3); // "pick" vertex
    auto const&  V2 = *rectangular_srf_vertices.at(0); // "pick" vertex
    auto const&  V3 = *rectangular_srf_vertices.at(1); // "pick" vertex
    auto const&  V4 = *rectangular_srf_vertices.at(2); // "pick" vertex
    auto const&  V5 = *rectangular_srf_vertices.at(4); // "pick" vertex
    auto const&  V6 = *rectangular_srf_vertices.at(5); // "pick" vertex
    auto const&  V7 = *rectangular_srf_vertices.at(6); // "pick" vertex
    auto const&  V8 = *rectangular_srf_vertices.at(7); // "pick" vertex

    // write to file for debugging
    BRepTools::Write(V1, "V1.brep");
    BRepTools::Write(V2, "V2.brep");
    BRepTools::Write(V3, "V3.brep");
    BRepTools::Write(V4, "V4.brep");
    BRepTools::Write(V5, "V5.brep");
    BRepTools::Write(V6, "V6.brep");
    BRepTools::Write(V7, "V7.brep");
    BRepTools::Write(V8, "V8.brep");

    // Select edges from the triangular_srf. 
    auto triangular_srf_edges = triangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_EDGE); });

    auto const&  C = *triangular_srf_edges.at(0); // "pick" edge
    auto const&  B = *triangular_srf_edges.at(1); // "pick" edge
    auto const&  A = *triangular_srf_edges.at(2); // "pick" edge

    // write to file for debugging
    BRepTools::Write(C, "C.brep");
    BRepTools::Write(B, "B.brep");
    BRepTools::Write(A, "A.brep");

    // Get edges in result that originate from edge X and have a vertex that originates from V1
    auto cut_result_edges_X_V1 = cut_result.select_subshapes([&](geoml::Shape const& s){
        return  s.is_type(TopAbs_EDGE) &&               
                s.is_descendent_of(X) &&
                s.has_subshape_that_is_child_of(V1);       
    });

    if(cut_result_edges_X_V1.size() == 1) {
    auto const& result_X_V1 = *cut_result_edges_X_V1.at(0); // there is only one entry in the std::vector

    // extrude in dfined direction
    TopoDS_Shape extrudedEdge_X_V1 = BRepPrimAPI_MakePrism(result_X_V1, direction);

    // write to file for debugging
    BRepTools::Write(extrudedEdge_X_V1, "extrudedEdge_X_V1.brep");
    }

    // Get edges in result that originate from edge X or from A or B
    auto cut_result_edges_X_A_B = cut_result.select_subshapes([&](geoml::Shape const& s){
        return  s.is_type(TopAbs_EDGE) &&               
                ( s.is_descendent_of(X) || 
                  s.is_descendent_of(A) ||
                  s.is_descendent_of(B)    );       
    });

    // extrusion direction
    gp_Vec direction_1 (0., 0., 1.); 

    // extrude and write to file for debugging
    int i = 0;
    for (auto const& edge : cut_result_edges_X_A_B)
    {
        TopoDS_Shape current_edge = BRepPrimAPI_MakePrism(*edge, direction_1);
        std::string filename = "edge_X_A_B" + std::to_string(i++) + ".brep";
        BRepTools::Write(current_edge, filename.c_str());
    }

    // test choosing after two modelling steps:

    // Get edges in result that originate from edge X and have a vertex originating from V1 or originates from A:
    auto cut_result_2_edges_X_V1_A = cut_result_2.select_subshapes([&](geoml::Shape const& s){
        return  s.is_type(TopAbs_EDGE) &&               
                ( (s.is_descendent_of(X) && s.has_subshape_that_is_child_of(V1)) ||
                  s.is_descendent_of(A) );       
    });

    // extrusion direction
    gp_Vec direction_3 (0., -1., 1.);
    

    // extrude and write to file for bebugging
    int j = 0;
    for (auto const& edge : cut_result_2_edges_X_V1_A)
    {
        TopoDS_Shape current_edge = BRepPrimAPI_MakePrism(*edge, direction_3);
        std::string filename = "edge_X_V1_A" + std::to_string(j++) + ".brep";
        BRepTools::Write(current_edge, filename.c_str());
    }

    // test horizontal tags:

    // define criterion_2
    std::function<bool(geoml::Shape const&)> criterion_2 = [&](geoml::Shape const& s){
        return s.is_type(TopAbs_EDGE) &&  
               s.is_descendent_of(X) &&      
               !(s.has_subshape_that_is_child_of(V1)); 
    }; 

    // define tag 
    std::string tag_2 ("tag_2");

    // add tag to subshapes that fulfill criterion_2
    cut_result.add_meta_tag_to_subshapes(criterion_2, tag_2);

    // Select edges in cut_result carrying tag_2
    auto cut_result_edges_tag_2 = cut_result.select_subshapes([&](geoml::Shape& s){
        return  s.has_tag(tag_2);       
    });

    // extrude the edges carrying tag_2 in negative z-direction and write them to file for debugging
    gp_Vec direction_4 (0., 0., -3.); 
    int i_3 = 0;
    for (auto const& edge : cut_result_edges_tag_2)
    {
        TopoDS_Shape current_edge = BRepPrimAPI_MakePrism(*edge, direction_4);
        std::string filename = tag_2 + "_" + std::to_string(i_3++) + ".brep";
        BRepTools::Write(current_edge, filename.c_str());
    }

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

    // extrude the edges carrying tag_1 in negative z-direction and wirte to file for debugging
    gp_Vec direction_2 (0., 0., -1.); 
    int i_2 = 0;
    for (auto const& edge : cut_result_edges_tag_1)
    {
        TopoDS_Shape current_edge = BRepPrimAPI_MakePrism(*edge, direction_2);
        std::string filename = tag_1 + "_" + std::to_string(i_2++) + ".brep";
        BRepTools::Write(current_edge, filename.c_str());
        std::cout << "wrote a tag_1 edge" << std::endl;
    }
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

    auto const&  X = *rectangular_srf_edges.at(2); // "pick" edge
    auto const&  Y = *rectangular_srf_edges.at(0); // "pick" edge
    auto const&  Z = *rectangular_srf_edges.at(1); // "pick" edge
    auto const&  W = *rectangular_srf_edges.at(3); // "pick" edge

    // Select vertices from the rectangular_srf.
    auto rectangular_srf_vertices = rectangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_VERTEX); });

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

    auto const&  C = *triangular_srf_edges.at(0); // "pick" edge
    auto const&  B = *triangular_srf_edges.at(1); // "pick" edge
    auto const&  A = *triangular_srf_edges.at(2); // "pick" edge

    // add a tag track to rectangular_srf
    std::function<bool(geoml::Shape const&)> criterion = [&](geoml::Shape const& s){
        return s.is_type(TopAbs_EDGE) &&  
               s.is_descendent_of(X) &&      
               s.has_subshape_that_is_child_of(V1); 
    }; 

    std::string tag = "vertically persistent name";
    int remainingSteps = 2;

    geoml::TagTrack tag_track(tag, criterion, remainingSteps);

    rectangular_srf.add_tag_track(tag_track);

    // apply the tag tracks of rectangular_srf to the Shape itself
    rectangular_srf.apply_tag_tracks();

    // extrude edge in rectangular_srf with tag "vertically persisten name"

    // get edges in rectuangular_srf carrying tag "vertically persisten name"
    auto rectangular_srf_tagged_edges = rectangular_srf.select_subshapes([&](geoml::Shape& s){
        return  s.has_tag(tag_track.m_tag);       
    });

    // extrude the edges carrying this tag in negative z-direction and wirte to file for debugging
    gp_Vec direction (0., 0., -1.); 
    int i = 0;
    for (auto const& edge : rectangular_srf_tagged_edges)
    {
        TopoDS_Shape current_edge = BRepPrimAPI_MakePrism(*edge, direction);
        std::string filename = "0_th_step_" + std::to_string(i++) + ".brep";
        BRepTools::Write(current_edge, filename.c_str());
    }

    // perform first modelling operation
    geoml::Shape cut_result = rectangular_srf - triangular_srf;

    // extrude edge in cut_result with tag "vertically persisten name"

    // get edges in cut_result carrying tag "vertically persisten name"
    auto cut_result_tagged_edges = cut_result.select_subshapes([&](geoml::Shape& s){
        return  s.has_tag(tag_track.m_tag);       
    });

    // extrude the edges carrying this tag in negative z-direction and write to file for debugging
    int i_1 = 0;
    for (auto const& edge : cut_result_tagged_edges)
    {
        TopoDS_Shape current_edge = BRepPrimAPI_MakePrism(*edge, direction);
        std::string filename = "1_st_step_" + std::to_string(i++) + ".brep";
        BRepTools::Write(current_edge, filename.c_str());
    }

    // perform second modelling operation
    geoml::Shape cut_result_2 = cut_result - triangular_srf_2;

    // extrude edge in cut_result_2 with tag "vertically persisten name"

    // get edges in cut_result_2 carrying tag "vertically persisten name"
    auto cut_result_2_tagged_edges = cut_result_2.select_subshapes([&](geoml::Shape& s){
        return  s.has_tag(tag_track.m_tag);       
    });

    // extrude the edges carrying this tag in negative z-direction and write them to file for debugging
    int i_2 = 0;
    for (auto const& edge : cut_result_2_tagged_edges)
    {
        TopoDS_Shape current_edge = BRepPrimAPI_MakePrism(*edge, direction);
        std::string filename = "2_st_step_" + std::to_string(i++) + ".brep";
        BRepTools::Write(current_edge, filename.c_str());
    }
    
    // write shapes in 0-th, 1-st and 2-nd step to file for bebugging
    BRepTools::Write(rectangular_srf, "rectangular_srf.brep");
    BRepTools::Write(cut_result, "cut_result.brep");
    BRepTools::Write(cut_result_2, "cut_result_2.brep");

}


