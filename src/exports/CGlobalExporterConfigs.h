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

#ifndef CGLOBALEXPORTERCONFIGS_H
#define CGLOBALEXPORTERCONFIGS_H

#include "geoml_internal.h"
#include "CADExporter.h"
#include <map>
#include <string>

namespace geoml
{

/** Singleton instance holding the configs */
class CGlobalExporterConfigs
{
public:
    GEOML_EXPORT static CGlobalExporterConfigs &Instance();

    GEOML_EXPORT void Add(const std::string& supportedFileTypes, const ExporterOptions& config);

    GEOML_EXPORT const ExporterOptions& Get(const std::string& fileType) const;
    GEOML_EXPORT ExporterOptions& Get(const std::string& fileType);

private:
    CGlobalExporterConfigs();

    std::vector<ExporterOptions> m_configs;

    typedef std::map<std::string, size_t> IndexMap;
    IndexMap m_index_map;
};

inline ExporterOptions& getExportConfig(const std::string& filetype)
{
    return CGlobalExporterConfigs::Instance().Get(filetype);
}

} // namespace geoml

#endif // CGLOBALEXPORTERCONFIGS_H
