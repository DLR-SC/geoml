#pragma once

/**
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 */

#include "geoml/naming_choosing/Shape.hpp"
#include <vector>

namespace geoml {

/**
 * @brief The Operation class is a CRTP base class for operations that modify shapes. A class
 * derived from this must implement
 *
 *   Shape perform() const
 *   void map_subshapes(Shape& result) const
 *   std::string id() const
 *
 * The first function performs the operation and returns the result. The second function maps
 * the result's subshapes to the inputs' subshapes by setting the result's subshapes' origin.
 * The third is used as input into the persistent naming scheme
 */
template <typename Derived>
class Operation
{
public:
    Operation(std::vector<Shape const*> const& inputs) // check if Shape const* is the right choice for this purpose (see the respective Issue)
        : m_inputs(inputs)
    {};

    virtual ~Operation() = default;

    Shape value() const {
        Shape ret = static_cast<Derived const&>(*this).perform();
        static_cast<Derived const&>(*this).map_subshapes(ret);
        
        merge_and_apply_tag_tracks(ret);

        return ret;
    }

protected:

    static void add_origin(std::shared_ptr<Shape> output, std::shared_ptr<Shape> const input){
        output->m_origins.push_back({input});
    }

    std::vector<Shape const*> m_inputs; // Lifetime issue: Operation may not outlive the inputs!

private:

    void merge_and_apply_tag_tracks(Shape & result) const
    {
        // collect tag tracks of input Shapes and add them to the result Shape
        for(auto const &shape : m_inputs)
        {
            for (auto const &input_tag_track : shape->get_tag_tracks())
            {
                result.add_tag_track(input_tag_track);
            }
        }

        // apply tag tracks to the output
        result.apply_tag_tracks();

        // delete the "worn out" Tag tracks and reduce the remaining step number of the remaining tag tracks by one
        for (auto it = result.m_tag_tracks.begin(); it != result.m_tag_tracks.end();)
        {
            if (it->m_remainingSteps <= 1)
            {
                it = result.m_tag_tracks.erase(it);
            } else {
                it->m_remainingSteps--;
                ++it;
            }    
        }

    }
};

} // namespace geoml
