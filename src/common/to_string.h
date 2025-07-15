/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2016 Airbus Defense and Space
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
