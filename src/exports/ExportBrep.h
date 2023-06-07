/* 
* Copyright (C) 2015 German Aerospace Center (DLR/SC)
*
* Created: 2015-04-22 Martin Siggel <Martin.Siggel@dlr.de>
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
/**
* @file
* @brief  BRep-Export routines for CPACS configurations.
*/

#ifndef EXPORTBREP_H
#define EXPORTBREP_H

#include <string>

#include "geoml_internal.h"
#include "ListPNamedShape.h"
#include "CADExporter.h"

namespace geoml 
{

class CCPACSConfiguration;

class BRepOptions : public ExporterOptions
{
public:
    BRepOptions()
    {
        Set("ShapeGroupMode", WHOLE_SHAPE);
    }
};

class ExportBrep : public CADExporter
{

public:
    // Constructor
    GEOML_EXPORT ExportBrep(const ExporterOptions& opt = DefaultExporterOption())
        : CADExporter(opt)
    {
    }

    GEOML_EXPORT ExporterOptions GetDefaultOptions() const override;
    GEOML_EXPORT ShapeExportOptions GetDefaultShapeOptions() const override;

private:
    // Writes the shapes to BREP. In multiple shapes were added
    // a compound is created.
    GEOML_EXPORT bool WriteImpl(const std::string& filename) const override;

    std::string SupportedFileTypeImpl() const override
    {
        return "brep";
    }
};

} // namespace geoml

#endif // EXPORTBREP_H
