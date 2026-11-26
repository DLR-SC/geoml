#include "geoml/modeling/ModelingContext.h"
#include "topology/MetadataPolicyContext.h"
#include "topology/MetadataTrackContext.h"

#include <stdexcept>
#include <utility>

namespace geoml {

TrackLifetime TrackLifetime::steps(int count)
{
    if (count <= 0) {
        throw std::invalid_argument("Metadata track lifetime must be positive");
    }
    return TrackLifetime{count};
}

ModelingContext::ModelingContext()
    : m_track_context(std::make_unique<internal::MetadataTrackContext>())
    , m_policy_context(std::make_unique<internal::MetadataPolicyContext>())
{}

ModelingContext::~ModelingContext() = default;

ModelingContext::ModelingContext(ModelingContext&&) noexcept = default;

ModelingContext& ModelingContext::operator=(ModelingContext&&) noexcept = default;

MetadataTrackHandle ModelingContext::register_metadata_track(Shape const& target,
                                                             metadata::Metadata contribution,
                                                             ShapePredicate criterion,
                                                             TrackLifetime lifetime,
                                                             Shape const* source)
{
    return m_track_context->register_track(target, std::move(contribution), std::move(criterion),
                                           lifetime, source);
}

bool ModelingContext::cancel_metadata_track(MetadataTrackHandle handle)
{
    return m_track_context->cancel_track(handle);
}

metadata::MetadataScope ModelingContext::create_policy_scope(metadata::MetadataPolicy policy)
{
    return m_policy_context->create_scope(std::move(policy));
}

void ModelingContext::propagate_metadata_tracks(Shape& result, std::vector<Shape> const& inputs)
{
    m_track_context->propagate(result, inputs, *m_policy_context);
}

} // namespace geoml
