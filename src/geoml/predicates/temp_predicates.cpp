#include "predicates/temp_predicates.h"

namespace geoml{

StringPredicate operator&&(StringPredicate const& l, StringPredicate const& r)
{
    return [=](std::string const& s){ return l(s) && r(s); };
}

// ShapePredicate operator||(ShapePredicate const& l, ShapePredicate const& r)
// {
//     return [=](Shape const& s){ return l(s) || r(s); };
// }

// ShapePredicate operator!(ShapePredicate const& pred)
// {
//     return [=](Shape const& s){ return !pred(s); };
// }

StringPredicate hhas_tag(std::string const& tag)
{
    return [=](std::string const& s){ return false; };
}

// ShapePredicate is_type(TopAbs_ShapeEnum shape_type)
// {
//     return [=](Shape const& s){ return s.is_type(shape_type); };
// }

// ShapePredicate is_same(Shape const& other)
// {
//     return [=](Shape const& s){ return s.is_same(other); };
// }

// ShapePredicate is_same(TopoDS_Shape const& other)
// {
//     return [=](Shape const& s){ return s.is_same(other); };
// } 

// ShapePredicate has_subshape(Shape const& shape)
// {
//     return [=](Shape const& s){ return s.has_subshape(shape); };
// }

// ShapePredicate is_subshape_of(Shape const& other)
// {
//     return [=](Shape const& s){ return other.has_subshape(s); };
// }

// ShapePredicate is_unmodified_descendent_of(Shape const& other, int max_depth)
// {
//     return [=](Shape const& s){ return s.is_unmodified_descendent_of(other, max_depth); };
// }

// ShapePredicate is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth)
// {
//     return [=](Shape const& s){ return s.is_unmodified_descendent_of_subshape_in(other, max_depth); };
// }

// ShapePredicate is_modified_descendent_of(Shape const& other, int max_depth)
// {
//     return [=](Shape const& s){ return s.is_modified_descendent_of(other, max_depth); };
// }

// ShapePredicate is_modified_descendent_of_subshape_in(Shape const& other, int max_depth)
// {
//     return [=](Shape const& s){ return s.is_modified_descendent_of_subshape_in(other, max_depth); };
// }

// ShapePredicate is_descendent_of_subshape_in(Shape const& other, int max_depth)
// {
//     return [=](Shape const& s){ return s.is_descendent_of_subshape_in(other, max_depth); };
// }

// ShapePredicate is_descendent_of(Shape const& other, int max_depth)
// {
//     return [=](Shape const& s){ return s.is_descendent_of(other, max_depth); };
// }

// ShapePredicate has_subshape_that(ShapePredicate const& pred) 
// {
//     return [=](geoml::Shape const& s){ return s.has_subshape_that(pred); };
// }

} // namespace geoml
