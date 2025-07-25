/*
* Copyright (C) 2015 German Aerospace Center (DLR/SC)
*
* Created: 2015-05-27 Martin Siggel <Martin.Siggel@dlr.de>
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

#ifndef ICADIMPORTER_H
#define ICADIMPORTER_H

#include "ListPNamedShape.h"

#include <string>

namespace geoml
{

/**
 * @brief The ICADImporter Interface class should be common
 * for all CAD importing implementations
 */
class ICADImporter
{
public:
    /// read in file name
    virtual ListPNamedShape Read(const std::string fileName) = 0;

    /// should return the supported file type, e.g. "step"
    virtual std::string SupportedFileType() const  = 0;

    virtual ~ICADImporter() {}

};

}

#endif // ICADIMPORTER_H
