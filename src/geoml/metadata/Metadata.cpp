#include "geoml/metadata/Metadata.h"
#include "geoml/metadata/MetadataResolution.h"
#include "geoml/metadata/detail/ResolveValue.h"

#include <mutex>
#include <utility>

namespace geoml {
namespace metadata {
namespace {

void require_value_type(std::string const& name, std::type_index expected,
                        std::type_index actual)
{
    if (expected != actual) {
        throw std::logic_error("Incompatible metadata value type for key: " + name);
    }
}

struct DefaultResolvers
{
    struct Entry
    {
        std::type_index type;
        std::any resolver;
    };

    std::mutex mutex;
    std::map<std::string, Entry> entries{
        {keys::name().name(), {typeid(std::string), MetadataResolver<std::string>{RequireEqualResolver{}}}},
        {keys::color().name(), {typeid(Color), MetadataResolver<Color>{RequireEqualResolver{}}}},
        {keys::tags().name(), {typeid(std::vector<std::string>),
                              MetadataResolver<std::vector<std::string>>{SetUnionResolver{}}}}
    };
};

DefaultResolvers& default_resolvers()
{
    static DefaultResolvers registry;
    return registry;
}

std::any const* find_default_resolver(std::string const& name, std::type_index type)
{
    auto& registry = default_resolvers();
    std::lock_guard<std::mutex> lock(registry.mutex);
    auto const it = registry.entries.find(name);
    if (it == registry.entries.end()) {
        return nullptr;
    }
    require_value_type(name, type, it->second.type);
    // Registrations only insert, so callbacks can run after releasing the lock.
    return &it->second.resolver;
}

} // namespace

MetadataConflictException::MetadataConflictException(
    std::string key_name, std::type_index value_type, std::any existing_value,
    std::any incoming_value, std::vector<MetadataContribution> contributions,
    std::optional<TopoDS_Shape> target_topology)
    : std::runtime_error("Conflicting metadata values for key: " + key_name)
    , m_key_name(std::move(key_name))
    , m_value_type(value_type)
    , m_existing_value(std::move(existing_value))
    , m_incoming_value(std::move(incoming_value))
    , m_contributions(std::move(contributions))
    , m_target_topology(std::move(target_topology))
{}

void details::register_default_resolver(std::string const& name, std::type_index type,
                                        std::any resolver)
{
    auto& registry = default_resolvers();
    DefaultResolvers::Entry entry{type, std::move(resolver)};
    std::lock_guard<std::mutex> lock(registry.mutex);
    if (!registry.entries.try_emplace(name, std::move(entry)).second) {
        throw std::logic_error("Default metadata resolver already registered for key: " + name);
    }
}

void Metadata::store_value(std::string const& name, Entry entry)
{
    auto it = m_entries.find(name);
    if (it != m_entries.end()) {
        require_value_type(name, it->second.value.type(), entry.value.type());
    }
    m_entries.insert_or_assign(name, std::move(entry));
}

std::any const* Metadata::find_value(std::string const& name, std::type_index type) const
{
    auto it = m_entries.find(name);
    if (it == m_entries.end()) {
        return nullptr;
    }
    require_value_type(name, type, it->second.value.type());
    return &it->second.value;
}

bool Metadata::erase_value(std::string const& name, std::type_index type)
{
    auto it = m_entries.find(name);
    if (it == m_entries.end()) {
        return false;
    }
    require_value_type(name, type, it->second.value.type());
    m_entries.erase(it);
    return true;
}

void Metadata::resolve_and_set(Entry const& entry, details::ResolutionRequest request)
{
    std::type_index const type = entry.value.type();
    request.existing = find_value(request.name, type);
    request.default_resolver = find_default_resolver(request.name, type);
    try {
        Entry resolved;
        resolved.value = entry.m_resolve(request);
        resolved.m_resolve = entry.m_resolve;
        store_value(request.name, std::move(resolved));
    }
    catch (MetadataConflictException const& conflict) {
        throw MetadataConflictException(conflict.key_name(), conflict.value_type(),
                                        conflict.existing_value(), conflict.incoming_value(),
                                        request.contributions, request.target);
    }
}

void Metadata::merge_from(Metadata const& source)
{
    if (this == &source) {
        return;
    }
    for (auto const& [name, entry] : source.m_entries) {
        find_value(name, entry.value.type());
    }

    Metadata merged = *this;
    for (auto const& [name, entry] : source.m_entries) {
        merged.resolve_and_set(entry, {name, {{entry.value}}, std::nullopt});
    }
    swap(merged);
}

} // namespace metadata
} // namespace geoml
