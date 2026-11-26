#pragma once

#include "geoml/metadata/MetadataResolution.h"

namespace geoml {
namespace metadata {
namespace details {

struct ResolutionRequest
{
    std::string const& name;
    std::vector<MetadataContribution> const& contributions;
    std::optional<TopoDS_Shape> target;
    std::any const* existing = nullptr;
    std::any const* default_resolver = nullptr;
    std::any const* override_resolver = nullptr;
};

template <typename Value>
std::any resolve_value(ResolutionRequest const& request)
{
    auto ordinary = [default_resolver = request.default_resolver](ResolutionContext<Value> const& context) {
        if (default_resolver) {
            return std::any_cast<MetadataResolver<Value> const&>(*default_resolver)(context);
        }
        return RequireEqualResolver{}(context);
    };

    ResolutionContext<Value> context;
    context.key_name = request.name;
    context.target_topology = request.target;
    if (request.existing) {
        context.existing_value.emplace(std::any_cast<Value const&>(*request.existing));
    }
    context.contributions.reserve(request.contributions.size());
    for (auto const& incoming : request.contributions) {
        context.contributions.push_back({std::any_cast<Value const&>(incoming.value),
                                         incoming.input_position, incoming.source_track_id,
                                         incoming.source_topology});
    }

    if (request.override_resolver) {
        context.key_default = ordinary;
        auto const& resolve = std::any_cast<MetadataResolver<Value> const&>(*request.override_resolver);
        return std::make_any<Value>(resolve(context));
    }
    return std::make_any<Value>(ordinary(context));
}

} // namespace details
} // namespace metadata
} // namespace geoml
