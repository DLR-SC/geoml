// Select subshapes of a box with predicates
#include <cmath>
#include <iostream>
#include <string>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

#include "geoml/naming_choosing/Shape.h"
#include "geoml/predicates/predicate_functions.h"
#include "geoml/primitives/modeling.hpp"

namespace
{
// A custom predicate: true for faces whose centre of mass lies at height z.
// Any callable taking a geoml::Shape and returning a bool can be wrapped in a
// geoml::ShapePredicate and then be combined with the built-in predicates.
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

int failures = 0;
void expect_count(std::string const& what, geoml::Shape const& selection, size_t expected)
{
    std::cout << what << ": " << selection.size() << "\n";
    if (selection.size() != expected) {
        std::cout << "  ERROR: expected " << expected << "\n";
        ++failures;
    }
}
} // namespace

int main()
{
    using namespace geoml;

    Shape box = create_box(10., 10., 10.);

    // Select subshapes by type. select_subshapes searches the whole topology
    // and returns each matching subshape once, wrapped in a compound Shape.
    expect_count("vertices", box.select_subshapes(is_vertex), 8);
    expect_count("edges", box.select_subshapes(is_edge), 12);
    expect_count("faces", box.select_subshapes(is_face), 6);

    // Use a custom predicate and unwrap the single result with unique_element().
    // unique_element() throws if the selection does not contain exactly one shape.
    Shape top = box.select_subshapes(is_face_at_height(10.)).unique_element();
    Shape bottom = box.select_subshapes(is_face_at_height(0.)).unique_element();

    // Combine predicates with &&, || and !
    Shape top_edges = box.select_subshapes(is_edge && is_subshape_of(top));
    expect_count("edges of the top face", top_edges, 4);

    Shape vertical_edges = box.select_subshapes(
        is_edge && !is_subshape_of(top) && !is_subshape_of(bottom));
    expect_count("vertical edges", vertical_edges, 4);

    // Select by topological relations: faces that share an edge with the top face
    Shape side_faces = box.select_subshapes(
        is_face && !is_same(top) && has_subshape_that(is_edge && is_subshape_of(top)));
    expect_count("side faces", side_faces, 4);

    // Selections are Shapes themselves and can be searched further, e.g. the
    // vertices of the side faces that lie on the bottom face
    Shape bottom_vertices = side_faces.select_subshapes(is_vertex && is_subshape_of(bottom));
    expect_count("bottom vertices", bottom_vertices, 4);

    // Iterate over a selection
    for (Shape const& edge : top_edges) {
        if (!edge.is_type(TopAbs_EDGE) || !top.has_subshape(edge)) {
            ++failures;
        }
    }

    std::cout << (failures == 0 ? "OK" : "FAILED") << std::endl;
    return failures == 0 ? 0 : 1;
}
