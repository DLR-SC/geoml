#include "BRepBuilderAPI_MakeShape_Operation.hpp"
#include "BRepAlgoAPI_Cut.hxx"
#include "BRepAlgoAPI_Fuse.hxx"

#include <algorithm>

namespace geoml {

BRepBuilderAPI_MakeShape_Operation::BRepBuilderAPI_MakeShape_Operation(
    std::string const& id,
    BRepBuilderAPI_MakeShape* algo,
    std::vector<Shape const*> const& inputs
)
    : Operation(inputs)
    , m_id(id)
    , m_algo(algo)
{}

std::string BRepBuilderAPI_MakeShape_Operation::id() const {
    return m_id;
}

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

// void BRepBuilderAPI_MakeShape_Operation::manage_tag_tracks(Shape &result)
// {
//     // collect tag tracks of input Shapes and add them to the result Shape
//     for(auto const &shape : m_inputs)
//     {
//         for (auto const &input_tag_track : shape->get_tag_tracks())
//         {
//             result.add_tag_track(input_tag_track);
//         }
//     }

//     // apply tag tracks to the output
//     result.apply_tag_tracks();

//     // decrease number of remaining steps for tag tracks and delete the "worn out" ones
//     for(auto &tag_track : result.m_tag_tracks)
//     {
//         if(tag_track.m_remainingSteps > 1)
//         {   
//             tag_track.m_remainingSteps--;
//         }
//         else
//         {
//             auto it = std::find(result.m_tag_tracks.begin(), result.m_tag_tracks.end(), tag_track); 
  
//             // If element is found found, erase it 
//             if (it != result.m_tag_tracks.end()) 
//             {
//                result.m_tag_tracks.erase(it);
//             } 
//         }
//     }
// }

Shape operator-(Shape const& l, Shape const& r)
{
    BRepAlgoAPI_Cut cutter(l, r);
    auto operation = BRepBuilderAPI_MakeShape_Operation("cut", &cutter, {&l,&r});
    return operation.value();
}

Shape operator+(Shape const& l, Shape const& r)
{
    BRepAlgoAPI_Fuse fuser(l, r);
    auto operation = BRepBuilderAPI_MakeShape_Operation("fuse", &fuser, {&l,&r});
    return operation.value();
}

} // namespace geoml
