// Selection cases: L = (Case_1 and Selection_1) or (Case_2 and Selection_2)
#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

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

// Returns the selection of the one case that applies
geoml::ShapePredicate select_case(std::vector<std::pair<bool, geoml::ShapePredicate>> const& cases)
{
    int n_applying = 0;
    for (auto const& c : cases) {
        n_applying += c.first ? 1 : 0;
    }
    if (n_applying != 1) {
        throw std::logic_error("the cases are not mutually exclusive and exhaustive");
    }
    for (auto const& c : cases) {
        if (c.first) {
            return c.second;
        }
    }
    throw std::logic_error("unreachable");
}

// A box B, moved along the frontal upper edge X of a box by d, is subtracted
// from the box. Modelling intention:
//   case 1, B cuts X:         fillet the edges of the notch on the front face
//   case 2, B does not cut X: fillet X
geoml::Shape make_part(double d)
{
    geoml::Shape box = geoml::create_box(10., 6., 4.);
    geoml::Shape front = box.select_subshapes(is_face_at(1, 0.)).unique_element();
    geoml::Shape top = box.select_subshapes(is_face_at(2, 4.)).unique_element();
    geoml::Shape X = box.select_subshapes(
        geoml::is_edge && geoml::is_subshape_of(front) && geoml::is_subshape_of(top)).unique_element();

    geoml::Shape B = BRepPrimAPI_MakeBox(gp_Pnt(d, -1., 3.), 2., 2., 2.).Shape();
    geoml::Shape result = box - B;

    // Cases, formulated with a history predicate: B cuts X if the result
    // contains a modified descendent of X
    bool case_1 = !result.select_subshapes(geoml::is_modified_descendent_of(X)).is_empty();
    bool case_2 = !case_1;

    // Selections: the edges shared by the front face and the faces from B,
    // and the edge X
    geoml::Shape front_now =
        result.select_subshapes(geoml::is_face && geoml::is_descendent_of(front)).unique_element();
    geoml::Shape faces_from_B =
        result.select_subshapes(geoml::is_face && geoml::is_descendent_of_subshape_in(B));
    geoml::ShapePredicate selection_1 =
        geoml::is_edge && geoml::is_subshape_of(front_now) && geoml::is_subshape_of(faces_from_B);
    geoml::ShapePredicate selection_2 = geoml::is_edge && geoml::is_descendent_of(X);

    // The selection criterion L
    geoml::ShapePredicate L = select_case({{case_1, selection_1}, {case_2, selection_2}});

    geoml::Shape edges = result.select_subshapes(L);
    return geoml::make_fillet(result, edges, 0.3);
}

int main()
{
    geoml::Shape part_case_1 = make_part(4.);  // B cuts X: three edges of the notch filleted
    geoml::Shape part_case_2 = make_part(12.); // B misses the box: X filleted

    return 0;
}
