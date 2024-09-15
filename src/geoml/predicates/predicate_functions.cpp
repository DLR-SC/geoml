#include "predicates/predicate_functions.hpp"

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




ShapePredicate make_predicate_has_tag(std::string const& tag)
{
    return [&](geoml::Shape const& s){ return s.has_tag(tag); };
}

ShapePredicate make_predicate_has_subshape_that_is_child_of(Shape const &shape)
{
    return [&](geoml::Shape const& s){ return s.has_subshape_that_is_child_of(shape); };
}

ShapePredicate make_predicate_is_same(Shape const& other)
{
    return [&](geoml::Shape const& s){ return s.is_same(other); };
}

ShapePredicate make_predicate_is_same(TopoDS_Shape const& other)
{
    return [&](geoml::Shape const& s){ return s.is_same(other); };
}

ShapePredicate make_predicate_has_subshape(Shape const& shape)
{
    return [&](geoml::Shape const& s){ return s.has_subshape(shape); };
}

ShapePredicate make_predicate_has_origin()
{
    return [&](geoml::Shape const& s){ return s.has_origin(); };
}

ShapePredicate make_predicate_is_unmodified_descendent_of(Shape const& other, int max_depth)
{
    return [&](geoml::Shape const& s){ return s.is_unmodified_descendent_of(other, max_depth); };
}

ShapePredicate make_predicate_is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth)
{
    return [&](geoml::Shape const& s){ return s.is_unmodified_descendent_of_subshape_in(other, max_depth); };
}

ShapePredicate make_predicate_is_modified_descendent_of(Shape const& other, int max_depth)
{
    return [&](geoml::Shape const& s){ return s.is_modified_descendent_of(other, max_depth); };
}

ShapePredicate make_predicate_is_modified_descendent_of_subshape_in(Shape const& other, int max_depth)
{
    return [&](geoml::Shape const& s){ return s.is_modified_descendent_of_subshape_in(other, max_depth); };
}

ShapePredicate make_predicate_is_descendent_of_subshape_in(Shape const& other, int max_depth)
{
    return [&](geoml::Shape const& s){ return s.is_descendent_of_subshape_in(other, max_depth); };
}

ShapePredicate make_predicate_is_child_of_subshape_in(Shape const& other)
{
    return [&](geoml::Shape const& s){ return s.is_child_of_subshape_in(other); };
}

ShapePredicate make_predicate_is_descendent_of(Shape const& other, int max_depth)
{
    return [&](geoml::Shape const& s){ return s.is_descendent_of(other, max_depth); };
}

ShapePredicate make_predicate_is_child_of(Shape const& other)
{
    return [&](geoml::Shape const& s){ return s.is_child_of(other); };
}


} // namespace geoml
