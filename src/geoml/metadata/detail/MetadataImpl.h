#pragma once

#include "geoml/metadata/detail/ResolveValue.h"

namespace geoml {
namespace metadata {

template <typename Value>
void Metadata::set(Key<Value> const& key, typename Key<Value>::value_type value)
{
    Entry entry;
    entry.value = std::make_any<Value>(std::move(value));
    entry.m_resolve = &details::resolve_value<Value>;
    store_value(key.name(), std::move(entry));
}

template <typename Value>
bool Metadata::contains(Key<Value> const& key) const
{
    return find_value(key.name(), typeid(Value)) != nullptr;
}

template <typename Value>
std::optional<Value> Metadata::find(Key<Value> const& key) const
{
    auto const* value = find_value(key.name(), typeid(Value));
    if (!value) {
        return std::nullopt;
    }
    return std::optional<Value>{std::any_cast<Value const&>(*value)};
}

template <typename Value>
Value const& Metadata::at(Key<Value> const& key) const
{
    if (auto value = find_value(key.name(), typeid(Value))) {
        return std::any_cast<Value const&>(*value);
    }
    throw std::out_of_range("No metadata value for key: " + key.name());
}

template <typename Value>
bool Metadata::erase(Key<Value> const& key)
{
    return erase_value(key.name(), typeid(Value));
}

} // namespace metadata
} // namespace geoml
