#include "topology/MetadataTrackContext.h"
#include "topology/MetadataPolicyContext.h"
#include "geoml/metadata/detail/ResolveValue.h"

#include <algorithm>
#include <atomic>
#include <map>
#include <unordered_set>
#include <utility>

namespace geoml {

namespace details {

std::uint64_t next_metadata_track_id()
{
    static std::atomic<std::uint64_t> next_id{1};
    return next_id.fetch_add(1, std::memory_order_relaxed);
}

} // namespace details

namespace internal {
namespace {

using details::MetadataTrack;
using metadata::Metadata;

struct InputOccurrence
{
    std::size_t input_position;
    std::uint64_t input_track_id;
    std::size_t track_index;
};

// A track can arrive through several inputs. Keep one branch-local copy of its
// predicate and lifetime, and record every occurrence in operation input order.
// Its registration handle survives the copy so it can be cancelled later.
struct IncomingTracks
{
    std::vector<MetadataTrack> tracks;
    std::vector<InputOccurrence> occurrences;
    std::map<std::uint64_t, std::size_t> index_by_id;

    void add(std::size_t input_position, std::vector<MetadataTrack> input_tracks)
    {
        for (auto& track : input_tracks) {
            auto [index, first_occurrence] = index_by_id.emplace(track.id, tracks.size());
            occurrences.push_back({input_position, track.id, index->second});
            if (first_occurrence) {
                track.id = details::next_metadata_track_id();
                tracks.push_back(std::move(track));
            }
        }
    }
};

// Check all value types before evaluating predicates. Even tracks that match
// disjoint faces must agree on what a key name means in the result model.
void validate_key_types(std::vector<MetadataTrack> const& tracks)
{
    std::map<std::string, std::type_index> types;
    for (auto const& track : tracks) {
        for (auto const& [name, entry] : track.contribution.entries()) {
            auto [known, inserted] = types.try_emplace(name, entry.value.type());
            if (!inserted && known->second != std::type_index(entry.value.type())) {
                throw std::logic_error("Incompatible metadata value type for key: " + name);
            }
        }
    }
}

struct KeyContributions
{
    Metadata::Entry const& entry;
    std::vector<metadata::MetadataContribution> values;
};

std::map<std::string, KeyContributions> collect_contributions(Shape const& target,
                                                               IncomingTracks const& incoming)
{
    std::map<std::string, KeyContributions> by_key;
    for (auto const& occurrence : incoming.occurrences) {
        auto const& track = incoming.tracks[occurrence.track_index];
        if (!track.criterion(target)) {
            continue;
        }
        std::optional<TopoDS_Shape> source_topology;
        if (!track.source_shape.IsNull()) {
            source_topology = track.source_shape;
        }
        for (auto const& [name, entry] : track.contribution.entries()) {
            auto group = by_key.try_emplace(name, KeyContributions{entry, {}}).first;
            group->second.values.push_back({entry.value, occurrence.input_position,
                                            occurrence.input_track_id, source_topology});
        }
    }
    return by_key;
}

struct UniqueTargets
{
    details::ShapeContainer visited;
    std::vector<Shape> in_topology_order;

    bool visit(Shape const& shape, int)
    {
        if (visited.insert(shape).second) {
            in_topology_order.push_back(shape);
        }
        return false;
    }

