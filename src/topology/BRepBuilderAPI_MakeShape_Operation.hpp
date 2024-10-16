#pragma once

/**
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 */

#include "Operation.hpp"
#include "BRepBuilderAPI_MakeShape.hxx"

namespace geoml {

/**
 * @brief The BRepBuilderAPI_MakeShape_Operation class implements the Operation API based on
 * BRepBuilderAPI_MakeShape
 */
class BRepBuilderAPI_MakeShape_Operation : public Operation<BRepBuilderAPI_MakeShape_Operation>
{
public:
    BRepBuilderAPI_MakeShape_Operation(
        BRepBuilderAPI_MakeShape* algo,
        std::vector<Shape const*> const& inputs
    );

    Shape perform() const;
    void map_subshapes(Shape& result) const;

private:
    BRepBuilderAPI_MakeShape* m_algo;
   // std::vector<Shape const*> m_inputs; // Lifetime issue: Operation may not outlive the inputs!
};

} // namespace geoml
