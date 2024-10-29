#pragma once

#include "Operation.h"
#include "geoml/naming_choosing/Shape.h"
#include "TopoDS_Compound.hxx"
#include "BRep_Builder.hxx"

namespace geoml {

/**
 * @brief The BRepBuilderAPI_MakeShape_Operation class implements the Operation API based on
 * BRepBuilderAPI_MakeShape
 */
class MakeCompound_Operation : public Operation<MakeCompound_Operation>
{
public:
    MakeCompound_Operation(
        std::vector<std::shared_ptr<Shape>> const& inputs
    );

    Shape perform() const;
    void map_subshapes(Shape& result) const;
private:
    std::vector<std::shared_ptr<Shape>> m_shapes;
};

} // namespace geoml
