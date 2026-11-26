#pragma once

#include "geoml/metadata/MetadataConflict.h"
#include "geoml/metadata/MetadataKey.h"

#include <functional>
#include <optional>
#include <string>
#include <vector>

#include <TopoDS_Shape.hxx>

namespace geoml
{
namespace metadata
{

template <typename Value>
struct ResolutionContext;

/** Owned, copyable callback that resolves one metadata value from its context. */
template <typename Value>
using MetadataResolver = std::function<Value(ResolutionContext<Value> const&)>;

/**
 * Values and provenance supplied to a metadata resolver. Callbacks take a const
 * reference to this context and return Value. GeoML stages metadata writes during
 * resolution. Callbacks must keep the model unchanged and avoid external side
 * effects, which a failed merge cannot undo.
 */
template <typename Value>
struct ResolutionContext
{
    std::string key_name; ///< Stable name of the key being resolved.
    std::optional<Value> existing_value; ///< Value already stored on the target, if any.
    std::vector<ResolutionContribution<Value>> contributions; ///< Incoming values in operation order.
    std::optional<TopoDS_Shape> target_topology; ///< Target topology, if known.

    /** Registered default or generic fallback, available to policy overrides. */
    MetadataResolver<Value> key_default;
};

/**
 * Register a custom key's default resolver. Registration and lookup are thread-safe.
 * A second registration for the same name, including a built-in name, raises
 * std::logic_error. Callbacks can run concurrently and must synchronize shared state.
 */
template <typename Value, typename Callable>
void register_default_resolver(Key<Value> const& key, Callable&& callable);

/** Resolver that selects the last incoming value, retaining the existing value if none arrive. */
struct ReplaceResolver
{
    template <typename Value>
    Value operator()(ResolutionContext<Value> const& context) const;
};

/** Default resolver: equal values merge; differing values raise a structured conflict. */
struct RequireEqualResolver
{
    template <typename Value>
    Value operator()(ResolutionContext<Value> const& context) const;
};

/** Merge containers, keeping the first occurrence of each value. */
struct SetUnionResolver
{
    template <typename Container>
    Container operator()(ResolutionContext<Container> const& context) const;
};

} // namespace metadata
} // namespace geoml

#include "geoml/metadata/detail/ResolverImpl.h"
