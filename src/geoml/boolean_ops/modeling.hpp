#pragma once

/**
 * @brief geoml/boolean_operations/modeling.hpp includes functions to perform boolean operations
 */
#include <geoml/geoml.h>
#include <geoml/modeling/ModelingContext.h>

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
 * Subtract `cutting_tool` from `shape` with metadata tracking.
 * @param shape Shape from which material is removed.
 * @param cutting_tool Shape used as the cutting tool.
 * @param context Modeling context owning active metadata tracks and policy scopes.
 * @return The subtraction result with its modeling history and propagated metadata.
 */
GEOML_API_EXPORT Shape boolean_subtract(Shape const& shape,
                                        Shape const& cutting_tool,
                                        ModelingContext& context);

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
 * Return the boolean union of two shapes with metadata tracking.
 * @param shape_1 First input shape.
 * @param shape_2 Second input shape.
 * @param context Modeling context owning active metadata tracks and policy scopes.
 * @return The union result with its modeling history and propagated metadata.
 */
GEOML_API_EXPORT Shape boolean_union(
    Shape const& shape_1,
    Shape const& shape_2,
    ModelingContext& context);

/**
 * @brief A function that returns the boolean union of two Shapes. This function has a history mapping.
 * 
 * 
 * @param shape_1 The first Shape
 * @param shape_2 The second Shape
 */
Shape operator+(Shape const& shape_1, Shape const& shape_2);


} // namespace geoml

