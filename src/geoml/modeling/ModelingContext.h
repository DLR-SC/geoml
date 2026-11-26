#pragma once

#include "geoml/exports.h"
#include "geoml/metadata/Metadata.h"
#include "geoml/metadata/MetadataPolicy.h"
#include "geoml/metadata/MetadataScope.h"
#include "geoml/naming_choosing/Shape.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace geoml {

namespace internal {
class MetadataPolicyContext;
class MetadataTrackContext;
} // namespace internal

template <typename Derived>
class Operation;

/** Number of modeling results to which a metadata track can propagate. */
class TrackLifetime
{
public:
    /** Create a lifetime that remains active for every later modeling result. */
    static constexpr TrackLifetime unlimited() noexcept { return TrackLifetime{}; }

    /**
     * Create a lifetime measured in subsequent modeling results, including results without a match.
     * @param count Number of results for which the track remains active; must be positive.
     * @return A finite track lifetime.
     * @throws std::invalid_argument if `count` is zero or negative.
     */
    GEOML_API_EXPORT static TrackLifetime steps(int count);

private:
    friend class internal::MetadataTrackContext;

    constexpr TrackLifetime() noexcept = default;
    explicit constexpr TrackLifetime(int count) noexcept : m_steps(count) {}

    std::optional<int> m_steps;
};

/** Identifies one metadata track registration within a ModelingContext. */
class MetadataTrackHandle
{
public:
    /** Create an empty handle. */
    constexpr MetadataTrackHandle() noexcept = default;

    /** Check whether this handle identifies a registration. The track may have expired or been cancelled. */
    constexpr explicit operator bool() const noexcept { return m_registration != 0; }

private:
    friend class internal::MetadataTrackContext;

    explicit constexpr MetadataTrackHandle(std::uint64_t registration) noexcept
        : m_registration(registration)
    {}

    std::uint64_t m_registration = 0;
};

/**
 * Explicit facade owning metadata track rules and scoped conflict policies for modeling operations.
 *
 * Tracks are associated with shapes by the shared identity of their per-entity metadata store
 * without owning shapes through lookup keys. Destroying the context releases all tracks and any
 * references captured by their predicates. Shapes never hold an owning reference to a context.
 *
 * A ModelingContext is not thread-safe for concurrent access.
 */
class ModelingContext
{
public:
    /** Create an empty modeling context with no tracks or active policy scopes. */
    GEOML_API_EXPORT ModelingContext();

    /** Release all registered tracks, captured predicate references, and scoped policies. */
    GEOML_API_EXPORT ~ModelingContext();

    ModelingContext(ModelingContext const&) = delete;
    ModelingContext& operator=(ModelingContext const&) = delete;

    /** Move-construct a modeling context. Active scopes remain linked to the moved state. */
    GEOML_API_EXPORT ModelingContext(ModelingContext&&) noexcept;

    /** Move-assign a modeling context. */
    GEOML_API_EXPORT ModelingContext& operator=(ModelingContext&&) noexcept;

    /**
     * Register a typed metadata track associated with `target` and apply it immediately.
     *
     * Matching subshapes of `target` receive the contribution immediately without consuming
     * a modeling step. Later direct metadata edits do not change the captured contribution.
     *
     * @tparam Pred Predicate callable with `Shape const&` that selects targets.
     * @tparam Value Value type associated with `key`.
     * @param target Shape to associate with the track and evaluate immediately.
     * @param criterion Predicate used to select target shapes.
     * @param key Typed key under which the value is recorded.
     * @param value Value captured by this track.
     * @param lifetime Number of subsequent modeling results that can receive the contribution.
     * @return A handle usable to cancel the rule across all branches in this context.
     */
    template <typename Pred, typename Value>
    MetadataTrackHandle add_metadata_track(Shape const& target,
                                           Pred&& criterion,
                                           metadata::Key<Value> const& key,
                                           typename metadata::Key<Value>::value_type value,
                                           TrackLifetime lifetime = TrackLifetime::unlimited());

    /**
     * Register a track capturing a snapshot of all metadata currently stored on `source`.
     *
     * Registration associates the rule with `target` and applies the snapshot immediately
     * to matching subshapes of `target`, skipping `source`'s own metadata store. An empty
     * snapshot has no effect and returns an empty handle.
     *
     * @tparam Pred Predicate callable with `Shape const&` that selects targets.
     * @param target Shape to associate with the track and evaluate immediately.
     * @param source Shape whose current metadata entries are copied into the track.
     * @param criterion Predicate used to select target shapes.
     * @param lifetime Number of subsequent modeling results that can receive the snapshot.
     * @return A track handle, or an empty handle when `source` has no metadata.
     */
    template <typename Pred>
    MetadataTrackHandle add_metadata_track_from(Shape const& target,
                                                Shape const& source,
                                                Pred&& criterion,
                                                TrackLifetime lifetime = TrackLifetime::unlimited());

    /**
     * Convenience wrapper registering a track for the standard `tags` metadata key.
     *
     * Evaluates immediately on `target` and propagates like any other metadata track.
     *
     * @tparam Pred Predicate callable with `Shape const&` that selects targets.
     * @param target Shape to associate with the track and evaluate immediately.
     * @param criterion Predicate used to select target shapes.
     * @param tag Tag string captured by this track.
     * @param lifetime Number of subsequent modeling results that can receive the tag.
     * @return A handle usable to cancel the rule across all branches in this context.
     */
    template <typename Pred>
    MetadataTrackHandle add_tag_track(Shape const& target,
                                      Pred&& criterion,
                                      std::string const& tag,
                                      TrackLifetime lifetime = TrackLifetime::unlimited());

    /**
     * Cancel a registered tracking rule across all modeling branches in this context.
     * Previously written metadata values remain in place.
     *
     * @param handle Track handle returned when the rule was registered.
     * @return Whether any active copies of the rule were removed.
     */
    GEOML_API_EXPORT bool cancel_metadata_track(MetadataTrackHandle handle);

    /**
     * Push a policy override onto this context's policy stack for the lifetime of the returned guard.
     *
     * Inner scopes inherit resolvers for keys they omit. Destroying the returned guard restores
     * the previous policy state, including during exception unwinding.
     *
     * @param policy Resolver overrides to activate for modeling operations using this context.
     * @return An RAII scope guard owning this policy stack entry.
     */
    [[nodiscard]] GEOML_API_EXPORT metadata::MetadataScope
    create_policy_scope(metadata::MetadataPolicy policy);

private:
    template <typename Derived>
    friend class Operation;

    GEOML_API_EXPORT MetadataTrackHandle register_metadata_track(
        Shape const& target,
        metadata::Metadata contribution,
        ShapePredicate criterion,
        TrackLifetime lifetime,
        Shape const* source = nullptr);

    GEOML_API_EXPORT void propagate_metadata_tracks(Shape& result,
                                                    std::vector<Shape> const& inputs);

    std::unique_ptr<internal::MetadataTrackContext> m_track_context;
    std::unique_ptr<internal::MetadataPolicyContext> m_policy_context;
};

} // namespace geoml

#include "geoml/modeling/detail/ModelingContextImpl.h"
