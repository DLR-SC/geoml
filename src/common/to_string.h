/*
* Copyright (C) 2016 Airbus Defense and Space
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
#ifndef TO_STRING_H
#define TO_STRING_H

#include <sstream>

namespace geoml
{

template <typename FordwardIter>
std::string vecToString(FordwardIter begin, FordwardIter end, const char sep)
{
    std::stringstream ss;
    for (FordwardIter it = begin; it != end; ++it) {
        ss << *it;
        if (it != end - 1) {
            ss << sep;
        }
    }
    return ss.str();
}

}

#endif
