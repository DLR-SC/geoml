#include "common.hpp"

#include <unordered_map>

namespace geoml {

std::string new_id(std::string const& base_name)
{
    static std::unordered_map<std::string, int> base_name_counter;
    return base_name + std::to_string(base_name_counter[base_name]++);
}

} // namespace geoml
