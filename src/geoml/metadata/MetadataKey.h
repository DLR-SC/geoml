#pragma once

#include "geoml/color/Color.h"
#include <geoml/exports.h>

#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace geoml
{
namespace metadata
{

/** A typed key identified by its name. The name must be used with a consistent value type. */
template <typename Value>
class Key
{
    static_assert(std::is_same_v<Value, std::decay_t<Value>>,
                  "Metadata keys require an unqualified value type");

public:
    using value_type = Value;

    explicit Key(std::string name) : m_name(std::move(name)) {}

    /** Return the stable name that identifies this key. */
    std::string const& name() const noexcept { return m_name; }

private:
    std::string m_name;
};

namespace keys {

/** Return the process-wide key for a shape's display name. */
GEOML_API_EXPORT Key<std::string> const& name();

/** Return the process-wide key for a shape's color. */
GEOML_API_EXPORT Key<geoml::Color> const& color();

/** Return the key for a vector of string tags. The default merge keeps each tag once. */
GEOML_API_EXPORT Key<std::vector<std::string>> const& tags();

} // namespace keys

} // namespace metadata
} // namespace geoml
