#pragma once

#include <geoml/exports.h>

#include <any>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <vector>

#include <TopoDS_Shape.hxx>

namespace geoml {
namespace metadata {

/** One incoming value and its origin in the modeling operation. */
template <typename Value>
struct ResolutionContribution
{
    Value value; ///< Value contributed for the key.
    std::optional<std::size_t> input_position; ///< Zero-based operation input, if known.
    std::optional<std::uint64_t> source_track_id; ///< Track that supplied the value, if any.
    std::optional<TopoDS_Shape> source_topology; ///< Source topology, if known.
};

/** Type-erased contribution used to report conflicts for metadata values of any type. */
using MetadataContribution = ResolutionContribution<std::any>;

/** Structured information about two incompatible values for one metadata key. */
class GEOML_API_EXPORT MetadataConflictException : public std::runtime_error
{
public:
    /**
     * Create a conflict describing incompatible metadata values.
     * @param key_name Stable name of the conflicting metadata key.
     * @param value_type Runtime type of values associated with the key.
     * @param existing_value Value used for comparison, if available.
     * @param incoming_value Incoming value incompatible with the comparison value.
     * @param contributions Incoming contributions and their operation origins.
     * @param target_topology Topology on which resolution was attempted, if known.
     */
    MetadataConflictException(std::string key_name,
                              std::type_index value_type,
                              std::any existing_value,
                              std::any incoming_value,
                              std::vector<MetadataContribution> contributions = {},
                              std::optional<TopoDS_Shape> target_topology = std::nullopt);

    /** Return the stable name of the conflicting key. */
    std::string const& key_name() const noexcept { return m_key_name; }
    /** Return the runtime type of the key's values. */
    std::type_index value_type() const noexcept { return m_value_type; }
    /**
     * Return the comparison value, or an empty `std::any` if unavailable.
     * RequireEqualResolver compares against the existing value in its context
     * when present and uses the first contribution otherwise.
     */
    std::any const& existing_value() const noexcept { return m_existing_value; }
    /** Return the incoming value that triggered the conflict. */
    std::any const& incoming_value() const noexcept { return m_incoming_value; }
    /** Return all incoming contributions considered during resolution. */
    std::vector<MetadataContribution> const& contributions() const noexcept
    {
        return m_contributions;
    }
    /** Return the target topology, or `std::nullopt` if it is unavailable. */
    std::optional<TopoDS_Shape> const& target_topology() const noexcept
    {
        return m_target_topology;
    }

private:
    std::string m_key_name;
    std::type_index m_value_type;
    std::any m_existing_value;
    std::any m_incoming_value;
    std::vector<MetadataContribution> m_contributions;
    std::optional<TopoDS_Shape> m_target_topology;
};

} // namespace metadata
} // namespace geoml
