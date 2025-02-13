#pragma once

#include "Operation.h"
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
        std::vector<Shape> const& inputs
    );
    virtual ~BRepBuilderAPI_MakeShape_Operation() = default;

    virtual Shape perform();
    virtual void map_subshapes(Shape& result) const;

private:
    BRepBuilderAPI_MakeShape* m_algo;
};

} // namespace geoml
