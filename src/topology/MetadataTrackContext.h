#pragma once

#include "geoml/modeling/ModelingContext.h"

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include <TopoDS_Shape.hxx>

namespace geoml {
namespace details {

struct MetadataTrack
{
    std::uint64_t id;             ///< Identifies this copy for contribution provenance.
    MetadataTrackHandle handle;  ///< Identifies the original registration for cancellation.
    metadata::Metadata contribution;
    ShapePredicate criterion;
    TopoDS_Shape source_shape;
    std::optional<int> remaining_steps;
};

std::uint64_t next_metadata_track_id();

} // namespace details

namespace internal {

class MetadataPolicyContext;

class MetadataTrackContext
{
public:
    MetadataTrackHandle register_track(Shape const& owner,
                                       metadata::Metadata contribution,
                                       ShapePredicate criterion,
                                       TrackLifetime lifetime,
                                       Shape const* source);

    bool cancel_track(MetadataTrackHandle handle);

    void propagate(Shape& result,
                   std::vector<Shape> const& inputs,
                   MetadataPolicyContext const& policy_context);

private:
    using EntityKey = std::weak_ptr<metadata::Metadata>;

    void prune_expired();
    std::vector<details::MetadataTrack> collect_tracks(Shape const& root) const;

    std::map<EntityKey, std::vector<details::MetadataTrack>, std::owner_less<void>> m_tracks;
};

} // namespace internal
} // namespace geoml
