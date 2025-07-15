#pragma once

/**
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 */

#include "geoml/naming_choosing/Shape.h"
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
    Operation(std::vector<Shape> const& inputs) 
        : m_inputs(inputs)
    {};

    virtual ~Operation() = default;

    Shape value() {
        Shape ret = static_cast<Derived&>(*this).perform();
        static_cast<Derived&>(*this).map_subshapes(ret);
        
        merge_and_apply_tag_tracks(ret);

        return ret;
    }

protected:

    static void add_origin(Shape& output, Shape const& input){
        output.m_data->origins.push_back(input);
    }

    std::vector<Shape> const m_inputs;

private:

    void merge_and_apply_tag_tracks(Shape & result) const
    {
        // collect tag tracks of input Shapes and add them to the result Shape
        for(auto const &shape : m_inputs)
        {
            for (auto const &input_tag_track : shape.get_tag_tracks())
            {
                result.add_tag_track(input_tag_track);
            }
        }

        // apply tag tracks to the output
        result.apply_tag_tracks();

        // delete the "worn out" Tag tracks and reduce the remaining step number of the remaining tag tracks by one
        auto& tag_tracks = result.get_tag_tracks();
        for (auto it = tag_tracks.begin(); it != tag_tracks.end();)
        {
            if (it->m_remainingSteps <= 1)
            {
                it = tag_tracks.erase(it);
            } else {
                it->m_remainingSteps--;
                ++it;
            }    
        }

    }
};

} // namespace geoml
