#pragma once

#include "geoml/metadata/MetadataKey.h"
#include <any>
#include <map>
#include <optional>
#include <typeindex>

namespace geoml
{
namespace internal {
class MetadataTrackContext;
}

namespace metadata
{
namespace details {
struct ResolutionRequest;
}

/**
 * Named values attached to one topological entity. The key determines the C++
 * type at the call site, so the same store can hold a name, color and custom
 * values. Keys with the same name and value type are interchangeable.
 */
class Metadata
{
public:
    /** An entry exposed for inspection and serialization. */
    struct Entry
    {
        std::any value; ///< Stored value, with its runtime type available through value.type().

    private:
        friend class Metadata;

        Entry() = default;

        std::any (*m_resolve)(details::ResolutionRequest const&) = nullptr;
    };
    using Entries = std::map<std::string, Entry>;

    /**
     * Store a value as supplied, replacing any existing value of the same type.
     */
    template <typename Value>
    void set(Key<Value> const& key, typename Key<Value>::value_type value);

    /** Check whether this store contains `key`. */
    template <typename Value>
    bool contains(Key<Value> const& key) const;

    /** Return a copy of the value, or an empty optional when the key is absent. */
    template <typename Value>
    std::optional<Value> find(Key<Value> const& key) const;

    /** Borrow the stored value. An absent key raises `std::out_of_range`. */
    template <typename Value>
    Value const& at(Key<Value> const& key) const;

    /** Remove the entry for `key`. Return whether an entry was removed. */
    template <typename Value>
    bool erase(Key<Value> const& key);

    /**
     * Merge `source` using the registered defaults or the generic fallback.
     * If resolution fails, no entries are updated.
     */
    GEOML_API_EXPORT void merge_from(Metadata const& source);

    /** Inspect borrowed values of different types in stable key-name order. */
    Entries const& entries() const noexcept { return m_entries; }

    /** Return whether the store has no entries. */
    bool empty() const noexcept { return m_entries.empty(); }

    /** Return the number of entries in the store. */
    std::size_t size() const noexcept { return m_entries.size(); }

private:
    friend class geoml::internal::MetadataTrackContext;

    void swap(Metadata& other) noexcept { m_entries.swap(other.m_entries); }

    GEOML_API_EXPORT void store_value(std::string const& name, Entry entry);
    GEOML_API_EXPORT std::any const* find_value(std::string const& name, std::type_index type) const;
    GEOML_API_EXPORT bool erase_value(std::string const& name, std::type_index type);

    void resolve_and_set(Entry const& entry, details::ResolutionRequest request);

    Entries m_entries;
};

} // namespace metadata
} // namespace geoml

#include "geoml/metadata/detail/MetadataImpl.h"
