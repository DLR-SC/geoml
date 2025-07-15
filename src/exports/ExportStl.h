/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2010-08-13 Markus Litz <Markus.Litz@dlr.de>
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
