#pragma once

#include "geoml/metadata/MetadataPolicy.h"
#include "geoml/metadata/MetadataScope.h"

#include <any>
#include <cstdint>
#include <memory>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

namespace geoml {
namespace metadata {
namespace details {

struct MetadataPolicyStack
{
    struct Entry
    {
        std::uint64_t id = 0;
        MetadataPolicy policy;
    };

    std::vector<Entry> entries;
    std::uint64_t next_id = 1;

    std::uint64_t push(MetadataPolicy policy);
    void erase(std::uint64_t id) noexcept;
    std::any const* find_resolver(std::string const& name, std::type_index type) const;
};

} // namespace details
} // namespace metadata

namespace internal {

class MetadataPolicyContext
{
public:
    MetadataPolicyContext();

    metadata::MetadataScope create_scope(metadata::MetadataPolicy policy);
    std::any const* find_resolver(std::string const& name, std::type_index type) const;

private:
    std::shared_ptr<metadata::details::MetadataPolicyStack> m_stack;
};

} // namespace internal
} // namespace geoml
