// Tag subshapes and keep tags alive through modelling operations with tag tracks
#include <cmath>
#include <iostream>
#include <string>

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
geoml::ShapePredicate const is_cylindrical_face([](geoml::Shape const& s) {
    return s.is_type(TopAbs_FACE) &&
           BRepAdaptor_Surface(TopoDS::Face(s.shape())).GetType() == GeomAbs_Cylinder;
});

// Helper that prints a result and counts the unexpected ones
int failures = 0;

void expect_count(std::string const& what, geoml::Shape const& selection, size_t expected)
{
    std::cout << what << ": " << selection.size() << "\n";
    if (selection.size() != expected) {
        std::cout << "  ERROR: expected " << expected << "\n";
        ++failures;
    }
}

// A cylinder that drills a hole through the box
geoml::Shape make_drill()
{
    return geoml::Shape(geoml::translate(geoml::create_cylinder(2., 12.), gp_Vec(5., 5., -1.)));
}

// A small box at (x, y, 8) that cuts away a top corner of the box
geoml::Shape make_corner_tool(double x, double y)
{
    return geoml::Shape(geoml::translate(geoml::create_box(3., 3., 3.), gp_Vec(x, y, 8.)));
}

int main()
{
    using namespace geoml;

    // ------------------------------------------------------------------
    // 1. Meta tags: attach names to subshapes and select them later
    // ------------------------------------------------------------------
    Shape box = create_box(10., 10., 10.);

    box.add_meta_tag_to_subshapes(is_face_at_height(10.), "top");
    box.add_meta_tag_to_subshapes(is_face_at_height(0.), "bottom");
    // the free function does the same as the member function
    add_persistent_meta_tag_to_subshapes(box, is_edge, "edge");

    expect_count("faces tagged 'top'", box.select_subshapes(has_tag("top")), 1);
    expect_count("faces tagged 'top' or 'bottom'",
                 box.select_subshapes(has_tag("top") || has_tag("bottom")), 2);
    expect_count("edges tagged 'edge'", box.select_subshapes(has_tag("edge")), 12);

    // Meta tags belong to the subshapes of this shape. A modelling operation
    // creates new subshapes, so the tags are not part of the result.
    Shape drilled = box - make_drill();
    expect_count("faces tagged 'top' after drilling", drilled.select_subshapes(has_tag("top")), 0);

    // ------------------------------------------------------------------
    // 2. Tag tracks: re-apply a tag after each of the next N operations
    // ------------------------------------------------------------------
    Shape part = create_box(10., 10., 10.);
    Shape drill = make_drill();

    Shape top = part.select_subshapes(is_face_at_height(10.)).unique_element();
    Shape drill_wall = drill.select_subshapes(is_cylindrical_face).unique_element();

    // A tag track stores a tag, a criterion and the number of modelling steps
    // it stays active. Its criterion is evaluated on the result of every
    // operation that has the shape as input, so it can use history predicates
    // to find the subshapes that originate from the tagged input subshapes.
    part.add_tag_track(TagTrack("top", is_face && is_descendent_of(top), 2));
    drill.add_tag_track(TagTrack("bore", is_face && is_descendent_of(drill_wall), 2));

    // apply_tag_tracks() also tags the input shape itself
    part.apply_tag_tracks();
    expect_count("part faces tagged 'top'", part.select_subshapes(has_tag("top")), 1);

    // Step 1: the tag tracks of both inputs are passed on to the result
    Shape step1 = part - drill;
    expect_count("step 1: faces tagged 'top'", step1.select_subshapes(has_tag("top")), 1);
    expect_count("step 1: faces tagged 'bore'", step1.select_subshapes(has_tag("bore")), 1);

    // Step 2: the tags are still applied, although the top face is modified again
    Shape step2 = step1 - make_corner_tool(8., 8.);
    expect_count("step 2: faces tagged 'top'", step2.select_subshapes(has_tag("top")), 1);
    expect_count("step 2: faces tagged 'bore'", step2.select_subshapes(has_tag("bore")), 1);

    // Step 3: the tag tracks were limited to two steps and have expired
    Shape step3 = step2 - make_corner_tool(-1., -1.);
    expect_count("step 3: faces tagged 'top'", step3.select_subshapes(has_tag("top")), 0);

    // Tags can be combined with all other predicates, e.g. the circular edge
    // where the bore meets the top face
    Shape bore = step2.select_subshapes(has_tag("bore")).unique_element();
    Shape top_face = step2.select_subshapes(has_tag("top")).unique_element();
    expect_count("edges between 'bore' and 'top' after step 2",
                 step2.select_subshapes(is_edge && is_subshape_of(bore) && is_subshape_of(top_face)), 1);

    std::cout << (failures == 0 ? "OK" : "FAILED") << std::endl;
    return failures == 0 ? 0 : 1;
}
