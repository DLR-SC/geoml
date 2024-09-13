#include "ShapePredicate.hpp"

namespace geoml {

ShapePredicate::ShapePredicate(std::function<bool(Shape const&)> const& fun) 
 : pred(fun) 
{}

bool ShapePredicate::operator()(Shape const& s) const
{
    return pred(s);
}

} // namespace geoml
