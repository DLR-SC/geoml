#include "predicates/predicate_functions.h"

namespace geoml{

ShapePredicate AND(ShapePredicate const& l, ShapePredicate const& r)
{
    return [=](Shape const& s){ return l(s) && r(s); };
}

ShapePredicate operator&&(ShapePredicate const& l, ShapePredicate const& r)
{
    return AND(l,r);
}

ShapePredicate OR(ShapePredicate const& l, ShapePredicate const& r)
{
    return [=](Shape const& s){ return l(s) || r(s); };
}

ShapePredicate operator||(ShapePredicate const& l, ShapePredicate const& r)
{
    return OR(l,r);
}

bool is_vertex(Shape const& s) {
    return s.is_type(TopAbs_VERTEX);
}

bool is_edge(Shape const& s) {
    return s.is_type(TopAbs_EDGE);
}

bool is_face(Shape const& s) {
    return s.is_type(TopAbs_FACE);
}

bool is_solid(Shape const& s) {
    return s.is_type(TopAbs_SOLID);
}

bool has_origin(Shape const& s) {
    return s.has_origin();
}

ShapePredicate has_tag(std::string const& tag)
{
    return [=](Shape const& s){ return s.has_tag(tag); };
}

ShapePredicate is_type(TopAbs_ShapeEnum shape_type)
{
    return [=](Shape const& s){ return s.is_type(shape_type); };
}

ShapePredicate is_same(Shape const& other)
{
    return [=](Shape const& s){ return s.is_same(other); };
}

ShapePredicate is_same(TopoDS_Shape const& other)
{
    return [=](Shape const& s){ return s.is_same(other); };
} 

ShapePredicate has_subshape(Shape const& shape)
{
    return [=](Shape const& s){ return s.has_subshape(shape); };
}

ShapePredicate is_unmodified_descendent_of(Shape const& other, int max_depth)
{
    return [=](Shape const& s){ return s.is_unmodified_descendent_of(other, max_depth); };
}

ShapePredicate is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth)
{
    return [=](Shape const& s){ return s.is_unmodified_descendent_of_subshape_in(other, max_depth); };
}

ShapePredicate is_modified_descendent_of(Shape const& other, int max_depth)
{
    return [=](Shape const& s){ return s.is_modified_descendent_of(other, max_depth); };
}

ShapePredicate is_modified_descendent_of_subshape_in(Shape const& other, int max_depth)
{
    return [=](Shape const& s){ return s.is_modified_descendent_of_subshape_in(other, max_depth); };
}

ShapePredicate is_descendent_of_subshape_in(Shape const& other, int max_depth)
{
    return [=](Shape const& s){ return s.is_descendent_of_subshape_in(other, max_depth); };
}

ShapePredicate is_child_of_subshape_in(Shape const& other)
{
    return [=](Shape const& s){ return s.is_child_of_subshape_in(other); };
}

ShapePredicate is_descendent_of(Shape const& other, int max_depth)
{
    return [=](Shape const& s){ return s.is_descendent_of(other, max_depth); };
}

ShapePredicate is_child_of(Shape const& other)
{
    return [=](Shape const& s){ return s.is_child_of(other); };
}

ShapePredicate has_subshape_that(ShapePredicate const& pred) 
{
    return [=](geoml::Shape const& s){ return s.has_subshape_that(pred); };
}

} // namespace geoml
