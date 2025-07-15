/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-02-28 Markus Litz <Markus.Litz@dlr.de>
*/
/**
* @file
* @brief  STEP-Export routines for CPACS configurations.
*/

#ifndef EXPORTSTEP_H
#define EXPORTSTEP_H

#include "geoml_internal.h"
#include "ListPNamedShape.h"
#include "CADExporter.h"

class STEPControl_Writer;

namespace geoml 
{

class CCPACSConfiguration;

class StepOptions : public ExporterOptions
{
public:
    StepOptions()
    {
        Set("ShapeGroupMode", NAMED_COMPOUNDS);
    }
};

class StepShapeOptions : public ShapeExportOptions
{
public:
    StepShapeOptions(){}
};

class ExportStep : public CADExporter
{

public:
    // Constructor
    GEOML_EXPORT ExportStep(const ExporterOptions& opt = DefaultExporterOption());

    GEOML_EXPORT ExporterOptions GetDefaultOptions() const override;
    GEOML_EXPORT ShapeExportOptions GetDefaultShapeOptions() const override;

private:
    // Writes the step file
    GEOML_EXPORT bool WriteImpl(const std::string& filename) const override;

    std::string SupportedFileTypeImpl() const override
    {
        return "step;stp";
    }

    // Assignment operator
    void operator=(const ExportStep& ) { /* Do nothing */ }

    void AddToStep(PNamedShape shape, STEPControl_Writer &writer) const;
};

} // end namespace geoml

#endif // EXPORTSTEP_H
