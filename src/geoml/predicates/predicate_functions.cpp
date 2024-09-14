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

std::function<bool(Shape const&)> AND(std::function<bool(Shape const&)> const& l, std::function<bool(Shape const&)> const& r)
{
    return [=](Shape const& s){ return l(s) && r(s); };
}

std::function<bool(Shape const&)> operator&&(std::function<bool(Shape const&)> const& l, std::function<bool(Shape const&)> const& r)
{
    return AND(l,r);
}

ShapePredicate OR(ShapePredicate const& l, ShapePredicate const& r)
{
    return ShapePredicate([=](Shape const& s){ return l(s) || r(s); });
}

std::function<bool(Shape const&)> OR(std::function<bool(Shape const&)> const& l, std::function<bool(Shape const&)> const& r)
{
    return [=](Shape const& s){ return l(s) || r(s); };
}

ShapePredicate operator||(ShapePredicate const& l, ShapePredicate const& r)
{
    return OR(l,r);
}

std::function<bool(Shape const&)> operator||(std::function<bool(Shape const&)> const& l, std::function<bool(Shape const&)> const& r)
{
    return OR(l,r);
}

} // namespace geoml
