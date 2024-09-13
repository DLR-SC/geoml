#pragma once

#include <functional>

#include "geoml/naming_choosing/Shape.hpp"



namespace geoml {

/**
 * @brief The class ShapePredicate has a predicate as attribute
 */
class ShapePredicate
{
public:
    
    ShapePredicate(std::function<bool(Shape const&)> const& fun);

    // evaluation of a ShapePredicate
    bool operator()(Shape const& s) const;

private:
    std::function<bool(Shape const&)> pred;

};

} // namespace geoml
