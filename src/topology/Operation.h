#pragma once

#include "geoml/naming_choosing/Shape.h"
#include "geoml/modeling/ModelingContext.h"

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

    /**
     * Execute the operation without metadata tracking.
     * @return The modeled shape with subshape history applied.
     */
    Shape value() {
        return execute(nullptr);
    }

    /**
     * Execute the operation and return its result with metadata tracks propagated from the inputs.
     * @param context Modeling context owning active metadata tracks and policy scopes.
     * @return The modeled shape with subshape history and metadata propagation applied.
     */
    Shape value(ModelingContext& context) {
        return execute(&context);
    }

    /**
     * Execute the operation with an optional context pointer.
     * @param context Optional modeling context, or `nullptr` for untracked modeling.
     * @return The modeled shape with subshape history and optional metadata propagation applied.
     */
    Shape value(ModelingContext* context) {
        return execute(context);
    }

protected:

    static void add_origin(Shape& output, Shape const& input){
        output.m_data->origins.push_back(input);
    }

    std::vector<Shape> const m_inputs;

private:

    Shape execute(ModelingContext* context) {
        Shape result = static_cast<Derived&>(*this).perform();
        result.detach_from_inputs(m_inputs);
        static_cast<Derived&>(*this).map_subshapes(result);

        if (context) {
            context->propagate_metadata_tracks(result, m_inputs);
        }

        return result;
    }

};

} // namespace geoml
