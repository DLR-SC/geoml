#pragma once

#include "geoml/metadata/MetadataResolution.h"

#include <any>
#include <map>
#include <typeindex>

namespace geoml
{
namespace metadata
{

namespace details {
struct MetadataPolicyStack;
}

/** Typed resolver overrides for a ModelingContext policy scope. */
class MetadataPolicy
{
public:
    /**
     * Set or replace the merge resolver for this key. The callback takes a
     * `ResolutionContext<Value> const&` and returns `Value`.
     */
    template <typename Value, typename Callable>
    MetadataPolicy& for_key(Key<Value> const& key, Callable&& callable);

private:
    friend struct details::MetadataPolicyStack;

    struct Entry
    {
        std::type_index type;
        std::any resolver;
    };

    GEOML_API_EXPORT void register_resolver(std::string const& name, std::type_index type,
                                            std::any resolver);
    std::any const* find(std::string const& name, std::type_index type) const;

    std::map<std::string, Entry> m_resolvers;
};

/** Resolver returned by prefer_input(). */
struct PreferInputResolver
{
    std::size_t input_index; ///< Zero-based operation input to prefer.

    template <typename Value>
    Value operator()(ResolutionContext<Value> const& context) const;
};

/**
 * Resolve only the preferred input's contributions, dropping any existing target value.
 * If the preferred input has no contributions, resolve the original context.
 * Both paths use the registered default resolver or the generic fallback.
 * @param input_index Zero-based index of the preferred input.
 * @return A resolver suitable for `MetadataPolicy::for_key`.
 */
inline PreferInputResolver prefer_input(std::size_t input_index)
{
    return {input_index};
}

} // namespace metadata
} // namespace geoml

#include "geoml/metadata/detail/PolicyImpl.h"
