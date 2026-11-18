// Track subshapes through modelling operations with history predicates
#include <cmath>

#include <BRepAdaptor_Surface.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS.hxx>
#include <gp_Vec.hxx>

#include "geoml/boolean_ops/modeling.hpp"
#include "geoml/naming_choosing/Shape.h"
#include "geoml/predicates/predicate_functions.h"
#include "geoml/primitives/modeling.hpp"
#include "geoml/transformations/transformations.h"

// Selects the faces whose centre of mass lies at height z
geoml::ShapePredicate is_face_at_height(double z)
{
    return geoml::ShapePredicate([z](geoml::Shape const& s) {
        if (!s.is_type(TopAbs_FACE)) {
            return false;
        }
        GProp_GProps props;
        BRepGProp::SurfaceProperties(s.shape(), props);
        return std::abs(props.CentreOfMass().Z() - z) < 1e-6;
    });
}

// Selects the cylindrical faces
geoml::ShapePredicate const select_cylindrical_face([](geoml::Shape const& s) {
    return s.is_type(TopAbs_FACE) &&
           BRepAdaptor_Surface(TopoDS::Face(s.shape())).GetType() == GeomAbs_Cylinder;
});

int main()
{
    // Input shapes: a box and a cylinder that drills a hole through it
    geoml::Shape box = geoml::create_box(10., 10., 10.);
    geoml::Shape drill =
        geoml::Shape(geoml::translate(geoml::create_cylinder(2., 12.), gp_Vec(5., 5., -1.)));

    // Pick the subshapes we want to follow through the modelling operations
    geoml::Shape top = box.select_subshapes(is_face_at_height(10.)).unique_element();
    geoml::Shape drill_wall = drill.select_subshapes(select_cylindrical_face).unique_element();

    // Modelling step 1: drill the hole
    geoml::Shape drilled = box - drill;

    // The top face got a hole, so it is a *modified* descendent of the box's top face
    geoml::Shape drilled_top =
        drilled.select_subshapes(geoml::is_face && geoml::is_descendent_of(top)).unique_element();
    bool top_was_modified = drilled_top.is_modified_descendent_of(top); // true

    // Faces of the box that were not touched by the drill
    geoml::Shape unmodified_faces = drilled.select_subshapes(
        geoml::is_face && geoml::is_unmodified_descendent_of_subshape_in(box)); // 4 faces
    geoml::Shape modified_faces = drilled.select_subshapes(
        geoml::is_face && geoml::is_modified_descendent_of_subshape_in(box)); // 2 faces

    // The wall of the hole originates from the cylinder's lateral face
    geoml::Shape hole_wall =
        drilled.select_subshapes(geoml::is_face && geoml::is_descendent_of(drill_wall))
            .unique_element();

    // Edges where the hole meets the top face
    geoml::Shape rim = drilled.select_subshapes(
        geoml::is_edge && geoml::is_subshape_of(hole_wall) && geoml::is_subshape_of(drilled_top));

    // Modelling step 2: cut away a corner of the box
    geoml::Shape corner =
        geoml::Shape(geoml::translate(geoml::create_box(3., 3., 3.), gp_Vec(8., 8., 8.)));
    geoml::Shape result = drilled - corner;

    // History predicates follow the history over several modelling steps
    geoml::Shape result_top =
        result.select_subshapes(geoml::is_face && geoml::is_descendent_of(top)).unique_element();
    geoml::Shape result_hole_wall =
        result.select_subshapes(geoml::is_face && geoml::is_descendent_of(drill_wall))
            .unique_element();
    geoml::Shape corner_faces = result.select_subshapes(
        geoml::is_face && geoml::is_descendent_of_subshape_in(corner)); // 3 faces

    // Faces of the result that originate from neither tool
    geoml::Shape box_only_faces = result.select_subshapes(
        geoml::is_face && !geoml::is_descendent_of_subshape_in(drill) &&
        !geoml::is_descendent_of_subshape_in(corner)); // 6 faces

    return 0;
}
