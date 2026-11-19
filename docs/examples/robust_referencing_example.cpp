// Robust referencing in a parametric model: fillet the rim of a drilled hole
#include <cmath>
#include <utility>

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
#include "geoml/utilities/utilities.h"

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

// The parametric model: a box with a hole and a fillet on the edge where the
// hole meets the top face. The parameters change the topology of the result,
// the references below stay valid for all of them.
geoml::Shape make_part(double hole_x, double hole_radius, double fillet_radius)
{
    geoml::Shape box = geoml::create_box(10., 10., 10.);
    geoml::Shape drill = geoml::Shape(geoml::translate(
        geoml::create_cylinder(hole_radius, 12.), gp_Vec(hole_x, 5., -1.)));

    // Reference the faces of the inputs that the model is built on
    geoml::Shape top = box.select_subshapes(is_face_at_height(10.)).unique_element();
    geoml::Shape drill_wall = drill.select_subshapes(select_cylindrical_face).unique_element();

    // Name them with tag tracks: the tags are re-applied to their descendents
    // after each of the next two modelling operations
    box.add_tag_track(geoml::TagTrack("top", geoml::is_face && geoml::is_descendent_of(top), 2));
    drill.add_tag_track(
        geoml::TagTrack("bore", geoml::is_face && geoml::is_descendent_of(drill_wall), 2));

    // Modelling step 1: drill the hole
    geoml::Shape drilled = box - drill;

    // The rim is the edge between the descendents of the top face and of the
    // drill wall. It is found by history predicates, not by an index.
    geoml::Shape drilled_top =
        drilled.select_subshapes(geoml::is_face && geoml::is_descendent_of(top)).unique_element();
    geoml::Shape hole_wall =
        drilled.select_subshapes(geoml::is_face && geoml::is_descendent_of(drill_wall))
            .unique_element();
    geoml::Shape rim = drilled.select_subshapes(
        geoml::is_edge && geoml::is_subshape_of(drilled_top) && geoml::is_subshape_of(hole_wall));

    // Modelling step 2: fillet the rim
    geoml::Shape part = geoml::make_fillet(drilled, rim, fillet_radius);

    // The fillet face is the only face that does not descend from the drilled box
    part.add_meta_tag_to_subshapes(
        geoml::is_face && !geoml::is_descendent_of_subshape_in(drilled), "fillet");

    return part;
}

int main()
{
    // Two variants of the same model with different hole positions and radii
    for (auto [hole_x, hole_radius] : {std::pair{5., 3.}, std::pair{4., 2.}}) {
        geoml::Shape part = make_part(hole_x, hole_radius, 1.);

        // The tagged faces can be selected by name in every variant
        geoml::Shape top = part.select_subshapes(geoml::has_tag("top")).unique_element();
        geoml::Shape bore = part.select_subshapes(geoml::has_tag("bore")).unique_element();
        geoml::Shape fillet = part.select_subshapes(geoml::has_tag("fillet")).unique_element();

        // e.g. the edge where the fillet meets the top face
        geoml::Shape fillet_edge = part.select_subshapes(
            geoml::is_edge && geoml::is_subshape_of(fillet) && geoml::is_subshape_of(top));
    }

    return 0;
}
