#include "MetaDataStore.hpp"

namespace geoml {

MetaDataStore::map_type& MetaDataStore::operator[](MetaDataStore::key_type const& key)
{
    return m_metadata[key];
}

} // namespace geoml
