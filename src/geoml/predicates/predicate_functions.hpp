#pragma once

/**
 * @brief geoml/curves/modeling.h includes functions to create and modify curves
 */
#include <geoml/geoml.h>

#include <geoml/naming_choosing/Shape.hpp>


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
ShapePredicate AND(ShapePredicate const& l, ShapePredicate const& r);

/**
 * @brief Returns the ShapePredicate representing the conjunction of the two input ShapePredicates
 * 
 * 
 * @param l First ShapePredicate
 * @param r Second ShapePredicate
 */
ShapePredicate operator&&(ShapePredicate const& l, ShapePredicate const& r);

/**
 * @brief Returns the ShapePredicate representing the disjunction of the two input ShapePredicates
 * 
 * 
 * @param l First ShapePredicate
 * @param r Second ShapePredicate
 */
ShapePredicate OR(ShapePredicate const& l, ShapePredicate const& r);

/**
 * @brief Returns the ShapePredicate representing the disjunction of the two input ShapePredicates
 * 
 * 
 * @param l First ShapePredicate
 * @param r Second ShapePredicate
 */
ShapePredicate operator||(ShapePredicate const& l, ShapePredicate const& r);



ShapePredicate make_predicate_has_tag(std::string const& tag);

ShapePredicate make_predicate_has_subshape_that_is_child_of(Shape const &shape);


} // namespace geoml
