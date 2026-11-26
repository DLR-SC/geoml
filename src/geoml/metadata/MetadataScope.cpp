#include "geoml/metadata/MetadataScope.h"
#include "topology/MetadataPolicyContext.h"

#include <algorithm>
#include <utility>

namespace geoml {
namespace metadata {

namespace details {

std::uint64_t MetadataPolicyStack::push(MetadataPolicy policy)
{
    auto const id = next_id++;
    entries.push_back({id, std::move(policy)});
    return id;
}

void MetadataPolicyStack::erase(std::uint64_t id) noexcept
{
    auto it = std::find_if(entries.begin(), entries.end(),
                           [id](Entry const& entry) { return entry.id == id; });
    if (it != entries.end()) {
        entries.erase(it);
    }
}

std::any const* MetadataPolicyStack::find_resolver(std::string const& name,
                                                   std::type_index type) const
{
    for (auto entry = entries.rbegin(); entry != entries.rend(); ++entry) {
        if (auto const* resolver = entry->policy.find(name, type)) {
            return resolver;
        }
    }
    return nullptr;
}

} // namespace details

MetadataScope::MetadataScope(std::weak_ptr<details::MetadataPolicyStack> stack,
                             std::uint64_t scope_id) noexcept
    : m_stack(std::move(stack))
    , m_scope_id(scope_id)
{}

MetadataScope::~MetadataScope()
{
    release();
}

MetadataScope::MetadataScope(MetadataScope&& other) noexcept
    : m_stack(std::move(other.m_stack))
    , m_scope_id(std::exchange(other.m_scope_id, 0))
{}

MetadataScope& MetadataScope::operator=(MetadataScope&& other) noexcept
{
    if (this != &other) {
        release();
        m_stack = std::move(other.m_stack);
        m_scope_id = std::exchange(other.m_scope_id, 0);
    }
    return *this;
}

void MetadataScope::release() noexcept
{
    if (m_scope_id == 0) {
        return;
    }
    if (auto stack = m_stack.lock()) {
        stack->erase(m_scope_id);
    }
    m_stack.reset();
    m_scope_id = 0;
}

} // namespace metadata

namespace internal {

MetadataPolicyContext::MetadataPolicyContext()
    : m_stack(std::make_shared<metadata::details::MetadataPolicyStack>())
{}

metadata::MetadataScope MetadataPolicyContext::create_scope(metadata::MetadataPolicy policy)
{
    auto const id = m_stack->push(std::move(policy));
    return metadata::MetadataScope(m_stack, id);
}

std::any const* MetadataPolicyContext::find_resolver(std::string const& name,
                                                     std::type_index type) const
{
    return m_stack->find_resolver(name, type);
}

} // namespace internal
} // namespace geoml
