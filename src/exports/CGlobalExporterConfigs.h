/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2018 German Aerospace Center (DLR)
*
* Created: 2018-04-04 Martin Siggel <Martin.Siggel@dlr.de>
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
