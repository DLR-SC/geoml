/* 
* Copyright (C) 2018 German Aerospace Center (DLR/SC)
*
* Created: 2018-04-04 Martin Siggel <Martin.Siggel@dlr.de>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

#include "typename.h"
#include "geoml/error.h"

#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

namespace geoml
{

inline std::string to_lower(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);

    return result;
}

inline std::string to_upper(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);

    return result;
}

inline std::vector<std::string> split_string(const std::string& mystring, char delimiter)
{
    std::vector<std::string> strings;
    std::istringstream f(mystring);
    std::string s;
    while (std::getline(f, s, delimiter)) {
        strings.push_back(s);
    }
    return strings;
}

template <class to_value>
inline void from_string(const std::string& s, to_value& t)
{
    using namespace std::string_literals;
    throw geoml::Error("no template specialization for type"s + typeName<to_value>());
}

template <>
inline void from_string(const std::string& s, std::string& t)
{
    t = s;
}


template <>
inline void from_string(const std::string& s, int& t)
{
    try {
        t = std::stoi(s);
    }
    catch(std::exception&) {
        throw geoml::Error("Cannot convert string to int");
    }
}

template <>
inline void from_string(const std::string& s, Standard_Real& t)
{
    try {
        t = std::stod(s);
    }
    catch(std::exception&) {
        throw geoml::Error("Cannot convert string to int");
    }
}

template <>
inline void from_string(const std::string& s, float& t)
{
    try {
        t = std::stof(s);
    }
    catch(std::exception&) {
        throw geoml::Error("Cannot convert string to int");
    }
}


template <>
inline void from_string<bool>(const std::string& s, bool& t)
{
    std::string str = geoml::to_lower(s);
    if (str == "1" || str == "true" || str == "yes") {
        t = true;
    }
    else if (str == "0" || str == "false" || str == "no") {
        t = false;
    }
    else {
        throw geoml::Error("Cannot convert string '" + s + "' to bool.");
    }
}

} // namespace geoml

#endif // STRINGTOOLS_H
