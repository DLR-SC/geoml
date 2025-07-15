/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
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
