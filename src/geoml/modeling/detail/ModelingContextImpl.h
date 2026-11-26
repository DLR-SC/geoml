#pragma once

#include <utility>
#include <vector>

namespace geoml {

template <typename Pred, typename Value>
MetadataTrackHandle ModelingContext::add_metadata_track(
    Shape const& target,
    Pred&& criterion,
    metadata::Key<Value> const& key,
    typename metadata::Key<Value>::value_type value,
    TrackLifetime lifetime)
{
    metadata::Metadata contribution;
    contribution.set(key, std::move(value));
    return register_metadata_track(target, std::move(contribution),
                                   ShapePredicate(std::forward<Pred>(criterion)), lifetime);
}

template <typename Pred>
MetadataTrackHandle ModelingContext::add_metadata_track_from(Shape const& target,
                                                             Shape const& source,
                                                             Pred&& criterion,
                                                             TrackLifetime lifetime)
{
    return register_metadata_track(target, source.metadata(),
                                   ShapePredicate(std::forward<Pred>(criterion)), lifetime,
                                   &source);
}

template <typename Pred>
MetadataTrackHandle ModelingContext::add_tag_track(Shape const& target,
                                                   Pred&& criterion,
                                                   std::string const& tag,
                                                   TrackLifetime lifetime)
{
    return add_metadata_track(target, std::forward<Pred>(criterion), metadata::keys::tags(),
                              std::vector<std::string>{tag}, lifetime);
}

} // namespace geoml
