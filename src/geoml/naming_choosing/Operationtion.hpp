#pragma once

#include "Shape.hpp"
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
    Operation() = default;
    virtual ~Operation() = default;

    Shape value() const {
        Shape ret = static_cast<Derived const&>(*this).perform();
        static_cast<Derived const&>(*this).map_subshapes(ret);

        update_persistent_name(ret);
        for (auto& subshape : ret.get_subshapes()) {
            update_persistent_name(*subshape);
        }

        return ret;
    }

protected:

    static void add_origin(std::shared_ptr<Shape> output, std::shared_ptr<Shape> const input){
        output->m_origins.push_back({input});
    }

    void update_persistent_name(Shape& s) const{
        std::string id = static_cast<Derived const&>(*this).id();
        for (auto const& origin : s.m_origins) {
            id += std::to_string(origin->m_id);
        }
        s.m_id = std::hash<std::string>{}(id);
    }
};

} // namespace geoml
