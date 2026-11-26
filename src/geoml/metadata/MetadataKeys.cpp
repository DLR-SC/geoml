#include "geoml/metadata/MetadataKey.h"

namespace geoml {
namespace metadata {
namespace keys {

Key<std::string> const& name()
{
    static const Key<std::string> key{"name"};
    return key;
}

Key<geoml::Color> const& color()
{
    static const Key<geoml::Color> key{"color"};
    return key;
}

Key<std::vector<std::string>> const& tags()
{
    static const Key<std::vector<std::string>> key{"tags"};
    return key;
}

} // namespace keys
} // namespace metadata
} // namespace geoml
