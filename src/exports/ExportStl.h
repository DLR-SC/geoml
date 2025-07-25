/* 
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2010-08-13 Markus Litz <Markus.Litz@dlr.de>
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
* @brief  Export routines for CPACS configurations.
*/

#ifndef EXPORTSTL_H
#define EXPORTSTL_H

#include "geoml_internal.h"
#include "CADExporter.h"




namespace geoml 
{

class StlOptions : public ExporterOptions
{
public:
    StlOptions()
    {
    }
};

class ExportStl : public CADExporter
{

public:
    // Constructor
    GEOML_EXPORT ExportStl(const ExporterOptions& opt = DefaultExporterOption());

    GEOML_EXPORT ExporterOptions GetDefaultOptions() const override;
    GEOML_EXPORT ShapeExportOptions GetDefaultShapeOptions() const override;

private:

    GEOML_EXPORT bool WriteImpl(const std::string& filename) const override;

    std::string SupportedFileTypeImpl() const override
    {
        return "stl";
    }

    // Assignment operator
    void operator=(const ExportStl& ) { /* Do nothing */ }
};

} // end namespace geoml

#endif // EXPORTSTL_H
