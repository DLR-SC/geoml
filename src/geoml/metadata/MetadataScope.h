#pragma once

#include <geoml/exports.h>

#include <cstdint>
#include <memory>

namespace geoml {

namespace internal {
class MetadataPolicyContext;
} // namespace internal

namespace metadata {

namespace details {
struct MetadataPolicyStack;
} // namespace details

/**
 * RAII guard owning one policy override frame on a `ModelingContext`.
 *
 * Created via `ModelingContext::create_policy_scope()`. An inner scope inherits
 * resolvers for keys it omits. Destruction removes this guard's frame from its
 * context, including during exception unwinding.
 * Direct store merges and track registration use the ordinary resolvers.
 */
class MetadataScope
{
public:
    /** Remove this scope's policy from its ModelingContext, if the context is still alive. */
    GEOML_API_EXPORT ~MetadataScope();

    MetadataScope(MetadataScope const&) = delete;
    MetadataScope& operator=(MetadataScope const&) = delete;

    /** Transfer ownership of this active policy scope. */
    GEOML_API_EXPORT MetadataScope(MetadataScope&& other) noexcept;

    /** Release any active scope held by `*this` and take ownership from `other`. */
    GEOML_API_EXPORT MetadataScope& operator=(MetadataScope&& other) noexcept;

private:
    friend class geoml::internal::MetadataPolicyContext;

    MetadataScope(std::weak_ptr<details::MetadataPolicyStack> stack, std::uint64_t scope_id) noexcept;
    void release() noexcept;

    std::weak_ptr<details::MetadataPolicyStack> m_stack;
    std::uint64_t m_scope_id = 0;
};

} // namespace metadata
} // namespace geoml
