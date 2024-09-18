#pragma once

/**
 * @brief geoml/curves/modeling.h includes functions to create and modify curves
 */
#include <geoml/geoml.h>

#include <geoml/naming_choosing/Shape.h>


namespace geoml
{

using ShapePredicate = std::function<bool(Shape const&)>;

/**
 * @brief Returns the ShapePredicate representing the conjunction of the two input ShapePredicates
 * 
 * 
 * @param l First ShapePredicate
 * @param r Second ShapePredicate
 */
GEOML_API_EXPORT ShapePredicate AND(ShapePredicate const& l, ShapePredicate const& r);

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
GEOML_API_EXPORT ShapePredicate OR(ShapePredicate const& l, ShapePredicate const& r);

/**
 * @brief Returns the ShapePredicate representing the disjunction of the two input ShapePredicates
 * 
 * 
 * @param l First ShapePredicate
 * @param r Second ShapePredicate
 */
GEOML_API_EXPORT ShapePredicate operator||(ShapePredicate const& l, ShapePredicate const& r);



GEOML_API_EXPORT ShapePredicate make_predicate_has_tag(std::string const& tag);

GEOML_API_EXPORT ShapePredicate make_predicate_has_subshape_that_is_child_of(Shape const &shape);

GEOML_API_EXPORT ShapePredicate make_predicate_is_type(TopAbs_ShapeEnum shape_type);

GEOML_API_EXPORT ShapePredicate make_predicate_is_same(Shape const& other);
GEOML_API_EXPORT ShapePredicate make_predicate_is_same(TopoDS_Shape const& other); 
GEOML_API_EXPORT ShapePredicate make_predicate_has_subshape(Shape const& shape);
GEOML_API_EXPORT ShapePredicate make_predicate_has_origin();
GEOML_API_EXPORT ShapePredicate make_predicate_is_unmodified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max());
GEOML_API_EXPORT ShapePredicate make_predicate_is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max());
GEOML_API_EXPORT ShapePredicate make_predicate_is_modified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max());
GEOML_API_EXPORT ShapePredicate make_predicate_is_modified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max());
GEOML_API_EXPORT ShapePredicate make_predicate_is_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max());
GEOML_API_EXPORT ShapePredicate make_predicate_is_child_of_subshape_in(Shape const& other);
GEOML_API_EXPORT ShapePredicate make_predicate_is_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max());
GEOML_API_EXPORT ShapePredicate make_predicate_is_child_of(Shape const& other);
// has_subshape_that_is_descendet_of
// has subshape_with(is_descendent_of(Shape const& other))

} // namespace geoml
