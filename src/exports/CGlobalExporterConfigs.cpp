/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2018 German Aerospace Center (DLR)
*
* Created: 2018-04-04 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "CGlobalExporterConfigs.h"
#include "stringtools.h"

namespace geoml
{

CGlobalExporterConfigs &CGlobalExporterConfigs::Instance()
{
    static CGlobalExporterConfigs globalConfig;
    return globalConfig;
}

void CGlobalExporterConfigs::Add(const std::string &supportedFileTypes, const ExporterOptions &config)
{
    m_configs.push_back(config);
    size_t idx = m_configs.size() - 1;
    
    std::vector<std::string> fileTypes = split_string(supportedFileTypes, ';');
    for (std::vector<std::string>::const_iterator it = fileTypes.begin(); it != fileTypes.end(); ++it) {
        std::string type = *it;
        m_index_map[to_lower(type)] = idx;
    }
}

const ExporterOptions& CGlobalExporterConfigs::Get(const std::string &fileType) const
{
    IndexMap::const_iterator it = m_index_map.find(to_lower(fileType));
    if (it == m_index_map.end()) {
        throw Error("No Exporter for type '" + fileType + "'", geoml::NOT_FOUND);
    }
    else {
        size_t idx = it->second;
        return m_configs[idx];
    }
}

ExporterOptions& CGlobalExporterConfigs::Get(const std::string &fileType)
{
     return const_cast<ExporterOptions&>(static_cast<const CGlobalExporterConfigs*>(this)->Get(fileType));
}

CGlobalExporterConfigs::CGlobalExporterConfigs()
{
}

} // namespace geoml
