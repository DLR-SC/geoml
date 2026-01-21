#pragma once

/**
 * @brief geoml/curves/modeling.h includes functions to create and modify curves
 */
#include <geoml/geoml.h>

#include <geoml/naming_choosing/Shape.h>


namespace geoml
{

/**
 * @brief Returns the ShapePredicate representing the conjunction of the two input ShapePredicates
 * 
 * 
 * @param l First ShapePredicate
 * @param r Second ShapePredicate
 */
GEOML_API_EXPORT ShapePredicate operator&&(ShapePredicate const& l, ShapePredicate const& r);

/**
 * @brief Returns the ShapePredicate representing the disjunction of the two input ShapePredicates
 * 
 * 
 * @param l First ShapePredicate
 * @param r Second ShapePredicate
 */
GEOML_API_EXPORT ShapePredicate operator||(ShapePredicate const& l, ShapePredicate const& r);

GEOML_API_EXPORT ShapePredicate operator!(ShapePredicate const& pred);


// This is necessary, because SWIG had difficulty parsing the inline definitions with lambda functions
// directly
namespace details {
    inline bool _is_vertex(Shape const& s){ return s.is_type(TopAbs_VERTEX); }
    inline bool _is_edge(Shape const& s){ return s.is_type(TopAbs_EDGE); }
    inline bool _is_face(Shape const& s){ return s.is_type(TopAbs_FACE); }
    inline bool _is_solid(Shape const& s){ return s.is_type(TopAbs_SOLID); }
    inline bool _has_origin(Shape const& s){ return s.has_origin(); }
}

inline ShapePredicate const is_vertex = ShapePredicate(&details::_is_vertex);
inline ShapePredicate const is_edge = ShapePredicate(&details::_is_edge);
inline ShapePredicate const is_face = ShapePredicate(&details::_is_face);
inline ShapePredicate const is_solid = ShapePredicate(&details::_is_solid);
inline ShapePredicate const has_origin = ShapePredicate(&details::_has_origin);

// some free functions returning shape predicates
GEOML_API_EXPORT ShapePredicate has_tag(std::string const& tag);
GEOML_API_EXPORT ShapePredicate is_type(TopAbs_ShapeEnum shape_type);
GEOML_API_EXPORT ShapePredicate is_same(Shape const& other);
GEOML_API_EXPORT ShapePredicate is_same(TopoDS_Shape const& other); 
GEOML_API_EXPORT ShapePredicate has_subshape(Shape const& shape);
GEOML_API_EXPORT ShapePredicate is_subshape_of(Shape const& other);
GEOML_API_EXPORT ShapePredicate is_unmodified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max());
GEOML_API_EXPORT ShapePredicate is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max());
GEOML_API_EXPORT ShapePredicate is_modified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max());
GEOML_API_EXPORT ShapePredicate is_modified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max());
GEOML_API_EXPORT ShapePredicate is_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max());
GEOML_API_EXPORT ShapePredicate is_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max());
GEOML_API_EXPORT ShapePredicate has_subshape_that(ShapePredicate const& pred);


} // namespace geoml
