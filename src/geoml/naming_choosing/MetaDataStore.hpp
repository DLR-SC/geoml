#pragma once

#include <string>
#include <unordered_map>
#include <any>

namespace geoml {

class Shape;

/**
 * @brief The MetaDataStore class stores metadata associated to hashes
 */
class MetaDataStore
{
    friend class Shape;
public:
    using key_type = std::size_t; // hashes for shapes
    struct Entry {
        std::any value;

        template <typename T>
        T as() {
            return std::any_cast<T>(value);
        }
    };
    using map_type = std::unordered_map<std::string, Entry>;

    map_type& operator[](key_type const& key);

private:
    MetaDataStore() = default;
    std::unordered_map<key_type, map_type> m_metadata;
};

} // namepsace geoml
