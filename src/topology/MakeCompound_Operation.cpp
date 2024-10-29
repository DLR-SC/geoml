#include "MakeCompound_Operation.h"
#include <BRep_Builder.hxx>

#include <algorithm>

namespace geoml {

MakeCompound_Operation::MakeCompound_Operation(
    std::vector<std::shared_ptr<Shape>> const& inputs
)
    : Operation(
        [&](){
            std::vector<Shape const*> ret;
            ret.reserve(inputs.size());
            std::transform(
                inputs.begin(),
                inputs.end(),
                std::back_inserter(ret),
                [](auto sptr){ return sptr.get(); }
            );
            return ret;
        }()
    )
    , m_shapes(inputs)
{}

Shape MakeCompound_Operation::perform() const
{
    TopoDS_Compound compound;
    BRep_Builder builder;
    builder.MakeCompound(compound);
    for (auto const& s : m_inputs) {
        builder.Add(compound, *s);
    }
    return compound;
}

void MakeCompound_Operation::map_subshapes(Shape& result) const
{
    auto& result_subshapes = result.get_subshapes();
    for(auto const& shape : m_shapes) {
        for (auto const& result_subshape : result_subshapes) {
            if (shape->is_same(*result_subshape)) {
                add_origin(result_subshape, shape);
            }
        }
    }
}

} // namespace geoml
