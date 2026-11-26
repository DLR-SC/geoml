#include "BRepBuilderAPI_MakeShape_Operation.hpp"
#include "BRepAlgoAPI_Cut.hxx"
#include "BRepAlgoAPI_Fuse.hxx"

#include <NCollection_DataMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>

#include <vector>

namespace geoml {

BRepBuilderAPI_MakeShape_Operation::BRepBuilderAPI_MakeShape_Operation(
    BRepBuilderAPI_MakeShape& algo,
    std::vector<Shape> const& inputs
)
    : Operation(inputs), 
      m_algo(algo)
{}

Shape BRepBuilderAPI_MakeShape_Operation::perform()
{
    return m_algo.Shape();

}

void BRepBuilderAPI_MakeShape_Operation::map_subshapes(Shape& result)
{
    // Collect all Shape::Data instances in the result topology tree, grouped by
    // OCCT topological identity (IsSame), so every occurrence of a shared subshape
    // receives its origins.
    NCollection_DataMap<TopoDS_Shape, std::vector<Shape>, TopTools_ShapeMapHasher> result_subshapes;
    details::ForEachVisitor collect_visitor([&](Shape const& s) {
        TopoDS_Shape const occt_shape = s.shape();
        if (auto* instances = result_subshapes.ChangeSeek(occt_shape)) {
            instances->push_back(s);
        } else {
            result_subshapes.Bind(occt_shape, std::vector<Shape>{s});
        }
    });
    result.accept_topology_visitor(collect_visitor);

    auto add_origin_to_matching = [&](TopoDS_Shape const& target_shape, Shape const& input_subshape) {
        if (auto* instances = result_subshapes.ChangeSeek(target_shape)) {
            for (auto& result_subshape : *instances) {
                add_origin(result_subshape, input_subshape);
            }
        }
    };

    // map input origins to outputs
    for (auto const& shape : m_inputs) {
        for (auto const& input_subshape : shape.get_subshapes()) {
            // set origin for all unmodified shapes
            add_origin_to_matching(input_subshape, input_subshape);

            // set origin for all modified shapes
            TopTools_ListOfShape const& modified_by_this = m_algo.Modified(input_subshape);
            TopTools_ListIteratorOfListOfShape it;
            for (it.Initialize(modified_by_this); it.More(); it.Next()) {
                add_origin_to_matching(it.Value(), input_subshape);
            }
        }
    }
}

} // namespace geoml
