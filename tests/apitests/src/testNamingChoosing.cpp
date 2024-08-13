#include <geoml/naming_choosing/BRepBuilderAPI_MakeShape_Operation.hpp>
#include <geoml/naming_choosing/Shape.hpp>
#include <geoml/naming_choosing/Operation.hpp>
#include <geoml/naming_choosing/MetaDataStore.hpp>
#include <geoml/naming_choosing/common.hpp>

#include <gtest/gtest.h>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include "BRepTools.hxx"

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


