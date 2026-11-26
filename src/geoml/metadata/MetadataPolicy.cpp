#include "geoml/metadata/MetadataPolicy.h"

namespace geoml {
namespace metadata {

void MetadataPolicy::register_resolver(std::string const& name, std::type_index type,
                                       std::any resolver)
{
    auto found = m_resolvers.find(name);
    if (found != m_resolvers.end() && found->second.type != type) {
        throw std::logic_error("Incompatible metadata policy value type for key: " + name);
    }
    m_resolvers.insert_or_assign(name, Entry{type, std::move(resolver)});
}

std::any const* MetadataPolicy::find(std::string const& name, std::type_index type) const
{
    auto const found = m_resolvers.find(name);
    if (found == m_resolvers.end()) {
        return nullptr;
    }
    if (found->second.type != type) {
        throw std::logic_error("Incompatible metadata value type for key: " + name);
    }
    return &found->second.resolver;
}

} // namespace metadata
} // namespace geoml
