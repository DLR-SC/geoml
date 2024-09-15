#include "boolean_ops/modeling.h"
#include "topology/BRepBuilderAPI_MakeShape_Operation.h"

#include "BRepAlgoAPI_Cut.hxx"
#include "BRepAlgoAPI_Fuse.hxx"

namespace geoml{

Shape boolean_subtract (Shape const& shape, Shape const& cutting_tool)
{
    BRepAlgoAPI_Cut cutter(shape, cutting_tool);
    auto operation = BRepBuilderAPI_MakeShape_Operation(&cutter, {&shape,&cutting_tool});
    return operation.value();
}

Shape operator-(Shape const& shape, Shape const& cutting_tool)
{
    return boolean_subtract(shape, cutting_tool);
}


} // namespace geoml

