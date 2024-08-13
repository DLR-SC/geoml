#pragma once

#include "Operationtion.hpp"
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
        std::string const& id,
        BRepBuilderAPI_MakeShape* algo,
        std::vector<Shape const*> const& inputs
    );

    std::string id() const;
    Shape perform() const;
    void map_subshapes(Shape& result) const;

private:
    std::string m_id;
    BRepBuilderAPI_MakeShape* m_algo;
    std::vector<Shape const*> m_inputs; // Lifetime issue: Operation may not outlive the inputs!
};


// Some convenience operators for Boolean operations from BRepBuilderAPI_MakeShape
Shape operator-(Shape const& l, Shape const& r);
Shape operator+(Shape const& l, Shape const& r);

} // namespace geoml
