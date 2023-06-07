/*
* Copyright (C) 2018 German Aerospace Center (DLR/SC)
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

#ifndef EXPORTERFACTORY_H
#define EXPORTERFACTORY_H

#include "geoml_internal.h"
#include "CADExporter.h"

#include <map>

namespace geoml
{

class ExporterFactory
{
public:
    GEOML_EXPORT static ExporterFactory& Instance();

    /// Registers a CAD Exporter at the factory
    GEOML_EXPORT void RegisterExporter(ICADExporterBuilder* creator, const ExporterOptions& exporterConfig);

    /// Creates an exporter for the matching file format
    /// Returns NULL, if the format is unknown
    GEOML_EXPORT std::unique_ptr<CADExporter> Create(const std::string& filetype, const ExporterOptions& options = DefaultExporterOption()) const;

    /// Returns true, if an exporter was registered for the specified file type
    GEOML_EXPORT bool ExporterSupported(const std::string& filetype) const;

private:
    ExporterFactory();
    
    typedef std::map<std::string, ICADExporterBuilder*>  ExporterMap;
    ExporterMap _exporterBuilders;
};

inline std::unique_ptr<CADExporter> createExporter(const std::string& filetype, const ExporterOptions& options = DefaultExporterOption())
{
    return ExporterFactory::Instance().Create(filetype, options);
}

} // namespace geoml

#endif // EXPORTERFACTORY_H
