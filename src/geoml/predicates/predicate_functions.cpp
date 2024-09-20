/**
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 */

#include "predicates/predicate_functions.hpp"

namespace geoml{

ShapePredicate AND(ShapePredicate const& l, ShapePredicate const& r)
{
    return [=](Shape const& s){ return l(s) && r(s); };
}

ShapePredicate operator&&(ShapePredicate const& l, ShapePredicate const& r)
{
    return AND(l,r);
}

ShapePredicate OR(ShapePredicate const& l, ShapePredicate const& r)
{
    return [=](Shape const& s){ return l(s) || r(s); };
}

ShapePredicate operator||(ShapePredicate const& l, ShapePredicate const& r)
{
    return OR(l,r);
}




ShapePredicate make_predicate_has_tag(std::string const& tag)
{
    return [&](geoml::Shape const& s){ return s.has_tag(tag); };
}

ShapePredicate make_predicate_has_subshape_that_is_child_of(Shape const &shape)
{
    return [&](geoml::Shape const& s){ return s.has_subshape_that_is_child_of(shape); };
}

} // namespace geoml
