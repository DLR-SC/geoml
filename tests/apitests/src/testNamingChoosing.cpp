#include <geoml/naming_choosing/BRepBuilderAPI_MakeShape_Operation.hpp>
#include <geoml/naming_choosing/Shape.hpp>
#include <geoml/naming_choosing/Operation.hpp>
#include <geoml/naming_choosing/MetaDataStore.hpp>
#include <geoml/naming_choosing/common.hpp>
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


#include <iostream>

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
            std::string filename = "fac111e_out" + std::to_string(idx++) + ".brep";
            BRepTools::Write(*face_out, filename.c_str());
        }
    }

    if (result_faces_from_cylinders.size() > 0) {
        int idx = 0;
        for (auto const& face_out : result_faces_from_cylinders)
        {
            std::string filename = "cyl111face_out" + std::to_string(idx++) + ".brep";
            BRepTools::Write(*face_out, filename.c_str());
        }
    }

    BRepTools::Write(result, "result.brep");

}

TEST(Test_naming_choosing_code, example_rectangle_triangle)
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
    Standard_Real d (8.0);

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

    geoml::Shape cut_result = rectangular_srf - triangular_srf;

    BRepTools::Write(rectangular_srf, "rectangular_srf.brep");

    BRepTools::Write(triangular_srf, "triangular_srf.brep");

    BRepTools::Write(cut_result, "cut_result.brep");

    std::cout << "written out" << std::endl;

    // extrusion direction
    gp_Vec direction(1., -1., 0.);  

    // Get an edge from the rectangular_srf. Selecting intended one here.
    auto rectangular_srf_edges = rectangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_EDGE); });
    std::cout << rectangular_srf_edges.size() << " edges in rectangular_srf\n";
    auto const&  intended_edge = *rectangular_srf_edges.at(2); // "pick" edge

    // Get a vertex from the rectangular_srf. Selecting intended one here.
    auto rectangular_srf_vertices = rectangular_srf.select_subshapes([](geoml::Shape const& s){ return s.is_type(TopAbs_VERTEX); });
    std::cout << rectangular_srf_vertices.size() << " edges in rectangular_srf\n";
    auto const&  intended_vertex = *rectangular_srf_vertices.at(3); // "pick" vertex

    //TopoDS_Shape extrudedShape = BRepPrimAPI_MakePrism(first_edge, direction);

    // BRepTools::Write(extrudedShape, "extrudedShape.brep");

    // Get edges in result that originate from the edge selected in rectangular_srf
    auto cut_result_edges = cut_result.select_subshapes([&](geoml::Shape const& s){
        return  s.is_type(TopAbs_EDGE) &&               
                s.is_descendent_of(intended_edge) &&
                s.has_subshape_that_is_child_of(intended_vertex);       
    });
    std::cout << cut_result_edges.size() << " edges in result that originate from the intended edge in rectangular_srf and have a vertex that is a child of the intended vertex in rectangular_srf\n";

    if(cut_result_edges.size() > 0) {
    auto const& result_intended_edge = *cut_result_edges.at(0); // there is only one entry in the std::vector

    //BRepTools::Write(result_intended_edge, "result_intended_edge.brep");

    TopoDS_Shape extrudedShape = BRepPrimAPI_MakePrism(result_intended_edge, direction);

    BRepTools::Write(extrudedShape, "extrudedShape.brep");
    }
    












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
            std::string filename = "fac111e_out" + std::to_string(idx++) + ".brep";
            BRepTools::Write(*face_out, filename.c_str());
        }
    }

    if (result_faces_from_cylinders.size() > 0) {
        int idx = 0;
        for (auto const& face_out : result_faces_from_cylinders)
        {
            std::string filename = "cyl111face_out" + std::to_string(idx++) + ".brep";
            BRepTools::Write(*face_out, filename.c_str());
        }
    }

    BRepTools::Write(result, "result.brep");

}


