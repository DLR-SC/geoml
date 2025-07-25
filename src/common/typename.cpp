/*
* Copyright (c) 2016 RISC Software GmbH
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
