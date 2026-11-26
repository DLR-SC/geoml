#pragma once

namespace geoml {
namespace metadata {

template <typename Value, typename Callable>
MetadataPolicy& MetadataPolicy::for_key(Key<Value> const& key, Callable&& callable)
{
    register_resolver(key.name(), typeid(Value),
                      details::checked_resolver<Value>(std::forward<Callable>(callable)));
    return *this;
}

template <typename Value>
Value PreferInputResolver::operator()(ResolutionContext<Value> const& context) const
{
    if (!context.key_default) {
        throw std::logic_error("prefer_input requires the key's default resolver");
    }

    std::vector<ResolutionContribution<Value>> matches;
    for (auto const& contribution : context.contributions) {
        if (contribution.input_position == input_index) {
            matches.push_back(contribution);
        }
    }
    if (matches.empty()) {
        return context.key_default(context);
    }
    ResolutionContext<Value> preferred{context.key_name, std::nullopt, std::move(matches),
                                       context.target_topology, context.key_default};
    return context.key_default(preferred);
}

} // namespace metadata
} // namespace geoml
