#include "BRepBuilderAPI_MakeShape_Operation.hpp"
#include "BRepAlgoAPI_Cut.hxx"
#include "BRepAlgoAPI_Fuse.hxx"

namespace geoml {

BRepBuilderAPI_MakeShape_Operation::BRepBuilderAPI_MakeShape_Operation(
    BRepBuilderAPI_MakeShape* algo,
    std::vector<Shape const*> const& inputs
)
    : Operation(inputs), 
      m_algo(algo)
{}

Shape BRepBuilderAPI_MakeShape_Operation::perform() const
{
    return m_algo->Shape();

}

void BRepBuilderAPI_MakeShape_Operation::map_subshapes(Shape& result) const
{
    auto& result_subshapes = result.get_subshapes();

    // map input origins to outputs
    for(auto const& shape : m_inputs) {
        for (auto const& input_subshape : shape->get_subshapes()) {

            // set origin for all unmodified shapes
            for (auto& result_subshape : result_subshapes) {
                if (result_subshape->is_same(*input_subshape)){
                    add_origin(result_subshape, input_subshape);
                }
            }

            // set origin for all modified shapes
            TopTools_ListOfShape const& modified_by_this = m_algo->Modified(*input_subshape);
            TopTools_ListIteratorOfListOfShape it;
            for (it.Initialize(modified_by_this); it.More(); it.Next()) {
                for (auto& result_subshape : result_subshapes) {
                    if (result_subshape->is_same(it.Value())) {
                        add_origin(result_subshape, input_subshape);
                    }
                }
            }

        }
    }
}

Shape operator-(Shape const& l, Shape const& r)
{
    BRepAlgoAPI_Cut cutter(l, r);
    auto operation = BRepBuilderAPI_MakeShape_Operation(&cutter, {&l,&r});
    return operation.value();
}

Shape operator+(Shape const& l, Shape const& r)
{
    BRepAlgoAPI_Fuse fuser(l, r);
    auto operation = BRepBuilderAPI_MakeShape_Operation(&fuser, {&l,&r});
    return operation.value();
}

} // namespace geoml
