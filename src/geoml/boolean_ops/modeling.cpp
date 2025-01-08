#include "boolean_ops/modeling.h"
#include "topology/BRepBuilderAPI_MakeShape_Operation.h"
#include "Splitter.h"

#include "BRepAlgoAPI_Cut.hxx"
#include "BRepAlgoAPI_Fuse.hxx"

namespace geoml{

Shape boolean_subtract (Shape const& shape, Shape const& cutting_tool)
{
    BRepAlgoAPI_Cut cutter(shape, cutting_tool);
    auto operation = BRepBuilderAPI_MakeShape_Operation(&cutter, {shape, cutting_tool});
    return operation.value();
}

Shape operator-(Shape const& shape, Shape const& cutting_tool)
{
    return boolean_subtract(shape, cutting_tool);
}

Shape boolean_union(Shape const& shape_1, Shape const& shape_2)
{
    BRepAlgoAPI_Fuse fuser(shape_1, shape_2);
    auto operation = BRepBuilderAPI_MakeShape_Operation(&fuser, {shape_1, shape_2});
    return operation.value();
}

Shape operator+(Shape const& shape_1, Shape const& shape_2)
{
    return boolean_union(shape_1, shape_2);
}

Shape split(Shape const& shape, Shape const& cutting_tool)
{
    Splitter splitter(shape, cutting_tool);
    return splitter.value();
}

} // namespace geoml

