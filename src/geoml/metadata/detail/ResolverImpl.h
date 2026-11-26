#pragma once

#include <algorithm>
#include <any>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <utility>

namespace geoml {
namespace metadata {

namespace details {

template <typename Value>
struct IsEqualityComparable : std::is_invocable_r<bool, std::equal_to<>, Value const&, Value const&> {};

// In C++17 vector's equality operator is declared even when its elements cannot be compared.
template <typename Value, typename Allocator>
struct IsEqualityComparable<std::vector<Value, Allocator>> : IsEqualityComparable<Value> {};

template <typename Value>
bool metadata_values_equal(Value const& left, Value const& right)
{
    if constexpr (IsEqualityComparable<Value>::value) {
        return left == right;
    }
    return false;
}

template <typename Container>
void merge_unique_values(Container& destination, Container const& incoming)
{
    for (auto const& value : incoming) {
        if (std::find(destination.begin(), destination.end(), value) == destination.end()) {
            destination.insert(destination.end(), value);
        }
    }
}

template <typename Value>
std::vector<MetadataContribution>
erase_contributions(std::vector<ResolutionContribution<Value>> const& contributions)
{
    std::vector<MetadataContribution> result;
    result.reserve(contributions.size());
    for (auto const& contribution : contributions) {
        result.push_back({std::make_any<Value>(contribution.value), contribution.input_position,
                          contribution.source_track_id, contribution.source_topology});
    }
    return result;
}

template <typename Value, typename Callable, typename = void>
struct IsMetadataResolver : std::false_type {};

template <typename Value, typename Callable>
using ResolverResult = std::invoke_result_t<Callable&, ResolutionContext<Value> const&>;

template <typename Value, typename Callable>
struct IsMetadataResolver<Value, Callable, std::void_t<ResolverResult<Value, Callable>>>
    : std::is_same<ResolverResult<Value, Callable>, Value> {};

// Default and policy registrations accept the same callback signature.
template <typename Value, typename Callable>
MetadataResolver<Value> checked_resolver(Callable&& callable)
{
    static_assert(IsMetadataResolver<Value, std::decay_t<Callable>>::value,
                  "A metadata resolver must accept ResolutionContext<Value> const& "
                  "and return exactly Value");
    MetadataResolver<Value> resolver(std::forward<Callable>(callable));
    if (!resolver) {
        throw std::invalid_argument("Cannot register an empty metadata resolver");
    }
    return resolver;
}

GEOML_API_EXPORT void register_default_resolver(
    std::string const& name, std::type_index type, std::any resolver);

} // namespace details

template <typename Value, typename Callable>
void register_default_resolver(Key<Value> const& key, Callable&& callable)
{
    details::register_default_resolver(
        key.name(), typeid(Value), details::checked_resolver<Value>(std::forward<Callable>(callable)));
}

template <typename Value>
Value ReplaceResolver::operator()(ResolutionContext<Value> const& context) const
{
    if (!context.contributions.empty()) {
        return context.contributions.back().value;
    }
    if (context.existing_value) {
        return *context.existing_value;
    }
    throw std::logic_error("Cannot resolve metadata without values");
}

template <typename Value>
Value RequireEqualResolver::operator()(ResolutionContext<Value> const& context) const
{
    std::optional<Value> result = context.existing_value;
    for (auto const& contribution : context.contributions) {
        if (!result) {
            result = contribution.value;
        }
        else if (!details::metadata_values_equal(*result, contribution.value)) {
            throw MetadataConflictException(
                context.key_name, typeid(Value), std::make_any<Value>(*result),
                std::make_any<Value>(contribution.value),
                details::erase_contributions(context.contributions),
                context.target_topology);
        }
    }
    if (!result) {
        throw std::logic_error("Cannot resolve metadata without values");
    }
    return *result;
}

template <typename Container>
Container SetUnionResolver::operator()(ResolutionContext<Container> const& context) const
{
    if (!context.existing_value && context.contributions.empty()) {
        throw std::logic_error("Cannot resolve metadata without values");
    }
    Container result;
    if (context.existing_value) {
        details::merge_unique_values(result, *context.existing_value);
    }
    for (auto const& contribution : context.contributions) {
        details::merge_unique_values(result, contribution.value);
    }
    return result;
}

} // namespace metadata
} // namespace geoml
