#pragma once

/**
 * @brief geoml/curves/modeling.h includes functions to create and modify curves
 */
#include <geoml/geoml.h>

#include <geoml/predicates/ShapePredicate.hpp>
#include <geoml/naming_choosing/Shape.hpp>


namespace geoml
{

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
 * @brief Returns the predicate representing the conjunction of the two input predicates
 * 
 * 
 * @param l First predicate
 * @param r Second predicate
 */
std::function<bool(Shape const&)> AND(std::function<bool(Shape const&)> const& l, std::function<bool(Shape const&)> const& r);

/**
 * @brief Returns the predicate representing the conjunction of the two input predicates
 * 
 * 
 * @param l First predicate
 * @param r Second predicate
 */
std::function<bool(Shape const&)> operator&&(std::function<bool(Shape const&)> const& l, std::function<bool(Shape const&)> const& r);

/**
 * @brief Returns the ShapePredicate representing the disjunction of the two input ShapePredicates
 * 
 * 
 * @param l First ShapePredicate
 * @param r Second ShapePredicate
 */
ShapePredicate OR(ShapePredicate const& l, ShapePredicate const& r);

/**
 * @brief Returns the predicate representing the disjunction of the two input predicates
 * 
 * 
 * @param l First predicate
 * @param r Second predicate
 */
std::function<bool(Shape const&)> OR(std::function<bool(Shape const&)> const& l, std::function<bool(Shape const&)> const& r);

/**
 * @brief Returns the ShapePredicate representing the disjunction of the two input ShapePredicates
 * 
 * 
 * @param l First ShapePredicate
 * @param r Second ShapePredicate
 */
ShapePredicate operator||(ShapePredicate const& l, ShapePredicate const& r);

/**
 * @brief Returns the predicate representing the disjunction of the two input predicate
 * 
 * 
 * @param l First predicate
 * @param r Second predicate
 */
std::function<bool(Shape const&)> operator||(std::function<bool(Shape const&)> const& l, std::function<bool(Shape const&)> const& r);

ShapePredicate make_shape_predicate_has_tag(std::string const& tag);

std::function<bool(Shape const&)> make_predicate_has_tag(std::string const& tag);

ShapePredicate make_shape_predicate_has_subshape_that_is_child_of(Shape const &shape);

std::function<bool(Shape const&)> make_predicate_has_subshape_that_is_child_of(Shape const &shape);


} // namespace geoml
