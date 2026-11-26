#pragma once

#include "geoml/naming_choosing/Shape.h"

#include <unordered_map>

namespace geoml {

struct Shape::TopoMetaMap
{
    struct TopologyHasher
    {
        std::size_t operator()(TopoDS_Shape const& shape) const
        {
            auto const topology_hash = std::hash<const void*>{}(shape.TShape().get());
            auto const location_hash = shape.Location().HashCode(std::numeric_limits<int>::max());
            return topology_hash ^ (location_hash << 1);
        }
    };

    struct TopologyIsSame
    {
        bool operator()(TopoDS_Shape const& left, TopoDS_Shape const& right) const
        {
            return left.IsSame(right);
        }
    };

    std::unordered_map<TopoDS_Shape,
                       std::shared_ptr<metadata::Metadata>,
                       TopologyHasher,
                       TopologyIsSame> metadata_by_topology;
};

} // namespace geoml
