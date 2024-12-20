#pragma once

/**
 * @brief geoml/boolean_operations/modeling.hpp includes functions to perform boolean operations
 */
#include <geoml/geoml.h>

#include "geoml/naming_choosing/Shape.h"

namespace geoml
{

/**
 * @brief A function that subracts a Shape (cutting tool) from a Shape. This function has a history mapping.
 * 
 * 
 * @param shape The Shape from which the cutting tool should be subtracted
 * @param cutting_tool The Shape that is the cutting tool
 */
Shape boolean_subtract(Shape const& shape, Shape const& cutting_tool);

/**
 * @brief A function that subracts a Shape (cutting tool) from a Shape. This function has a history mapping.
 * 
 * 
 * @param shape The Shape from which the cutting tool should be subtracted
 * @param cutting_tool The Shape that is the cutting tool
 */
Shape operator-(Shape const& shape, Shape const& cutting_tool);

/**
 * @brief A function that returns the boolean union of two Shapes. This function has a history mapping.
 * 
 * 
 * @param shape_1 The first Shape
 * @param shape_2 The second Shape
 */
Shape boolean_union(Shape const& shape_1, Shape const& shape_2);

/**
 * @brief A function that returns the boolean union of two Shapes. This function has a history mapping.
 * 
 * 
 * @param shape_1 The first Shape
 * @param shape_2 The second Shape
 */
Shape operator+(Shape const& shape_1, Shape const& shape_2);


} // namespace geoml

