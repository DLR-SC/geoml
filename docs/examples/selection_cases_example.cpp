// Selection cases: L = (case_1 && selection_1) || (case_2 && selection_2)
#include <cmath>

#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <gp_Pnt.hxx>

#include "geoml/boolean_ops/modeling.hpp"
#include "geoml/naming_choosing/Shape.h"
#include "geoml/predicates/predicate_functions.h"
#include "geoml/primitives/modeling.hpp"
#include "geoml/utilities/utilities.h"

// Selects the faces whose centre of mass has the coordinate `value` in
// direction `axis` (0: x, 1: y, 2: z)
geoml::ShapePredicate is_face_at(int axis, double value)
{
    return geoml::ShapePredicate([axis, value](geoml::Shape const& s) {
        if (!s.is_type(TopAbs_FACE)) {
            return false;
        }
        GProp_GProps props;
        BRepGProp::SurfaceProperties(s.shape(), props);
        return std::abs(props.CentreOfMass().Coord(axis + 1) - value) < 1e-6;
    });
}

// A box with a notch of width 2 at position d in its frontal upper edge X.
// Modelling intention:
//   case 1 (d >= 3): fillet both pieces of X
//   case 2 (d <  3): fillet only the piece of X right of the notch
geoml::Shape make_part(double d)
{
    geoml::Shape box = geoml::create_box(10., 6., 4.);
    geoml::Shape front = box.select_subshapes(is_face_at(1, 0.)).unique_element();
    geoml::Shape top = box.select_subshapes(is_face_at(2, 4.)).unique_element();
    geoml::Shape right = box.select_subshapes(is_face_at(0, 10.)).unique_element();
    geoml::Shape X = box.select_subshapes(
        geoml::is_edge && geoml::is_subshape_of(front) && geoml::is_subshape_of(top)).unique_element();

    geoml::Shape tool = BRepPrimAPI_MakeBox(gp_Pnt(d, -1., 3.), 2., 2., 2.).Shape();
    geoml::Shape notched = box - tool;

    // Selections: the pieces of X, and the piece of X that touches the right face
    geoml::Shape right_now =
        notched.select_subshapes(geoml::is_face && geoml::is_descendent_of(right)).unique_element();
    geoml::ShapePredicate pieces_of_X = geoml::is_edge && geoml::is_descendent_of(X);
    geoml::ShapePredicate right_piece_of_X =
        pieces_of_X && geoml::has_subshape_that(geoml::is_vertex && geoml::is_subshape_of(right_now));

    // Cases: conditions on the parameter only, mutually exclusive and exhaustive
    geoml::ShapePredicate case_1([d](geoml::Shape const&) { return d >= 3.; });
    geoml::ShapePredicate case_2 = !case_1;

    // The selection criterion L
    geoml::ShapePredicate L = (case_1 && pieces_of_X) || (case_2 && right_piece_of_X);

    geoml::Shape edges = notched.select_subshapes(L);
    return geoml::make_fillet(notched, edges, 0.5);
}

int main()
{
    geoml::Shape part_case_2 = make_part(2.); // one edge filleted
    geoml::Shape part_case_1 = make_part(5.); // two edges filleted

    return 0;
}
