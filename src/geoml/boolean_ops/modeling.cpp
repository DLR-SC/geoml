#include "boolean_ops/modeling.hpp"
#include "topology/BRepBuilderAPI_MakeShape_Operation.hpp"

#include "BRepAlgoAPI_Cut.hxx"
#include "BRepAlgoAPI_Fuse.hxx"

namespace geoml{

namespace {

Shape boolean_subtract_impl(Shape const& shape, Shape const& cutting_tool, ModelingContext* context)
{
    BRepAlgoAPI_Cut cutter(shape, cutting_tool);
    auto operation = BRepBuilderAPI_MakeShape_Operation(cutter, {shape, cutting_tool});
    return operation.value(context);
}

Shape boolean_union_impl(Shape const& shape_1, Shape const& shape_2, ModelingContext* context)
{
    BRepAlgoAPI_Fuse fuser(shape_1, shape_2);
    auto operation = BRepBuilderAPI_MakeShape_Operation(fuser, {shape_1, shape_2});
    return operation.value(context);
}

} // namespace

Shape boolean_subtract (Shape const& shape, Shape const& cutting_tool)
{
    return boolean_subtract_impl(shape, cutting_tool, nullptr);
}

Shape boolean_subtract(Shape const& shape,
                       Shape const& cutting_tool,
                       ModelingContext& context)
{
    return boolean_subtract_impl(shape, cutting_tool, &context);
}


Shape operator-(Shape const& shape, Shape const& cutting_tool)
{
    return boolean_subtract(shape, cutting_tool);
}


Shape boolean_union(Shape const& shape_1, Shape const& shape_2)
{
    return boolean_union_impl(shape_1, shape_2, nullptr);
}

Shape boolean_union(Shape const& shape_1,
                    Shape const& shape_2,
                    ModelingContext& context)
{
    return boolean_union_impl(shape_1, shape_2, &context);
}


Shape operator+(Shape const& shape_1, Shape const& shape_2)
{
    return boolean_union(shape_1, shape_2);
}



} // namespace geoml

