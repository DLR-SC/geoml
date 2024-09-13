#include "predicates/predicate_functions.hpp"

namespace geoml{

ShapePredicate AND(ShapePredicate const& l, ShapePredicate const& r)
{
    return ShapePredicate([=](Shape const& s){ return l(s) && r(s); });
}

ShapePredicate operator&&(ShapePredicate const& l, ShapePredicate const& r)
{
    return AND(l,r);
}

ShapePredicate OR(ShapePredicate const& l, ShapePredicate const& r)
{
    return ShapePredicate([=](Shape const& s){ return l(s) || r(s); });
}

ShapePredicate operator||(ShapePredicate const& l, ShapePredicate const& r)
{
    return ShapePredicate([=](Shape const& s){ return l(s) || r(s); });
}

} // namespace geoml