    int max_depth() const { return std::numeric_limits<int>::max(); }
};

std::vector<MetadataTrack> tracks_for_next_operation(std::vector<MetadataTrack> tracks)
{
    auto expired = std::remove_if(tracks.begin(), tracks.end(), [](MetadataTrack const& track) {
        return track.remaining_steps == 1;
    });
    tracks.erase(expired, tracks.end());
    for (auto& track : tracks) {
        if (track.remaining_steps) {
            --*track.remaining_steps;
        }
    }
    return tracks;
}

struct Update
{
    Shape target;
    Metadata resolved;
};

std::vector<Update> compute_track_updates(Shape const& owner,
                                          MetadataTrack const& track,
                                          Metadata const* source_metadata)
{
    auto targets = owner.select_subshapes([&](Shape const& candidate) {
        return &candidate.metadata() != source_metadata && track.criterion(candidate);
    });

    std::vector<Update> updates;
    updates.reserve(targets.size());
    for (auto& target : targets) {
        auto resolved = target.metadata();
        resolved.merge_from(track.contribution);
        updates.push_back({target, std::move(resolved)});
    }
    return updates;
}

} // namespace

void MetadataTrackContext::prune_expired()
{
    for (auto it = m_tracks.begin(); it != m_tracks.end(); ) {
        if (it->first.expired() || it->second.empty()) {
            it = m_tracks.erase(it);
        }
        else {
            ++it;
        }
    }
}

MetadataTrackHandle MetadataTrackContext::register_track(Shape const& owner,
                                                         metadata::Metadata contribution,
                                                         ShapePredicate criterion,
                                                         TrackLifetime lifetime,
                                                         Shape const* source)
{
    if (contribution.empty()) {
        return {};
    }

    auto const id = details::next_metadata_track_id();
    MetadataTrackHandle const handle{id};
    details::MetadataTrack track{id, handle, std::move(contribution), std::move(criterion),
                                 source ? source->shape() : owner.shape(), lifetime.m_steps};

    auto updates = compute_track_updates(owner, track, source ? &source->metadata() : nullptr);

    prune_expired();
    auto& entity_tracks = m_tracks[owner.metadata_storage()];
    entity_tracks.push_back(std::move(track));

    for (auto& update : updates) {
        update.target.metadata().swap(update.resolved);
    }
    return handle;
}

bool MetadataTrackContext::cancel_track(MetadataTrackHandle handle)
{
    if (!handle) {
        return false;
    }

    bool removed = false;
    for (auto it = m_tracks.begin(); it != m_tracks.end(); ) {
        if (it->first.expired()) {
            it = m_tracks.erase(it);
            continue;
        }
        auto& tracks = it->second;
        auto const cancelled = std::remove_if(
            tracks.begin(), tracks.end(),
            [handle](details::MetadataTrack const& track) {
                return track.handle.m_registration == handle.m_registration;
            });
        if (cancelled != tracks.end()) {
            removed = true;
            tracks.erase(cancelled, tracks.end());
        }
        if (tracks.empty() || it->first.expired()) {
            it = m_tracks.erase(it);
        }
        else {
            ++it;
        }
    }
    return removed;
}

std::vector<details::MetadataTrack> MetadataTrackContext::collect_tracks(Shape const& root) const
{
    std::vector<details::MetadataTrack> tracks;
    std::unordered_set<metadata::Metadata const*> visited_entities;

    auto collect = [&](auto&& self, Shape const& shape) -> void {
        auto const& storage = shape.metadata_storage();
        if (visited_entities.insert(storage.get()).second) {
            auto const found = m_tracks.find(storage);
            if (found != m_tracks.end()) {
                tracks.insert(tracks.end(), found->second.begin(), found->second.end());
            }
        }
        for (auto const& child : shape.direct_subshapes()) {
            self(self, child);
        }
    };
    collect(collect, root);
    return tracks;
}

void MetadataTrackContext::propagate(Shape& result,
                                     std::vector<Shape> const& inputs,
                                     MetadataPolicyContext const& policy_context)
{
    IncomingTracks incoming;
    for (std::size_t i = 0; i < inputs.size(); ++i) {
        incoming.add(i, collect_tracks(inputs[i]));
    }
    if (incoming.tracks.empty()) {
        return;
    }
    validate_key_types(incoming.tracks);

    // Predicates and resolvers see result metadata as it stood after history mapping.
    UniqueTargets targets;
    result.accept_topology_visitor(targets);
    std::vector<Update> updates;
    for (auto const& target : targets.in_topology_order) {
        auto by_key = collect_contributions(target, incoming);
        if (by_key.empty()) {
            continue;
        }
        auto resolved = target.metadata();
        for (auto const& [name, group] : by_key) {
            auto const* override_resolver =
                policy_context.find_resolver(name, group.entry.value.type());
            metadata::details::ResolutionRequest request{name, group.values, target.shape()};
            request.override_resolver = override_resolver;
            resolved.resolve_and_set(group.entry, std::move(request));
        }
        updates.push_back({target, std::move(resolved)});
    }

    auto next_tracks = tracks_for_next_operation(std::move(incoming.tracks));
    prune_expired();
    if (!next_tracks.empty()) {
        m_tracks[result.metadata_storage()] = std::move(next_tracks);
    }

    for (auto& update : updates) {
        update.target.metadata().swap(update.resolved);
    }
}

} // namespace internal
} // namespace geoml
