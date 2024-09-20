/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2016 RISC Software GmbH
*/

#include "typename.h"

#include <stdexcept>

namespace geoml
{

std::string typeName(const std::type_info& ti)
{
    // name holds fully qualified class name
    std::string name = ti.name(); // e.g. "[class] geoml::CCPACSWing"

    // strip leading "class" (VC++)
    const size_t  pos = name.find_last_of(' ');
    if (pos != std::string::npos) {
        name = name.substr(pos + 1); // e.g. "geoml::CCPACSWing"
    }

    return name;
}

} // namespace geoml
