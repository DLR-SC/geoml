#include "predicates/predicate_functions.h"

namespace geoml{

ShapePredicate operator&&(ShapePredicate const& l, ShapePredicate const& r)
{
    return ShapePredicate([=](Shape const& s){ return l(s) && r(s); });
}

ShapePredicate operator||(ShapePredicate const& l, ShapePredicate const& r)
{
    return ShapePredicate([=](Shape const& s){ return l(s) || r(s); });
}

ShapePredicate operator!(ShapePredicate const& pred)
{
    return ShapePredicate([=](Shape const& s){ return !pred(s); });
}

ShapePredicate has_metadata_tag(std::string const& tag)
{
    return ShapePredicate([tag](Shape const& shape) { return shape.has_tag(tag); });
}

ShapePredicate has_tag(std::string const& tag)
{
    return has_metadata_tag(tag);
}

ShapePredicate is_type(TopAbs_ShapeEnum shape_type)
{
    return ShapePredicate([=](Shape const& s){ return s.is_type(shape_type); });
}

ShapePredicate is_same(Shape const& other)
{
    return is_same(other.shape());
}

ShapePredicate is_same(TopoDS_Shape const& other)
{
    return ShapePredicate([=](Shape const& s){ return s.is_same(other); });
} 

ShapePredicate has_subshape(Shape const& shape)
{
    return ShapePredicate([=](Shape const& s){ return s.has_subshape(shape); });
}

ShapePredicate is_subshape_of(Shape const& other)
{
    return ShapePredicate([=](Shape const& s){ return other.has_subshape(s); });
}

ShapePredicate is_unmodified_descendent_of(Shape const& other, int max_depth)
{
    return ShapePredicate([=](Shape const& s){ return s.is_unmodified_descendent_of(other, max_depth); });
}

ShapePredicate is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth)
{
    return ShapePredicate([=](Shape const& s){ return s.is_unmodified_descendent_of_subshape_in(other, max_depth); });
}

ShapePredicate is_modified_descendent_of(Shape const& other, int max_depth)
{
    return ShapePredicate([=](Shape const& s){ return s.is_modified_descendent_of(other, max_depth); });
}

ShapePredicate is_modified_descendent_of_subshape_in(Shape const& other, int max_depth)
{
    return ShapePredicate([=](Shape const& s){ return s.is_modified_descendent_of_subshape_in(other, max_depth); });
}

ShapePredicate is_descendent_of_subshape_in(Shape const& other, int max_depth)
{
    return ShapePredicate([=](Shape const& s){ return s.is_descendent_of_subshape_in(other, max_depth); });
}

ShapePredicate is_descendent_of(Shape const& other, int max_depth)
{
    // A track stored on the source model must not keep that model alive through its predicate.
    return ShapePredicate([topology = other.shape(), max_depth](Shape const& shape) {
        return details::is_descendent_of(shape, topology, max_depth);
    });
}

ShapePredicate has_subshape_that(ShapePredicate const& pred) 
{
    return ShapePredicate([=](geoml::Shape const& s){ return s.has_subshape_that(pred); });
}

} // namespace geoml
